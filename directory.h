#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <QString>
#include <vector>
#include <string>
#include <functional>
#include <QList>
#include "config.h"
#include "filemanager.h"
#include "yaml-cpp/parser.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"

using namespace std;

class IgnotionDir{
public:
    QString displayName;
    QString routeName;
    bool isRoot{false};
    QDir dir;
    QList<shared_ptr<IgnotionDir>> sub;
    IgnotionDir * up{nullptr};
public:
    IgnotionDir(){}
    IgnotionDir(QDir dir);
    void traverse(function<void(IgnotionDir*,int depth)>,int depth=0);
    static bool checkDirectory();
};

#endif // DIRECTORY_H
