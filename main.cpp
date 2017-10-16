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
    char *src_str = "/home/montafan/X403";
    char *des_str = "/home/montafan/Y403";

    FILE *infile = fopen(src_str, "rb");
    FILE *outfile = fopen(des_str, "wb");
    encode(infile, outfile);

    fclose(infile); // 关闭文件
    fclose(outfile); // 关闭文件
    #elif 0
    //decode
    char *src_str = "/home/montafan/Y403";
    char *des_str = "/home/montafan/Z403";

    FILE *infile = fopen(src_str, "rb");
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
  const char *home =getenv("HOME"); //HOMEDRIVE ///home/montafan
  printf("the home path is %s\n", home);
#if 1 //temp
    //生成拓扑
    src_init_topology();
    //文件遍历
    //2:LZO压缩，3:split操作
    printf("file_traversal()\n");
    file_traversal();//stats.cpp

    //报头遍历
    printf("ini_traversal()\n");
    ini_traversal();
    //遍历片段
    printf("fragment_traversal()\n");
    fragment_traversal();
    printf("fragment_traversal end()\n");
#endif

    //二维码显示
    QApplication a(argc, argv);
    QRGenerator w;

    w.showFullScreen();
    //同时禁止最大化最小化按钮 & 禁止关闭
    w.setWindowFlags(w.windowFlags() &~ (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint));
    //w.show();

    //w.showMaximized();
    //w.resize(800,800);
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
