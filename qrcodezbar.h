#ifndef QRCODEZBAR_H
#define QRCODEZBAR_H

#include <QWidget>
#include <QTextCodec>
#include <QFileDialog>
#include <zbar/QZBar.h>
#include "ScanCode.h"

namespace Ui {
class QZbar;
}


class QRCodeZbar : public QWidget
{
    Q_OBJECT

public:
    QRCodeZbar(QWidget *parent = 0);
    ~QRCodeZbar();

private:


private slots:
    void on_pushButton_clicked();

private:
    Ui::QZbar *ui;
    zbar::QZBar *qz;
    ////ScanCode *m_scancode; //added by flq
    int aa;
};

#endif // QRCODEZBAR_H
