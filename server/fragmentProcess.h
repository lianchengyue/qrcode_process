#ifndef FRAGMENTPROCESS_H
#define FRAGMENTPROCESS_H

#include "fileParameters.h"

/*
#define MD5SUM_MAX_S 64
#define NAME_MAX_S 255
#define PATH_MAX_S 255

typedef struct{
    unsigned char* md5sum;  //64
    int size;
    char* path; //512  /should be 255
    char name [NAME_MAX_S+1];
} fragment_metadata;//Or in stats.h
*/

class fragmentProcess
{
public:
    fragmentProcess();
    virtual ~fragmentProcess();

    int process_QRdata_to_fragment(char* QRdata); //for test
    int process_QRdata_to_fragment(char *QRdata, char *des_str);
    bool is_md5sum_match(char* QRdata);
    int process_fragment_base64_decode(char *QRdata, char *des_str);

private:
    int init();
    int readFragmentINI();
    void des_fragment_traversal(string dir, int depth);
    int create_folder_tree_from_ini();

    char *iniWholePath;//发送端完整文件的遍历结果
    char *iniPath;//接收到的每个大文件的ini识别并拼接后
};

#endif // FRAGMENTPROCESS_H
