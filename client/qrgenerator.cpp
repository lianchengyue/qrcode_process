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
using namespace std;

//#define WAIT_FRAME_COUNT 20
//#define DISPLAY_INTERVAL 100000  //unit: us

char *pdesBuf;  //定义文件指针
vector<string> vecINIString;
vector<string> vecString;

//与信号量不同的程序
QWaitCondition UDPRunning; //QWaitCondition允许在一定条件下触发其它多个线程
QWaitCondition NormalRunning;
QMutex mutex;

//QThread
//Thread用来控制文件的开始与结束
generatorThread::generatorThread(QObject *parent)
{
    number = 0;
    ///m_cbNotifier = new ClientCbNotifier();
}

generatorThread::~generatorThread()
{

}

void generatorThread::run()
{
    sleep(1);//temp add, a.execute后再启动线程
    //emit UpdateSignal(number);
//QtConcurrent
//start
    for(int i = 0; i<5; i++)
    {
        sleep(1);
        printf("Normal,Running%d\n", i);

        if(2 ==i)
        {
            //UDPRunning.wait(&mutex);
            //UDPRunning.wakeOne();

            //UDPRunning.quit();
        }
    }
//end

    number++;

}

void generatorThread::ResetSlot()
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

void UDPThread::run()
{
    sleep(1);//temp add, a.execute后再启动线程
    //emit UDPSignal();


    for(int i = 0; i<5; i++)
    {
        sleep(1);
        printf("UDP,Running%d\n", i);

        if(2 ==i)
        {
            //UDPRunning.wait(&mutex);
            //UDPRunning.wakeOne();
        }
    }
//end
}


