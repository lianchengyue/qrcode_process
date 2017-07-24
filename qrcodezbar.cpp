#include "qrcodezbar.h"
#include "ui_qzbar.h"

QRCodeZbar::QRCodeZbar(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
    , ui(new Ui::QZbar)
{
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);
    //QTextCodec::setCodecForCStrings(codec);   //qt5 deleted
    //QTextCodec::setCodecForTr(codec);
    ui->setupUi(this);

    //sm_scancode = new ScanCode();
    aa=1;
};


QRCodeZbar::~QRCodeZbar()
{

}

void QRCodeZbar::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("choose a picture"),
                                                    QApplication::applicationDirPath(),
                                                    tr("all Files (*.*)"));
    ui->lineEdit->setText(fileName);
    ui->label_3->setPixmap(QPixmap(ui->lineEdit->text()));
    char result[1024] = {0};

    ScanCode *m_scancode = new ScanCode();
        m_scancode->scanimage(ui->lineEdit->text().toStdString().data(), result);
        //m_scancode->abc();
        printf("aa=%d\n",aa);
    ui->textEdit->setText(result);
}
