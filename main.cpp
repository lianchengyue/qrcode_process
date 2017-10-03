//打开文件过大的解决办法

//mmap 更优
//只读,文件多次打开

//Split
//读取文件+偏移量
//+多线程打开


#if 0
#include "instructions/inirw.h"
//http://bbs.csdn.net/topics/391842320
int main(int argc, char* argv[])
{
    const char *file = "/home/montafan/Qt5.6.2/project/zbar_gige/instructions/inirw/config.ini";

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
#include "client/qrgenerator.h"
#include <QApplication>

#include "LZO/lzopack.h"
#include "instructions/stats.h"
#include "instructions/base64.h"  //temp
#include "server/fragmentProcess.h"  //temp
#include "include/macros.h"
#include "include/DirPath.h" //tmp

#include "client/DirMonitor.h"

#ifdef BASLER_GRAB_FUNC
#include "server/gigeGrab.h"
#elif defined USB_GRAB_FUNC
#include "server/usbGrab.h"
#endif

int main(int argc, char* argv[])
{
#ifdef DISPLAY_QRCODE
    //使用base64二次封装接口
    #if 0
    //encode
    char *des_str = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X00png.txt";
    char *src_str = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X00";

    FILE *infile = fopen(src_str, "rb");
    FILE *outfile = fopen(des_str, "w");
    encode(infile, outfile);

    fclose(infile); // 关闭文件
    fclose(outfile); // 关闭文件
    #elif 0
    //decode
    char *des_str = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X11.png";
    char *src_str = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/X00png.txt";

    FILE *infile = fopen(src_str, "r");
    FILE *outfile = fopen(des_str, "wb");
    decode(infile, outfile);

    fclose(infile); // 关闭文件
    fclose(outfile); // 关闭文件
    #endif



    ///inotify_add_watch
    //DirMonitor *m_DirMonitor = new DirMonitor;
    //m_DirMonitor->startMonitoring();



//==================
  //printf("argv[0]:%s\n",argv[0]);//全路径
  const char *home =getenv("HOME"); //HOMEDRIVE
  printf("the home path is %s\n", home);
#if 1 //temp
    //生成拓扑
    src_init_topology();
    //文件遍历
    //2:LZO压缩，3:split操作
    file_traversal();//stats.cpp

    //报头遍历
    ini_traversal();
    //遍历片段
    fragment_traversal();
#endif

    //二维码显示
    QApplication a(argc, argv);
    QRGenerator w;

    //w.showFullScreen();
    //w.show();

    //w.showMaximized();
    w.resize(800,800);
    w.show();

    printf("a.exec()\n");
    return a.exec();

#else //#ifdef DISPLAY_QRCODE
//抓取二维码
    //生成拓扑
    des_init_topology();
    //grab & recognize
#ifdef BASLER_GRAB_FUNC
    gigeGrab *m_gigegrab = new gigeGrab();
    m_gigegrab->grab();
#elif defined USB_GRAB_FUNC
    usbGrab *m_usbgrab = new usbGrab();
    m_usbgrab->grab();
#endif

    return 0;

#endif
    return 0;
}

#endif
