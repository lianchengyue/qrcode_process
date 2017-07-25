#if 0
#ifndef QRGENERATOR_H
#define QRGENERATOR_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include "qrencode.h"

class QRGenerator : public QMainWindow
{
    Q_OBJECT //if use signal/slot
public:
    explicit QRGenerator(QWidget *parent = 0);
    ~QRGenerator();
    void setString(QString str);
    int getQRWidth() const;
    bool saveImage(QString name, int size);
private:
    void draw(QPainter &painter, int width, int height);
    QString string;
    QRcode *qr;
//added
/*
protected:
    QAction *openAction;  //定义一个动作
    QMenu *fileMenu;  //定义一个菜单
*/
signals:

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
public slots:
};
#endif // QRGENERATOR_H
#else
#ifndef QRGENERATOR_H
#define QRGENERATOR_H

#include <QWidget>
#include <QMainWindow>  //for main window
#include "qrencode.h"

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
    QString string;
    QRcode *qr;
signals:

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
public slots:
};
#endif // QRGENERATOR_H
#endif
