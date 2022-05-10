#include "MdParser.hpp"

// HTML 前置标签
const QString frontTag[] = {
    "","<p>","","<ul>","<ol>","<li>","<em>","<strong>",
    "<hr />","<br />",
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

MdNode::MdNode(){
    md = "";
    type = nul;
    html = "";
    appendix = "";
}
MdNode::MdNode(const QString & md){
    this->md = md;
    parseNode();
}
MdNode::MdNode(const QString & md, TAG_TPYE type){
    this->md = md;
    this->type = type;
    parseNode();
}

void MdNode::parseNode(bool force){
    if(type == nul && !force) return;
    children.clear();
    QStringList list = this->md.split("\n");
    if(!list.back().isEmpty())list.append("");
    for(auto & line : list){
        if(parser == nullptr){
            if(line == "") continue;
            else if(LineParser::capable(line,type)){
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

void MdNode::show(){
    qDebug() << frontTag[type];
    if(!md.isEmpty())qDebug() << md;
    for(auto& ch : children) ch.show();
    qDebug() << backTag[type];
}

QString MdNode::getPlainText(){
    if(children.empty()) return md;
    QString text;
    for(auto ch : children){
        text += ch.getPlainText();
    }
    return text;
}

QString MdNode::getHTML(){
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


MdParser::MdParser(const QString & md){
    this->md = md;
    //截取front-matter
    this->parseFrontMatter();
}

MdParser::MdParser(const std::string & md){
    this->md = md.c_str();
    //截取front-matter
    this->parseFrontMatter();
}

MdParser * MdParser::parse(){
    node.reset(new MdNode(md,root));
    return this;
}

void MdParser::parseFrontMatter(){
    if(this->frontmatter.isEmpty() && md.startsWith("---\n")){
        md = md.mid(4);
        int i = md.indexOf("---\n");
        frontmatter = md.left(i);
        md = md.mid(i+4);
    }
}

void MdParser::show(){
    node->show();
}

QString MdParser::html(){
    //qDebug() << chapter();
    return node->getHTML();
}

QString MdParser::frontMatter(){
    return this->frontmatter;
}

QString MdParser::chapter(){
    if(node->children.empty()) return "";
    QString chapter;
    for(auto & ch : node->children){
        if(ch.type >= h1 && ch.type <= h6){
            chapter.append(ch.getPlainText() + "\n");
        }
    }
    return chapter;
}

