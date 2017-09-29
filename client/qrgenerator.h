#ifndef QRGENERATOR_H
#define QRGENERATOR_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include <QTimer>
#include <QtGui>
#include <QtCore>
///#include <QLabel.h>

#include <vector>

#include "qrencode.h"
#include "ClientCbNotifier.h"

//宏控
#include "include/fileParameters.h"

using namespace std;


class ThreadObject;

////////////////////////generatorThread/////////////////////////
class generatorThread : public QThread
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
    generatorThread(QObject *parent=0);
    ~generatorThread();
    void resume();
    void pause();

signals:
    void UpdateSignal(int num);
    ////

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

    //public slots:
    //    void ResetSlot();
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
private:
    void draw(QPainter &painter, int width, int height);
    int CompleteSrcPath();
    ///更新事件，不再有新的碎片开始显示，让优先级更高的先跑
    //processXXUpdate(int /*int a*/)   flq

    QString string;
    QRcode *qr;

    generatorThread *myThread;
    ////UDP与Normal两个线程
    UDPThread *m_UDPTh;
    generatorThread *m_NormalTh;

    generatorThread *m_RecvPTh;

    ThreadObject* m_obj;

    ///vector<char*> vecString;
    ///vector<std::array<char,255>>* vecString;

    ClientCbNotifier *m_cbNotifier; //flq

signals:
    void ResetSignal(); //thread
    //新任务到来
    void UDPTaskIncomingSignal();
    void NormalTaskIncomingSignal();

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

//private slots:
//    void updateUI(); //QTimer

public slots:
    void StartSlot();
    void StopSlot();
    void UpdateSlot(int num);
    void ClearSlot();

    //处理线程的循环
    int EventRecevier();

    //一般事件
    void processNormalEventSlot();
    //优先级更高的UDP事件
    void processUDPEventSlot();
};
#endif // QRGENERATOR_H
