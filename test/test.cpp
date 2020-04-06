//
// Created by yangjiegogogo@gmail.com on 4/6/20.
//

#include "ini/ini.h"
#include "utils/debug.h"
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    DBG("file=%s", argv[1]);
    const char *value;
    int ret;
    ini_t ini = ini_create(argv[1]);
    if(ini==NULL)
        DIE("ini create failed, errno_type=%d, errno=%d", ini_errno_type(), ini_errno());
    INF("ini create success");
    ret=ini_find(ini,0,"hello","name",&value);
    if(ret!=0)
        INF("don't find");
    else
        INF("value=%s", value);
    ini_destroy(ini);
}

