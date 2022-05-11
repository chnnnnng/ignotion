#ifndef PAGE_H
#define PAGE_H

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
#include "directory.h"
#include "MdParser.hpp"

using namespace std;

class Page
{
private:
    const std::vector<std::pair<std::string,std::function<std::string()>>> TEMPLATE_PLACEHOLDERS = {
            std::pair<string,function<string()>>("{{ __CONTENT__ }}",[&](){
        return this->content.toStdString();
    }),
            std::pair<string,function<string()>>("{{ __PAGE_NAME__ }}",[&](){
        return this->filenameNoPath.toStdString();
    }),
            std::pair<string,function<string()>>("{{ __PAGE_TIME__ }}",[&](){
        return this->time.toString("yyyy-MM-dd hh:mm:ss").toStdString();
    }),
            std::pair<string,function<string()>>("{{ __SITE_NAME__ }}",[](){
        return Config::getInstance().siteName;
    }),
            std::pair<string,function<string()>>("{{ __SITE_DESCRIPTION__ }}",[](){
        return Config::getInstance().siteDesciption;
    }),
            std::pair<string,function<string()>>("{{ __SITE_COPYRIGHT__ }}",[](){
        return Config::getInstance().siteCopyright;
    }),
            std::pair<string,function<string()>>("{{ __AUTHOR_NAME__ }}",[](){
        return Config::getInstance().authorName;
    }),
            std::pair<string,function<string()>>("{{ __AUTHOR_EMAIL__ }}",[](){
        return Config::getInstance().authorEmail;
    }),
            std::pair<string,function<string()>>("{{ __AUTHOR_INTRODUCTION__ }}",[](){
        return Config::getInstance().authorIntroduction;
    }),
            std::pair<string,function<string()>>("{{ __ROOT__ }}",[this](){ //输出文件目录，如HTML
        return rootReletiveToOutputDir.toStdString();
    }),
            std::pair<string,function<string()>>("{{ __RES__ }}",[this](){ //输出资源目录，如RES
        return "../"+rootReletiveToOutputDir.toStdString() + Config::getInstance().resourceDir+"/";
    }),
            std::pair<string,function<string()>>("{{ __NAVIGATION__ }}",[this](){ //
        QString n;
        n.append("<div id=\"ignotion_navigation\">");
        IgnotionDir(QDir("")).traverse([&](IgnotionDir * igdir,int depth){
            QDir dir = igdir->dir;
            QList<QFileInfo> list = FileManager::ls(dir,QDir::Time);
            if(list.empty()){
                return;
            }
            n.append("<div class=\"ignotion_navigation_sub\">");
            n.append("<span class=\"ignotion_navigation_category_name\">");
            QString tab;
            for(int i=0;i<depth-1;i++) tab.append("&emsp;");
            n.append(tab);
            n.append(igdir->displayName+"</span>");
            for(int i = 0; i < list.count(); i++)
            {
                Page p(list.at(i).filePath(),igdir);
                n.append("<a href='");
                n.append("./"+this->rootReletiveToOutputDir+p.outputRoutePath);
                n.append("'>");
                n.append(tab);
                n.append(p.getTitle());
                n.append("</a><br>");
            }
            n.append("</div>");
        });
        n.append("</div>");
        return n.toStdString();
    }),
            std::pair<string,function<string()>>("{{ __TITLE__ }}",[&](){
        return this->getTitle().toStdString();
    }),
};
private:
    QString filename;
    QString filenameNoPath;
    QDateTime time;
    QString markdown;
    QString frontmatter;
    QString title;
    QString templateName;
    QString content;
    IgnotionDir * dir;
    unique_ptr<MdParser> mdparser;
    QString renderTemplate(const QString & temp);
    QString outputDir; //输出文件路径，用于文件检索，如 HTML/articles/aaa
    QString outputRoutePath; //输出文件陆游路径，如 articles/aaa/hello.html
    QString rootReletiveToOutputDir;
public:
    Page(QString filename, IgnotionDir * dir);
    QString translate(bool fource = false);
    bool isTranslated();    //判断Page是否翻译
    bool isUploaded();      //判断Page是否上传
    static void copyStyleCss();
    const QDateTime &getTime() const;
    const QString getTimeWithFilename() const;
    const QString &getTitle() const;
};

#endif // PAGE_H
