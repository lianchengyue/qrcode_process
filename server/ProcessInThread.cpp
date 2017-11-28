//Auther:fanlinqing
//负责接受碎片线程
//包括ini碎片与正文碎片

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

extern char *md5sumStr; //ini文件中保存的内容: md5sum
extern char *pathStr;  //ini文件中保存的内容:path
extern char *dateStr; //ini文件中保存的内容: date
extern char *nameStr; //ini文件中保存的内容: name
extern int MsgType;

ProcessInThread::ProcessInThread()
{
    m_stateMachine = RecvStateMachine::getInstance();

    CompleteDesPath();

    ini_traversal_flag = 1;
    fragment_traversal_flag = 1;
    type_whole = NORMAL;

    INI_prestart_flag = 1;

    //for test, //接受完碎片，开始处理 flq
    ///processEvt(RECV_SM_EVT_FRAG_START, NULL);
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
        //////////IDLE，将所有状态重置为激活///////
        ini_traversal_flag = 0;
        fragment_traversal_flag = 0;

        INI_prestart_flag = 0;
        return NO_ERROR;
    }

    //接收配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_PRESTART))
    {
        LOG_DBG("switch:TRANSMIT_PRESTART\n");
        setTransmitStatus(PRESTART);
        //////////新的帧到来，将所有状态重置为激活///////
        //ini_traversal_flag = 1;
        //fragment_traversal_flag = 1;

        return NO_ERROR;
    }

    else if(0 == strcmp(QRdata, TRANSMIT_PREEND))
    {
        LOG_DBG("switch:TRANSMIT_PREEND\n");
        setTransmitStatus(PREEND);
        return NO_ERROR;
    }
