#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QPointer>
#include <QDebug>
#include <QHostAddress>
#include <QThreadPool>
#include <QSemaphore>
#include <QMutex>
#include <QtConcurrent>
#include <QEventLoop>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSharedPointer>

#include <functional>
#include <memory>

namespace NGHttpServer{


class Session : public QObject{
    Q_OBJECT
    Q_DISABLE_COPY( Session )
public:

    Session(QTcpSocket * socket);
    ~Session();
    void setOnRequestHandler(const std::function<void(Session &)> & handler);
    const QString &requestSourceIp() const;
    const QString &requestMethod() const;
    const QString &requestUrl() const;
    const QString &requestCrlf() const;
    const QByteArray &requestBody() const;
    const QMap<QString, QString> &requestHeader() const;
    qint64 contentLength() const;
public slots:
    void responseText(const QString &replyData, const int &httpStatusCode = 200);
    void responseHTML(const QString &filePath, const int &httpStatusCode = 200);
    void responseRedirect(const QString &redirectUrl, const int &httpStatusCode = 308);
    void responseFile(const QString &filePath, const int &httpStatusCode = 200);
    void responseImage(const QString &filePath, const int &httpStatusCode = 200);
private:
    void analyseBuffer();
private:
    std::function<void(Session &)> _onRequestHandler;
    QTcpSocket * _socket;
    QTimer _timer;
    static QAtomicInt _remainSessions;
    QByteArray               _receiveBuffer;
    QString                  _requestSourceIp;
    QString                  _requestMethod;
    QString                  _requestUrl;
    QString                  _requestCrlf;
    QByteArray               _requestBody;
    QMap< QString, QString > _requestHeader;
    bool   _headerAcceptedFinished  = false;
    bool   _contentAcceptedFinished = false;
    qint64 _contentLength           = -1;
    int        _replyHttpCode = -1;
    QByteArray _replyBuffer;
    qint64     _replyBodySize = -1;
    qint64                      _waitWrittenByteCount = -1;
    QSharedPointer< QIODevice > _replyIoDevice;
private slots:
    void onBytesWritten(const qint64 &written);
    void onStateChanged(const QAbstractSocket::SocketState &socketState);
};

class AbsServer : public QObject{
    Q_OBJECT
    Q_DISABLE_COPY( AbsServer )
public:
    AbsServer(const int &maxHandlerThread);
    virtual ~AbsServer() override;
    void setOnRequestHandler(const std::function< void(Session &) > & onRequestHandler);
protected:
    bool initialize();
    void deinitialize();
    void onNewSession(Session * session);
    virtual bool isRunning() = 0;
    virtual bool onStart() = 0;
    virtual void onFinish() = 0;
protected:
    std::function<void(Session &)> _onRequestHandler;
    QSharedPointer<QThreadPool> _serverThreadPool;
    QSharedPointer<QThreadPool> _handlerThreadPool;
    QMutex _mutex;
    QSet< Session * > _availableSessions;
signals:
    void readyToClose();
};

class HttpServer : public AbsServer
{
    Q_OBJECT
    Q_DISABLE_COPY( HttpServer )
public:
    HttpServer(const int &maxHandlerThread);
    ~HttpServer();
    bool listen( const QHostAddress &address, const quint16 &port );
private:
    bool isRunning();
    bool onStart();
    void onFinish();
private:
    QHostAddress _listenAddr;
    quint16 _listenPort;
    std::unique_ptr<QTcpServer> _tcpServer;
signals:

};


};

#endif // HTTPSERVER_H
