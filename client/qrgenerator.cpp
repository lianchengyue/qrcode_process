//queue方式的信号槽是通过 postEvent()
//if(m_NormalTh->isRunning() && m_NormalTh->is_pause)
/*
定义一个queue的变量     queue<Type> M
查看是否为空范例        M.empty()    是的话返回1，不是返回0;
从已有元素后面增加元素   M.push()
输出现有元素的个数      M.size()
显示第一个元素          M.front()
显示最后一个元素        M.back()
清除第一个元素          M.pop()
*/
#include "qrgenerator.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>

//遍历文件
#include <dirent.h>
#include<sys/stat.h>
#include <unistd.h>

#include "include/DirPath.h" //dir
//接收端文件生成
#include "include/fileParameters.h"
#include "instructions/stats.h"

#include <string>

#ifdef USE_ACTIVEMQ
#include "instructions/ActiveMQConsumer.h"
#include "instructions/ActiveMQConsumer.h"
#include <json/json.h>
#endif

#include "instructions/inirw.h"

using namespace std;

//#define WAIT_FRAME_COUNT 20
//#define DISPLAY_INTERVAL 100000  //unit: us

char *pdesBuf;  //定义文件指针
vector<string> vecINIString;
vector<string> vecString;
#ifdef USE_ACTIVEMQ
vector<activeMQVec> UDPVec;
vector<activeMQVec> NormalVec;
vector<string> UDPrawVec;
vector<string> NormalrawVec;

vector<string> ResetVec;  //reset event
#endif
int taskcnt;//记录待处理的任务数
bool taskrunning;

//与信号量不同的程序
QWaitCondition UDPRunning; //QWaitCondition允许在一定条件下触发其它多个线程
QWaitCondition NormalRunning;
QMutex mutex;

queue<int> evt_UDP_queue;
queue<int> evt_Normal_queue;

bool isUDP;
bool isNormal;


//QThread
//Thread用来控制文件的开始与结束
NormalThread::NormalThread(QObject *parent)
{
    number = 0;

    qRegisterMetaType<activeMQVec>("activeMQVec>");
}

NormalThread::~NormalThread()
{

}

void NormalThread::run()
{
///断点续传
#if 0
    sleep(1);//temp add, a.execute后再启动线程
    emit UpdateSignal(number);
    //number++;

//https://stackoverflow.com/questions/9075837/pause-and-resume-a-qthread
    //while(someCondition) // gues it's your loop
    for(int i = 0; i<2; i++)  //50
    {
         sync.lock();
         if(is_pause)
             pauseCond.wait(&sync); // in this place, your thread will stop to execute until someone calls resume
         sync.unlock();

         // do your operation
         printf("Normal,Running%d\n", i);
         usleep(200000);
    }

    //线程执行完毕，发送通知
    //全局queue
    sync.lock();
    if(!evt_UDP_queue.empty())
    {
        int k = evt_UDP_queue.front();//拿到queue的值
        evt_UDP_queue.pop();
        //开始处理

        //处理完毕
        evt_UDP_queue.pop();
    }
    sync.unlock();

    //收到高优先级的抢占消息
#elif 0
    sleep(1);//temp add, a.execute后再启动线程
    emit UpdateSignal(number);
#else
    #ifdef USE_ACTIVEMQ
    sleep(1);//temp add, a.execute后再启动线程

    while(1)
    {
        //处理所有的UDP消息
        auto udpIter = UDPrawVec.begin();
        while (UDPrawVec.size()>0)
        {
            if(taskrunning)
            {
                sleep(5);
                continue;
            }

            printf("===============start process UDP Signal=========\n");
            //emit ProcessMsgSignal((activeMQVec)*udpIter);
            //emit ProcessMsgSignal(UDPrawVec[0]);
            QString transmitUDP = QString::fromStdString(UDPrawVec[0]);
            emit ProcessMsgSignal(transmitUDP);
            udpIter = UDPrawVec.erase(udpIter);

            continue;//优先处理高优先级
        }

        //处理一个normal消息
        auto normalIter = NormalrawVec.begin();
        if(NormalrawVec.size()>0)
        {
            if(taskrunning)
            {
                sleep(5);
                continue;
            }

            printf("===============start process Nomal Signal=========\n");
            ///emit ProcessMsgSignal((activeMQVec)*normalIter);
            //emit ProcessMsgSignal(NormalrawVec[0]);
            QString transmitNormal = QString::fromStdString(NormalrawVec[0]);
            emit ProcessMsgSignal(transmitNormal);
            normalIter = NormalrawVec.erase(normalIter);

            continue;//normal > reset
        }

        //处理一个reset消息
        auto resetIter = ResetVec.begin();
        if(ResetVec.size()>0)
        {
            if(taskrunning)
            {
                sleep(5);
                continue;
            }

            printf("===============start process Reset Signal=========\n");
            QString transmitReset = QString::fromStdString(ResetVec[0]);
            emit ProcessResetMsgSignal(transmitReset);
            //emit ProcessResetMsgSignal();
            resetIter = ResetVec.erase(resetIter);
        }

        ///给接收段留处理时间,后续根据文件size做成动态
        sleep(8);
    }
    #endif

#endif

}
#if 1
void NormalThread::resume()
{
    sync.lock();
    is_pause = false;
    sync.unlock();
    pauseCond.wakeAll();
}
void NormalThread::pause()
{
    sync.lock();
    is_pause = true;
    sync.unlock();
}
#endif

