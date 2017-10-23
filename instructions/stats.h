#ifndef STATS_H
#define STATS_H

#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include <unistd.h>
#include "include/macros.h"
#include "include/fileParameters.h"

int src_init_topology();
int des_init_topology();

void src_file_traversal_imp(char *dir, char* _short_dir, char *_2_dir, char *_3_dir, char *_4_dir, int depth);
void src_ini_traversal_imp(/*char *dir*/);
//void src_fragment_traversal_imp(char *dir, int depth);
void src_fragment_traversal_imp(char *dir, char* _short_dir, char *des, int depth);

int file_traversal();
int ini_traversal();
int fragment_traversal();


///对消息指定待传输文件的处理
#ifdef USE_ACTIVEMQ
///处理文件
//切割activeMQ命令中待传输的文件
int file_select(activeMQVec msg, const char* jsonStr);
//遍历3文件夹中的文件，并生成4中的碎片
void src_file_select_imp(char *dir, char* _short_dir, char *_2_dir, char *_3_dir, char *_4_dir, char *date, char *d_name, int type, const char* jsonstr);

///处理配置文件
int ini_select(activeMQVec msg);
void src_ini_select_imp(char *date, char *d_name, int type);

///处理碎片
//遍历日期目录下的碎片
int fragment_selected_traversal(activeMQVec msg);
void src_fragment_selected_traversal_imp(char *dir, char* _short_dir, char *des, char *date, char *d_name);
#endif
///对消息指定待传输文件的处理 end


//获取文件大小
unsigned long get_file_size(char *path);

unsigned char* generate_md5sum(char *filename);
unsigned char* generate_md5sum_from_charstr(char *QRdata);

static void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
static void getTimestamp();
//static void getTimestamp(char *des_time)


int cutQRdata(char *instr, int *offset, char *relative_path,char *filename);
int cutQRdata(char *instr, char *pureQRdata, char *relative_path,char *filename);

int cutDirName(char *instr, char *filename);
int getUpperTotalDir(char *instr);
int cutFileName(char *instr, char *filename);

int cutINIHeadData(char *relative_dir, char *date, char *name, char *ini_name);
int cutHeadData(char *relative_dir, char *date, char *name);

int CompletePath();
int CompleteSrcPath();
int CompleteDesPath();


#endif // STATS_H
