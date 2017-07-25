#if 0
#include "qrgenerator.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>
//added
#include <QMenu>
#include <QMenuBar>
#include <QAction>

QRGenerator::QRGenerator(QWidget *parent)
    : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
{
//    ui->setupUi(this);
    qr = NULL;
    setString("Hello QR Code");
/*    openAction = new QAction(tr("Open"),this);  //初始化动作
    fileMenu = new QMenu;  //创建一个菜单
    fileMenu = menuBar()->addMenu(tr("File"));  //将菜单添加到菜单栏上
    fileMenu->addAction(openAction);  //将动作添加到菜单上
*/
}

QRGenerator::~QRGenerator()
{
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
//    delete ui;
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



#else
#include "qrgenerator.h"
#include "qrencode.h"

#include <QPainter>
#include <QImage>

QRGenerator::QRGenerator(QWidget *parent) : QWidget(parent)
{
    qr = NULL;
    setString("Hello QR code");
    // long setString("The comet’s tail spread across the dawn, a red slash that bled above the crags of Dragonstone like a wound in the pink and purple sky.The maester stood on the windswept balcony outside his chambers. It was here the ravens came, after long flight. Their droppings speckled the gargoyles that rose twelve feet tall on either side of him, a hellhound and a wyvern, two of the thousand that brooded over the walls of the ancient fortress. When first he came to Dragonstone, the army of stone grotesques had made him uneasy, but as the years passed he had grown used to them. Now he thought of them as old friends. The three of them watched the sky together with foreboding. The maester did not believe in omens. And yet... old as he was, Cressen had never seen a comet half so bright, nor yet that color, that terrible color, the color of blood and flame and sunsets. He wondered if his gargoyles had ever seen its like. They had been here so much longer than he had, and would still be here long after he was gone. If stone tongues could speak...Such folly. He leaned against the battlement, the sea crashing beneath him, the black stone rough beneath his fingers. Talking gargoyles and prophecies in the sky. I am an old done man, grown giddy as a child again. Had a lifetime’s hard-won wisdom fled him along with his health and strength? He was a maester, trained and chained in the great Citadel of Oldtown. What had he come to, when superstition filled his head as if he were an ignorant fieldhand? And yet... and yet... the comet burned even by day now, while pale grey steam rose from the hot events of Dragonmont behind the castle, and yestermorn a white raven had brought word from the Citadel itself, word long-expected but no less fearful for all that, word of summer’s end. Omens, all. Too many to deny. What does it all mean? he wanted to cry.");
    setString("The comet’s tail spread across the dawn, a red slash that bled above the crags of Dragonstone like a wound in the pink and purple sky.The maester stood on the windswept balcony outside his chambers. It was here the ravens came, after long flight. Their droppings speckled the gargoyles that rose twelve feet tall on either side of him, a hellhound and a wyvern, two of the thousand that brooded over the walls of the ancient fortress. When first he came to Dragonstone, the army of stone grotesques had made him uneasy, but as the years passed he had grown used to them. Now he thought of them as old friends. The three of them watched the sky together with foreboding. The maester did not believe in omens. And yet... old as he was, Cressen had never seen a comet half so bright, nor yet that color, that terrible color, the color of blood and flame and sunsets. He wondered if his gargoyles had ever seen its like. They had been here so much longer than he had, and would still be here long after he was gone. If stone tongues could speak...Such folly. He leaned against the battlement, the sea crashing beneath him, the black stone rough beneath his fingers. Talking gargoyles and prophecies in the sky. I am an old done man, grown giddy as a child again. Had a lifetime’s hard-won wisdom fled him along with his health and strength? He was a maester, trained and chained in the great Citadel of Oldtown. What had he come to, when superstition filled his head as if he were an ignorant fieldhand? And yet...and yet...the comet burned even by day now, while pale grey steam rose from the hot");
}

QRGenerator::~QRGenerator()
{
    if(qr != NULL)
    {
        QRcode_free(qr);
    }
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
#endif
