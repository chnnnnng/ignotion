#include "config.h"

Config::Config()
{
    try{
        config = YAML::LoadFile(CONFIG_YAML_NAME);
    }catch(YAML::BadFile &e){
        std::cout<<"当前目录不是一个Ignition工作目录。"<<std::endl;
        return;
    }
    this->siteName = config["SITE_NAME"].as<std::string>();
    this->siteDesciption = config["SITE_DESCRIPTION"].as<std::string>();
    this->siteCopyright = config["SITE_COPYRIGHT"].as<std::string>();

    this->authorName = config["AUTHOR_NAME"].as<std::string>();
    this->authorEmail = config["AUTHOR_EMAIL"].as<std::string>();
    this->authorIntroduction = config["AUTHOR_INTRODUCTION"].as<std::string>();

    this->theme = config["THEME"].as<std::string>();

    this->outputDir = config["OUTPUT_DIR"].as<std::string>();
    this->resourceDir = config["RESOURCE_DIR"].as<std::string>();

    this->remoteUserAtAddr = config["REMOTE_USER_AT_ADDR"].as<std::string>();
    this->remoteWS = config["REMOTE_WS"].as<std::string>();
    this->remotePrivateKey = config["REMOTE_PRIVATE_KEY"].as<std::string>();
}


bool Config::checkWorkspace(){
    QFileInfo fileInfo(CONFIG_YAML_NAME);
    return fileInfo.isFile();
}

QDateTime Config::lastUpdateTime(){
    return QFileInfo(CONFIG_YAML_NAME).lastModified();
}
