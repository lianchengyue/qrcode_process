#ifndef STATS_H
#define STATS_H

#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include <unistd.h>

int src_init_topology();
int des_init_topology();

void src_file_traversal_imp(char *dir, char* _short_dir, char *_2_dir, char *_3_dir, char *_4_dir, int depth);
void src_ini_traversal_imp(/*char *dir*/);
void src_fragment_traversal_imp(char *dir, int depth);

int file_traversal();
int ini_traversal();
int fragment_traversal();

//获取文件大小
unsigned long get_file_size(char *path);

unsigned char* generate_md5sum(char *filename);
unsigned char* generate_md5sum_from_charstr(char *QRdata);

static void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void getTimestamp();
//static void getTimestamp(char *des_time)

#endif // STATS_H
