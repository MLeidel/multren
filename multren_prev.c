/*
multren.c
Multi-file Rename utility
Michael Leidel (Aug 2019)
rename multiple files:
    with added PREFIX
    with add, change, remove EXTENSION
    with added SUFFIX

    general format: multren COMMAND TARGET DATA
    useage:
        multren {pPREFIX | xEXTENSION | sSUFFIX} {TARGET} {PREFIX | NEWEXT | SUFFIX}

COMMAND = p|x|s
TARGET = file selection:
    may be an extension, like: .c .txt .dat .csv .php
    OR it may be the starting text of the file name
NO WILDCARDs ALLOWED

EXAMPLES
FILES: file1.txt, file2.txt ...
new extension with target extension:
    multren x .txt dat ====> file1.dat, file2.dat ...
new extension with target filename:
    multren x fil dat  ====> file1.dat, file2.dat ...
new prefix with target extension:
    multren p .txt test- ====> test-file1.txt, test-file2.txt ...
new prefix with target filename:
    multren p fi test-   ====> test-file1.txt, test-file2.txt ...
new suffix with target extension:
    multren s .txt "-new" ====> file1-new.txt, file2-new.txt ...
new suffix with target filename:
    multren s fil "-a"    ====> file1-a.txt, file2-a.txt ...
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>	/* Directory information.	*/

char fname[100];    // whole file name
char fbase[100];    // file name before extension
char fextn[10];     // file extension
char actn[100];     // cmd-line arg 1 (action)
char targ[100];     // cmd-line arg 2 (target)
char data[100];     // cmd-line arg 3
int targ_ext = 0;
int targ_fil = 0;
int counter  = 0;
void add_suffix();
void add_prefix();
void change_ext();

int endsWith (char* base, char* str) {
    int blen = strlen(base);
    int slen = strlen(str);
    return (blen >= slen) && (0 == strcmp(base + blen - slen, str));
}

int startsWith (char* base, char* str) {
    return (strstr(base, str) - base) == 0;
}

// ---------------------------------------------
int main(int argc, char *argv[]) {

    DIR           *dir_p;
    struct dirent *dir_entry_p;

    if (argc < 4) {
        printf("multren {pPREFIX | xEXTENSION | sSUFFIX} {TARGET} {PREFIX | NEWEXT | SUFFIX}\n");
        printf("EXAMPLES:\n");
        printf("multren x .dat txt\n");
        printf("multren x myfile txt\n");
        printf("multren x myfile \"\"\n");
        printf("multren p .dat 'new-pre-'\n");
        printf("multren p myfile 'new-pre-'\n");
        printf("multren s .dat '-new-suf'\n");
        printf("multren s myfile '-new-suf'\n");
        exit(1);
    }

    strcpy(actn, argv[1]);  // action: ext, bas, or pre
    strcpy(targ, argv[2]);  // target
    strcpy(data, argv[3]);  // data1

    if (targ[0] == '.')
        targ_ext = 1;
    else
        targ_fil = 1;

    dir_p = opendir(".");

    while( NULL != (dir_entry_p = readdir(dir_p))) {

        sprintf(fname, "%s", dir_entry_p->d_name);
        if (strcmp(fname, "..") == 0) continue;
        if (strcmp(fname, ".") == 0) continue;
        if (strstr(fname, ".") != NULL) {
            strcpy(fextn, strrchr(fname, '.'));
        } else {
            strcpy(fextn, "");
        }
        // now see if this file is a target
        if (targ_ext) {
            if ( !endsWith(fname, targ) ) {
                continue;
            }
        } else {  // target is file base name (minus extension)
            if ( !startsWith(fname, targ) ) {
                continue;
            }
        }
        // rename this file based on action code (ext pre ctr)
        if (strcmp(actn, "x") == 0) {
            change_ext();
            continue;
        }
        if (strcmp(actn, "p") == 0) {
            add_prefix();
            continue;
        }
        if (strcmp(actn, "s") == 0) {
            add_suffix();
            continue;
        }
    }

    closedir(dir_p);
}

/*
    getBaseName returns the file name before the "." extension
*/
char* getBaseName(char* fn) {
    int x = 0;
    for(x=0; x < strlen(fn); x++) {
        if (fn[x] == '.') {
            fn[x] = '\0';
            return fn;
        }
    }
    return fn;
}

/*
    add, remove, or change a file extension
*/
void change_ext() {
    if (startsWith(data, ".")) { // incase user included the "."
        strcpy(data, data+1);
    }
    int value;
    char old_name[100];
    char new_name[100];
    strcpy(old_name, fname);
    strcpy(new_name, getBaseName(old_name));
    if (strlen(data) > 0) {
        strcat(new_name, ".");
        strcat(new_name, data);
    }
    value = rename(fname, new_name);
    if (!value) {
        printf("%s ==> %s\n", fname, new_name);
    } else {
        printf("%s <==> rename unsuccessful !\n", fname);
    }
}

/*
    add a prefix to the body of the file name
*/
void add_prefix() {
    int value;
    char new_name[100];
    strcpy(new_name, data);
    strcat(new_name, fname);

    value = rename(fname, new_name);
    if (!value) {
        printf("%s ==> %s\n", fname, new_name);
    } else {
        printf("%s <==> rename unsuccessful !\n", fname);
    }
}


/*
    add a suffix to the body of the file name
*/
void add_suffix() {
    int value;
    char old_name[100];
    char new_name[100];
    strcpy(old_name, fname);
    strcpy(new_name, getBaseName(old_name));
    strcat(new_name, data);
    strcat(new_name, fextn);

    value = rename(fname, new_name);
    if (!value) {
        printf("%s ==> %s\n", fname, new_name);
    } else {
        printf("%s <==> rename unsuccessful !\n", fname);
    }
}
