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

using namespace std;

/*
#define MD5SUM_MAX_C 64
#define NAME_MAX_C 255
#define PATH_MAX_C 255

typedef struct{
    unsigned char* md5sum;  //64
    int size;
    char* path; //512  /should be 255
    char name [NAME_MAX+1];
} fragment_client_metadata;  //Or in stats.h
*/


class generatorThread : public QThread
{
    Q_OBJECT
private:
    int number;
protected:
    void run();
public:
    generatorThread(QObject *parent=0);
    ~generatorThread();
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
    int CompleteSrcPath();

    QString string;
    QRcode *qr;
    QTimer *timer;
    generatorThread *myThread;
//    QLabel *label;
//    QPushButton *startButton;
//    QPushButton *stopButton;
//    QPushButton *resetButton;
    ///vector<char*> vecString;
    ///vector<std::array<char,255>>* vecString;

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
