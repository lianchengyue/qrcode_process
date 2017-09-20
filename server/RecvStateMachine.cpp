#include "RecvStateMachine.h"
#include "include/Errors.h"

#include <string.h>

//状态不允许丢失，使用队列
//statemachine process thread routine to handle events in different state
void *RecvStateMachine::smEvtProcessRoutine(void *data)
{
    int running = 1;
    int ret;

    RecvStateMachine *pMe = (RecvStateMachine *)data;
}

//构造与析构函数
RecvStateMachine::RecvStateMachine(fragmentProcess *ctrl)
{
    m_parent = ctrl;
    m_state = RECV_IDLE;
    cmd_pid = 0;

    pthread_create(&cmd_pid,
                   NULL,
                   smEvtProcessRoutine,
                   this);
    pthread_setname_np(cmd_pid, "QR_StateMachine");
}

RecvStateMachine::~RecvStateMachine()
{

}

RecvStateMachine* RecvStateMachine::instance = new RecvStateMachine();
RecvStateMachine* RecvStateMachine::getInstance()
{
    return instance;
}


int RecvStateMachine::procEvt(recv_sm_evt_enum_t evt, void*evt_payload)
{
    recv_sm_cmd_t *node = (recv_sm_cmd_t *)malloc(sizeof(recv_sm_cmd_t));

    if(NULL == node)
    {
        printf("node==null\n");
        return -1;
    }

    memset(node, 0 ,sizeof(recv_sm_cmd_t));
    node->evt = evt;
    node->evt_payload = evt_payload;

    //插入队列
    /*
    if(evt_queue.enqueue((void*)node)
    {
        //cam_sem_post(&cmd_sem);
        return NO_ERROR;
    }
    else
    {
        free(node);
        return UNKNOWN_ERROR;
    }
    */
    free(node);
}

int RecvStateMachine::StateMachine(recv_sm_evt_enum_t evt)
{
    int rc = NO_ERROR;
    switch(m_state){
    case RECV_IDLE:
        //rc = ;
        rc = m_parent->is_md5sum_match("11111111");
        break;
    case RECV_PRESTART:
        //rc = ;
        break;
    case RECV_INI_TRANS:
        //rc = ;
        break;
    case RECV_START:
        //rc = ;
        break;
    case RECV_FRAG_TRANS:
        //rc = ;
        break;
    case RECV_FRAG_TRANS_END:
        //rc = ;
        break;
    case RECV_TEST_MODE:
        //rc = ;
        break;
    case RECV_TEST_FRAG_TRANS:
        //rc = ;
        break;

    default:
        break;
    }
}

int RecvStateMachine::getState()
{

}

int RecvStateMachine::setState()
{

}
