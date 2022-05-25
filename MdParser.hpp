#ifndef MDPARSER_H
#define MDPARSER_H
#include <string>
#include <QString>
#include <QStringList>
#include <vector>
#include <iostream>
#include <QDebug>
using namespace std;

// 词法关键字枚举
enum TAG_TPYE{
    nul             = 0,
    paragraph       = 1,
    href            = 2,
    ul              = 3,
    ol              = 4,
    li              = 5,
    em              = 6,
    strong          = 7,
    hr              = 8,
    br              = 9,
    image           = 10,
    quote           = 11,
    h1              = 12,
    h2              = 13,
    h3              = 14,
    h4              = 15,
    h5              = 16,
    h6              = 17,
    blockcode       = 18,
    code            = 19,
    task            = 20,
    root            = 21
};



class AbsParser;

class MdNode{
public:
    QString md;
    TAG_TPYE type;
    QString html;
    std::vector<MdNode> children;
    QString appendix;
private:
    std::shared_ptr<AbsParser> parser;
public:
    MdNode();
    MdNode(const QString & md);
    MdNode(const QString & md, TAG_TPYE type);
    void parseNode(bool force = false);
    void show();
    QString getPlainText();
    QString getHTML();
};

class MdParser{
private:
    QString md;
    QString frontmatter;
    std::unique_ptr<MdNode> node;
public:
    MdParser(const QString & md);
    MdParser(const std::string & md);
    MdParser * parse();
    void parseFrontMatter();
    void show();
    QString html();
    QString frontMatter();
    QString TOC();
};

class AbsParser{
public:
    QString md;
    bool ready = false;
public:
    virtual bool push_line(const QString & line) = 0;
    virtual void parse(MdNode*) = 0;
    virtual ~AbsParser(){};
};

