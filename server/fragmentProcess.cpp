#include "fragmentProcess.h"
#include "../Instuctions/stats.h"
#include "../Instuctions/inirw.h"

using namespace std;

#if 0
void get_file_to_generate_qrcode(string dir, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径
    string total_dir;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir.c_str())))
    {
        fprintf(stderr,"can not open dir:%s\n",dir.c_str());
        return;
    }

    //切换到这个目录
    chdir(dir.c_str());

    //遍历这个目录下的所有文件
    while(NULL != (enty = readdir(Dp) ))
    {
        //通过文件名，得到详细文件信息
        lstat(enty->d_name,&statbuf);
        //判断是不是目录
        if(S_ISDIR(statbuf.st_mode))
        {
            //当前目录和上一目录过滤掉
            if(0 == strcmp(".",enty->d_name) ||
                          0 == strcmp("..",enty->d_name))
            {
                continue;
            }

            total_dir = dir + enty->d_name + "/";
            //输出当前目录名
            //printf("%*s%s/\n",depth," ",enty->d_name);

            //继续递归调用
            get_file_to_generate_qrcode(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //get文件名
            vecString.push_back(total_dir);//NULL
            ///std::vector<std::array<char,255>>* vecString = reinterpret_cast<std::vector<std::array<char,255>>*>(total_dir);

        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

}
#endif

fragmentProcess::fragmentProcess()
{
    init();
}

fragmentProcess::~fragmentProcess()
{

}

int fragmentProcess::init(){

    const char *iniPath = "/home/montafan/QRcodeGrab/server/nocolor.png/config.ini";//读取二维码头后生成,目前暂写成这样
    iniFileLoad(iniPath);

}

int fragmentProcess::readFragmentINI(){
    const char *file = "/home/montafan/QRcodeGrab/source/temp_location/nocolor.png/ini/config.ini";

    char *sect;
    char *key;
    char value[256];
    int intval;

    printf("load file %s\n\n", file);
    iniFileLoad(file);

    sect = 0;
    key = "X01";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "X02";
    key = "str001";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "X03";
    key = "str005";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "X04";
    key = "str002";
    iniGetString(sect, key, value, sizeof(value), "notfound!");
    printf("[%s] %s = %s\n", sect, key, value);

    sect = "sect2";
    key = "int002";
    intval = iniGetInt(sect, key, 1000);
    printf("[%s] %s = %d\n", sect, key, intval);

}


int fragmentProcess::process_QRdata_to_fragment(char *QRdata)//for test
{
    //char *des_str = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/111/out1.txt";
    char *des_str = "/home/montafan/out1.txt";

    FILE *Destination = fopen(des_str, "wb"); //ab+
    int size = fwrite(QRdata, 1, strlen(QRdata), Destination);   //strlen(QRdata) =51
    printf("size=%d\n",size);

    fclose(Destination); // 关闭文件

    return 0;
}

int fragmentProcess::process_QRdata_to_fragment(char *QRdata, char *des_str)
{
    FILE *Destination = fopen(des_str, "wb"); //ab+
    int size = fwrite(QRdata, 1, strlen(QRdata), Destination);   //strlen(QRdata) =51
    printf("size=%d\n",size);

    fclose(Destination); // 关闭文件

    return 0;
}

bool fragmentProcess::is_md5sum_match(char *QRdata)
{
    unsigned char *client_md5sum;///[MD5SUM_MAX_S];

    char *input_str = new char[512];

    memset(input_str, 0, 512);
    input_str = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111222222222";


    ///memset(client_md5sum, 0, MD5SUM_MAX_S);
    //client_md5sum = generate_md5sum_from_charstr(input_str);

    return 0;
}
