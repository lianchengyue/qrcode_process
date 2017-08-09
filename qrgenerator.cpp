#include "qrgenerator.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>

char *pdesBuf;  //定义文件指针

//QThread
//Thread用来控制文件的开始与结束
Thread::Thread(QObject *parent)
{
    number = 0;
}

Thread::~Thread()
{

}

void Thread::run()
{
    while(1)
    {
        emit UpdateSignal(number);
        number++;
        sleep(1);
    }
}

void Thread::ResetSlot()
{
    number = 0;
    emit UpdateSignal(number);
}

void readFragment(char* pdesBuf/*,char* filepath*/)
{
    FILE *pFile=fopen("/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X000.txt","r"); //获取文件的指针
    //FILE *pFile=fopen(filepath,"r"); //获取文件的指针
    ///char *pdesBuf;  //定义文件指针
    fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    int len=ftell(pFile); //获取文件长度
    pdesBuf=new char[len+1];
    rewind(pFile); //把指针移动到文件开头
    fread(pdesBuf,1,len,pFile); //读文件
    pdesBuf[len]=0;

    fclose(pFile); // 关闭文件

    //return pBuf;
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

    //startButton = new QPushButton("start");
    //stopButton = new QPushButton("stop");
    //resetButton = new QPushButton("reset");
    //label = new QLabel("empty");


    //定时器
    //Timer用来做连续显示
    timer = new QTimer(this);
    timer->setInterval(500);//float,  ms
    connect(timer,SIGNAL(timeout()),this,SLOT(updateUI()));
    ////timer->start();

    //thread
    myThread = new Thread;

    //connect(stopButton, SIGNAL(clicked()), this, SLOT(StopSlot()));
    //connect(startButton, SIGNAL(clicked()), this, SLOT(StartSlot()));
    //connect(resetButton, SIGNAL(clicked()),  this, SLOT(ClearSlot()));
    connect(myThread, SIGNAL(UpdateSignal(int)), this, SLOT(UpdateSlot(int)));
    connect(this, SIGNAL(ResetSignal()), myThread, SLOT(ResetSlot()));

    //setString("Hello QR code");
    // long setString("The comet’s tail spread across the dawn, a red slash that bled above the crags of Dragonstone like a wound in the pink and purple sky.The maester stood on the windswept balcony outside his chambers. It was here the ravens came, after long flight. Their droppings speckled the gargoyles that rose twelve feet tall on either side of him, a hellhound and a wyvern, two of the thousand that brooded over the walls of the ancient fortress. When first he came to Dragonstone, the army of stone grotesques had made him uneasy, but as the years passed he had grown used to them. Now he thought of them as old friends. The three of them watched the sky together with foreboding. The maester did not believe in omens. And yet... old as he was, Cressen had never seen a comet half so bright, nor yet that color, that terrible color, the color of blood and flame and sunsets. He wondered if his gargoyles had ever seen its like. They had been here so much longer than he had, and would still be here long after he was gone. If stone tongues could speak...Such folly. He leaned against the battlement, the sea crashing beneath him, the black stone rough beneath his fingers. Talking gargoyles and prophecies in the sky. I am an old done man, grown giddy as a child again. Had a lifetime’s hard-won wisdom fled him along with his health and strength? He was a maester, trained and chained in the great Citadel of Oldtown. What had he come to, when superstition filled his head as if he were an ignorant fieldhand? And yet... and yet... the comet burned even by day now, while pale grey steam rose from the hot events of Dragonmont behind the castle, and yestermorn a white raven had brought word from the Citadel itself, word long-expected but no less fearful for all that, word of summer’s end. Omens, all. Too many to deny. What does it all mean? he wanted to cry.");
    setString("The comet’s tail spread across the dawn, a red slash that bled above the crags of Dragonstone like a wound in the pink and purple sky.The maester stood on the windswept balcony outside his chambers. It was here the ravens came, after long flight. Their droppings speckled the gargoyles that rose twelve feet tall on either side of him, a hellhound and a wyvern, two of the thousand that brooded over the walls of the ancient fortress. When first he came to Dragonstone, the army of stone grotesques had made him uneasy, but as the years passed he had grown used to them. Now he thought of them as old friends. The three of them watched the sky together with foreboding. The maester did not believe in omens. And yet... old as he was, Cressen had never seen a comet half so bright, nor yet that color, that terrible color, the color of blood and flame and sunsets. He wondered if his gargoyles had ever seen its like. They had been here so much longer than he had, and would still be here long after he was gone. If stone tongues could speak...Such folly. He leaned against the battlement, the sea crashing beneath him, the black stone rough beneath his fingers. Talking gargoyles and prophecies in the sky. I am an old done man, grown giddy as a child again. Had a lifetime’s hard-won wisdom fled him along with his health and strength? He was a maester, trained and chained in the great Citadel of Oldtown. What had he come to, when superstition filled his head as if he were an ignorant fieldhand? And yet...and yet...the comet burned even by day now, while pale grey steam rose from the hot");

    //启动线程
    //setWindowTitle("Thread Test");
    //resize(200, 200);
    myThread->start();
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
    update();
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
    printf("%d\n", i);
    //ui->textEdit->append("update!" + str);

    //读取每个小碎片,并做显示,裁剪完后通知启动定时器
    //readFragment
    /////////////////////////////////////////char *pdesBuf;  //定义文件指针
    //readFragment(pdesBuf);
    //printf("pdesBuf=%s",pdesBuf);




    //added by flq
    FILE *pFile=fopen("/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X24","rb"); //获取二进制文件的指针,rb二进制, rt文本文件
    //FILE *pFile=fopen(filepath,"r"); //获取文件的指针
    ///char *pdesBuf;  //定义文件指针
    fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    int len=ftell(pFile); //获取文件长度
    pdesBuf=new char[len+1];
    rewind(pFile); //把指针移动到文件开头
    fread(pdesBuf,1,len,pFile); //读文件
    pdesBuf[len]=0;

    fclose(pFile); // 关闭文件
    //added end


    //base64



    setString(pdesBuf);
    //setString("读取每个小碎片,并做显示,裁剪完后通知启动定时器");
    free(pdesBuf);
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
    //label->setText(QString::number(num));
    printf("a\n");
}

void QRGenerator::ClearSlot()
{
    emit ResetSignal();
}