class H1Parser : public AbsParser {
public:
    H1Parser(){
        //qDebug() << "H1Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("# "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(2),h1));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};
class H2Parser : public AbsParser {
public:
    H2Parser(){
        //qDebug() << "H2Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("## "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(3),h2));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};
class H3Parser : public AbsParser {
public:
    H3Parser(){
        //qDebug() << "H3Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("### "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(4),h3));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};
class H4Parser : public AbsParser {
public:
    H4Parser(){
        //qDebug() << "H4Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("#### "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(5),h4));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};
class H5Parser : public AbsParser {
public:
    H5Parser(){
        //qDebug() << "H5Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("##### "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(6),h5));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};
class H6Parser : public AbsParser {
public:
    H6Parser(){
        //qDebug() << "H6Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != code && type != blockcode && firstline.startsWith("###### "))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md.mid(7),h6));
        node->children.back().appendix = node->children.back().getPlainText();
    }
};

class LineParser : public AbsParser {
public:
    LineParser(){
        //qDebug() << "Line parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type == code || type == blockcode || type == quote) return false;
        if(firstline.startsWith("---")){
            for(QChar c : firstline){
                if(c != '-') return false;
            }
            return true;
        }
        if(firstline.startsWith("___")){
            for(QChar c : firstline){
                if(c != '_') return false;
            }
            return true;
        }
        if(firstline.startsWith("***")){
            for(QChar c : firstline){
                if(c != '*') return false;
            }
            return true;
        }
        return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode("",hr));
    }
};

class TOCParser : public AbsParser {
public:
    TOCParser(){
        //qDebug() << "TOC parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(firstline=="[TOC]" && type == root) return true;
        return false;
    }
    bool push_line(const QString &){
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        MdNode t("",nul);
        t.html = "{{ __TOC__ }}";
        node->children.push_back(t);
    }
};

class StrongParser : public AbsParser {
public:
    StrongParser(){
        //qDebug() << "Strong Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != root && type != code && type != blockcode && firstline.contains("**"))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        int i=0,j=0;
        while(j != -1){
            j = md.indexOf("**",i);
            MdNode node1(md.mid(i,j-i),nul);
            node1.parseNode(true);
            //node1.html = node1.md;
            node->children.push_back(node1);
            i=j+2;
            j = md.indexOf("**",i);
            MdNode node2(md.mid(i,j-i),strong);
            node->children.push_back(node2);
            i = j+2;
            j = md.indexOf("**",i);
        }
        MdNode node3(md.mid(i),nul);
        node3.parseNode(true);
        node->children.push_back(node3);
    }
};

class ItalicParser : public AbsParser {
public:
    ItalicParser(){
        //qDebug() << "Italic Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != root && type != code && type != blockcode && firstline.contains("*"))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        int i=0,j=0;
        while(j != -1){
            j = md.indexOf("*",i);
            MdNode node1(md.mid(i,j-i),nul);
            //node1.html = node1.md;
            node1.parseNode(true);
            node->children.push_back(node1);
            i=j+1;
            j = md.indexOf("*",i);
            MdNode node2(md.mid(i,j-i),em);
            node->children.push_back(node2);
            i = j+1;
            j = md.indexOf("*",i);
        }
        MdNode node3(md.mid(i),nul);
        //node3.html = node3.md;
        node3.parseNode(true);
        node->children.push_back(node3);
    }
};

class InlineCodeParser : public AbsParser {
public:
    InlineCodeParser(){
        //qDebug() << "Inline Code Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != root && type != code && type != blockcode && firstline.contains("`"))return true;
        else return false;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        int i=0,j=0;
        while(j != -1){
            j = md.indexOf("`",i);
            MdNode node1(md.mid(i,j-i),nul);
            node1.parseNode(true);
            //node1.html = node1.md;
            node->children.push_back(node1);
            i=j+1;
            j = md.indexOf("`",i);
            MdNode node2(md.mid(i,j-i),code);
            node->children.push_back(node2);
            i = j+1;
            j = md.indexOf("`",i);
        }
        MdNode node3(md.mid(i),nul);
        node3.parseNode(true);
        //node3.html = node3.md;
        node->children.push_back(node3);
    }
};

class QuoteParser : public AbsParser {
public:
    QuoteParser(){
        //qDebug() << "Quote Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(firstline.startsWith("> ") && type != quote && type != blockcode)return true;
        else return false;
    }
    bool push_line(const QString & line){
        if(line == ""){
            this->ready = true;
            return true;
        }
        if(line.startsWith("> ")){
            md.append(line.mid(2));
        }else{
            md.append(line);
        }
        if(line.size() > 2 && line.right(2) == "  "){
            md.chop(2);
            md.append("<br>");
        }
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md,quote));
    }
};

class BlockCodeParser : public AbsParser {
public:
    BlockCodeParser(){
        //qDebug() << "Block Code Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(firstline.startsWith("```") && type != quote && type != blockcode)return true;
        else return false;
    }
    bool push_line(const QString & line){
        if(line.startsWith("```") && !md.isEmpty()){
            this->ready = true;
            return true;
        }
        if(! line.startsWith("```")){
            md.append(line + "\n\r");
        }
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md,blockcode));
    }
};

class ListParser {
protected:
    static QString trimTabBegin(const QString & s){
        for(int i=0;i<s.size();i++){
            if(s[i] != ' '){
                return s.mid(i);
            }
        }
        return "";
    }
    static int getLiLevel(const QString & s){
        for(int i=0;i<s.size();i++){
            if(s[i] != ' ') return i/4;
        }
        return -1;
    }
};

class UnorderedListParser : public AbsParser, public ListParser{
private:
    MdNode rootlist;
    int level;
public:
    UnorderedListParser(const QString & line){
        //qDebug() << "Unordered list Parser";
        rootlist.type = ul;
        level = getLiLevel(line);
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(trimTabBegin(firstline).left(2) == "- " && type != quote && type != blockcode)return true;
        else return false;
    }
    bool push_line(const QString & line){
        //QString line{lines};
        //qDebug();
        if(line.isEmpty() || trimTabBegin(line).left(2) != "- "){
            //qDebug();//?????????????????????????????????????????????????????????????
            //cout<<"1";
            this->ready = true;
            return true;
        }
        if(getLiLevel(line) != level){
            //cout<<"!="<<line.toStdString()<<endl;
            rootlist.children.back().appendix.append(line.mid(1)+"\n");
        }else{
            //line.toStdString();
            QString subname = trimTabBegin(line).mid(2);
            rootlist.children.push_back(MdNode(subname,li));
        }
        //cout<<"over\n";
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        for(auto & li : rootlist.children){
            if(!li.appendix.isEmpty()){
                li.parseNode();
                decltype(node->children) tempchildren =  MdNode(li.appendix).children;
                li.children.insert(li.children.end(),tempchildren.begin(),tempchildren.end());
            }
        }
        node->children.push_back(rootlist);
    }
};

class OrderedListParser : public AbsParser, public ListParser{
private:
    MdNode rootlist;
    int level;
public:
    OrderedListParser(const QString & line){
        //qDebug() << "Ordered list Parser";
        rootlist.type = ol;
        level = getLiLevel(line);
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type == quote || type == blockcode) return false;
        if(! firstline.contains(".")) return false;
        QString trimed = trimTabBegin(firstline);
        QString num = trimed.left(trimed.indexOf("."));
        for(QChar c : num){
            if(c < '0' || c > '9') return false;
        }
        return true;
    }
    bool push_line(const QString & line){
        if(line=="" || !capable(line,nul)){
            //qDebug();
            this->ready = true;
            return true;
        }
        qDebug();
        if(getLiLevel(line) != level){
            rootlist.children.back().appendix.append(line.mid(1)+"\n");
        }else{
            QString subname = trimTabBegin(line).mid(3);
            rootlist.children.push_back(MdNode(subname,li));
        }
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        for(auto & li : rootlist.children){
            if(!li.appendix.isEmpty()){
                li.parseNode();
                decltype(node->children) tempchildren =  MdNode(li.appendix).children;
                li.children.insert(li.children.end(),tempchildren.begin(),tempchildren.end());
            }
        }
        node->children.push_back(rootlist);
    }
};

class TaskListParser : public AbsParser{
private:
    MdNode rootlist;
public:
    TaskListParser(){
        //qDebug() << "Ordered list Parser";
        rootlist.type = ul;
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type == quote || type == blockcode) return false;
        if(firstline.startsWith("- [x] ") || firstline.startsWith("- [ ] ")) return true;
        return false;
    }
    bool push_line(const QString & line){
        if(line=="" || !capable(line,nul)){
            this->ready = true;
            return true;
        }
        MdNode temp(line.mid(6),task);
        if(line[3] == 'x') temp.appendix = "class=\"ignotion-task-done\"";
        else temp.appendix = "class=\"ignotion-task-undone\"";
        rootlist.children.push_back(temp);
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        node->children.push_back(rootlist);
    }
};

class ImageParser : public AbsParser {
public:
    ImageParser(){
        //qDebug() << "Image Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type == code || type == blockcode) return false;
        int i = firstline.indexOf("![");
        if(i == -1) return false;
        int j = firstline.indexOf("](",i);
        if(j == -1) return false;
        int k = firstline.indexOf(")",j);
        if(k == -1) return false;
        return true;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        int i=0,j=0,k=0,m=0;
        while(i != -1 && j != -1 && k != -1 && m != -1){
            j = md.indexOf("![",i);
            k = md.indexOf("](",j);
            m = md.indexOf(")",k);
            MdNode node1(md.mid(i,j-i),nul);
            node1.html = node1.md;
            node->children.push_back(node1);
            j+=2;
            MdNode node2(md.mid(j,k-j),image);
            k+=2;
            node2.appendix = md.mid(k,m-k);
            m++;
            node->children.push_back(node2);
            i = m;
            j = md.indexOf("[",i);
        }
        MdNode node3(md.mid(i),nul);
        node3.html = node3.md;
        node->children.push_back(node3);
    }
};

class LinkParser : public AbsParser {
public:
    LinkParser(){
        //qDebug() << "Link Parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type == root || type == code || type == blockcode) return false;
        int i = firstline.indexOf("[");
        if(i == -1) return false;
        int j = firstline.indexOf("](",i);
        if(j == -1) return false;
        int k = firstline.indexOf(")",j);
        if(k == -1) return false;
        return true;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode* node){
        int i=0,j=0,k=0,m=0;
        while(i != -1 && j != -1 && k != -1 && m != -1){
            j = md.indexOf("[",i);
            k = md.indexOf("](",j);
            m = md.indexOf(")",k);
            MdNode node1(md.mid(i,j-i),nul);
            node1.parseNode(true);
            node->children.push_back(node1);
            j++;
            MdNode node2(md.mid(j,k-j),href);
            k+=2;
            node2.appendix = md.mid(k,m-k);
            m++;
            node->children.push_back(node2);
            i = m;
            j = md.indexOf("[",i);
        }
        MdNode node3(md.mid(i),nul);
        node3.parseNode(true);
        node->children.push_back(node3);
    }
};

class ParagraphParser : public AbsParser{
public:
    ParagraphParser(){
        //qDebug() << "paragraph parser";
    }
    static bool capable(const QString & firstline, TAG_TPYE type){
        if(type != root || type == blockcode || type == quote || type == ul || type == ol || firstline.isEmpty())return false;
        return true;
    }
    bool push_line(const QString & line){
        if(line == "" || UnorderedListParser::capable(line,nul) || OrderedListParser::capable(line,nul) || BlockCodeParser::capable(line,nul) || QuoteParser::capable(line,nul) || LineParser::capable(line,nul)){
            this->ready = true;
            return true;
        }
        md.append(line+"\n");
        if(line.size() > 2 && line.right(2) == "  "){
            md.chop(2);
            md.append("<br>");
        }
        this->ready = false;
        return false;
    }
    void parse(MdNode* node){
        node->children.push_back(MdNode(md,paragraph));
    }
};

class PlainTextParser : public AbsParser{
public:
    PlainTextParser(){
        //qDebug() << "PlainText";
    }
    static bool capable(const QString &){
        return true;
    }
    bool push_line(const QString & line){
        md = line;
        this->ready = true;
        return true;
    }
    void parse(MdNode * node){
        MdNode temp(md,nul);
        temp.html = md;
        node->children.push_back(temp);
    }
};

#endif // MDPARSER_H
