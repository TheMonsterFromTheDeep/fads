#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

typedef FILE *cfg;

cfg cfg_open(const char*,const char*);
int cfg_read(cfg,const char*,int def);
void cfg_clear(cfg);
void cfg_writeprop(cfg,const char*,int);
void cfg_free(cfg);

#endif
