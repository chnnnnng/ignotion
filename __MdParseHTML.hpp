#ifndef __MDPARSEHTML_H
#define __MDPARSEHTML_H
#include <string>
#include <vector>
#include <iostream>
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
const string frontTag[] = {
    "","<p>","","<ul>","<ol>","<li>","<em>","<strong>",
    "<hr color=#CCCCCC size=1 />","<br />",
    "","<blockquote>",
    "<h1","<h2","<h3","<h4","<h5","<h6", // 右边的尖括号预留给添加其他的标签属性
    "<pre><code>","<code>",""
};
// HTML 后置标签
const string backTag[] = {
    "","</p>","","</ul>","</ol>","</li>","</em>","</strong>",
    "","","","</blockquote>",
    "</h1>","</h2>","</h3>","</h4>","</h5>","</h6>",
    "</code></pre>","</code>",""
};

class Tag{
public:
    Tag(const string &);
    Tag(const string &,TAG_TPYE);
    Tag & parse();
    string& getContent();
    void show();
    string getHTML();
    string getPlainText();
private:
    string trimTabBegin(const string & s){
        for(size_t i=0;i<s.size();i++){
            if(s[i] != ' '){
                return s.substr(i);
            }
        }
        return "";
    }
    int getLiLevel(const string & s){
        for(size_t i=0;i<s.size();i++){
            if(s[i] != ' ') return i/4;
        }
        return -1;
    }
    bool isUL(const string & s){
        if(trimTabBegin(s).substr(0,2) == "- "){
            return true;
        }else{
            return false;
        }
    }
    bool isOL(const string & s){
        if(s.find(".") == s.npos) return false;
        string trimed = trimTabBegin(s);
        string num = trimed.substr(0,trimed.find("."));
        for(char c : num){
            if(c < '0' || c > '9') return false;
        }
        return true;
    }
    bool isA(const string & s){
        size_t i = s.find("[");
        if(i == s.npos) return false;
        size_t j = s.find("](",i);
        if(j == s.npos) return false;
        size_t k = s.find(")",j);
        if(k == s.npos) return false;
        return true;
    }
    bool isImg(const string & s){
        size_t i = s.find("![");
        if(i == s.npos) return false;
        size_t j = s.find("](",i);
        if(j == s.npos) return false;
        size_t k = s.find(")",j);
        if(k == s.npos) return false;
        return true;
    }
private:
    TAG_TPYE type;
    string md;
    string html;
    vector<Tag> children;
    string appendix;
};