void NormalThread::ResetSlot()
{
    number = 0;
    emit UpdateSignal(number);
}


//UDP Thread
UDPThread::UDPThread(QObject *parent)
{

}

UDPThread::~UDPThread()
{

}

//UDP使用signal/slot机制，顺序播放,不需要休眠唤醒
void UDPThread::run()
{

    sync.lock();
    evt_UDP_queue.push(1);
    sync.unlock();

    emit UDPSignal();

#if 0
    for(int i = 0; i<50; i++)
    {
        usleep(200000);
        printf("UDP,Running%d\n", i);

        if(2 ==i)
        {
        }
    }
#endif
}

void UDPThread::resume()
{
    sync.lock();
    is_pause = false;
    sync.unlock();
    pauseCond.wakeAll();
}
void UDPThread::pause()
{
    sync.lock();
    is_pause = true;
    sync.unlock();
}


///遍历3文件夹中，split+base64 encode完后的碎片,把读到的绝对路径保存到vector中
void src_traversal_and_push_fragment_to_vector(string dir, bool is_ini, int depth) //get_file_to_generate_qrcode
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径
    string total_dir;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir.c_str())))
    {
        fprintf(stderr,"src_traversal_and_push_fragment_to_vector(), can not open dir:%s\n",dir.c_str());
        return;
    }

    //切换到这个目录
    chdir(dir.c_str());

    //遍历这个目录下的所有文件
    while(NULL != (enty = readdir(Dp) ))
    {
        //通过文件名，得到详细文件信息
        lstat(enty->d_name,&statbuf);
        //判断是不是目录
        if(S_ISDIR(statbuf.st_mode))
        {
            //当前目录和上一目录过滤掉
            if(0 == strcmp(".",enty->d_name) ||
                          0 == strcmp("..",enty->d_name))
            {
                continue;
            }

            total_dir = dir + enty->d_name + "/";
            //输出当前文件名
            printf("%*s%s/\n",depth," ",enty->d_name);

            //继续递归调用
            src_traversal_and_push_fragment_to_vector(total_dir, is_ini, depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输出当前目录名
            //printf("%*s%s/\n",depth," ",enty->d_name);
            //get文件名
            if(is_ini)
            {
                vecINIString.push_back(total_dir);
                ///std::vector<std::array<char,255>>* vecString = reinterpret_cast<std::vector<std::array<char,255>>*>(total_dir);
            }
            else
            {
                vecString.push_back(total_dir);
            }

        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

}



#if 1
QRGenerator::QRGenerator(QWidget *parent) : QWidget(parent)
#else
QRGenerator::QRGenerator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
#endif
{
    //将不识别的参数结构进行注册，让QT能够识别
    #ifdef USE_ACTIVEMQ
    qRegisterMetaType<activeMQVec>("activeMQVec>");
    qRegisterMetaType<activeMQVec>("activeMQVec&>");
    qRegisterMetaType<ActiveMQVec>("ActiveMQVec>");
    #endif

    ///ui->setupUi(this);
    setWindowTitle("传输二维码");
    setWindowIcon(QIcon(":/new/prefix1/Image/ICON.ico"));

    qr = NULL;

    //thread
    m_UDPTh = new UDPThread;
    m_NormalTh = new NormalThread;
    //消息监听线程
    m_activeMQTh = new activeMQThread;

    taskrunning = false;
    taskcnt = 0;

    //empty
    printf("set TRANSMIT_IDLE\n");
    setHighLevelString(TRANSMIT_IDLE);

    //for test
    /*
    char content[2048] = {0};
    FILE *TestFile = fopen("/home/montafan/X0", "rb");
    fread(content,1,512,TestFile);

    setString(content);
    */
    //for test end

    CompleteSrcPath();

    ////UDP与Normal两个线程
    #if 0
    //connect(m_UDPTh, SIGNAL(UDPSignal(int)), this, SLOT(UpdateSlot(int)));//显示二维码 UpdateSlot(int)
    connect(m_UDPTh, SIGNAL(UDPSignal()), this, SLOT(processUDPEventSlot()));
    //connect(this, SIGNAL(ResetSignal()), m_UDPTh, SLOT(ResetSlot()));

    connect(m_NormalTh, SIGNAL(UpdateSignal(int)), this, SLOT(UpdateSlot(int)));//显示二维码 UpdateSlot(int)
    ///connect(m_NormalTh, SIGNAL(UpdateSignal(int)), this, SLOT(processNormalEventSlot()));
    connect(this, SIGNAL(ResetSignal()), m_NormalTh, SLOT(ResetSlot()));
    #endif

    #ifdef USE_ACTIVEMQ
    /////connect(m_NormalTh, SIGNAL(ProcessMsgSignal(activeMQVec)), this, SLOT(ProcessMsg(activeMQVec)));//显示二维码 UpdateSlot(int)
    connect(m_NormalTh, SIGNAL(ProcessMsgSignal(QString)), this, SLOT(ProcessMsgQ(QString)));//显示二维码 UpdateSlot(int)
    connect(m_NormalTh, SIGNAL(ProcessResetMsgSignal(QString)), this, SLOT(ProcessMsgReset()));
    #endif

    ///==============================start Thread=====================================/
#if 0
    ///后续改为在收到传输完成消息后调用 added by flq
    printf("1111111111\n");
    //m_UDPTh->start();
    printf("2222222222\n");
    m_NormalTh->start();
    printf("3333333333\n");

    //sleep(3);
    m_NormalTh->pause();
    //sleep(3);
    m_NormalTh->resume();
#else
    m_NormalTh->start();
#endif

#ifdef USE_ACTIVEMQ
    //消息监听线程
    m_activeMQTh->start();
#endif


    ///setDisplayInterval();
    time_interval = getDisplayInterval();
    code_level = getQRCodeLevel();
    reset_wait_time = getResetWait();
    //printf("QR_ECLEVEL_L=%d, QR_ECLEVEL_M=%d, Q=%d, H=%d\n\n", QR_ECLEVEL_L, QR_ECLEVEL_M, QR_ECLEVEL_Q, QR_ECLEVEL_H);
    //qDebug() << QString("main thread id:") << QThread::currentThreadId();
    //UDPRunning.wait(&mutex);
}

QRGenerator::~QRGenerator()
{
    //m_UDPTh->stopImmediately();
    m_UDPTh->quit();

    //m_NormalTh->stopImmediately();
    m_NormalTh->quit();

    if(qr != NULL)
    {
        QRcode_free(qr);
    }
    ///delete ui;
}

int QRGenerator::getQRWidth() const
{
    if(qr != NULL)
    {
        return qr->width;
    }
    else
    {
        return 0;
    }
}

void QRGenerator::setString(QString str)
{
    qstring = str;
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
    //生成二维码
    qr = QRcode_encodeString(qstring.toStdString().c_str(),
        1,
        (QRecLevel)code_level, //QR_ECLEVEL_L,
        QR_MODE_8,
        1);

    usleep(time_interval);
    repaint();
}

void QRGenerator::setHighLevelString(QString str)
{
    qstring = str;
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
    //生成二维码
    qr = QRcode_encodeString(qstring.toStdString().c_str(),
        1,
        (QRecLevel)3, //QR_ECLEVEL_L,
        QR_MODE_8,
        1);

    usleep(time_interval);
    repaint();
}


QSize QRGenerator::sizeHint()  const
{
    QSize s;
    if(qr != NULL)
    {
        int qr_width = qr->width > 0 ? qr->width : 1;
        s = QSize(qr_width * 4, qr_width * 4);
    }
    else
    {
        s = QSize(50, 50);
    }
    return s;
}

QSize QRGenerator::minimumSizeHint()  const
{
    QSize s;
    if(qr != NULL)
    {
        int qr_width = qr->width > 0 ? qr->width : 1;
        s = QSize(qr_width, qr_width);
    }
    else
    {
        s = QSize(50, 50);
    }
    return s;
}

int QRGenerator::getDisplayInterval()
{
    char file[256];

    char *sect;
    char *key;
    int intval;

    memset(file, 0, 256);

    sprintf(file, "%s/dispInterval.ini", getenv("HOME"));
    //LOG_DBG("load DISPLAY_INTERVAL file %s\n", file);

    iniFileLoad(file);

    sect = "dispInterval";
    key = "interval";
    intval = iniGetInt(sect, key, 100000); //default:10fps 100000
    LOG_DBG("[%s] %s = %d\n", sect, key, intval);

    //小于3fps,设为3fps
    if(intval > 333333)
    {
        return 333333;
    }
    //大于30fps,设为30fps
    else if (intval < 33333)
    {
        return 33333;
    }
    else
    {
        LOG_ERR("DISPLAY_INTERVAL=%d\n", intval);
        return intval;
    }
}

void QRGenerator::setDisplayInterval()
{/*
    char file[256];
    char value[34];

    memset(file, 0, 256);
    memset(value, 0, 34);

    sprintf(file, "%s/dispInterval.ini", getenv("HOME"));
    LOG_DBG("load DISPLAY INTERVAL file %s\n", file);

    sprintf(value, "dispInterval");
    iniFileLoad(file);

    iniSetInt(value, "DisplayInterval", 66662, 0);//size

    iniSetInt("QRCodeLevel", "CodeLevel", 2, 0);//size
    iniSetInt("ViewerStatus", "onoff", 1, 0);//size
*/
}

int QRGenerator::getQRCodeLevel()
{
    char file[256];

    char *sect;
    char *key;
    int intval;

    memset(file, 0, 256);

    sprintf(file, "%s/dispInterval.ini", getenv("HOME"));
    //LOG_DBG("load QRCode Level file %s\n", file);

    iniFileLoad(file);

    sect = "QRCodeLevel";
    key = "CodeLevel";
    intval = iniGetInt(sect, key, 0);
    LOG_DBG("[%s] %s = %d\n", sect, key, intval);

    //小于3fps,设为3fps
    if(intval < 0)
    {
        return 0;
    }
    //大于30fps,设为30fps
    else if (intval > 3)
    {
        return 3;
    }
    else
    {
        LOG_ERR("QRCodeLevel=%d\n", intval);
        return intval;
    }
}

int QRGenerator::getResetWait()
{
    char file[256];

    char *sect;
    char *key;
    int intval;

    memset(file, 0, 256);

    sprintf(file, "%s/dispInterval.ini", getenv("HOME"));

    iniFileLoad(file);

    sect = "ResetWaitTime";
    key = "wait_time";
    intval = iniGetInt(sect, key, 10);
    LOG_DBG("[%s] %s = %d\n", sect, key, intval);

    //小于3fps,设为3fps
    if(intval < 0)
    {
        return 10;
    }
    //大于60s,设为60s
    else if (intval > 60)
    {
        LOG_ERR("Reset wait time could not set more than %d second!!!\n", intval);
        return 60;
    }
    else
    {
        LOG_DBG("ResetWaitTime=%d\n", intval);
        return intval;
    }
}

bool QRGenerator::saveImage(QString fileName, int size)
{
    if(size != 0 && !fileName.isEmpty())
    {
        QImage image(size, size, QImage::Format_Mono);
        QPainter painter(&image);
        QColor background(Qt::white);
        painter.setBrush(background);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, size, size);
        if(qr != NULL)
        {
            draw(painter, size, size);
        }
        return image.save(fileName);
    }
    else
    {
        return false;
    }
}

void QRGenerator::draw(QPainter &painter, int width, int height)
{
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    const int qr_width = qr->width > 0 ? qr->width : 1;
    //避免拉伸
    double scale_x = (height-40) / qr_width;   //width
    double scale_y = (height-40) / qr_width;
    for( int y = 0; y < qr_width; y ++)
    {
        for(int x = 0; x < qr_width; x++)
        {
            unsigned char b = qr->data[y * qr_width + x];
            if(b & 0x01)
            {
                QRectF r(200 + x * scale_x, 20+y * scale_y, scale_x, scale_y);  //middle:180
                painter.drawRects(&r, 1);
            }
        }
    }
}

void QRGenerator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    //printf("FFF(%d,%d)\n",width(), height());//1920,1080
    painter.drawRect(0, 0, width(), height());
    if(qr != NULL)
    {
        draw(painter, width(), height());
    }
}

