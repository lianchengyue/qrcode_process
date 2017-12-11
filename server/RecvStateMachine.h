#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <queue>
#include "fragmentProcess.h"
#include "include/macros.h"


#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)//#define PR(...) printf(__VA_ARGS__)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#endif

class fragmentProcess;

typedef enum{
    RECV_SM_EVT_INI_START = 1,//处理报头
    RECV_SM_EVT_FRAG_START,//处理正文
    RECV_SM_EVT_FRAG_INTERRUPT, //优先级更高的中断到来
    RECV_SM_EVT_FRAG_UPDATE, //传输过程中更新
    RECV_SM_EVT_FRAG_PROCESSING, //处理过程中
    RECV_SM_EVT_IDLE,
    RECV_SM_EVT_RESET,
} recv_sm_evt_enum_t;

typedef struct{
    recv_sm_evt_enum_t evt;
    void* evt_payload;
} recv_sm_cmd_t;

#if 0
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
#endif

class RecvStateMachine{
protected:
    //这里将构造，析构，拷贝构造，赋值函数设为私有，杜绝了生成新例
    RecvStateMachine()
    {
    }
    RecvStateMachine(fragmentProcess *ctrl);
    ~RecvStateMachine();

public:
//singleton
    static RecvStateMachine* getInstance();
//singleton end

    int procEvt(recv_sm_evt_enum_t evt, void*evt_payload);
    int getState();
    int setState();

private:
    //singleton
    static RecvStateMachine* instance;
    //singleton end

    static void *smEvtProcessRoutine(void *data);//main stateMachine process routine

    int StateMachine(recv_sm_evt_enum_t evt, void *payload);//here, notice, function

    fragmentProcess *m_parent;  //ptr to fragmentProcess
    recv_sm_evt_enum_t m_state;
    pthread_t cmd_pid;

    queue<recv_sm_cmd_t> evt_queue;
};
#endif // STATEMACHINE_H
