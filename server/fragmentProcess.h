#ifndef FRAGMENTPROCESS_H
#define FRAGMENTPROCESS_H

#include "include/fileParameters.h"
#include "include/macros.h"
#include "RecvStateMachine.h"

#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#endif

class RecvStateMachine;

class fragmentProcess
{
public:
    fragmentProcess();
    virtual ~fragmentProcess();

    int QRdataProcess(char* QRdata);
    int des_fragment_traversal();
    int des_ini_traversal();

    int create_folder_tree_from_ini();

    int process_QRdata_to_fragment(char* QRdata);
    int process_QRdata_to_fragment(char *QRdata, char *des_str);

    friend class RecvStateMachine;  //是状态机的友元

private:
    int init();

    //接收报头
    int des_prestart_content_receiver(char *QRdata);
    int des_prestart_content_receiver(char *QRdata, char *des_str);

    //接收正文
    int des_start_content_receiver(char *QRdata);

    void des_fragment_traversal_imp(char *dir, char* _short_dir, char *des, int depth);//遍历相对目录
    void des_fragment_traversal_imp(string dir, int depth);//遍历绝对目录
    void des_ini_fragment_traversal_imp(string dir, int depth);
    bool is_md5sum_match(char* QRdata);

    TransmitStatus getTransmitStatus();
    int setTransmitStatus(TransmitStatus status);

    void CompleteDesPath();

    char *iniWholePath;//发送端完整文件的遍历结果
    char *iniPath;//接收到的每个大文件的ini识别并拼接后
    bool ini_flag;

    bool is_folder_created;
    TransmitStatus mTransStatus;

    RecvStateMachine *m_stateMachine;
};

#endif // FRAGMENTPROCESS_H