/////////////////////////////thread////////////////////////////////
//thread
void QRGenerator::StartSlot()
{
    m_NormalTh->start();
}

void QRGenerator::StopSlot()
{
    m_NormalTh->terminate();
}

void QRGenerator::UpdateSlot(int num)
{
    int is;
    int ie;
    printf("UpdateSlot,Thread\n");

    mutex.lock();

    //处理待发送文件
    #ifdef USE_ACTIVEMQ
    //文件遍历
    //2:LZO压缩，3:split操作
    printf("file_select()\n");
    ////file_select(msg);

    //报头遍历
    printf("ini_traversal()\n");
    ini_traversal();
    //遍历片段
    printf("fragment_traversal()\n");
    //fragment_traversal();
    //fragment_selected_traversal();
    printf("fragment_traversal end()\n");
    #endif
#if 1
    ///后续改为在收到传输完成消息后调用 added by flq
    //文件夹信息
    char *folderdir = SRC_INI_FOLD_FRAG_LOCATION;
    std::string folder_str =  folderdir;
    src_traversal_and_push_fragment_to_vector(folder_str, true, 0);  //isINI=true

    //文件属性信息
    char *configdir = SRC_INI_FILE_FRAG_LOCATION;
    std::string config_str =  configdir;
    src_traversal_and_push_fragment_to_vector(config_str, true, 0);

    //内容碎片信息
    //直接遍历4_base64_encode_location/文件,路径保存到vector
    char *topdir = SRC_BASE64_ENCODE_LOCATION;
    std::string topdir_str =  topdir;
    src_traversal_and_push_fragment_to_vector(topdir_str, false, 0);
    ///added end
 #endif

    printf("NEW TRANSMIT_PRESTART\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_PRESTART);
    }

    printf("NEW TRANSMIT ini\n");
    ///播放报头
    //here to add content
    //...
    for (size_t i = 0; i < vecINIString.size(); i++) {

        std::string s = vecINIString[i];
        FILE *pFile=fopen(s.c_str(),"rb"); //rb二进制, rt文本文件

        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile);
        pdesBuf[len]=0;

        fclose(pFile);
        //added end

        //added by flq
        //第一帧图像多播放两次，防止高速下第一帧切换时候掉帧
        if(0 == i)
        {
            setString(pdesBuf);
            setString(pdesBuf);
        }
        //added end

        //显示二维码
        setString(pdesBuf);
        ///usleep(100);
        delete(pdesBuf);
    }
    //added end

    printf("NEW TRANSMIT_PREEND\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_PREEND);
    }

    printf("NEW TRANSMIT_START\n");
    ///播放开始二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_START);
    }

    printf("TRANSMIT content\n");

    for (size_t i = 0; i < vecString.size(); i++) {

        std::string s = vecString[i];
        FILE *pFile=fopen(s.c_str(),"rb"); //rb二进制, rt文本文件

        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile);
        pdesBuf[len]=0;

        fclose(pFile);
        //added end

        //added by flq
        //第一帧图像多播放两次，防止高速下第一帧切换时候掉帧
        if(0 == i)
        {
            setString(pdesBuf);
            setString(pdesBuf);
        }
        //added end

        //显示二维码
        setString(pdesBuf);
        ///usleep(100);
        delete(pdesBuf);
    }

    ///播放结束二维码
    printf("TRANSMIT_END\n");
    for (ie = 0; ie < WAIT_FRAME_COUNT+3; ie++) {
        setString(TRANSMIT_END);
    }

    printf("TRANSMIT_IDLE\n");
    setHighLevelString(TRANSMIT_IDLE);

    mutex.unlock();
}

