#include "RecvStateMachine.h"
#include "include/Errors.h"

/*RecvStateMachine::RecvStateMachine()
{

}*/

RecvStateMachine::RecvStateMachine(fragmentProcess *ctrl)
{
    m_parent = ctrl;
    m_state = RECV_IDLE;
}

RecvStateMachine::~RecvStateMachine()
{

}

int RecvStateMachine::procEvt(recv_sm_evt_enum_t evt/*, void *evt_payload*/)
{

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
