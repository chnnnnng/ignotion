#include "page.h"

Page::Page(QString filename, IgnotionDir * dir)
{
    this->filename = filename;
    this->filenameNoPath = QFileInfo(filename).fileName().chopped(3);
    this->time = FileManager::getFileModifiedTime(filename);
    this->markdown = FileManager::read(filename);
    this->mdparser.reset(new MdParser(markdown));

    this->frontmatter = this->mdparser->frontMatter();
    YAML::Node node = YAML::Load(frontmatter.toStdString());
    if (node["title"]) {
      this->title = QString(node["title"].as<std::string>().c_str());
    }
    if (node["template"]) {
      this->templateName = QString(node["template"].as<std::string>().c_str());
    }

    this->dir = dir;
    IgnotionDir * p = this->dir;
    while(! p->isRoot){
        outputDir.prepend("/"+p->routeName);
        rootReletiveToOutputDir.prepend("../");
        p = p->up;
    }
    outputRoutePath = outputDir + "/"+filenameNoPath + ".html";
    if(outputRoutePath.startsWith("/")) outputRoutePath = outputRoutePath.mid(1);
    outputDir.prepend(QString(Config::getInstance().outputDir.c_str()));


}

QString Page::translate(bool force){
    if(isTranslated() && !force){
        return "Translation skipped,"+ outputDir+"/"+filenameNoPath+".html is the latest version.";
    }
    content = this->mdparser->parse()->html();
    if(this->templateName.isEmpty()) this->templateName = "page.html";
    else if(!this->templateName.endsWith(".html")) this->templateName.append(".html");
    QString outputFile = renderTemplate(this->templateName);
    FileManager::ensureDir(outputDir);
    FileManager::write(outputDir+"/"+filenameNoPath+".html",outputFile);
    return "Translation succeeded. Generate new file at "+ outputDir+"/"+filenameNoPath+".html";
}

void Page::copyStyleCss()
{
    FileManager::write(QString(Config::getInstance().outputDir.c_str()).append("/style.css"),FileManager::read(QString(Config::getInstance().resourceDir.c_str()) + "/theme/" + QString(Config::getInstance().theme.c_str()) + "/style.css"));
}

bool Page::isTranslated(){
    QString outputFileName = outputDir+"/"+filenameNoPath+".html";
    if(FileManager::checkFileExist(outputFileName) && FileManager::getFileModifiedTime(outputFileName) >= FileManager::getFileModifiedTime(filename)){
        return true;
    }else{
        return false;
    }
}


bool Page::isUploaded(){
    QDateTime lastUploadTime = Config::lastUpdateTime();
    QDateTime fileModifiedTime = FileManager::getFileModifiedTime(filename);
    return lastUploadTime > fileModifiedTime;
}


const QDateTime &Page::getTime() const
{
    return time;
}

const QString Page::getTimeWithFilename() const
{
    return title.isEmpty()?filenameNoPath:(title+"("+filenameNoPath+")");
}

const QString &Page::getTitle() const
{
    return title.isEmpty()?filenameNoPath:title;
}

QString Page::renderTemplate(const QString &temp)
{
    QString out;
    try {
         out = FileManager::read(QString(Config::getInstance().resourceDir.c_str())+"/theme/" + QString(Config::getInstance().theme.c_str()) + "/"+temp);
    }  catch (exception e) {
         out = "{{ __CONTENT__ }}";
    }
    for(auto& entry : TEMPLATE_PLACEHOLDERS){
        out.replace(QString(entry.first.c_str()),QString(entry.second().c_str()));
    }
    return out;
}