#if 1
    //QRdata

    //处理配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_START))
    {
        LOG_DBG("switch:TRANSMIT_START\n");
        mTransStatus = getTransmitStatus();
        setTransmitStatus(PREEND);

        //delay(300)//ms

        //创建所有的文件目录,（不包括碎片目录）
        ///遍历完后拼接fold.ini
        ///===============================发消息，遍历ini并恢复，做处理================================///
        if(0 == ini_traversal_flag)
        {
            ini_traversal_flag ++;
            processEvt(RECV_SM_EVT_INI_START, NULL);
            ///ini_traversal_flag ++;  //move to top to stop extra processEvt()
        }

        setTransmitStatus(TRANSMITTING);
    }

    //开始遍历1_receiver正文
    else if(0 == strcmp(QRdata, TRANSMIT_END))
    {
        LOG_DBG("switch:TRANSMIT_END\n");
        setTransmitStatus(END);
        //delay(300)//ms
        ///遍历待拼接文件
        ///=====================================遍历文件======================================///
        if(0 == fragment_traversal_flag)
        {
            LOG_DBG("FRAGMENT_TRAVERSAL, fragment_traversal_flag = %d\n", fragment_traversal_flag);
            fragment_traversal_flag ++;
            processEvt(RECV_SM_EVT_FRAG_START, NULL);
            setTransmitStatus(IDLE);
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

//INI内容解析只解析一次（只支持1帧）
#if 0
            des_prestart_content_receiver(QRdata);
#else
            if(0 == INI_prestart_flag)
            {
                INI_prestart_flag++;
                des_prestart_content_receiver(QRdata);
            }
#endif
        }

        //发送内容
        else if(TRANSMITTING == getTransmitStatus())
        {
            ///接收正文并放到目标目录
            ////LOG_DBG("RECEIVE_CONTENT\n");
            des_start_content_receiver(QRdata);
        }

        else{
            LOG_ERR("Drop a frame!!!\n");
            des_start_content_receiver(QRdata);
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
    //拆分relative_dir，获取日期，文件名与配置文件名
    char *date  = new char[NAME_MAX];
    char *d_name  = new char[NAME_MAX];
    char *ini_name  = new char[NAME_MAX];
    char typeStr[16] = {0};
    char *md5sum  = new char[MD5SUM_MAX];
    int type = NORMAL;

    LOG_DBG("des_prestart_content_receiver\n");
    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    memset(date, 0, NAME_MAX);
    memset(d_name, 0, NAME_MAX);
    memset(ini_name, 0, NAME_MAX);

    *offset = 0;
    //temp
    //pureQRdata = new char[QRDATA_SIZE];//temp
    //memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp   ///name:X0 //relative_dir:config/
    if(NO_ERROR != ret)
    {
        LOG_ERR("%s, cutQRdata err, ret=%d\n",__func__, ret);

        delete(relative_dir);
        delete(total_dir);
        delete(name);
        //delete(pureQRdata);//temp
        delete(offset);

        delete(date);
        delete(d_name);
        delete(ini_name);

        return -3;
    }

    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;

    //拆分relative_dir，获取日期，文件名与配置文件名
    cutINIHeadData(relative_dir, date, d_name, ini_name, typeStr, md5sum);
    LOG_DBG("des_prestart_content_receiver, relative_dir=%s, dateStr=%s, nameStr=%s, ini_name=%s\n", relative_dir, dateStr, nameStr, ini_name);
    if(NO_ERROR != ret)
    {
        LOG_ERR("%s, cutINIHeadData err, ret=%d\n",__func__, ret);

        delete(relative_dir);
        delete(total_dir);
        delete(name);
        //delete(pureQRdata);//temp
        delete(offset);

        delete(date);
        delete(d_name);
        delete(ini_name);
        return -4;
    }

    sscanf(typeStr,"%d",&type); //or atoi()
    type_whole = type;

    if(UDP == type)
    {
        sprintf(total_dir, "%s%s", DES_UDP_RECV_INI_LOCATION, date); ///生成到该目录 relative_dir:config/,应改为config.ini/
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s", DES_UDP_RECV_INI_LOCATION, date, d_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s/%s", DES_UDP_RECV_INI_LOCATION, date, d_name, ini_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);///生成到该目录 relative_dir:config/,应改为config.ini/
    }else if(NORMAL == type)
    {
        sprintf(total_dir, "%s%s", DES_RECV_INI_LOCATION, date); ///生成到该目录 relative_dir:config/,应改为config.ini/
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s", DES_RECV_INI_LOCATION, date, d_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s/%s", DES_RECV_INI_LOCATION, date, d_name, ini_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);///生成到该目录 relative_dir:config/,应改为config.ini/
    }

    //文件夹是否存在
    /*
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }*/

    memset(total_dir, 0, PATH_MAX);
    if (UDP == type)
    {//name:X0
        sprintf(total_dir, "%s%s/%s/%s/%s", DES_UDP_RECV_INI_LOCATION, date, d_name, ini_name, name);
    } else if (NORMAL == type)
    {
        sprintf(total_dir, "%s%s/%s/%s/%s", DES_RECV_INI_LOCATION, date, d_name, ini_name, name);
    }

    FILE *INI_frag_Destination = fopen(total_dir, "wb"); //ab+;
    //write raw ini fragment
    int size = fwrite(QRdata, 1, strlen(QRdata), INI_frag_Destination);//QRdata
    fclose(INI_frag_Destination); // 关闭文件

    //write processed ini fragment
    memset(total_dir, 0, PATH_MAX);
    if(UDP == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_UDP_INI_LOCATION, date); ///生成到该目录 relative_dir:config/,应改为config.ini/
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s", DES_UDP_INI_LOCATION, date, d_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s/%s", DES_UDP_INI_LOCATION, date, d_name, ini_name);
        //mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);///生成到该目录 relative_dir:config/,应改为config.ini/
    }else if(NORMAL == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_INI_LOCATION, date); ///生成到该目录 relative_dir:config/,应改为config.ini/
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s", DES_INI_LOCATION, date, d_name);
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        sprintf(total_dir, "%s%s/%s/%s", DES_INI_LOCATION, date, d_name, ini_name);
        //mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);///生成到该目录 relative_dir:config/,应改为config.ini/
    }

    FILE *INI_Destination = fopen(total_dir, "wb");
    size = fwrite(pureQRdata, 1, strlen(pureQRdata), INI_Destination);
    fclose(INI_Destination);

    //给全局变量赋值，遍历碎片之前使用
    strcpy(dateStr, date);
    strcpy(nameStr, d_name);
    MsgType = type;
    strcpy(md5sumStr, md5sum);

    LOG_DBG("des_prestart_content_receiver, dateStr=%s, nameStr=%s\n",dateStr, nameStr);
    //给全局变量赋值，遍历碎片之前使用end


    delete(relative_dir);
    delete(total_dir);
    delete(name);
    //delete(pureQRdata);//temp
    delete(offset);

    delete(date);
    delete(d_name);
    delete(ini_name);
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
    //拆分relative_dir，获取日期，文件名与配置文件名
    char *date  = new char[NAME_MAX];
    char *d_name  = new char[NAME_MAX];
    int type = NORMAL;

    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    //拆分relative_dir，获取日期，文件名与配置文件名
    memset(date, 0, NAME_MAX);
    memset(d_name, 0, NAME_MAX);


    *offset = 0;
    //temp
    //pureQRdata = new char[QRDATA_SIZE];//temp
    //memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp
    if(NO_ERROR != ret){

        LOG_LOW("%s, cutQRdata err, ret=%d\n",__func__, ret);

        delete(relative_dir);
        delete(total_dir);
        delete(name);
        delete(offset);

        delete(date);
        delete(d_name);
        return -3;
    }
    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;

    //拆分relative_dir，获取日期，文件名与配置文件名
    ret = cutHeadData(relative_dir, date, d_name);

    ////added by flq,防止接收不到配置文件时，碎片泄漏到根目录
    if(NO_ERROR != ret)
    {
        LOG_ERR("%s, cutHeadData err, ret=%d\n",__func__, ret);
        delete(relative_dir);
        delete(total_dir);
        delete(name);
        delete(offset);

        delete(date);
        delete(d_name);
        return -4;
    }
    ///added end by flq,防止接收不到配置文件时，碎片泄漏到根目录

    type = type_whole;

    if(UDP == type)
    {
        //1
        //cddir(DES_UDP_RECEIVE_LOCATION);
        sprintf(total_dir, "%s%s", DES_UDP_RECEIVE_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s", DES_UDP_RECEIVE_LOCATION, date, d_name);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //2
        //cddir(DES_UDP_BASE64_DECODE_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_UDP_BASE64_DECODE_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s", DES_UDP_BASE64_DECODE_LOCATION, date, d_name);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //3
        //cddir(DES_UDP_CAT_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_UDP_CAT_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //4
        //cddir(DES_UDP_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_UDP_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
    }
    else if(NORMAL == type)
    {
        //1
        //cddir(DES_RECEIVE_LOCATION);
        sprintf(total_dir, "%s%s", DES_RECEIVE_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s", DES_RECEIVE_LOCATION, date, d_name);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //2
        //cddir(DES_BASE64_DECODE_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_BASE64_DECODE_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s", DES_BASE64_DECODE_LOCATION, date, d_name);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //3
        //cddir(DES_CAT_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_CAT_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }

        //4
        //cddir(DES_LOCATION);
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s", DES_LOCATION, date);
        if(0 != access(total_dir, F_OK))
        {
            mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
    }

    if (UDP == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s/%s", DES_UDP_RECEIVE_LOCATION, date, d_name, name);
    } else if (NORMAL == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s/%s", DES_RECEIVE_LOCATION, date, d_name, name);
    }

    FILE *cut_head_Destination = fopen(total_dir, "wb"); //ab+;

#if 1  //#ifdef DES_DECODE
///    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);   //无报头
    int size = fwrite(QRdata, 1, strlen(QRdata), cut_head_Destination);      //有报头
    //解码并生成文件
    memset(total_dir, 0, PATH_MAX);

    //在2目录中创建接收传输文件的文件夹
    if(UDP == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s/", DES_UDP_BASE64_DECODE_LOCATION, date, d_name);
    }
    else if(NORMAL == type)
    {
        memset(total_dir, 0, PATH_MAX);
        sprintf(total_dir, "%s%s/%s/", DES_BASE64_DECODE_LOCATION, date, d_name);
    }

    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }
    strcat(total_dir, name);

    FILE *base64_decode_Destination = fopen(total_dir, "w");  //home/montafan/QRcodeGrab/destination/2_base64_decode_location/hu.jpg/X0
    decode(pureQRdata, base64_decode_Destination);//做完base64变换后放入2_base64_decode_location
    fclose(base64_decode_Destination);
#else
    //1_receive 生成文件
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);
#endif

    fclose(cut_head_Destination); // 关闭文件


    delete(relative_dir);
    delete(total_dir);
    delete(name);
    //delete(pureQRdata);//temp
    delete(offset);

    delete(date);
    delete(d_name);
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

