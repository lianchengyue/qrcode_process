#include "TCPMonitor.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>

//遍历文件
#include <dirent.h>
#include<sys/stat.h>
#include <unistd.h>


#include <string>
using namespace std;


#if 1
//QThread
//Thread用来控制文件的开始与结束
tcpMonitorThread::tcpMonitorThread(QObject *parent)
{
    number = 0;
    ///m_cbNotifier = new ClientCbNotifier();
}

tcpMonitorThread::~tcpMonitorThread()
{

}

void tcpMonitorThread::run()
{
    sleep(1);//temp add, a.execute后再启动线程
//    emit UpdateSignal(number);

}

void tcpMonitorThread::ResetSlot()
{
    //emit UpdateSignal(number);
}
#endif



TCPMonitor::TCPMonitor()
{

}

TCPMonitor::~TCPMonitor()
{

}

int TCPMonitor::getQRWidth() const
{

}




