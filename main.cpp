#include <QCoreApplication>
#include <QFile>
#include <iostream>
#include <QCommandLineParser>
#include <QList>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include "filemanager.h"
#include "config.h"
#include "page.h"
#include "directory.h"

//#include "MdParser.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Ignotion");
    bool ignotionWorkspace = Config::checkWorkspace();
    bool ignotionDirectory = IgnotionDir::checkDirectory();
    if(!ignotionWorkspace){
        if(ignotionDirectory){
            cout << "# 当前位于Ignotion工作子目录，请在Ignotion工作根目录使用ignotion指令。" << endl;
            //return 0;
        }
        cout << "# 当前工作目录不是一个Ignotion工作空间" << endl;
        cout << "# 输入指令 --init-ws  将当前目录初始化为Ignotion工作目录" << endl;
        cout << "# 输入指令 --make-dir 将当前目录作为Ignotion工作子目录" << endl;
        //return 0;
    }


//    IgnotionDir rootIgnotionDir(QDir(""));
//    Page page(QString("test.md"),&rootIgnotionDir);
//    MdParser mp(page.markdown);
//    mp.show();
//    QString outputFile = mp.html();
//    FileManager::write("test.html",outputFile);
//    return 0;


    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser parser;
    parser.setApplicationDescription("A easy-for-use Markdown Blog");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
                          {{"l", "list"},"list all pages in workspace."},
                          {"name-order","list in name order"},
                          {{"n","new"},"create a new page from template.","template","default.md"},
                          {"template","specify a template md file.","template","default"},
                          {{"t", "translate"},"translate a specific page.","page","no-page-given"},
                          {{"u", "upload"},"upload all pages."},
                          {"clear","clear all generated HTML files."},
                          {"init-ws", "initialize current directory as Ignotion workspace"},
                          {"make-dir","make current directory an Ignotion sub-directory"}
                      });
    parser.process(a);

    /*  删除所有的生成文件
    *   用法：
    *   ignotion --clear
    */
    if(ignotionWorkspace && parser.isSet("clear")){
        QString dir{Config::getInstance().outputDir.c_str()};
        QDir(dir).removeRecursively();
        cout << "Remove ./"+dir.toStdString()+"/* successfully."<<endl<<endl;
    }
    /*  列出当前工作区下的所有markdown文档
    *   用法：
    *   ignotion -l 列出所有文档，按时间顺序
    *   ignotion --list 同上
    *   ignotion -l --name-order 按文件名排序
    *   ignotion --list --name-order 同上
    */
    if(ignotionWorkspace && parser.isSet("list")){
        QDir::SortFlag sort = QDir::Time;
        if(parser.isSet("name-order")) sort = QDir::Name;

        cout << endl;
        cout << "\tUPLD\tTRAN\ttime\t\tname"<<endl;
        cout << "\t===================================================================================="<<endl;
        IgnotionDir rootIgnotionDir(QDir(""));
        rootIgnotionDir.traverse([&](IgnotionDir * igdir,int){
            QDir dir = igdir->dir;
            QList<QFileInfo> list = FileManager::ls(dir,sort);
            if(list.empty()){
                return;
            }    
            if(! igdir->isRoot){
                cout << "<"+igdir->displayName.toStdString()+">"<<endl;
            }
            for(int i = 0; i < list.count(); i++)
            {
                cout <<"\t";
                Page p(list.at(i).filePath(),igdir);
                if(p.isUploaded()){
                    cout << "[√]\t";
                }else{
                    cout << "[ ]\t";
                }
                if(p.isTranslated()){
                    cout << "[√]\t";
                }else{
                    cout << "[ ]\t";
                }
                cout << p.getTime().toString("yyMMdd hh:mm").toStdString() <<" |\t"<<p.getTimeWithFilename().toStdString() << endl;
            }
        });
        cout << "\t====================================================================================\n"<<endl;
        return 0;
    }
    /*  解析markdown文档至HTML文档
    *   用法：
    *   ignotion [-t | --translate] <name> 解析指定文档
    *   ignotion [-t | --translate] [all | a | -a | --all] 解析所有文档
    *   注意：
    *   参数中的name要写.md后缀，有无详细路径皆可，只做字符串结尾匹配，若有多个重名文件将都会解析
    */
    if(ignotionWorkspace && parser.isSet("translate")){
        cout << endl;
        QString filename = parser.value("translate");
        IgnotionDir rootIgnotionDir(QDir(""));
        rootIgnotionDir.traverse([&](IgnotionDir * igdir,int){//遍历每一级目录
            QList<QFileInfo> list = FileManager::ls(igdir->dir,QDir::Time);
            for(int i = 0; i < list.count(); i++)//遍历目录中每一个文件
            {
                if(filename == "all" || filename == "a" || filename == "-a"|| filename == "--all"){
                    Page page(list.at(i).filePath(),igdir);
                    cout << page.translate(igdir->isRoot).toStdString() << endl;
                }else{
                    if(filename.endsWith(list.at(i).fileName())){
                        Page page(list.at(i).filePath(),igdir);
                        cout << page.translate(true).toStdString() << endl;
                    }
                }

            }

        });
        Page::copyStyleCss();
        cout << endl;
        //return 0;
    }

    /*  新建文件
    *   用法：
    *   ignotion [ -n | --new ] <name>     使用默认模版新建name文件
    *   ignotion [ -n | --new] <name> --template <template_name>    使用template_name模版创建name文件
    *   注：
    *   模板文件存放于主题文件夹下，默认模板文件为page.md
    *   参数中的name和template_name均不需要写.md后缀
    */
    if(ignotionWorkspace && parser.isSet("new")){
        cout << endl;
        QString filename = parser.value("new");
        if(FileManager::checkFileExist(filename+".md")){
            cout << filename.toStdString() + ".md already exists, failed to create."<<endl<<endl;
            return 0;
        }
        QString templateFilename = "page";
        if(parser.isSet("template")){
            templateFilename = parser.value("template");
        }
        templateFilename =QString( Config::getInstance().resourceDir.c_str())+"/theme/"+  QString(Config::getInstance().theme.c_str())+"/"+templateFilename + ".md";
        if(FileManager::checkFileExist(templateFilename)){
            FileManager::write(filename+".md",FileManager::read(templateFilename));
            cout << "Create new file "+ filename.toStdString() + ".md by template " + templateFilename.toStdString()<<endl<<endl;
        }else{
            cout << templateFilename.toStdString() + " is not found."<<endl<<endl;
        }
        return 0;
    }
    /*  上传文件
    *   用法：
    *   ignotion -u
    *   ignotion --uplaod
    */
    if(ignotionWorkspace && parser.isSet("upload")){
        string cmd = "";
        cmd = "ssh "+Config::getInstance().remoteUserAtAddr+" -i "+Config::getInstance().remotePrivateKey+" \"rm -rf "+Config::getInstance().remoteWS+"/*\"";
        system(cmd.c_str());
        cmd = "scp -i "+Config::getInstance().remotePrivateKey+" -r "+Config::getInstance().outputDir+" "+Config::getInstance().remoteUserAtAddr+":"+Config::getInstance().remoteWS;
        system(cmd.c_str());
        cmd = "scp -i "+Config::getInstance().remotePrivateKey+" -r "+Config::getInstance().resourceDir+" "+Config::getInstance().remoteUserAtAddr+":"+Config::getInstance().remoteWS;
        system(cmd.c_str());
        system("touch config.yaml");
        return 0;
    }

    if(!ignotionWorkspace && !ignotionDirectory && parser.isSet("init-ws")){
        qDebug() << "init";
        return 0;
    }

    if(!ignotionWorkspace && parser.isSet("make-dir")){
        cout << "enter the display name for current directory. (empty for default) : ";
        string displayname;
        getline(cin,displayname);
        if(displayname==""){
            displayname = QDir::current().dirName().toStdString();
        }
        cout << "enter the route name for current directory. (empty for default) : ";
        string routename;
        getline(cin,routename);
        if(routename==""){
            routename = QDir::current().dirName().toStdString();
        }
        QString filecontent = QString("# Ignotion 工作目录\
\n# Ignotion的文章存放在目录中，工作空间是根目录。 通过合理安排目录，你可以让你的文章井然有序！\
\n\n# 一个目录具有三个名字，分别为：\
\n#   - Directory name，即文件夹真实的名字\
\n#   - Display name, 即在文章或导航栏中显示的名字\
\n#   - Route name，即在URL中路由时使用的名字\
\n\n# 对于根目录，仅Display name有效\
\n# 若根目录的 .ignotion_dir文件缺省，将使用 \"/\" 作为Display name\
\n\n# 以下为.ignotion_dir内容\
\n\nDIR_DISPLAY_NAME: %1\n\nDIR_ROUTE_NAME: %2").arg(QString(displayname.c_str())).arg(QString(routename.c_str()));
        FileManager::write(".ignotion_dir",filecontent);
        cout << "Current directory "+QDir("").dirName().toStdString()+" is now an Ignotion Dir\nuse <-n> to create a new page.";
        return 0;
    }


    return 0;
    //return a.exec();
}
