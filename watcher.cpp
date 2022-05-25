#include "watcher.h"

Watcher::Watcher(QObject *parent)
    : QThread{parent}
{

}

void Watcher::run()
{
    while(1){
        qDebug() << "Ignotion watcher's running...";
        IgnotionDir rootIgnotionDir(QDir(""));
        bool changes = false;
        rootIgnotionDir.traverse([&](IgnotionDir * igdir,int){//遍历每一级目录
            QList<QFileInfo> list = FileManager::ls(igdir->dir,QDir::Time);
            for(int i = 0; i < list.count(); i++)//遍历目录中每一个文件
            {
                Page page(list.at(i).filePath(),igdir);
                if(! page.isTranslated()){
                    qDebug() << "Detect changed on" << page.getTitle();
                    qDebug() << page.translate(true);
                    changes = true;
                }
            }

        });
        if(changes) Page::copyStatic();
        msleep(3000);
    }
}
