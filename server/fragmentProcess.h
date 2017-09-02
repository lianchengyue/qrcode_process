#ifndef FRAGMENTPROCESS_H
#define FRAGMENTPROCESS_H

#include "include/fileParameters.h"
#include "RecvStateMachine.h"

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
class RecvStateMachine;

class fragmentProcess
{
public:
    fragmentProcess();
    virtual ~fragmentProcess();

    int QRdataProcess(char* QRdata);

    int create_folder_tree_from_ini();

    int des_prestart_content_receiver(char *QRdata);
    int des_prestart_content_receiver(char *QRdata, char *des_str);

    int des_start_content_receiver(char *QRdata);

    int process_QRdata_to_fragment(char* QRdata);
    int process_QRdata_to_fragment(char *QRdata, char *des_str);
    bool is_md5sum_match(char* QRdata);
    int process_fragment_base64_decode(char *QRdata, char *des_str);

    friend class RecvStateMachine;  //是状态机的友元

private:
    int init();
    int des_fragment_traversal();
    void des_fragment_traversal_imp(char *dir, char* _short_dir, char *des, int depth);//遍历相对目录
    void des_fragment_traversal_imp(string dir, int depth);//遍历绝对目录
    //int create_folder_tree_from_ini();
    //int des_prestart_content_receiver(char *QRdata);
    //int des_prestart_content_receiver(char *QRdata, char *des_str);
    void des_ini_fragment_traversal(string dir, int depth);

    TransmitStatus getTransmitStatus();
    int setTransmitStatus(TransmitStatus status);

    char *iniWholePath;//发送端完整文件的遍历结果
    char *iniPath;//接收到的每个大文件的ini识别并拼接后
    bool ini_flag;
    bool need_cat;
    TransmitStatus mTransStatus;


    RecvStateMachine *m_stateMachine;
};

#endif // FRAGMENTPROCESS_H
