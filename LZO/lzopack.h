#ifndef LZOPACK_H
#define LZOPACK_H

#include <lzo/lzoconf.h>
#include <lzo/lzo1x.h>
//#include "portab.h"
#include <stdio.h>

enum lzo_compress_mode
{
    LZO_COMPRESS=0,
    LZO_DECOMPRESS,
};

int do_compress(FILE *fi, FILE *fo, int compression_level, lzo_uint block_size);
int do_decompress(FILE *fi, FILE *fo);
void usage(void);
FILE *xopen_fi(const char *name);
FILE *xopen_fo(const char *name);
void abcd();
void xclose(FILE *fp);

//int processLZO(int argc, char *argv[], enum lzo_compress_mode mode);
int processLZO(int argc, char *argv[], const char *in_name, const char *out_name, enum lzo_compress_mode mode);
int processLZO(const char *in_name, const char *out_name, enum lzo_compress_mode mode);
#endif // LZOPACK_H
