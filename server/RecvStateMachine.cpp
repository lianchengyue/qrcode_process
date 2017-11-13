 #include <unistd.h>  //sleep()

#include "RecvStateMachine.h"
#include "include/Errors.h"
#include "include/DirPath.h"

#include <string.h>

//状态不允许丢失，使用队列
//statemachine process thread routine to handle events in different state
void *RecvStateMachine::smEvtProcessRoutine(void *data)
{
    int running = 1;
    int ret;

    RecvStateMachine *pMe = (RecvStateMachine *)data;

    /*
    do{
        recv_sm_cmd_t *node = (recv_sm_cmd_t *)pMe->evt_queue.front();
        if(node != NULL)
        {
            //switch(node->evt)
            //{
            //}
            pMe->StateMachine(node->evt, node->evt_payload);

            (recv_sm_cmd_t *)pMe->evt_queue.pop();//flq

            free(node->evt_payload);
            free(node);
        }
    } while (running);
    */
}

//构造与析构函数
RecvStateMachine::RecvStateMachine(fragmentProcess *ctrl)
{
    m_parent = ctrl;
    m_state = RECV_SM_EVT_IDLE;
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
    StateMachine(evt, evt_payload);
#if 0
    recv_sm_cmd_t *node = (recv_sm_cmd_t *)malloc(sizeof(recv_sm_cmd_t));

    if(NULL == node)
    {
        printf("node==null\n");
        return -1;
    }

    memset(node, 0 ,sizeof(recv_sm_cmd_t));
    node->evt = evt;
    node->evt_payload = evt_payload;

    /*
    //插入队列
    if(evt_queue.push((void *)node))
    {
        return NO_ERROR;
    }
    else
    {
        free(node);
        return UNKNOWN_ERROR;
    }*/
    free(node);
#endif
}

int RecvStateMachine::StateMachine(recv_sm_evt_enum_t evt, void *payload)
{
    int rc = NO_ERROR;
    switch(evt){//m_state
    case RECV_SM_EVT_INI_START:
        LOG_DBG("RECV_SM_EVT_INI_START\n");
        ///m_parent->des_ini_traversal();  //之前的方案
        m_parent->des_ini_select();
        break;
    case RECV_SM_EVT_FRAG_START:
        ///遍历待拼接文件
        LOG_DBG("RECV_SM_EVT_FRAG_START\n");
        ///m_parent->des_fragment_traversal(); //之前的方案

        //等待线程完成
        sleep(1);
        m_parent->des_fragment_select();
        //rc = ;
        break;
    case RECV_SM_EVT_FRAG_INTERRUPT:
        //rc = ;
        break;
    case RECV_SM_EVT_FRAG_UPDATE:
        //rc = ;
        break;
    case RECV_SM_EVT_FRAG_PROCESSING:
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
