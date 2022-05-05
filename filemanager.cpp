#include "filemanager.h"

FileManager::FileManager()
{

}


QList<QFileInfo> FileManager::ls(QDir::SortFlag sort){
    QDir *dir = new QDir("");
    QStringList filters;
    filters << "*.md";         //过滤器，过滤你想要的文件后缀
    //用链表保存过滤的文件名
    return  QList<QFileInfo>(dir->entryInfoList(filters,QDir::Files,sort));
}

QList<QFileInfo> FileManager::ls(QDir dir,QDir::SortFlag sort){
    QStringList filters;
    filters << "*.md";         //过滤器，过滤你想要的文件后缀
    //用链表保存过滤的文件名
    return  QList<QFileInfo>(dir.entryInfoList(filters,QDir::Files,sort));
}


QList<QDir> FileManager::subDir(QDir curDir,QDir::SortFlag sort)
{
    QList<QFileInfo> list = (curDir.entryInfoList(QDir::Dirs|QDir::NoDot|QDir::NoDotDot,sort));
    QList<QDir> res;
    for(int i = 0; i < list.count(); i++){
        QDir dir(list.at(i).filePath());
        if(QFileInfo(dir,".ignotion_dir").exists())
            res.append(dir);
    }
    return res;
}

QString FileManager::read(QString filename){
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        throw std::invalid_argument("File to read not Found.");
    }
    QString s = file.readAll();
    file.close();
    return s;
}


void FileManager::write(QString filename,QString content){
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        throw std::invalid_argument("File to write not Found.");
    }
    file.write(content.toStdString().c_str());
    file.close();
}


void FileManager::ensureDir(QString dirPath){
    QDir dir(dirPath);
    if(!dir.exists()){
        QDir().mkpath(dirPath);
    }
}

void FileManager::ensureDir(std::string dirPath){
    ensureDir(QString(dirPath.c_str()));
}


bool FileManager::checkFileExist(QString filename){
    QFile file(filename);
    return file.exists();
}


QDateTime FileManager::getFileModifiedTime(QString filename){
    return QFileInfo(filename).lastModified();
}
