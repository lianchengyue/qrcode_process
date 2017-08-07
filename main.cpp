#if 0
#include "Instuctions/inirw.h"
//http://bbs.csdn.net/topics/391842320
int main(int argc, char* argv[])
{
    const char *file = "/home/montafan/Qt5.6.2/project/zbar_gige/Instuctions/inirw/config.ini";

    char *sect;
    char *key;
    char value[256];
    int intval;

    printf("load file %s\n\n", file);
    iniFileLoad(file);

    sect = 0;
    key = "_str001";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "sect1";
    key = "str001";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "sect1";
    key = "str005";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "sEcT2";
    key = "str002";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "sect2";
    key = "int002";
    intval = iniGetInt(sect, key, 1000);
    printf("[%s] %s = %d\n", sect, key, intval);

    iniSetString("sect2", "str002", "\"v'a;l;u#e'002\"");
    iniSetString("sect2", "str003", "value003");

    iniSetInt("sect2", "int001", 100, 0);
    iniSetInt("sect2", "int002", 200, 16);
    iniSetInt("sect2", "int003", 300, 8);

    getchar();
}
#endif



#if 1
//#define DISPLAY_QRCODE

#include "gigegrab.h"
#include "qrgenerator.h"
#include <QApplication>

//#include <stdlib.h> //for system()
#include "LZO/lzopack.h"
#include "Instuctions/stats.h"


int main(int argc, char* argv[])
{
    //abcd();
    file_traversal();//
    //processLZO(argc, argv, LZO_COMPRESS);
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo");
    //system("cd /home/montafan/Qt5.6.2/project/zbar_gige/CFile && split -b 2k /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo -d X");

    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo");
    //system("cat /home/montafan/Qt5.6.2/project/zbar_gige/CFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/CFile/2.lzo");
    //processLZO(argc, argv, LZO_DECOMPRESS);


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

#endif