#ifdef USE_ACTIVEMQ
void QRGenerator::ProcessMsg(activeMQVec msg)
{
    printf("SSSSSSSSSSSSSSSSSSSSSSSS\n");
    int i =1;
}

void QRGenerator::ProcessMsgReset()
{
    int is;
    //process Reset

    ///播放报头二维码
    setString(TRANSMIT_RESET);
    //重启等待时间默认为10秒，范围0-60秒
    reset_wait_time = getResetWait();
    sleep(reset_wait_time);
    printf("=====RESET END======\n");
    setHighLevelString(TRANSMIT_IDLE);
}

void QRGenerator::ProcessMsgQ(QString msg)
{
    int is;
    int ie;
    printf("ProcessMsgQ,Thread\n");

    mutex.lock();

    taskrunning = true;

    //格式转换
    ActiveMQVec receivedMessage;
    //str = qstr.toStdString();
    //qstr = QString::fromStdString(str);
    std::string ssmg = msg.toStdString();

    Json::Reader reader(Json::Features::strictMode());
    Json::Value parseData;

    if (reader.parse(ssmg.c_str(), parseData))
    {
       std::string filename = parseData.get("filename", 0).asString();
       std::string date = parseData.get("date", 0).asString();
       int size = parseData.get("size", 0).asInt();
       std::string md5sum = parseData.get("md5sum", 0).asString();
       int type = parseData.get("type", 0).asInt();
       printf("parseJSON(): filename:%s, date:%s, size:%d, md5sum:%s, type=%d\n", filename.c_str(), date.c_str(), size, md5sum.c_str(), type);
       if(UDP == type)
           printf("\n\n------------------UDP------------------\n");
       else if (NORMAL == type)
           printf("\n\n--------------- -Normal----------------\n");

       //将读取到的消息写到本地vector中
       receivedMessage.filename = filename;
       receivedMessage.size = size;
       receivedMessage.date = date;
       receivedMessage.md5sum = md5sum;
       receivedMessage.type =type;
    }

    //处理待发送文件
    #ifdef USE_ACTIVEMQ
    //文件遍历
    //2:LZO压缩，3:split操作
    printf("file_select()\n");
    file_select(receivedMessage, ssmg.c_str());

    //报头遍历
    printf("ini_traversal()\n");
    //ini_traversal();
    ini_select(receivedMessage);

    //遍历片段
    printf("fragment_selected_traversal()\n");
    fragment_selected_traversal(receivedMessage);
    printf("fragment_selected_traversal end()\n");
    #endif
    #if 1
    ///后续改为在收到传输完成消息后调用 added by flq
    //文件夹信息foldler.ini
    /*if(UDP == receivedMessage.type){
        char *folderdir = SRC_UDP_INI_FOLD_FRAG_LOCATION;
        std::string folder_str =  folderdir;
        src_traversal_and_push_fragment_to_vector(folder_str, true, 0);  //isINI=true
    }
    else if(NORMAL == receivedMessage.type){
        char *folderdir = SRC_INI_FOLD_FRAG_LOCATION;
        std::string folder_str =  folderdir;
         src_traversal_and_push_fragment_to_vector(folder_str, true, 0);  //isINI=true
    }*/

    //文件属性信息//config.ini
    char configdir[PATH_MAX]={0};
    if(UDP == receivedMessage.type){
        sprintf(configdir,"%s%s/%s/config/", SRC_UDP_INI_LOCATION, receivedMessage.date.c_str(), receivedMessage.filename.c_str());
        std::string config_str =  configdir;
        src_traversal_and_push_fragment_to_vector(config_str, true, 0);
    }

    else if(NORMAL == receivedMessage.type){
        sprintf(configdir,"%s%s/%s/config/",SRC_INI_LOCATION, receivedMessage.date.c_str(), receivedMessage.filename.c_str());
        std::string config_str =  configdir;
        src_traversal_and_push_fragment_to_vector(config_str, true, 0);
    }

    //内容碎片信息contents
    //直接遍历4_base64_encode_location/文件,路径保存到vector
    char topdir[PATH_MAX]={0};
    if(UDP == receivedMessage.type){
        sprintf(topdir,"%s%s/%s/",SRC_UDP_BASE64_ENCODE_LOCATION, receivedMessage.date.c_str(), receivedMessage.filename.c_str());
        std::string topdir_str =  topdir;
        src_traversal_and_push_fragment_to_vector(topdir_str, false, 0);
    }
    else if(NORMAL == receivedMessage.type){
        sprintf(topdir,"%s%s/%s/",SRC_BASE64_ENCODE_LOCATION, receivedMessage.date.c_str(), receivedMessage.filename.c_str());
        std::string topdir_str =  topdir;
        src_traversal_and_push_fragment_to_vector(topdir_str, false, 0);
    }
    ///added end
    #endif

    printf("NEW TRANSMIT_PRESTART\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
       setString(TRANSMIT_PRESTART);
    }

    printf("NEW TRANSMIT ini\n");
    ///播放报头
    //here to add content
    //...
    for (size_t i = 0; i < vecINIString.size(); i++) {

       std::string s = vecINIString[i];
       FILE *pFile=fopen(s.c_str(),"rb"); //rb二进制, rt文本文件

       fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
       int len=ftell(pFile); //获取文件长度
       pdesBuf=new char[len+1];
       rewind(pFile); //把指针移动到文件开头
       fread(pdesBuf,1,len,pFile);
       pdesBuf[len]=0;

       fclose(pFile);
       //added end

       //added by flq
       //第一帧图像多播放两次，防止高速下第一帧切换时候掉帧
       if(0 == i)
       {
           setString(pdesBuf);
           setString(pdesBuf);
       }
       //added end

       //显示二维码
       setString(pdesBuf);
       ///usleep(100);
       delete(pdesBuf);
    }
    //added end

    printf("NEW TRANSMIT_PREEND\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
       setString(TRANSMIT_PREEND);
    }

    printf("NEW TRANSMIT_START\n");
    ///播放开始二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
       setString(TRANSMIT_START);
    }

    printf("TRANSMIT content\n");

    for (size_t i = 0; i < vecString.size(); i++) {

       std::string s = vecString[i];
       FILE *pFile=fopen(s.c_str(),"rb"); //rb二进制, rt文本文件

       fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
       int len=ftell(pFile); //获取文件长度
       pdesBuf=new char[len+1];
       rewind(pFile); //把指针移动到文件开头
       fread(pdesBuf,1,len,pFile);
       pdesBuf[len]=0;

       fclose(pFile);
       //added end

       //added by flq
       //第一帧图像多播放两次，防止高速下第一帧切换时候掉帧
       if(0 == i)
       {
           setString(pdesBuf);
           setString(pdesBuf);
       }
       //added end

       //显示二维码
       setString(pdesBuf);
       delete(pdesBuf);
    }

    ///播放结束二维码
    printf("TRANSMIT_END\n");
    for (ie = 0; ie < WAIT_FRAME_COUNT+3; ie++) {
       setString(TRANSMIT_END);
    }

    //发送结果
