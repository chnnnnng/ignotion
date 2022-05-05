#include "directory.h"


IgnotionDir::IgnotionDir(QDir dir){
    this->dir = dir;
    if(dir.exists("config.yaml")){//如果是工作空间根目录
        this->routeName = "";
        this->isRoot = true;
        try{
            YAML::Node ignotion_dir_yaml = YAML::LoadFile(this->dir.filePath(".ignotion_dir").toStdString());
            this->displayName = QString(ignotion_dir_yaml["DIR_DISPLAY_NAME"].as<std::string>().c_str());
        }catch(YAML::BadFile &e){
            this->displayName = "/";
        }
    }else{
        try{
            YAML::Node ignotion_dir_yaml = YAML::LoadFile(this->dir.filePath(".ignotion_dir").toStdString());
            this->displayName = QString(ignotion_dir_yaml["DIR_DISPLAY_NAME"].as<std::string>().c_str());
            this->routeName = QString(ignotion_dir_yaml["DIR_ROUTE_NAME"].as<std::string>().c_str());
        }catch(YAML::BadFile &e){
            std::cout<<"当前目录"<<this->dir.dirName().toStdString()<<" 不是一个Ignition目录。"<<std::endl;
            return;
        }
    }
    QList<QDir> list = FileManager::subDir(this->dir);
    for(auto& s : list){
        shared_ptr<IgnotionDir> nIgnotionDir(new IgnotionDir(s));
        nIgnotionDir->up = this;
        sub.append(nIgnotionDir);
    }
}


void IgnotionDir::traverse(function<void(IgnotionDir*,int)> f,int depth){
    f(this,depth);
    for(auto& ch : sub){
        ch->traverse(f,depth+1);
    }
}

bool IgnotionDir::checkDirectory()
{
    QFileInfo fileInfo(".ignotion_dir");
    return fileInfo.isFile();
}

