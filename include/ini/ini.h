//
// Created by yangjiegogogo@gmail.com on 3/29/20.
//

#ifndef INI_INI_H
#define INI_INI_H


extern "C" {

#include <stddef.h>
typedef void * ini_t;

enum ini_error_t {
    INI_ERR_REDUNDANT_KEY = 1,
    INI_ERR_SECTION_NAME_EMPTY = 2,
    INI_ERR_TOO_FEW_CHARACTERS = 4,
    INI_ERR_ILLEGAL_BEGINNING_OF_LINE = 5,
    INI_ERR_NO_SPLIT_CHARACTER = 6,
};

int ini_errno_type();
int ini_errno();
ini_t ini_create(const char * file);
size_t ini_section_count(ini_t ini, const char * section_name);
int ini_find(ini_t ini, size_t index, const char * section_name, const char * key, const char ** value);
void ini_destroy(ini_t ini);

};

#endif //INI_INI_H
