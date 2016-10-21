#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

#define DEF_PERMS 0755

#include "config.h"

/* this is http://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix/11425692 -- TODO implement this manually */
int mkdir_recursive(const char* path, mode_t mode) {
    char *file_path = malloc(strlen(path) + 1);
    int i;
    int r;
    for(i = 0; path[i] != '\0'; ++i) {
        file_path[i] = path[i];
    }
    char* p;
    for (p=strchr(file_path+1, '/'); p; p=strchr(p+1, '/')) {
        *p='\0';
        if (mkdir(file_path, mode)==-1) {
            if (errno!=EEXIST) { *p='/'; r = -1; goto cleanup; }
        }
        *p='/';
    }

    r = 0;    

    cleanup:
    free(file_path);
    free(p);
    return r;
}

cfg chckfile(const char *path, const char *mode) {
    cfg cf = fopen(path, mode);
    if(cf == NULL) {
        mkdir_recursive(path, DEF_PERMS);
        cf = fopen(path, "w");
        freopen(path, mode, cf);
    }
    return cf;
}

cfg cfg_open(const char *configname, const char *mode) {
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    char *filepath;
    filepath = malloc(strlen(homedir) + strlen("/.config/fads/") + strlen(configname));

    sprintf(filepath, "%s/.config/fads/%s", homedir, configname);

    cfg cf = chckfile(filepath, mode);

    free(filepath);

    return cf;
}

int cfg_read(cfg cf, const char *propname, int def) {
    int ch = 0;
    int i;
    int nc = 0;
    while(ch != EOF) {
        i = 0;
        ch = fgetc(cf);
        nc = propname[i++];
        while(ch != '\n' && ch != EOF) {
            printf("i: %d, ch: %c, nc: %c\n",i,ch,nc);
            if(nc == '\0') {
                printf("Hit end! ch: %c\n", ch);
                if(ch == ':') {
                    int r = 0;
                    ch = fgetc(cf);
                    while(ch != '\n' && ch != EOF) {
                        if(ch >= '0' && ch <= '9') {
                            r *= 10;
                            r += ch - '0';
                        }
                        ch = fgetc(cf);
                    }
                    return r;
                }
            }
            else if(nc != ch) {
                printf("Failing chars: %c, %c\n", nc, ch);
                while(ch != '\n' && ch != EOF){ ch = fgetc(cf); }
            }
            else {
                ch = fgetc(cf);
                nc = propname[i++];
            }
        }
        puts("New line!");
    }
    return def;
}

void cfg_clear(cfg cf) {
    //TODO
}

void cfg_writeprop(cfg cf, const char *propname, int value) {
    fprintf(cf, "%s: %d\n", propname, value);
}

void cfg_free(cfg cf) {
    fclose(cf);
}
