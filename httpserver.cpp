#include "httpserver.h"
namespace NGHttpServer {

NGHttpServer::AbsServer::AbsServer(const int &maxHandlerThread)
{
    qDebug() << "abs server construct. maxHandlerThread="<<maxHandlerThread;
    this->_serverThreadPool.reset(new QThreadPool());
    this->_handlerThreadPool.reset(new QThreadPool());
    this->_serverThreadPool->setMaxThreadCount(1);
    this->_handlerThreadPool->setMaxThreadCount(maxHandlerThread);
}

NGHttpServer::AbsServer::~AbsServer()
{
    qDebug() << "abs server destruct";
}

void NGHttpServer::AbsServer::setOnRequestHandler(const std::function<void (Session &)> &onRequestHandler)
{
    qDebug() << "onRequestHandler set";
    _onRequestHandler = onRequestHandler;
}

bool NGHttpServer::AbsServer::initialize()
{
    qDebug() << "initialized abs server";
    if ( QThread::currentThread() != this->thread() )
    {
        qDebug() << "NGHttpServer::AbsServer::initialize: error: listen from other thread";
        return false;
    }
    if( this->isRunning() ){
        qDebug() << "NGHttpServer::AbsServer::initialize: error: server already running";
    }
    QSemaphore sem;
    QtConcurrent::run(_serverThreadPool.data(),[this,&sem](){
        QEventLoop eventloop;
        connect(this,&AbsServer::readyToClose,&eventloop,&QEventLoop::quit);//将AbsServer的readyToClose信号绑定至eventLoop的quit槽。
        if(!this->onStart()){
            qDebug() << "onStart failed";
            sem.release(1);
            return;
        }
        qDebug() << "onStart succeed";
        sem.release(1);
        qDebug() << "Event Loop Begin";
        eventloop.exec();
        qDebug() << "Event Loop Stop";
        this->onFinish();
    });
    sem.acquire(1);//函数将阻塞在此，直至Concurrent中的线性结束，即onFinish()调用结束。
    qDebug() << "initialize over";
    return this->isRunning();
}

void NGHttpServer::AbsServer::deinitialize()
{
    qDebug() << "deinitialized abs server";
    if(!this->isRunning()){
        qDebug() << "NGHttpServer::AbsServer::deinitialize: error: server not running";
        return;
    }
    emit readyToClose();
    if(this->_serverThreadPool->activeThreadCount()){
        this->_serverThreadPool->waitForDone();
    }
    if(this->_handlerThreadPool->activeThreadCount()){
        this->_handlerThreadPool->waitForDone();
    }
}

void NGHttpServer::AbsServer::onNewSession(Session * session)
{
    qDebug() << "new session:" << session->requestSourceIp();
    if(this->_onRequestHandler){
        session->setOnRequestHandler([this](Session & session){
            QtConcurrent::run(this->_handlerThreadPool.data(),[this,&session](){
                this->_onRequestHandler(session);
            });
        });
    }
    connect(session,&QObject::destroyed,[this,session](){
        this->_mutex.lock();
        qDebug() << "delete session:"<<session->requestSourceIp();
        this->_availableSessions.remove(session);
        this->_mutex.unlock();
    });
    this->_availableSessions.insert(session);
}

NGHttpServer::HttpServer::HttpServer(const int &maxHandlerThread):AbsServer(maxHandlerThread)
{
    qDebug() << "HTTP server construct.";
}

NGHttpServer::HttpServer::~HttpServer()
{
    qDebug() << "Http server destruct";
    if(this->isRunning()){
        this->deinitialize();
    }
}

bool NGHttpServer::HttpServer::listen(const QHostAddress &address, const quint16 &port)
{
    qDebug() << "listen at" <<address.toString()<<":"<<port;
    this->_listenAddr = address;
    this->_listenPort = port;
    return this->initialize();//开启子线程，调用onStart，开启事件循环，直至事件循环结束，调用onFinish并返回。
}

bool NGHttpServer::HttpServer::isRunning()
{
    return this->_tcpServer != nullptr;
}

bool NGHttpServer::HttpServer::onStart()
{
    _mutex.lock();
    _tcpServer.reset(new QTcpServer());
    _mutex.unlock();
    connect(_tcpServer.get(),&QTcpServer::newConnection,[this](){
        QTcpSocket * socket = this->_tcpServer->nextPendingConnection();
        this->onNewSession(new Session(socket));
    });//当收到一个新的connection，获取其socket，封装成会话session，传入onNewSession()
    if(! this->_tcpServer->listen(this->_listenAddr,this->_listenPort)){//尝试listen，如果失败，释放指针
        _mutex.lock();
        _tcpServer.reset();
        _mutex.unlock();
        return false;
    }
    return true;
}

void NGHttpServer::HttpServer::onFinish()
{
    qDebug() << "HttpServer on Finish";
    _mutex.lock();
    _tcpServer->close();
    _tcpServer.reset();
    _mutex.unlock();
}

static QString replyTextFormat(
    "HTTP/1.1 %1 OK\r\n"
    "Content-Type: %2\r\n"
    "Content-Length: %3\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Headers: Content-Type,X-Requested-With\r\n"
    "\r\n"
    "%4"
);

static QString replyHTMLFormat(
    "HTTP/1.1 %1 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Content-Length: %2\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Headers: Content-Type,X-Requested-With\r\n"
    "\r\n"
);

static QString replyRedirectsFormat(
    "HTTP/1.1 %1 Redirect\r\n"
    "Content-Type: %2\r\n"
    "Location: %3\r\n"
    "\r\n"
    "%4"
);

static QString replyImageFormat(
    "HTTP/1.1 %1\r\n"
    "Content-Type: image/%2\r\n"
    "Content-Length: %3\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Headers: Content-Type,X-Requested-With\r\n"
    "\r\n"
);

static QString replyFileFormat(
    "HTTP/1.1 %1 OK\r\n"
    "Content-Disposition: attachment;filename=%2\r\n"
    "Content-Length: %3\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Headers: Content-Type,X-Requested-With\r\n"
    "\r\n"
);

QAtomicInt NGHttpServer::Session::_remainSessions = 0;

NGHttpServer::Session::Session(QTcpSocket * socket):_socket(socket),_timer(new QTimer())
{
    _remainSessions++;
    if ( qobject_cast< QAbstractSocket * >( socket ) )
    {
        _requestSourceIp = ( qobject_cast< QAbstractSocket * >( socket ) )->peerAddress().toString().replace( "::ffff:", "" );
    }
    connect(socket,&QTcpSocket::readyRead,[this](){
        this->_timer.stop();
        this->_receiveBuffer.append( this->_socket->readAll() );
        this->analyseBuffer();
        this->_timer.start();
    });
    connect(socket,&QTcpSocket::bytesWritten,this,&Session::onBytesWritten);
    connect(socket,&QTcpSocket::stateChanged,this,&Session::onStateChanged);
    _timer.setInterval( 30 * 1000 );
    _timer.setSingleShot( true );
    _timer.start();
    connect( &_timer, &QTimer::timeout, this, &QObject::deleteLater );
}

NGHttpServer::Session::~Session()
{
    _remainSessions--;
    if(_socket != nullptr) delete _socket;
}

void NGHttpServer::Session::setOnRequestHandler(const std::function<void (Session &)> &handler)
{
    this->_onRequestHandler = handler;
}

void NGHttpServer::Session::analyseBuffer()
{
    QString s = QString(this->_receiveBuffer);
    int bodystart = s.indexOf("\r\n\r\n");
    QStringList list = s.left(bodystart).split("\r\n");
//    for(auto line : list){
//        qDebug() << line;
//    }
    //请求行
    QString requestline = list[0];
    QStringList requestlinelist = requestline.split(" ");
    if(requestlinelist.size() != 3) goto ABORT;
    this->_requestMethod = requestlinelist[0];
    this->_requestMethod = this->_requestMethod.toUpper();
    if ( ( _requestMethod != "GET" ) &&
         ( _requestMethod != "OPTIONS" ) &&
         ( _requestMethod != "POST" ) &&
         ( _requestMethod != "PUT" ) ) goto ABORT;
    this->_requestUrl = QUrl::fromPercentEncoding(requestlinelist[1].toStdString().c_str());
    this->_requestCrlf = requestlinelist[2];

    try {
        for(int i=1;i<list.size()-1;i++){
            int pos = list[i].indexOf(":");
            QString k = list[i].left(pos);
            QString v = list[i].mid(pos+1).trimmed();
            this->_requestHeader[k] = v;
            if(k.toLower() == "content-length") this->_contentLength = v.toLongLong();
        }
        this->_requestBody += s.mid(bodystart+4).toStdString();
        if ( ( _contentLength != -1 ) && ( _requestBody.size() != _contentLength ) ) goto ABORT;
        _receiveBuffer.clear();
    }  catch (QException e) {
        goto ABORT;
    }
    //    for(auto it=this->_requestHeader.begin();it!=this->_requestHeader.end();it++){
    //        qDebug() << it.key() << " : " << it.value();
    //    }
    //    qDebug() << "BODY:";
    //    qDebug() << this->_requestBody;
    if(this->_onRequestHandler) this->_onRequestHandler(*this);
    return;
ABORT:
    this->deleteLater();
    return;
}


qint64 Session::contentLength() const
{
    return _contentLength;
}

void Session::responseText(const QString &replyData, const int &httpStatusCode)
{
    if ( QThread::currentThread() != this->thread() )
    {
       _replyHttpCode = httpStatusCode;
       _replyBodySize = replyData.toUtf8().size();

       QMetaObject::invokeMethod(
           this,
           "responseText",
           Qt::QueuedConnection,
           Q_ARG( QString, replyData ),
           Q_ARG( int, httpStatusCode ) );
       return;
    }

    const auto &&data = replyTextFormat
                       .arg(
                           QString::number( httpStatusCode ),
                           "text;charset=UTF-8",
                           QString::number( _replyBodySize ),
                           replyData )
                       .toUtf8();

    _waitWrittenByteCount = data.size();
    _socket->write( data );
}

void Session::responseHTML(const QString &filePath, const int &httpStatusCode)
{
    if ( QThread::currentThread() != this->thread() )
    {
        QMetaObject::invokeMethod(
                    this,
                    "responseHTML",
                    Qt::QueuedConnection,
                    Q_ARG( QString, filePath ),
                    Q_ARG( int, httpStatusCode )
                    );
        return;
    }
    this->_replyIoDevice.reset( new QFile( filePath ) );
    QPointer< QFile > file = ( qobject_cast< QFile * >( this->_replyIoDevice.data() ) );

    if ( !file->open( QIODevice::ReadOnly ) )
    {
        qDebug() << "NGHttpServer::Session::responseFile: open HTML error:" << filePath;
        this->_replyIoDevice.clear();
        this->deleteLater();
        return;
    }

    this->_replyBodySize = file->size();

    const auto &&data = replyHTMLFormat
                            .arg(
                                QString::number( httpStatusCode ),
                                QString::number( _replyBodySize ) )
                            .toUtf8();

    this->_waitWrittenByteCount = data.size() + file->size();
    this->_socket->write( data );
}

void Session::responseRedirect(const QString &redirectUrl, const int &httpStatusCode)
{
    if ( QThread::currentThread() != this->thread() )
    {
       QMetaObject::invokeMethod(
           this,
           "responseRedirect",
           Qt::QueuedConnection,
           Q_ARG( QString, redirectUrl ),
           Q_ARG( int, httpStatusCode ) );
       return;
    }
    switch(httpStatusCode){
    case 308:
    case 307:
    case 301:
    case 302:
    case 303:
        _replyHttpCode = httpStatusCode;
        break;
    default:
        _replyHttpCode = 308;
    }
   const auto &&data = replyRedirectsFormat
                       .arg(
                           QString::number( _replyHttpCode ),
                           "text;charset=UTF-8",
                           redirectUrl,
                           "" )
                       .toUtf8();

   _waitWrittenByteCount = data.size();
   _socket->write( data );
}

void Session::responseFile(const QString &filePath, const int &httpStatusCode)
{
    if ( QThread::currentThread() != this->thread() )
    {
        QMetaObject::invokeMethod(
                    this,
                    "responseFile",
                    Qt::QueuedConnection,
                    Q_ARG( QString, filePath ),
                    Q_ARG( int, httpStatusCode )
                    );
        return;
    }
    this->_replyIoDevice.reset( new QFile( filePath ) );
    QPointer< QFile > file = ( qobject_cast< QFile * >( this->_replyIoDevice.data() ) );

    if ( !file->open( QIODevice::ReadOnly ) )
    {
        qDebug() << "NGHttpServer::Session::responseFile: open file error:" << filePath;
        this->_replyIoDevice.clear();
        this->deleteLater();
        return;
    }

    this->_replyBodySize = file->size();

    const auto &&data = replyFileFormat
                            .arg(
                                QString::number( httpStatusCode ),
                                QFileInfo( filePath ).fileName(),
                                QString::number( _replyBodySize ) )
                            .toUtf8();

    this->_waitWrittenByteCount = data.size() + file->size();
    this->_socket->write( data );
}

void Session::responseImage(const QString &filePath, const int &httpStatusCode)
{
    if ( QThread::currentThread() != this->thread() )
    {
        QMetaObject::invokeMethod(
                    this,
                    "responseImage",
                    Qt::QueuedConnection,
                    Q_ARG( QString, filePath ),
                    Q_ARG( int, httpStatusCode )
                    );
        return;
    }
    this->_replyIoDevice.reset( new QFile( filePath ) );
    QPointer< QFile > file = ( qobject_cast< QFile * >( this->_replyIoDevice.data() ) );

    if ( !file->open( QIODevice::ReadOnly ) )
    {
        qDebug() << "NGHttpServer::Session::responseImage: open image error:" << filePath;
        this->_replyIoDevice.clear();
        this->deleteLater();
        return;
    }

    this->_replyBodySize = file->size();

    const auto &&data = replyImageFormat
                .arg(
                QString::number( httpStatusCode ),
                QFileInfo( filePath ).suffix(),
                QString::number( _replyBodySize ) )
                .toUtf8();

    this->_waitWrittenByteCount = data.size() + file->size();
    this->_socket->write( data );
}

const QMap<QString, QString> &Session::requestHeader() const
{
    return _requestHeader;
}



const QByteArray &Session::requestBody() const
{
    return _requestBody;
}



const QString &Session::requestCrlf() const
{
    return _requestCrlf;
}



const QString &Session::requestUrl() const
{
    return _requestUrl;
}



const QString &Session::requestMethod() const
{
    return _requestMethod;
}


const QString &Session::requestSourceIp() const
{
    return _requestSourceIp;
}

void NGHttpServer::Session::onBytesWritten(const qint64 &written)
{
    if(this->_waitWrittenByteCount < 0) return; //无数据发送，直接返回
    this->_timer.stop();
    this->_waitWrittenByteCount -= written; //减去已发送的数据
    if(this->_waitWrittenByteCount <= 0){   //如果已发送完毕
        this->_waitWrittenByteCount = 0;
        this->_socket->disconnectFromHost();
        return;
    }
    if(!this->_replyIoDevice.isNull()){ //如果未发送完，且有要发送的文件
        if(this->_replyIoDevice->atEnd()){ //文件已发送完
            this->_replyIoDevice->deleteLater();
            this->_replyIoDevice.clear();
        }else{  //文件未发送完
            this->_socket->write(_replyIoDevice->read(1024*1024));
        }
    }
    this->_timer.start();
}

void NGHttpServer::Session::onStateChanged(const QAbstractSocket::SocketState &socketState)
{

}


}