#ifdef USE_ACTIVEMQ
    string JSONStr;
    if(vecString.size() > 0)
    {
        JSONStr = writeJSON_TransResult(receivedMessage.date.c_str(), receivedMessage.filename.c_str(), receivedMessage.type, TRANS_SUCCESS);
    }
    else
    {
        JSONStr = writeJSON_TransResult(receivedMessage.date.c_str(), receivedMessage.filename.c_str(), receivedMessage.type, TRANS_FAILED);
    }
    SetActiveMQMessage(JSONStr);
#endif


    printf("TRANSMIT_IDLE\n");
    setHighLevelString(TRANSMIT_IDLE);

//    sleep(8);
    sleep(3);

    //释放vector中的内容
    //vecINIString.swap(vector<string>);
    //vecString.swap(vector<string>);
    vector<std::string>().swap(vecINIString);
    vector<std::string>().swap(vecString);

    taskrunning = false;
    mutex.unlock();

    //taskcnt--;
}
#endif

void QRGenerator::ClearSlot()
{
    emit ResetSignal();
}

int QRGenerator::CompleteSrcPath()
{
    sprintf(ROOT_DIR, "%s", getenv("HOME"));
    sprintf(SRC_BASE_LOCATION, "%s%s", ROOT_DIR ,SRC_BASE_LOCATION_REL);

    ///==============NORMAL path================
    //SRC
    sprintf(SRC_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_LOCATION_REL);
    sprintf(SRC_LZO_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_LZO_LOCATION_REL);
    sprintf(SRC_SPLIT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_SPLIT_LOCATION_REL);
    sprintf(SRC_BASE64_ENCODE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_BASE64_ENCODE_LOCATION_REL);
    //SRC INI
    sprintf(SRC_INI_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_LOCATION_REL);
    sprintf(SRC_INI_FILE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FILE_LOCATION_REL);
    sprintf(SRC_INI_FOLD_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FOLD_LOCATION_REL);
    sprintf(SRC_INI_FRAGMENT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FRAGMENT_LOCATION_REL);
    sprintf(SRC_INI_FILE_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FILE_FRAG_LOCATION_REL);
    sprintf(SRC_INI_FOLD_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FOLD_FRAG_LOCATION_REL);

    ///==============UDP path================
    //SRC
    sprintf(SRC_UDP_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_LOCATION_REL);
    sprintf(SRC_UDP_LZO_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_LZO_LOCATION_REL);
    sprintf(SRC_UDP_SPLIT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_SPLIT_LOCATION_REL);
    sprintf(SRC_UDP_BASE64_ENCODE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_BASE64_ENCODE_LOCATION_REL);
    //SRC INI
    sprintf(SRC_UDP_INI_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_LOCATION_REL);
    sprintf(SRC_UDP_INI_FILE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FILE_LOCATION_REL);
    sprintf(SRC_UDP_INI_FOLD_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FOLD_LOCATION_REL);
    sprintf(SRC_UDP_INI_FRAGMENT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FRAGMENT_LOCATION_REL);
    sprintf(SRC_UDP_INI_FILE_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FILE_FRAG_LOCATION_REL);
    sprintf(SRC_UDP_INI_FOLD_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FOLD_FRAG_LOCATION_REL);

    return 0;
}

