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
#define DISPLAY_QRCODE
///#define FILE_PROCESS

#include "gigegrab.h"
#include "qrgenerator.h"
#include <QApplication>

#include "LZO/lzopack.h"
#include "Instuctions/stats.h"


int main(int argc, char* argv[])
{
#ifdef FILE_PROCESS
    //abcd();
    //文件遍历
    ///file_traversal();//stats.cpp
    //文件压缩
    //processLZO(argc, argv, LZO_COMPRESS);
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo");
    //裁剪
    //system("cd /home/montafan/Qt5.6.2/project/zbar_gige/CFile && split -b 20k /home/montafan/Qt5.6.2/project/zbar_gige/CFile/1.lzo -d X");

    //合并
    //system("cat /home/montafan/Qt5.6.2/project/zbar_gige/CFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/CFile/2.lzo");
    //检查md5sum并匹配，改在stats文件中
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/CFile/2.lzo");
    //解压
    //processLZO(argc, argv, LZO_DECOMPRESS);
#endif

#ifdef DISPLAY_QRCODE
    //文件遍历
    file_traversal();//stats.cpp
    //文件压缩
    processLZO(argc, argv, LZO_COMPRESS);
    system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/nocolor.PNG");
    //裁剪
    system("cd /home/montafan/Qt5.6.2/project/zbar_gige/testFile && split -b 2k /home/montafan/Qt5.6.2/project/zbar_gige/testFile/1.lzo -d X");
    //system("cd /home/montafan/Qt5.6.2/project/zbar_gige/testFile && split -b 2k /home/montafan/Qt5.6.2/project/zbar_gige/testFile/1.lzo -d X /home/montafan/Qt5.6.2/project/zbar_gige/testFile/111");

    //合并
    //system("cat /home/montafan/Qt5.6.2/project/zbar_gige/testFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //检查md5sum并匹配，改在stats文件中
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //解压
    //processLZO(argc, argv, LZO_DECOMPRESS);

    //二维码显示
    QApplication a(argc, argv);
    QRGenerator w;
    w.StartTimer();
    //w.showFullScreen();
    //w.show();

    //w.showMaximized();
    w.resize(800,800);
    w.show();

    //w.setString("1234567890");
    return a.exec();

#elif 0 //DISPLAY_QRCODE
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
