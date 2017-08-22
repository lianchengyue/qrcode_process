#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "fragmentProcess.h"

class fragmentProcess;

typedef enum{
    RECV_SM_EVT_INI_START = 1,
    RECV_SM_EVT_FRAG_START,
} recv_sm_evt_enum_t;

class RecvStateMachine{
public:
    //RecvStateMachine();
    RecvStateMachine(fragmentProcess *ctrl);
    virtual ~RecvStateMachine();

    int procEvt(recv_sm_evt_enum_t evt/*, void *evt_payload*/);
    int getState();
    int setState();

private:
    typedef enum {
        RECV_IDLE = 0,//空闲
        RECV_PRESTART,//准备发送报头
        RECV_INI_TRANS,//发送报头
        RECV_START,    //准备开始
        RECV_FRAG_TRANS,//开始
        RECV_FRAG_TRANS_END,//发送文件内容结束
        RECV_TEST_MODE,
        RECV_TEST_FRAG_TRANS
    } recv_state_enum_t;

    fragmentProcess *m_parent;  //ptr to fragmentProcess
    recv_state_enum_t m_state;


    int StateMachine(recv_sm_evt_enum_t evt);//here, notice, function
};

#endif // STATEMACHINE_H
