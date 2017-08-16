#ifndef STATS_H
#define STATS_H

#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include <unistd.h>

int file_traversal();
int fragment_traversal();

unsigned char* generate_md5sum(char *filename);
unsigned char* generate_md5sum_from_charstr(char *QRdata);

static void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void getTimestamp();
//static void getTimestamp(char *des_time)

#endif // STATS_H
