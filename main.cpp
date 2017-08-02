//#define DISPLAY_QRCODE

#include "gigegrab.h"
#include "qrgenerator.h"
#include <QApplication>

//#include <stdlib.h> //for system()


int main(int argc, char* argv[])
{
#ifdef DISPLAY_QRCODE
    QApplication a(argc, argv);
    QRGenerator w;
    //w.showFullScreen();
    //w.show();

    //w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    //w.showMaximized();
    w.resize(800,800);
    w.show();

    ///w.setString("1234567890");
    return a.exec();
#else
    QApplication a(argc, argv);
    QRCodeZbar w;
    w.show();

    gigegrab *m_gigegrab = new gigegrab();
    m_gigegrab->grab();
    //system("md5sum /home/montafan/software_download/lzo-2.10/examples/TESTFILE/TEST1.lzo");

#endif
    return 0;
}
