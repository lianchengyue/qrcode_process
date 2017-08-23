#include "qrgenerator.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>

//遍历文件
#include <dirent.h>
#include<sys/stat.h>
#include <unistd.h>

#include "include/DirPath.h"//dir
//接收端文件生成
#include "include/fileParameters.h"

#include <string>
using namespace std;

#define WAIT_FRAME_COUNT 20
#define DISPLAY_INTERVAL 100000  //unit: us

char *pdesBuf;  //定义文件指针
vector<string> vecINIString;
vector<string> vecString;

//QThread
//Thread用来控制文件的开始与结束
generatorThread::generatorThread(QObject *parent)
{
    number = 0;
}

generatorThread::~generatorThread()
{

}

void generatorThread::run()
{
    sleep(1);//temp add, a.execute后再启动线程
    emit UpdateSignal(number);


    #if 0
    while(1)
    {
        emit UpdateSignal(number);
        number++;
        sleep(1);
    }
    #endif
}

void generatorThread::ResetSlot()
{
    number = 0;
    emit UpdateSignal(number);
}

void readFragment(char* pdesBuf)
{
#if 0
    for(vector<std::string>::iterator it = vecString.begin(); it != vecString.end(); ++it) {  //const_iterator
        //process file
        //added by flq
        std::string s = *it;
        //FILE *pFile=fopen("/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X00png.txt","rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        FILE *pFile=fopen(s.c_str(),"rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        ///char *pdesBuf;  //定义文件指针
        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile); //读文件
        pdesBuf[len]=0;

        fclose(pFile); // 关闭文件
        //added end

        //显示二维码
        setString(pdesBuf);
        free(pdesBuf);
    }
#else
    //提高效率，选择该种遍历方式
    for (size_t i =0; i < vecString.size(); i ++) {

        std::string s = vecString[i];
        FILE *pFile=fopen(s.c_str(),"rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        ///char *pdesBuf;  //定义文件指针
        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile); //读文件
        pdesBuf[len]=0;

        fclose(pFile); // 关闭文件
        //added end

        //显示二维码
        //setString(pdesBuf);
        //free(pdesBuf);

    }
#endif
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
        fprintf(stderr,"can not open dir:%s\n",dir.c_str());
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
    ///ui->setupUi(this);
    qr = NULL;

    //setString("The comet’s tail spread across the dawn, a red slash that bled above the crags of Dragonstone like a wound in the pink and purple sky.The maester stood on the windswept balcony outside his chambers. It was here the ravens came, after long flight. Their droppings speckled the gargoyles that rose twelve feet tall on either side of him, a hellhound and a wyvern, two of the thousand that brooded over the walls of the ancient fortress. When first he came to Dragonstone, the army of stone grotesques had made him uneasy, but as the years passed he had grown used to them. Now he thought of them as old friends. The three of them watched the sky together with foreboding. The maester did not believe in omens. And yet... old as he was, Cressen had never seen a comet half so bright, nor yet that color, that terrible color, the color of blood and flame and sunsets. He wondered if his gargoyles had ever seen its like. They had been here so much longer than he had, and would still be here long after he was gone. If stone tongues could speak...Such folly. He leaned against the battlement, the sea crashing beneath him, the black stone rough beneath his fingers. Talking gargoyles and prophecies in the sky. I am an old done man, grown giddy as a child again. Had a lifetime’s hard-won wisdom fled him along with his health and strength? He was a maester, trained and chained in the great Citadel of Oldtown. What had he come to, when superstition filled his head as if he were an ignorant fieldhand? And yet...and yet...the comet burned even by day now, while pale grey steam rose from the hot");
    //empty
    setString(TRANSMIT_IDLE);


    //startButton = new QPushButton("start");
    //stopButton = new QPushButton("stop");
    //resetButton = new QPushButton("reset");
    //label = new QLabel("empty");

    ///后续改为在收到传输完成消息后调用 added by flq
    //首先遍历文件,路径保存到vector
    //文件夹信息
    char *folderdir = SRC_INI_FOLD_FRAG_LOCATION;
    std::string folder_str =  folderdir;
    src_fragment_traversal(folder_str, true, 0);  //isINI=true

    //文件属性信息
    char *configdir = SRC_INI_FILE_FRAG_LOCATION;
    std::string config_str =  configdir;
    src_fragment_traversal(config_str, true, 0);

    //内容碎片信息
    char *topdir = SRC_BASE64_ENCODE_LOCATION;
    std::string topdir_str =  topdir;
    src_fragment_traversal(topdir_str, false, 0);
    ///added end

    //定时器
    //Timer用来做连续显示
    #if 0
    timer = new QTimer(this);
    timer->setInterval(1000);//float,  ms
    connect(timer,SIGNAL(timeout()),this,SLOT(updateUI()));
    ///===============================start timer=====================================/
    timer->start();
    #endif

    //thread
    myThread = new generatorThread;

    //connect(stopButton, SIGNAL(clicked()), this, SLOT(StopSlot()));
    //connect(startButton, SIGNAL(clicked()), this, SLOT(StartSlot()));
    //connect(resetButton, SIGNAL(clicked()),  this, SLOT(ClearSlot()));
    connect(myThread, SIGNAL(UpdateSignal(int)), this, SLOT(UpdateSlot(int)));
    connect(this, SIGNAL(ResetSignal()), myThread, SLOT(ResetSlot()));

    //启动线程
    //setWindowTitle("Thread Test");
    //resize(200, 200);
    ///==============================start Thread=====================================/
    myThread->start();    ///后续改为在收到传输完成消息后调用 added by flq
}

