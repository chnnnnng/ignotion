#ifndef CONFIG_H
#define CONFIG_H

#include "yaml-cpp/parser.h"
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <QFileInfo>
#include <string>

#define CONFIG_YAML_NAME "config.yaml"

class Config
{
private:
    YAML::Node config;
public:
    std::string siteName;
    std::string siteDesciption;
    std::string siteCopyright;
    std::string authorName;
    std::string authorEmail;
    std::string authorIntroduction;
    std::string theme;
    std::string outputDir;
    std::string resourceDir;
    std::string remoteUserAtAddr;
    std::string remoteWS;
    std::string remotePrivateKey;
public:
    static Config& getInstance()
        {
        static Config instance;
        return instance;
    }
    static bool checkWorkspace(); //检查当前工作路径是否是一个ignition工作空间
    static QDateTime lastUpdateTime();
    Config();

};

#endif // CONFIG_H
