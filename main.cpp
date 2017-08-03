//#define DISPLAY_QRCODE

#include "gigegrab.h"
#include "qrgenerator.h"
#include <QApplication>

//#include <stdlib.h> //for system()
#include "LZO/lzopack.h"


int main(int argc, char* argv[])
{
    //abcd();
    processLZO(argc, argv, LZO_COMPRESS);
    system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo");
    system("cd /home/montafan/Qt5.6.2/project/zbar_gige/CFile && split -b 20k /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo -d X");

    system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo");
    system("cat /home/montafan/Qt5.6.2/project/zbar_gige/CFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/CFile/2.lzo");
    processLZO(argc, argv, LZO_DECOMPRESS);


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
#elif 0
    //grab frame
    QApplication a(argc, argv);
    QRCodeZbar w;
    w.show();

    gigegrab *m_gigegrab = new gigegrab();
    m_gigegrab->grab();
    //system("md5sum /home/montafan/software_download/lzo-2.10/examples/TESTFILE/TEST1.lzo");

#endif
    return 0;
}