QRGenerator::~QRGenerator()
{
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
    #if 1
    usleep(DISPLAY_INTERVAL);
    repaint();
    #else
    //repaint();
    setUpdatesEnabled(false);
    //bigVisualChanges();

    setUpdatesEnabled(true);
    repaint();
    #endif
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

void QRGenerator::StartTimer() // timer
{
    timer->start();
}

void QRGenerator::updateUI() // timer
{
    static int i=0;
    QString str = QString::number(i++);
    printf("updateUI():%d\n", i);
    //ui->textEdit->append("update!" + str);

    //ADDED BY FLQ, 遍历目标文件夹的内容,  //循环readFragment(pdesBuf);
#if 0
    for(vector<std::string>::iterator it = vecString.begin(); it != vecString.end(); ++it) {  //const_iterator
        //process file
        //added by flq
        std::string s = *it;
        //FILE *pFile=fopen("/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X00png.txt","rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        FILE *pFile=fopen(s.c_str(),"rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        ///char *pdesBuf;  //定义文件指针
        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile); //读文件
        pdesBuf[len]=0;

        fclose(pFile); // 关闭文件
        //added end

        //显示二维码
        setString(pdesBuf);
        free(pdesBuf);
    }
#else
    //提高效率，选择该种遍历方式
    for (size_t i =0; i < vecString.size(); i ++) {

        std::string s = vecString[i];
        FILE *pFile=fopen(s.c_str(),"rb"); //获取二进制文件的指针,rb二进制, rt文本文件
        ///char *pdesBuf;  //定义文件指针
        fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
        int len=ftell(pFile); //获取文件长度
        pdesBuf=new char[len+1];
        rewind(pFile); //把指针移动到文件开头
        fread(pdesBuf,1,len,pFile); //读文件
        pdesBuf[len]=0;

        fclose(pFile); // 关闭文件
        //added end

        //显示二维码
        setString(pdesBuf);
        free(pdesBuf);

    }
#endif

}

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
    //label->setText(QString::number(num));
    printf("UpdateSlot,Thread\n");

    ///播放报头二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_PRESTART);
    }

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

    ///播放开始二维码
    for (is = 0; is < WAIT_FRAME_COUNT; is++)
    {
        setString(TRANSMIT_START);
    }

#if 1
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
#endif

    ///播放结束二维码
    for (ie = 0; ie < WAIT_FRAME_COUNT; ie++) {
        setString(TRANSMIT_END);
    }
    setString(TRANSMIT_IDLE);
}

void QRGenerator::ClearSlot()
{
    emit ResetSignal();
}

