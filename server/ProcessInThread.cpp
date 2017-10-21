#include "ProcessInThread.h"

#include "../instructions/stats.h"
#include "../instructions/inirw.h"
#include "../instructions/cat.h"

#include "include/Errors.h"
#include "include/macros.h"
#include "include/DirPath.h"

//#include "LZO/lzopack.h"

//#include "include/DirPath.h"
#include "../instructions/base64.h"


#include <string>
using namespace std;

ProcessInThread::ProcessInThread()
{
    m_stateMachine = RecvStateMachine::getInstance();

    CompleteDesPath();

    ini_traversal_flag = 1;
    fragment_traversal_flag = 1;

    //for test, //接受完碎片，开始处理
//    processEvt(RECV_SM_EVT_FRAG_START, NULL);
}

ProcessInThread::~ProcessInThread()
{
}

//整个处理的入口函数，识别二维码成功后进入
int ProcessInThread::QRdataProcess(char* QRdata)
{
    if(0 == strcmp(QRdata, TRANSMIT_IDLE))
    {
        //printf("switch:TRANSMIT_IDLE\n");
        return NO_ERROR;
    }

    //接收配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_PRESTART))
    {
        printf("switch:TRANSMIT_PRESTART\n");
        setTransmitStatus(PRESTART);
        //////////新的帧到来，将所有状态重置为激活///////
        ini_traversal_flag = 1;
        fragment_traversal_flag = 1;

        return NO_ERROR;
    }

    else if(0 == strcmp(QRdata, TRANSMIT_PREEND))
    {
        printf("switch:TRANSMIT_PREEND\n");
        setTransmitStatus(PREEND);
        return NO_ERROR;
    }
#if 1
    //QRdata

    //处理配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_START))
    {
        printf("switch:TRANSMIT_START\n");
        mTransStatus = getTransmitStatus();
        setTransmitStatus(PREEND);

        //delay(300)//ms

        //创建所有的文件目录,（不包括碎片目录）
        ///遍历完后拼接fold.ini
        ///===============================发消息，遍历ini并恢复，做处理================================///
        if(1 == ini_traversal_flag)
        {
            processEvt(RECV_SM_EVT_INI_START, NULL);
            ini_traversal_flag = 0;
        }

        setTransmitStatus(TRANSMITTING);
    }

    //开始遍历1_receiver正文
    else if(0 == strcmp(QRdata, TRANSMIT_END))
    {
        printf("switch:TRANSMIT_END\n");
        setTransmitStatus(END);
        //delay(300)//ms
        ///遍历待拼接文件
        LOG_DBG("FRAGMENT_TRAVERSAL\n");
        ///=====================================遍历文件======================================///
        if(1 == fragment_traversal_flag)
        {
            processEvt(RECV_SM_EVT_FRAG_START, NULL);
            setTransmitStatus(IDLE);
            fragment_traversal_flag = 0;
        }

    }
#endif
    //处理配置文件或文件正文
    else
    {
        //发送报头
        if(PRESTART == getTransmitStatus()){
            ///接收报头并放到目标目录
            LOG_DBG("RECEIVE_INI\n");
            des_prestart_content_receiver(QRdata);
        }
        //发送内容
        else if(TRANSMITTING == getTransmitStatus()){
            ///接收正文并放到目标目录
            ////LOG_DBG("RECEIVE_CONTENT\n");
            des_start_content_receiver(QRdata);
        }
        else{
            LOG_ERR("Drop a frame!!!\n");
        }

        //记录每一个需要拼接的路径
    }

}

//process_QRdata_to_fragment完后执行
//含有路径信息和文件名
//函数功能：去掉报头，仅保留正文.                      DES_RECV_INI_LOCATION:生成到该目录
int ProcessInThread::des_prestart_content_receiver(char *QRdata)
{
    char *relative_dir  = new char[PATH_MAX];
    char *total_dir  = new char[PATH_MAX];
    char *name = new char[NAME_MAX];
    char *pureQRdata;//temp
    int *offset = (int *)malloc(sizeof(int));

    printf("des_prestart_content_receiver\n");
    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    *offset = 0;
    //temp
    //pureQRdata = new char[QRDATA_SIZE];//temp
    //memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp   ///name:X0 //relative_dir:config/
    if(NO_ERROR != ret){
        return -1;
    }
    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;


    sprintf(total_dir, "%s%s", DES_RECV_INI_LOCATION, relative_dir); ///生成到该目录 relative_dir:config/,应改为config.ini/

    //文件夹是否存在
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }

    strcat(total_dir, name);

    FILE *INI_Destination = fopen(total_dir, "wb"); //ab+;
    //测试读取二维码并生成文件，正式版删去
    ///flq  报错过一次，后续压力测试关注
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), INI_Destination);

    fclose(INI_Destination); // 关闭文件


    free(relative_dir);
    free(total_dir);
    free(name);
    //free(pureQRdata);//temp
    free(offset);
    return NO_ERROR;
}