Tag& Tag::parse(){
    if(md==""){
        return *this;
    }
    size_t i=0,j=0;
    Tag blockTemp("");
    Tag listTemp("");
    Tag listItemTemp("");
    bool isBlock = false;
    bool isCode = false;
    bool isParagraph = false;
    bool isList = false;
    int listLevel = -1;
    while(j != md.npos){
        j = md.find("\n",i);
        string t;
        if(j == md.npos) t = md.substr(i);
        else t = md.substr(i,j-i);
        i=j+1;
        if(!isCode && (t.substr(0,3)=="---" || t.substr(0,3)=="***" || t.substr(0,3)=="___")){
            Tag tag("");
            tag.type = hr;
            tag.html = "<hr/>";
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,2)=="# "){
            Tag tag(t.substr((2)));
            tag.type = h1;
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,3)=="## "){
            Tag tag(t.substr((3)));
            tag.type = h2;
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,4)=="### "){
            Tag tag(t.substr((4)));
            tag.type = h3;
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,5)=="#### "){
            Tag tag(t.substr((5)));
            tag.type = h4;
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,6)=="##### "){
            Tag tag(t.substr((6)));
            tag.type = h5;
            children.push_back(tag);
        }
        else if(!isCode && t.substr(0,7)=="###### "){
            Tag tag(t.substr((7)));
            tag.type = h6;
            children.push_back(tag);
        }
        //加粗
        else if(!isCode && type != root && t.find("**") != t.npos){
            size_t i=0,j=0;
            while(j!=t.npos){
                j = t.find("**",i);
                Tag tag(t.substr(i,j-i),nul);
                tag.html = tag.md;
                children.push_back(tag);
                i=j+2;
                j = t.find("**",i);
                Tag tag2(t.substr(i,j-i),strong);
                children.push_back(tag2);
                i = j+2;
                j = t.find("**",i);
            }
            Tag tag3(t.substr(i),nul);
            tag3.html = tag3.md;
            children.push_back(tag3);
        }
        //斜体
        else if(!isCode && type != root && t.find("*") != t.npos){
            size_t i=0,j=0;
            while(j!=t.npos){
                j = t.find("*",i);
                Tag tag(t.substr(i,j-i),nul);
                tag.html = tag.md;
                children.push_back(tag);
                i=j+1;
                j = t.find("*",i);
                Tag tag2(t.substr(i,j-i),em);
                children.push_back(tag2);
                i = j+1;
                j = t.find("*",i);
            }
            Tag tag3(t.substr(i),nul);
            tag3.html = tag3.md;
            children.push_back(tag3);
        }

        //行内代码
        else if(!isCode && type != root && t.find("`") != t.npos){
            size_t i=0,j=0;
            while(j!=t.npos){
                j = t.find("`",i);
                Tag tag(t.substr(i,j-i),nul);
                tag.html = tag.md;
                children.push_back(tag);
                i=j+1;
                j = t.find("`",i);
                Tag tag2(t.substr(i,j-i),code);
                children.push_back(tag2);
                i = j+1;
                j = t.find("`",i);
            }
            Tag tag3(t.substr(i),nul);
            tag3.html = tag3.md;
            children.push_back(tag3);
        }
        //引用
        else if(!isParagraph && !isCode && t.substr(0,2)=="> " && !isBlock){
            blockTemp.type = quote;
            blockTemp.md = t.substr((2)) + "\n\r";
            if(t.size() >2 && t.substr(t.size()-2)=="  "){
                blockTemp.md += "<br/>";
            }
            isBlock = true;
        }
        //代码块
        else if(!isParagraph && t == "```" && !isCode){
            blockTemp.type = blockcode;
            blockTemp.md = "";
            isBlock = true;
            isCode = true;
        }
        //无序列表
        else if(!isParagraph && !isCode && isUL(t)){
            if(!isList){
                listTemp.type = ul;
                listItemTemp.type = li;
                listTemp.md.clear();
                listTemp.children.clear();
                listItemTemp.md.clear();
                isList = true;
                int thisLevel = getLiLevel(t);
                listLevel = thisLevel;
                listItemTemp.md += trimTabBegin(t).substr(2) +"\n";
            }else{
                int thisLevel = getLiLevel(t);
                if(listLevel != thisLevel){
                    listItemTemp.md += t.substr(1)+"\n";
                }else{
                    Tag tempTag(listItemTemp);
                    tempTag.parse();
                    listTemp.children.push_back(tempTag);
                    listItemTemp.md.clear();
                    listItemTemp.md += trimTabBegin(t).substr(2)+"\n";
                }
            }
        }
        //有序列表
        else if(!isParagraph && !isCode && isOL(t)){
            if(!isList){
                listTemp.type = ol;
                listItemTemp.type = li;
                listTemp.md.clear();
                listTemp.children.clear();
                listItemTemp.md.clear();
                isList = true;
                int thisLevel = getLiLevel(t);
                listLevel = thisLevel;
                string trimed = trimTabBegin(t);
                listItemTemp.md += trimed.substr(trimed.find(". ")+2) + "\n";
            }else{
                int thisLevel = getLiLevel(t);
                if(listLevel != thisLevel){
                    listItemTemp.md += t.substr(1)+"\n";
                }else{
                    Tag tempTag(listItemTemp);
                    tempTag.parse();
                    listTemp.children.push_back(tempTag);
                    listItemTemp.md.clear();
                    string trimed = trimTabBegin(t);
                    listItemTemp.md += trimed.substr(trimed.find(". ")+2) + "\n";
                }
            }
        }
        else if(!isParagraph && !isCode && isList && !isUL(t)){
            isList = false;
            Tag tempTag(listItemTemp);
            tempTag.parse();
            listTemp.children.push_back(tempTag);
            children.push_back(listTemp);
            listTemp.md.clear();
            listTemp.children.clear();
        }
        //图片
        else if(!isCode && isImg(t)){
            size_t i=0,j=0,k=0,m=0;
            while(i != t.npos && j != t.npos && k != t.npos && m != t.npos){
                j = t.find("![",i);
                k = t.find("](",j);
                m = t.find(")",k);
                Tag tag1(t.substr(i,j-i),nul);
                tag1.html =tag1.md;
                children.push_back(tag1);
                j+=2;
                Tag tag2(t.substr(j,k-j),image);
                k+=2;
                tag2.appendix = t.substr(k,m-k);
                m++;
                children.push_back(tag2);
                i = m;
                j = t.find("[",i);
            }
            Tag tag3(t.substr(i),nul);
            tag3.html = tag3.md;
            children.push_back(tag3);
        }
        //超链接
        else if(!isCode && isA(t)){
            size_t i=0,j=0,k=0,m=0;
            while(i != t.npos && j != t.npos && k != t.npos && m != t.npos){
                j = t.find("[",i);
                k = t.find("](",j);
                m = t.find(")",k);
                Tag tag1(t.substr(i,j-i),nul);
                tag1.html =tag1.md;
                children.push_back(tag1);
                j++;
                Tag tag2(t.substr(j,k-j),href);
                k+=2;
                tag2.appendix = t.substr(k,m-k);
                m++;
                children.push_back(tag2);
                i = m;
                j = t.find("[",i);
            }
            Tag tag3(t.substr(i),nul);
            tag3.html = tag3.md;
            children.push_back(tag3);
        }
        else{
            if(isBlock) {
                if((t == "" && !isCode)|| (t=="```"&&blockTemp.md.size()!=0)){
                    blockTemp.parse();
                    isBlock = false;
                    isCode = false;
                    isParagraph = false;
                    children.push_back(blockTemp);
                    blockTemp.md.clear();
                    blockTemp.children.clear();
                }else{
                    if(t.substr(0,2)=="> "){
                        blockTemp.md += t.substr((2))+"\n\r";
                    }else{
                        blockTemp.md += t+"\n\r";
                    }
                    if(t.size() >2 && t.substr(t.size()-2)=="  "){
                        blockTemp.md += "<br/>";
                    }
                }
            }
            else if(type == root && !t.empty()){
                blockTemp.md += t;
                if(t.size() >2 && t.substr(t.size()-2)=="  "){
                    blockTemp.md += "<br/>";
                }
                blockTemp.type = paragraph;
                isBlock = true;
                isParagraph = true;
            }
            else if(t.size()!=0){
                Tag tag("");
                tag.type = nul;
                tag.md = t;
                tag.html = t;
                children.push_back(tag);
            }
        }
    }
    return *this;
}