//ActiveMQ Producer
int QRGenerator::SetActiveMQMessage(string JSONStr)
{
    ///activemq::library::ActiveMQCPP::initializeLibrary();
    std::cout << "=====================================================\n";
    std::cout << "Starting produce message:" << std::endl;
    std::cout << "-----------------------------------------------------\n";

#ifdef USE_LOCAL_ACTIVEMQ_ADDR
    std::string ProducebrokerURI ="failover://(tcp://localhost:61616)";
#else
    std::string ProducebrokerURI ="failover://(tcp://114.55.4.189:61616)";
#endif
    unsigned int numMessages = 1;
    std::string destURI = "transmit.queue";

    bool useTopics = false;
    ActiveMQMsgProducer producer( ProducebrokerURI, numMessages, destURI, useTopics );
    producer.setUploadText(JSONStr);
    producer.run();
    producer.close();

    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finished produce" << std::endl;
    std::cout << "=====================================================\n";

    ///activemq::library::ActiveMQCPP::shutdownLibrary();

    return 0;
}

//接收发送请求
int QRGenerator::EventRecevier()
{
#if 0
    while (is_active)
    {

        while (!event_queue_is_empty)
            dispatch_next_event();

        wait_for_more_events();

    }
#endif
    bool is_active = true;
    while(is_active)
    {
        while((!evt_UDP_queue.empty()) || (!evt_Normal_queue.empty()))
        {
            //dispatch
            //if()


            if(isUDP)
            {
                //开始UDP，暂停Normal
                m_NormalTh->pause();
                m_UDPTh->start();
            }
            else if(isNormal)
            {
                evt_UDP_queue.push(1);
            }

        }
    }
    return 0;
}