/*函数功能:识别的二维码信息裁剪完后,放入1_receive文件夹.   做base64 decode完后,再放入2_base64文件夹*/
//process_QRdata_to_fragment完后执行
//含有路径信息和文件名
//函数功能：去掉报头，仅保留正文.                      DES_RECEIVE_LOCATION:生成到该目录
int ProcessInThread::des_start_content_receiver(char *QRdata)
{
    char *relative_dir  = new char[PATH_MAX];
    char *total_dir  = new char[PATH_MAX];
    char *name = new char[NAME_MAX];
    char *pureQRdata;//temp
    int *offset = (int *)malloc(sizeof(int));

    //printf("des_start_content_receiver\n");

    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    *offset = 0;
    //temp
    pureQRdata = new char[QRDATA_SIZE];//temp
    memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp
    if(NO_ERROR != ret){
        return -1;
    }
    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;


    sprintf(total_dir, "%s%s", DES_RECEIVE_LOCATION, relative_dir);

    //文件夹是否存在
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }

    strcat(total_dir, name);

    FILE *cut_head_Destination = fopen(total_dir, "wb"); //ab+;

#if 1  //#ifdef DES_DECODE
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);
    //解码并生成文件
    memset(total_dir, 0, PATH_MAX);

    #if 1
    //在2目录中创建接收传输文件的文件夹
    sprintf(total_dir, "%s%s", DES_BASE64_DECODE_LOCATION, relative_dir);
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }
    strcat(total_dir, name);

    if(0 == strcmp(name,"X403"))//X403
    {
       int k = 1;
    }
    #else
    //sprintf(total_dir, "%s%s%s", DES_BASE64_DECODE_LOCATION, relative_dir,name);
    #endif

    FILE *base64_decode_Destination = fopen(total_dir, "w");  //home/montafan/QRcodeGrab/destination/2_base64_decode_location/hu.jpg/X0

    decode(pureQRdata, base64_decode_Destination);//做完base64变换后放入2_base64_decode_location

    fclose(base64_decode_Destination);
#else
    //1_receive 生成文件
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);
#endif

    fclose(cut_head_Destination); // 关闭文件


    free(relative_dir);
    free(total_dir);
    free(name);
    //free(pureQRdata);//temp
    free(offset);
    return NO_ERROR;
}

int ProcessInThread::processEvt(recv_sm_evt_enum_t evt, void *evt_payload)
{
    return m_stateMachine->procEvt(evt, evt_payload);
}

int ProcessInThread::setTransmitStatus(TransmitStatus status)
{
    mTransStatus = status;
    return NO_ERROR;
}

TransmitStatus ProcessInThread::getTransmitStatus()
{
    return mTransStatus;
}


void ProcessInThread::CompleteDesPath()
{
    sprintf(ROOT_DIR, "%s", getenv("HOME"));
    sprintf(DES_BASE_LOCATION, "%s%s", ROOT_DIR ,DES_BASE_LOCATION_REL);

    ///==============NORMAL path================
    //DES
    sprintf(DES_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_LOCATION_REL);
    sprintf(DES_RECEIVE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_RECEIVE_LOCATION_REL);
    sprintf(DES_BASE64_DECODE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_BASE64_DECODE_LOCATION_REL);
    sprintf(DES_CAT_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_CAT_LOCATION_REL);
    //DES INI
    sprintf(DES_RECV_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_RECV_INI_LOCATION_REL);
    sprintf(DES_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_LOCATION_REL);
    sprintf(DES_INI_FILE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_FILE_LOCATION_REL);
    sprintf(DES_INI_FOLD_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_FOLD_LOCATION_REL);
    sprintf(DES_INI_FILE, "%s%s", DES_BASE_LOCATION ,DES_INI_FILE_REL);
    sprintf(DES_INI_FOLD, "%s%s", DES_BASE_LOCATION ,DES_INI_FOLD_LOCATION_REL);

    ///==============UDP path===================
    //DES
    sprintf(DES_UDP_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_LOCATION_REL);
    sprintf(DES_UDP_RECEIVE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_RECEIVE_LOCATION_REL);
    sprintf(DES_UDP_BASE64_DECODE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_BASE64_DECODE_LOCATION_REL);
    sprintf(DES_UDP_CAT_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_CAT_LOCATION_REL);
    //DES INI
    sprintf(DES_UDP_RECV_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_RECV_INI_LOCATION_REL);
    sprintf(DES_UDP_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_LOCATION_REL);
    sprintf(DES_UDP_INI_FILE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FILE_LOCATION_REL);
    sprintf(DES_UDP_INI_FOLD_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FOLD_LOCATION_REL);
    sprintf(DES_UDP_INI_FILE, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FILE_REL);
    sprintf(DES_UDP_INI_FOLD, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FOLD_LOCATION_REL);
}

