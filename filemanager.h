#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QList>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <iostream>

class FileManager
{
public:
    FileManager();
    static QList<QFileInfo> ls(QDir::SortFlag = QDir::Time);
    static QList<QFileInfo> ls(QDir dir = QDir(""),QDir::SortFlag = QDir::Time);
    static QList<QDir> subDir(QDir curDir = QDir(""),QDir::SortFlag sort = QDir::Time);
    static QString read(QString filename);
    static void write(QString filename,QString content);
    static void ensureDir(QString dirPath);
    static void ensureDir(std::string dirPath);
    static bool checkFileExist(QString filename);
    static QDateTime getFileModifiedTime(QString filename);
};

#endif // FILEMANAGER_H