void QRGenerator::processUDPEventSlot()
{
    printf("processUDPEventSlot In,Thread\n");

    int is;
    int ie;
    printf("UpdateSlot,Thread\n");

    mutex.lock();

    //完后UDP队列pop(push入队列在发信号时)
    //evt_UDP_queue.pop();

    mutex.unlock();
}

void QRGenerator::processNormalEventSlot()
{
    printf("processNormalEventSlot,Thread\n");

    ///mutex.lock();


    for(int i = 0; i<5; i++)
    {
        sleep(1);
        printf("Normal,Running%d\n", i);

        if(2 ==i)
        {
            //UDPRunning.wait(&mutex);
            UDPRunning.wakeOne();
        }
    }
    printf("processNormalEventSlot,Thread End\n");



    ///mutex.unlock();
}


#ifdef USE_ACTIVEMQ
///activeMQThread定义
void activeMQThread::run()
{
    printf("activeMQThread: run()\n");
    RegisterActiveMQRecevier();
}

void activeMQThread::RegisterActiveMQRecevier()
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    std::cout << "=====================================================\n";
    std::cout << "Start listerner:" << std::endl;
    std::cout << "-----------------------------------------------------\n";

#ifdef USE_LOCAL_ACTIVEMQ_ADDR
    std::string ConsumerbrokerURI ="failover://(tcp://localhost:61616)";
