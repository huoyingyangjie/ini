# ini
## Backgroud
I believe that many scenarios still need "ini" configuration files, So I prepared this.
## Bright spot
* easy to use
* sufficient functions
## Support
* C and C++
* global and local
* use "//" as comments at the beginning of line 
* multiple sections
* section name can carry space on both sides
## How to use
```c
const char * value;
ini_t ini = ini_create(<config file path>); // create ini object
ini_section_count(ini,<section name>); // get number of section with the same name
ini_find(ini,<index of section>, <section name> , <key name>,&value); // get key value, if section name is "", it will search at global section
ini_destroy(ini);
```
## Demo
test/test.cpp
## Notes
* don't use the same key at section
* section name can't be empty
