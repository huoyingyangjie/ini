//
// Created by yangjiegogogo@gmail.com on 3/29/20.
//
#ifndef NDEBUG
#include "utils/debug.h"
#endif
#include "ini/ini.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <string.h>
#include <strings.h>

using namespace std;

thread_local int errno_type=0;
thread_local int errno_value=0;

struct section_t {
    string name;
    map<string,string> kv;
};

struct ini_ctx_t {
    section_t * global;
    vector<section_t *> locals;
};

// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

void super_split(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.emplace_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != len)
        v.emplace_back(s.substr(pos1));
}

vector<string> split(const string &s,char delimiter){
    int size=s.size();
    string token;
    vector<string> tokens;

    for(int i=0;i<size;i++){
        if(s[i]!=delimiter)
        {
            token+=s[i];
            continue;
        }
        tokens.push_back(trim(token));
        if(i+1<size) {
            string sTmp = s.substr(i + 1, size - i - 1);
            tokens.push_back(trim(sTmp));
        }
        else
            tokens.push_back("");
        return tokens;

    }

    tokens.push_back(trim(token));
    tokens.push_back("");
    return tokens;

}

static int str_parse(ini_ctx_t * ini_ctx, string &str)
{
    vector<string> lines;
    super_split(str,lines,"\n");
    vector<string>::iterator iter;
    section_t * section=NULL;
    string tmp;
    for(iter=lines.begin();iter!=lines.end();iter++)
    {
        DBG("line=%s",(*iter).c_str());
        tmp=*iter;
        tmp = trim(tmp);
        if(tmp.length() == 0)
            continue;
        if(tmp.length() < 2)
        {
            errno_type = 1;
            errno_value = INI_ERR_TOO_FEW_CHARACTERS;
            return -1;
        }
        if(tmp[0]=='/'&&tmp[1]=='/')
            continue;
        if(tmp[0]=='['&&tmp[tmp.length()-1]==']')
        {
            section=new section_t;
            ini_ctx->locals.push_back(section);
            tmp[0]=' ';tmp[tmp.length()-1]=' ';
            tmp=trim(tmp);
            if(tmp.empty())
            {
                DBG("section name is emtpy");
                errno_type = 1;
                errno_value = INI_ERR_SECTION_NAME_EMPTY;
                return -1;
            }
            section->name=tmp;
            continue;
        }
        if(tmp[0]=='=')
        {
            errno_type = 1;
            errno_value = INI_ERR_ILLEGAL_BEGINNING_OF_LINE;
            return -1;
        }
        if(tmp.find("=")==string::npos)
        {
            errno_type = 1;
            errno_value = INI_ERR_NO_SPLIT_CHARACTER;
            return -1;
        }
        if(section==NULL)
        {
            section=new section_t;
            ini_ctx->global=section;
        }
        vector<string> kv = split(tmp,'=');
        if(section->kv.count(kv[0])>0)
        {
            errno_type = 1;
            errno_value = INI_ERR_REDUNDANT_KEY;
            return -1;
        }
        section->kv[kv[0]]=kv[1];
    }
    return 0;
}

ini_t ini_create(const char * file)
{
    ini_ctx_t * ini_ctx;
    long sz;
    char * buf=NULL;
    char c;
    size_t cursor=0;
    string str;
    errno_type=0;
    ini_ctx=new ini_ctx_t;
    ini_ctx->global=NULL;
    FILE * fp=fopen(file, "r");
    if(fp==NULL)
        return NULL;
    if(fseek(fp,0L, SEEK_END)!=0)
        goto failed;
    sz=ftell(fp);
    if(sz==-1)
        goto failed;
    rewind(fp);
    if(sz==0)
        return ini_ctx;
    buf= (char*)malloc(sz);
    if(buf==NULL)
    {
        fclose(fp);
        free(ini_ctx);
        return NULL;
    }
    bzero(buf, sz);
    while ((c = getc(fp)) != EOF)
    {
        buf[cursor]=c;
        ++cursor;
    }     
    fclose(fp);
    str=buf;
    free(buf);
    if(str_parse(ini_ctx,str)!=0)
    {
        ini_destroy((ini_t)ini_ctx);
        return NULL;
    }
    return ini_ctx;
    failed:
    fclose(fp);
    free(buf);
    free(ini_ctx);
    return NULL;

}

size_t ini_section_count(ini_t ini, const char * section_name)
{
    ini_ctx_t * ini_ctx=(ini_ctx_t*)ini;
    size_t cursor=0;
    vector<section_t*>::iterator iter;
    for(iter=ini_ctx->locals.begin();iter!=ini_ctx->locals.end();iter++)
    {
        if(strcmp(section_name,(*iter)->name.c_str())!=0)
            continue;
        ++cursor;
    }
    return cursor;
}

int ini_find(ini_t ini, size_t index, const char * section_name, const char * key, const char ** value)
{
    ini_ctx_t * ini_ctx=(ini_ctx_t*)ini;
    size_t cursor=0;
    section_t * section_ptr=NULL;
    vector<section_t*>::iterator iter;
    map<string, string>::iterator iter_map;
    if(strcmp(section_name,"")==0)
    {
        section_ptr = ini_ctx->global;
        goto find_key;
    }
    for(iter=ini_ctx->locals.begin();iter!=ini_ctx->locals.end();iter++)
    {
        if(strcmp(section_name,(*iter)->name.c_str())!=0)
            continue;
        if(cursor!=index)
        {
            ++cursor;
            continue;
        }
        section_ptr=*iter;
        break;
    }
    if(section_ptr==NULL)
        return -1;
    find_key:
    iter_map=section_ptr->kv.find(key);
    if(iter_map!=section_ptr->kv.end())
    {
        *value=iter_map->second.c_str();
        return 0;
    }
    else
        return -1;
}

int ini_errno_type()
{
    return errno_type;
}

int ini_errno()
{
    if(errno_type==0)
        return errno;
    return errno_value;
}

void ini_destroy(ini_t ini)
{
    ini_ctx_t * ini_ctx = (ini_ctx_t*)ini;
    if(ini_ctx->global!=NULL)
        delete ini_ctx->global;
    vector<section_t *>::iterator iter;
    for(iter=ini_ctx->locals.begin();iter!=ini_ctx->locals.end();iter++)
    {
            delete *iter;
    }
    delete ini_ctx;
}
