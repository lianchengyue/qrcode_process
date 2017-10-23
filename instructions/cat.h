#ifndef CAT_H
#define CAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>


//input: dir
//output: dir
int cat(char *input, char *output, char *outfilendir);
int CountFilenum(char *dir);

int cat(char *input, char *output);

#endif // CAT_H
