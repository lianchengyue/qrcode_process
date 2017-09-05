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
#include "server/gigeGrab.h"
#include "client/qrgenerator.h"
#include <QApplication>

#include "LZO/lzopack.h"
#include "instructions/stats.h"
#include "instructions/base64.h"  //temp
#include "server/fragmentProcess.h"  //temp
#include "include/macros.h"

#include "client/DirMonitor.h"

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



    //生成拓扑
    src_init_topology();
    //文件遍历
    file_traversal();//stats.cpp
    #if 0
    //文件压缩
    const char* in_name = "/home/montafan/QRcodeGrab/source/1_location/nocolor.png";
    const char* out_name = "/home/montafan/QRcodeGrab/source/2_lzo_location/nocolor.png.lzo";
    processLZO(argc, argv, in_name, out_name, LZO_COMPRESS);//后续需要做遍历, 2K大小判断
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/nocolor.PNG");
    //裁剪
    ///system("cd /home/montafan/Qt5.6.2/project/zbar_gige/testFile && split -b 2k /home/montafan/Qt5.6.2/project/zbar_gige/testFile/1.lzo -d X");
    system("cd /home/montafan/QRcodeGrab/source/2_lzo_location && split -b 2k /home/montafan/QRcodeGrab/source/2_lzo_location/nocolor.png.lzo -d X");
    #endif
    //报头遍历
    ini_traversal();
    //遍历片段
    fragment_traversal();

    //合并
    //system("cat /home/montafan/Qt5.6.2/project/zbar_gige/testFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //检查md5sum并匹配，改在stats文件中
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //解压
    //processLZO(argc, argv, LZO_DECOMPRESS);


    //二维码显示
    QApplication a(argc, argv);
    QRGenerator w;
    ///w.StartTimer();


    //w.showFullScreen();
    //w.show();

    //w.showMaximized();
    w.resize(800,800);
    w.show();

    //w.setString("1234567890");
    return a.exec();

#else //#ifdef DISPLAY_QRCODE
//抓取二维码

    //grab frame
    //QApplication a(argc, argv);
    //QRCodeZbar w;
    //w.show();
    //return a.exec();

    //temp
    #if 0
    system("cat /home/montafan/QRcodeGrab/destination/2_base64_decode_location/nocolor.png/X* >>/home/montafan/QRcodeGrab/destination/3_cat_location/nocolor.png.lzo");
    const char* in_name = "/home/montafan/QRcodeGrab/destination/3_cat_location/nocolor.png.lzo";
    const char* out_name = "/home/montafan/QRcodeGrab/destination/4_location/nocolor.png";
    processLZO(argc, argv, in_name, out_name, LZO_DECOMPRESS);
    #endif
    //temp end

    //生成拓扑
    des_init_topology();
    //grab & recognize
    gigegrab *m_gigegrab = new gigegrab();
    m_gigegrab->grab();

    return 0;

#endif
    return 0;
}

#endif