#else
    std::string ConsumerbrokerURI ="failover://(tcp://114.55.4.189:61616)";
#endif

    std::string NormaldestURI = "normal.queue";
    std::string UDPdestURI = "udp.queue";
    std::string resetURI = "reset.queue";
    bool useTopics = false;
    bool clientAck = false;

    ActiveMQAsyncConsumer UDPConsumer;
    ActiveMQAsyncConsumer NormalConsumer;
    ActiveMQAsyncConsumer ResetConsumer;

    UDPConsumer.start(ConsumerbrokerURI, UDPdestURI, useTopics, clientAck);
    UDPConsumer.runConsumer();

    NormalConsumer.start(ConsumerbrokerURI, NormaldestURI, useTopics, clientAck);
    NormalConsumer.runConsumer();

    ResetConsumer.start(ConsumerbrokerURI, resetURI, useTopics, clientAck);
    ResetConsumer.runConsumer();

    std::cout << "ActiveMQ start listening:" << std::endl;
    while( std::cin.get() != 'q') {}

    UDPConsumer.close();
    NormalConsumer.close();
    ResetConsumer.close();

    std::cout << "-----------------------------------------------------\n";
    std::cout << "Finish ActiveMQRecevier." << std::endl;
    std::cout << "=====================================================\n";

    activemq::library::ActiveMQCPP::shutdownLibrary();
}


void activeMQThread::processActiveMQVecMsg(activeMQVec msg)
{
}
#endif
