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
#define DISPLAY_QRCODE  //grab pic
#define FILE_PROCESS

#include "gigeGrab.h"
#include "qrgenerator.h"
#include <QApplication>

#include "LZO/lzopack.h"
#include "Instuctions/stats.h"
#include "Instuctions/base64.h"  //temp
#include "server/fragmentProcess.h"  //temp


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

    #ifdef FILE_PROCESS

    const char* in_name = "/home/montafan/QRcodeGrab/source/1_location/nocolor.png";
    const char* out_name = "/home/montafan/QRcodeGrab/source/2_lzo_location/nocolor.png.lzo";
    //文件遍历
    file_traversal();//stats.cpp
    //文件压缩
    processLZO(argc, argv, in_name, out_name, LZO_COMPRESS);//后续需要做遍历, 2K大小判断
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/nocolor.PNG");
    //裁剪
    ///system("cd /home/montafan/Qt5.6.2/project/zbar_gige/testFile && split -b 2k /home/montafan/Qt5.6.2/project/zbar_gige/testFile/1.lzo -d X");
    system("cd /home/montafan/QRcodeGrab/source/2_lzo_location && split -b 2k /home/montafan/QRcodeGrab/source/2_lzo_location/nocolor.png.lzo -d X");
    //遍历片段
    fragment_traversal();

    //合并
    //system("cat /home/montafan/Qt5.6.2/project/zbar_gige/testFile/X* >>/home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //检查md5sum并匹配，改在stats文件中
    //system("md5sum /home/montafan/Qt5.6.2/project/zbar_gige/testFile/2.lzo");
    //解压
    //processLZO(argc, argv, LZO_DECOMPRESS);
    #endif

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

#elif 1 //#ifdef DISPLAY_QRCODE
//抓取二维码
    //grab frame
    QApplication a(argc, argv);
    QRCodeZbar w;
    w.show();

    gigegrab *m_gigegrab = new gigegrab();
    m_gigegrab->grab();

    return a.exec();
#else
    char* input_str = "QR-Code:http://u.wechat.com/EEq9zMbMatp40tW2WV6cKkA";
    fragmentProcess *m_fragmentProcess = new fragmentProcess();
    m_fragmentProcess->process_QRdata_to_fragment(input_str);
    //m_fragmentProcess->is_md5sum_match(input_str);  Not ok yet

#endif
    return 0;
}

#endif