Tag::Tag(const string & s){
    md = s;
    parse();
}

Tag::Tag(const string & s,TAG_TPYE t){
    md = s;
    type = t;
    parse();
}

string Tag::getPlainText(){
    if(children.empty()) return md;
    string text;
    for(auto ch : children){
        text += ch.getPlainText();
    }
    return text;
}

string Tag::getHTML(){
    if(children.empty()) return html;
    if(type == href){
        return "<a href=\"" + appendix + "\">" + md +"</a>";
    }
    if(type == image){
        return "<img src=\"" + appendix + "\" alt=\""+md+"\" title=\""+md+"\" />";
    }
    if(type == blockcode){
        return frontTag[type] + md + backTag[type];
    }
    string html;
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

void Tag::show(){
    if(children.size()==0){
        cout << frontTag[type] << "\n" << md << "\n";
    }else{
        cout << frontTag[type] << "\n";
        for(auto ch : children){
            ch.show();
        }
    }
    cout << backTag[type] <<endl;
}

class MdParser{
private:
    string md;
    unique_ptr<Tag> tag;
public:
    MdParser(const string & md){
        this->md = md;
        this->tag.reset(new Tag(md,root));
    }
    void show(){
        tag->show();
    }
    string html(){
        return tag->getHTML();
    }
};





#endif // __MDPARSEHTML_H
