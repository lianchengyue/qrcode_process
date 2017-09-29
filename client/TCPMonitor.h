#ifndef TCPMonitor_H
#define TCPMonitor_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include <QTimer>
#include <QtGui>
#include <QtCore>


class tcpMonitorThread : public QThread
{
    //Q_OBJECT
private:
    int number;
protected:
    void run();
public:
    tcpMonitorThread(QObject *parent=0);
    ~tcpMonitorThread();

signals:
    void UpdateSignal(int num);

    public slots:
        void ResetSlot();
};



class TCPMonitor //: public QWidget
{
    //Q_OBJECT //要使用信号/槽功能
public:
    explicit TCPMonitor();
    ~TCPMonitor();
    void setString(QString str);
    int getQRWidth() const;

#if 0
private:


signals:
    void ResetSignal(); //thread

protected:


public slots:
    void StartSlot();
#endif


};

#endif // TCPMonitor_H
