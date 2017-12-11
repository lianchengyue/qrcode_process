#ifndef PROCESSINTHREAD_H
#define PROCESSINTHREAD_H

#include "include/fileParameters.h"
#include "include/macros.h"
#include "RecvStateMachine.h"


#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)//#define PR(...) printf(__VA_ARGS__)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#endif

class ProcessInThread
{
public:
    ProcessInThread();
    virtual ~ProcessInThread();

    int QRdataProcess(char* QRdata);

    int des_prestart_content_receiver(char *QRdata);
    int des_start_content_receiver(char *QRdata);

private:
    int processEvt(recv_sm_evt_enum_t evt, void *evt_payload);
    TransmitStatus getTransmitStatus();
    int setTransmitStatus(TransmitStatus status);
    void CompleteDesPath();

    TransmitStatus mTransStatus;
    RecvStateMachine *m_stateMachine;

    int ini_traversal_flag;
    int fragment_traversal_flag;
    //INI内容解析只解析一次（只支持1帧）
    int INI_prestart_flag;
    //reset标志位
    int reset_flag;

    int type_whole;

};

#endif // PROCESSINTHREAD_H
