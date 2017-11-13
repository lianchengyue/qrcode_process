#ifndef QRGENERATOR_H
#define QRGENERATOR_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include <QTimer>
#include <QtGui>
#include <QtCore>
///#include <QLabel.h>

#include <vector>
#include <queue>

#include "qrencode.h"
#include "ClientCbNotifier.h"

//宏控
#include "include/fileParameters.h"


//activeMQ队列
#ifdef USE_ACTIVEMQ
#include <QMetaType> //将不识别的参数结构进行注册，让QT能够识别
#include "instructions/ActiveMQConsumer.h"
#include "instructions/ActiveMQProducer.h"
#include "instructions/JSON.h"
#endif

using namespace std;

/*
#ifdef USE_ACTIVEMQ
typedef struct ActiveMQVec {
    std::string filename;
    std::string date;
    int size;
    std::string md5sum;
    int type;// 1: UDP 2:tcp
} activeMQVec;
#endif*/

class ThreadObject;

#ifdef USE_ACTIVEMQ
////////////////////////activeMQThread/////////////////////////
class activeMQThread : public QThread
{
private:
    QMutex sync;
    bool is_pause;
protected:
    void run();
public:
    activeMQThread(QObject *parent=0){}
    ~activeMQThread(){}
    void RegisterActiveMQRecevier();
    void processActiveMQVecMsg(activeMQVec msg);
};
#endif

////////////////////////NormalThread/////////////////////////
class NormalThread : public QThread
{
    Q_OBJECT
private:
    int number;
    QMutex sync;
    QWaitCondition pauseCond;
    bool is_pause;
protected:
    void run();
public:
    NormalThread(QObject *parent=0);
    ~NormalThread();
    void resume();
    void pause();

signals:
    void UpdateSignal(int num);
    #ifdef USE_ACTIVEMQ
    void ProcessMsgSignal(QString raw_msg);
    void ProcessMsgSignal(activeMQVec msg);
    #endif

    public slots:
        void ResetSlot();
};

///////////////////////////UDPThread///////////////////////
class UDPThread : public QThread
{
    Q_OBJECT
private:
    int number;
    QMutex sync;
    QWaitCondition pauseCond;
    bool is_pause;
protected:
    void run();
public:
    UDPThread(QObject *parent=0);
    ~UDPThread();
    void resume();
    void pause();

signals:
    void UDPSignal();
};

class QRGenerator : public QWidget
{
    Q_OBJECT //要使用信号/槽功能
public:
    explicit QRGenerator(QWidget *parent = 0);
    ~QRGenerator();
    void setString(QString str);
    int getQRWidth() const;
    bool saveImage(QString name, int size);

    int RegisterRecvActiveMQ();
private:
    void draw(QPainter &painter, int width, int height);
    int CompleteSrcPath();
    #ifdef USE_ACTIVEMQ
    int SetActiveMQMessage(string JSONStr);
    #endif
    int getDisplayInterval();
    void setDisplayInterval();

    ///更新事件，不再有新的碎片开始显示，让优先级更高的先跑
    //processXXUpdate(int /*int a*/)   flq

    QString qstring;
    QRcode *qr;

    NormalThread *myThread;
    ////UDP与Normal两个线程
    UDPThread *m_UDPTh;
    NormalThread *m_NormalTh;

    NormalThread *m_RecvPTh;

    //ActiveMQ接收线程
    #ifdef USE_ACTIVEMQ
    activeMQThread *m_activeMQTh;
    #endif

    ThreadObject* m_obj;

    ///vector<char*> vecString;
    ///vector<std::array<char,255>>* vecString;

    ClientCbNotifier *m_cbNotifier; //flq
    //queue<int> evt_UDP_queue;

    int time_interval;

signals:
    void ResetSignal(); //thread
    //新任务到来
    void UDPTaskIncomingSignal();
    void NormalTaskIncomingSignal();

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void StartSlot();
    void StopSlot();
    void UpdateSlot(int num);
    #ifdef USE_ACTIVEMQ
    void ProcessMsgQ(QString msg);
    void ProcessMsg(activeMQVec msg);
    #endif
    void ClearSlot();

    //处理线程的循环
    int EventRecevier();

    //一般事件
    void processNormalEventSlot();
    //优先级更高的UDP事件
    void processUDPEventSlot();
};
#endif // QRGENERATOR_H
