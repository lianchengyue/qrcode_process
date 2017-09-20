#ifndef PROCESSINTHREAD_H
#define PROCESSINTHREAD_H

#include "include/fileParameters.h"
#include "RecvStateMachine.h"

#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)//#define PR(...) printf(__VA_ARGS__)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#endif
//#define dev_err(dev, format, ...) fprintf (stderr, format, ## __VA_ARGS__)

class ProcessInThread
{
public:
    ProcessInThread();
    virtual ~ProcessInThread();

    int QRdataProcess(char* QRdata);

    int des_prestart_content_receiver(char *QRdata);
    int des_start_content_receiver(char *QRdata);

private:
    int proccessEvt();
    TransmitStatus getTransmitStatus();
    int setTransmitStatus(TransmitStatus status);

    TransmitStatus mTransStatus;
    RecvStateMachine *m_stateMachine;

};

#endif // PROCESSINTHREAD_H
