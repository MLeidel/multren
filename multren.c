/* 2
multren.c
Multi-file Rename utility
Michael Leidel (Aug 2019)
rename multiple files:
    with added PREFIX
    with add, change, remove EXTENSION
    with added SUFFIX
    with string replacement

    general format: multren COMMAND TARGET DATA
    useage:
        multren {p | x | s | r} {TARGET} {PREFIX | NEWEXT | SUFFIX | STRING}

COMMAND = p|x|s|r
TARGET = file selection:
    may be an extension, like: .c .txt .dat .csv .php
    OR it may be the starting text of the file name
    OR the string to be replaced
DATA = the string used to make a change to the file name

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

new character string replacing old character string
    multren r "oldchars" "newchars"

  NOTE: for replacement (r) the target identifying the file
        is also the string that is getting replaced.

*/
#include <myc.h>

#define BUFSIZE 255

char fname[BUFSIZE];    // whole file name
char actn[BUFSIZE];     // cmd-line arg 1 (action)
char targ[BUFSIZE];     // cmd-line arg 2 (target)
char data[BUFSIZE];     // cmd-line arg 3
int targ_ext = 0;
int counter  = 0;
int nbr = 0; // to increment prefix number
void add_suffix();
void add_prefix();
void change_ext();
void repstring();


// ---------------------------------------------
int main(int argc, char *argv[]) {

    DIR           *dir_p;
    struct dirent *dir_entry_p;

    if (argc < 4) {
        printf("\nmultren {p | x | s | r} {TARGET} {PREFIX | NEWEXT | SUFFIX | STRING}\n");
        printf("EXAMPLES:\n");
        printf("multren x .dat txt ---> change extension\n");
        printf("multren x myfile txt ---> change|add extension\n");
        printf("multren x myfile '' ---> remove extension\n");
        printf("multren p .dat 'new-pre-' ---> add prefix\n");
        printf("multren p myfile 'new-pre-' ---> add prefix\n");
        printf("multren s .dat '-new-suf' ---> add suffix\n");
        printf("multren s myfile '-new-suf' ---> add suffix\n");
        printf("multren r 'test' 'production' ---> string replacement\n");
        printf("multren r 'test' '' ---> string deletion\n\n");

        exit(1);
    }

    strcpy(actn, argv[1]);  // action: ext, bas, or pre
    strcpy(targ, argv[2]);  // target
    strcpy(data, argv[3]);  // data1

    /*
    If the target param starts with a "." we're assuming
    the target is identified by the files extension.
    */
    if (targ[0] == '.')
        targ_ext = 1;
    else
        targ_ext = 0;

    dir_p = opendir(".");

    while( NULL != (dir_entry_p = readdir(dir_p))) {

        sprintf(fname, "%s\0", dir_entry_p->d_name);
        if (equals(fname, "..")) continue;
        if (equals(fname, ".")) continue;

          // BYPASS NON-TARGETED FILES

        // is this file a target by its extention ?
        if (targ_ext) {
            if ( !endswith(fname, targ) ) {
                continue;
            }
        } else {
            // is this file a target by its leading file name ?
            if ( !startswith(fname, targ) && actn[0] != 'r') {
                continue;
            } else {
                // could it be a string replacement target ?
                if ( strstr( fname, targ ) == NULL ) {
                    continue;
                }
            }
        }

        // rename this file based on the action code (x p s r)

        if (actn[0] == 'x') {
            change_ext();
            continue;
        }
        if (actn[0] == 'p') {
            add_prefix();
            continue;
        }
        if (actn[0] == 's') {
            add_suffix();
            continue;
        }
        if (actn[0] == 'r') {
            repstring();
            continue;
        }
    }  // end while

    closedir(dir_p);
}

/*
    getBaseName returns the file name before the "." extension
    getbasepath
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
    if (startswith(data, ".")) { // incase user included the "."
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
    int inc = 0;
    char new_name[BUFSIZE];

    inc = atoi(data);
    if (inc > 0) {
        nbr += inc;
        sprintf(new_name, "%03d_%s", nbr, fname);
    } else {
        strcpy(new_name, data);   // new prefix text
        strcat(new_name, fname); // this filename
    }

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
    char old_name[BUFSIZE];
    char new_name[BUFSIZE];
    char fextn[64];

    // first save existing extension
    if (strstr(fname, ".") != NULL) {
        strcpy(fextn, strrchr(fname, '.'));
    } else {
        strcpy(fextn, "");
    }

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

/*
    replace target string with new string to rename
*/
void repstring() {
    int value;
    char work_name[BUFSIZE];
    char new_name[BUFSIZE];
    char front[BUFSIZE];
    char tail[BUFSIZE];
    char* i;

    strcpy(work_name, fname);
    i = strstr(work_name, targ);
    *i = '\0';
    strcpy( front, work_name );
    strcpy( tail, i + strlen(targ) );
    strcpy( new_name, strcat(strcat(front, data), tail) );

    value = rename(fname, new_name);
    if (!value) {
        printf("%s ==> %s\n", fname, new_name);
    } else {
        printf("%s <==> rename unsuccessful !\n", fname);
    }
}