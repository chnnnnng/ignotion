#ifndef MDPARSE_HPP
#define MDPARSE_HPP
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
    root            = 20
};
// HTML 前置标签
const QString frontTag[] = {
    "","<p>","","<ul>","<ol>","<li>","<em>","<strong>",
    "<hr color=#CCCCCC size=1 />","<br />",
    "","<blockquote>",
    "<h1","<h2","<h3","<h4","<h5","<h6", // 右边的尖括号预留给添加其他的标签属性
    "<pre><code>","<code>",""
};
// HTML 后置标签
const QString backTag[] = {
    "","</p>","","</ul>","</ol>","</li>","</em>","</strong>",
    "","","","</blockquote>",
    "</h1>","</h2>","</h3>","</h4>","</h5>","</h6>",
    "</code></pre>","</code>",""
};


class AbsParser;
class MdNode{
public:
    QString md;
    TAG_TPYE type;
    QString html;
    vector<MdNode> children;
    QString appendix;
private:
    shared_ptr<AbsParser> parser;
public:
    MdNode(){
        md = "";
        type = nul;
        html = "";
        appendix = "";
    }
    MdNode(const QString & md){
        this->md = md;
        parseNode();
    }
    MdNode(const QString & md, TAG_TPYE type){
        this->md = md;
        this->type = type;
        parseNode();
    }

    void parseNode(bool force = false);
    void show(){
        qDebug() << frontTag[type];
        if(!md.isEmpty())qDebug() << md;
        for(auto& ch : children) ch.show();
        qDebug() << backTag[type];
    }

    QString getPlainText(){
        if(children.empty()) return md;
        QString text;
        for(auto ch : children){
            text += ch.getPlainText();
        }
        return text;
    }

    QString getHTML(){
        if(children.empty() && type != hr) return html;
        if(type == href){
            return "<a href=\"" + appendix + "\">" + md +"</a>";
        }
        if(type == image){
            return "<img src=\"" + appendix + "\" alt=\""+md+"\" title=\""+md+"\" />";
        }
        if(type == blockcode){
            return frontTag[type] + md + backTag[type];
        }
        QString html;
        html += frontTag[type];
        if(type >= h1 && type <= h6){
            html += " id=\"";
            html += getPlainText();
            html += "\">";
        }
        for(auto ch : children){
            html += ch.getHTML();
        }
        html += backTag[type];
        return html;
    }
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
        //node3.html = node3.md;
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
        if(line=="" || trimTabBegin(line).left(2) != "- "){
            //qDebug() << "ul parse ready";
            this->ready = true;
            return true;
        }
        int thisLevel = getLiLevel(line);
        if(thisLevel != level){
            rootlist.children.back().appendix.append(line.mid(1)+"\n");
        }else{
            rootlist.children.push_back(MdNode(trimTabBegin(line).mid(2),li));
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
            //qDebug() << "ol parse ready";
            this->ready = true;
            return true;
        }
        int thisLevel = getLiLevel(line);
        if(thisLevel != level){
            rootlist.children.at(rootlist.children.size()==0?0:rootlist.children.size()-1).appendix.append(line.mid(1)+"\n");
        }else{
            rootlist.children.push_back(MdNode(trimTabBegin(line).mid(3),li));
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
        if(type == code || type == blockcode) return false;
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
            node1.html = node1.md;
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
        node3.html = node3.md;
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
    void parse(MdNode* node){
        MdNode temp(md,nul);
        temp.html = temp.md;
        node->children.push_back(temp);
    }
};

void MdNode::parseNode(bool force){
    if(type == nul && !force) return;
    children.clear();
    QStringList list = this->md.split("\n");
    if(!list.back().isEmpty())list.append("");
    for(auto& line : list){
        //qDebug() <<"read line: "<< line;
        if(parser == nullptr){
            if(line == "") continue;
            if(LineParser::capable(line,type)){
                parser.reset(new LineParser);
            }
            else if(H1Parser::capable(line,type)){
                parser.reset(new H1Parser());
            }
            else if(H2Parser::capable(line,type)){
                parser.reset(new H2Parser());
            }
            else if(H3Parser::capable(line,type)){
                parser.reset(new H3Parser());
            }
            else if(H4Parser::capable(line,type)){
                parser.reset(new H4Parser());
            }
            else if(H5Parser::capable(line,type)){
                parser.reset(new H5Parser());
            }
            else if(H6Parser::capable(line,type)){
                parser.reset(new H6Parser());
            }
            else if(StrongParser::capable(line,type)){
                parser.reset(new StrongParser());
            }
            else if(ItalicParser::capable(line,type)){
                parser.reset(new ItalicParser());
            }
            else if(InlineCodeParser::capable(line,type)){
                parser.reset(new InlineCodeParser());
            }
            else if(QuoteParser::capable(line,type)){
                parser.reset(new QuoteParser());
            }
            else if(BlockCodeParser::capable(line,type)){
                parser.reset(new BlockCodeParser());
            }
            else if(UnorderedListParser::capable(line,type)){
                parser.reset(new UnorderedListParser(line));
            }
            else if(OrderedListParser::capable(line,type)){
                parser.reset(new OrderedListParser(line));
            }
            else if(ImageParser::capable(line,type)){
                parser.reset(new ImageParser());
            }
            else if(LinkParser::capable(line,type)){
                parser.reset(new LinkParser());
            }
            else if(ParagraphParser::capable(line,type)){
                parser.reset(new ParagraphParser());
            }
            else{
                parser.reset(new PlainTextParser());
            }
        }
        if(parser != nullptr && parser->ready == false){
            parser->push_line(line);
        }
        if(parser->ready){
            parser->parse(this);
            parser = nullptr;
        }
    }
}

class MdParser{
private:
    QString md;
    unique_ptr<MdNode> node;
private:
    void parse(){
        //qDebug() << "MdParser parser begin";
        node.reset(new MdNode(md,root));
    }
public:
    MdParser(const QString & md){
        this->md = md;
        parse();
    }
    MdParser(const string & md){
        this->md = md.c_str();
        parse();
    }
    void show(){
        //qDebug() << "MdParser show";
        node->show();
        //qDebug() << "MdParser show over";
    }
    QString html(){
        return node->getHTML();
    }
};





#endif // MDPARSE_HPP
