#ifndef QRGENERATOR_H
#define QRGENERATOR_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include <QTimer>
#include <QtGui>
#include <QtCore>
///#include <QLabel.h>

#include "qrencode.h"

class Thread : public QThread
{
    Q_OBJECT
private:
    int number;
protected:
    void run();
public:
    Thread(QObject *parent=0);
    ~Thread();
signals:
    void UpdateSignal(int num);
    public slots:
        void ResetSlot();
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
    void StartTimer();
private:
    void draw(QPainter &painter, int width, int height);
    /////////////////////////////////////////////////void readFragment(char* filepath);
    QString string;
    QRcode *qr;
    QTimer *timer;
    Thread *myThread;
//    QLabel *label;
//    QPushButton *startButton;
//    QPushButton *stopButton;
//    QPushButton *resetButton;

signals:
    void ResetSignal(); //thread

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private slots:
    //void buttonClicked();
    void updateUI(); //QTimer

public slots:
    void StartSlot();
    void StopSlot();
    void UpdateSlot(int num);
    void ClearSlot();
};
#endif // QRGENERATOR_H