///遍历3文件夹中，split+base64 encode完后的碎片,把读到的绝对路径保存到vector中
void src_fragment_traversal(string dir, bool is_ini, int depth) //get_file_to_generate_qrcode
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
        fprintf(stderr,"src_fragment_traversal(), can not open dir:%s\n",dir.c_str());
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
            src_fragment_traversal(total_dir, is_ini, depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输icvUnDistort_8u_CnR_first_thread出当前目录名
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
    ///ui->setupUi(this);
    setWindowTitle("传输二维码");
    setWindowIcon(QIcon(":/new/prefix1/Image/ICON.ico"));

    qr = NULL;

    //thread
    m_UDPTh = new UDPThread;
    m_NormalTh = new generatorThread;
    m_RecvPTh = new generatorThread;

    //empty
    setString(TRANSMIT_IDLE);

    CompleteSrcPath();

    //thread
    myThread = new generatorThread;

    connect(myThread, SIGNAL(UpdateSignal(int)), this, SLOT(UpdateSlot(int)));
    connect(this, SIGNAL(ResetSignal()), myThread, SLOT(ResetSlot()));

    ////UDP与Normal两个线程
    connect(m_UDPTh, SIGNAL(UDPSignal()), this, SLOT(processUDPEventSlot()));
    //connect(this, SIGNAL(ResetSignal()), m_UDPTh, SLOT(ResetSlot()));

    connect(m_NormalTh, SIGNAL(UpdateSignal(int)), this, SLOT(processNormalEventSlot()));
    connect(this, SIGNAL(ResetSignal()), m_NormalTh, SLOT(ResetSlot()));

    //接收器，收到消息后，操作UDP与Normal线程
    //connect(m_RecvPTh, SIGNAL(UDPTaskIncomingSignal()), this, SLOT(EventRevevier()));
    //connect(m_RecvPTh, SIGNAL(NormalTaskIncomingSignal()), this, SLOT(EventRevevier()));

    //启动线程
    //setWindowTitle("Thread Test");
    //resize(200, 200);
    ///==============================start Thread=====================================/
    //myThread->start();    ///后续改为在收到传输完成消息后调用 added by flq
    printf("1111111111\n");
    m_UDPTh->start();
    printf("2222222222\n");
    m_NormalTh->start();
    printf("3333333333\n");

    printf("m_UDPTh->wait\n");
    m_UDPTh->wait();
    printf("m_NormalTh->wait\n");
    m_NormalTh->wait();

    //如果想唤醒某个指定的线程，一般的办法是定义不同的等待条件QWaitCondition，让不同的线程等待不同的等待条件
    // UDPRunning.wait(&mutex);
    // UDPRunning.wakeOne();

    ///sleep(5);
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
    string = str;
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
    //生成二维码
    qr = QRcode_encodeString(string.toStdString().c_str(),
        1,
        QR_ECLEVEL_L,
        QR_MODE_8,
        1);

    usleep(DISPLAY_INTERVAL);
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
    double scale_x = width / qr_width;
    double scale_y = height / qr_width;
    for( int y = 0; y < qr_width; y ++)
    {
        for(int x = 0; x < qr_width; x++)
        {
            unsigned char b = qr->data[y * qr_width + x];
            if(b & 0x01)
            {
                QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
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
    myThread->start();
}

void QRGenerator::StopSlot()
{
    myThread->terminate();
}

void QRGenerator::UpdateSlot(int num)
{
    int is;
    int ie;
    printf("UpdateSlot,Thread\n");

    mutex.lock();
#if 1
    //处理待发送文件
#if 1
    ///后续改为在收到传输完成消息后调用 added by flq
    //文件夹信息
    char *folderdir = SRC_INI_FOLD_FRAG_LOCATION;
    std::string folder_str =  folderdir;
    src_fragment_traversal(folder_str, true, 0);  //isINI=true

    //文件属性信息
    char *configdir = SRC_INI_FILE_FRAG_LOCATION;
    std::string config_str =  configdir;
    src_fragment_traversal(config_str, true, 0);

    //内容碎片信息
    //直接遍历4_base64_encode_location/文件,路径保存到vector
    char *topdir = SRC_BASE64_ENCODE_LOCATION;
    std::string topdir_str =  topdir;
    src_fragment_traversal(topdir_str, false, 0);
    ///added end
 #endif

    printf("TRANSMIT_PRESTART\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_PRESTART);
    }

    printf("TRANSMIT ini\n");
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

        //显示二维码
        setString(pdesBuf);
        ///usleep(100);
        free(pdesBuf);
    }
    //added end

    printf("TRANSMIT_PREEND\n");
    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_PREEND);
    }

    printf("TRANSMIT_START\n");
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

        //显示二维码
        setString(pdesBuf);
        ///usleep(100);
        free(pdesBuf);
    }

    ///播放结束二维码
    printf("TRANSMIT_END\n");
    for (ie = 0; ie < WAIT_FRAME_COUNT; ie++) {
        setString(TRANSMIT_END);
    }

    printf("TRANSMIT_IDLE\n");
    setString(TRANSMIT_IDLE);
#endif
    mutex.unlock();
}

void QRGenerator::ClearSlot()
{
    emit ResetSignal();
}

int QRGenerator::CompleteSrcPath()
{
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

    return 0;
}

//接收发送请求
int QRGenerator::EventRecevier()
{
    //switch(1)
    {
        emit UDPTaskIncomingSignal();
        emit NormalTaskIncomingSignal();
    }
#if 0
    for(; ;)
    {
        ///改为在收到传输完成消息后调用 added by flq

        //thread
        generatorThread myThread = new generatorThread;

        connect(myThread, SIGNAL(UpdateSignal(int)), this, SLOT(UpdateSlot(int)));
        connect(this, SIGNAL(ResetSignal()), myThread, SLOT(ResetSlot()));

        //启动线程
        ///==============================start Thread=====================================/
        myThread->start();    ///后续改为在收到传输完成消息后调用 added by flq
    }
#endif
    return 0;
}

void QRGenerator::processUDPEventSlot()
{
    printf("processUDPEventSlot,Thread\n");

    ///mutex.lock();


    for(int i = 0; i<5; i++)
    {
        sleep(1);
        printf("UDP,Running%d\n", i);

        if(2 ==i)
        {
            //UDPRunning.wait(&mutex);
            //UDPRunning.wakeOne();

            //UDPRunning.quit();
        }
    }

    printf("processUDPEventSlot,Thread End\n");



    ///mutex.unlock();
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

