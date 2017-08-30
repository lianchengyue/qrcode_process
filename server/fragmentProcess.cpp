#include "fragmentProcess.h"
#include "../instructions/stats.h"
#include "../instructions/inirw.h"
#include "../instructions/cat.h"

#include "include/DirPath.h"
#include "../instructions/base64.h"

#include "include/Errors.h"
#include <mutex>

#include <string>
using namespace std;


fragmentProcess::fragmentProcess()
{
    init();
    ini_flag = false;

    m_stateMachine = new RecvStateMachine(this);
}

fragmentProcess::~fragmentProcess()
{
    free(m_stateMachine);
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

//整个处理的入口函数，识别二维码成功后进入
int fragmentProcess::QRdataProcess(char* QRdata)
{
    //TEST
    des_ini_fragment_traversal(DES_INI_FOLD_LOCATION, 0);
    create_folder_tree_from_ini(); //only once;

    des_ini_fragment_traversal(DES_INI_FILE_LOCATION, 0);
    //TEST END

    if(0 == strcmp(QRdata, TRANSMIT_IDLE))
    {
        return NO_ERROR;
    }

    //接收配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_PRESTART))
    {
        setTransmitStatus(PRESTART);
        return NO_ERROR;
    }

    else if(0 == strcmp(QRdata, TRANSMIT_PREEND))
    {
        setTransmitStatus(PREEND);
        return NO_ERROR;
    }

    //处理配置文件
    else if(0 == strcmp(QRdata, TRANSMIT_START))
    {
        mTransStatus = getTransmitStatus();
        setTransmitStatus(PREEND);

        //delay(300)//ms
        //创建所有的文件目录,（不包括碎片目录）
        des_ini_fragment_traversal(DES_INI_FOLD_LOCATION, 0);
        create_folder_tree_from_ini(); //only once;

        des_ini_fragment_traversal(DES_INI_FILE_LOCATION, 0);

        setTransmitStatus(TRANSMITTING);
    }

    else if(0 == strcmp(QRdata, TRANSMIT_END))
    {
        setTransmitStatus(END);
        //delay(300)//ms
        //遍历待拼接文件
        des_fragment_traversal(DES_RECEIVE_LOCATION, 0);
    }

    //处理配置文件或文件正文
    else
    {
        //发送报头
        if(PRESTART == getTransmitStatus()){
            des_prestart_content_receiver(QRdata);
        }
        //发送内容
        else if(TRANSMITTING == getTransmitStatus()){
            des_start_content_receiver(QRdata);
        }
        else{
            printf("Missed a frame!!!");
        }

        //记录每一个需要拼接的路径
    }
}

//解析最先收到的INI文件,接收并拼接完成后执行
int fragmentProcess::create_folder_tree_from_ini()
{
    char *sect = "FOLDER";

    //ini中记录所有的文件夹路径
    iniFileLoad(DES_INI_FOLD);

    static int count = 0;
    char key[256]= {0};
    char value[256] = {0};
    char *dir;

    dir = new char[PATH_MAX];
    memset(dir, 0, PATH_MAX);

    //发送的内容中没有文件夹
    if(0)
    {
        return -1;//稍后定义错误码
    }

    do{
        sprintf(key,"folder%d",count++);
        //iniSetString("FOLDER", str, total_dir);//name
        iniGetString(sect, key, value, sizeof(value), "");
        if(0 != strlen(value))
        {
            //mkdir 4 location
            sprintf(dir, "%s%s", DES_LOCATION, value);
            //printf("%s\n", dir);
            mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO);

            //mkdir 1_receive_location
            sprintf(dir, "%s%s", DES_RECEIVE_LOCATION, value);
            //printf("%s\n", dir);
            mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO);
        }
        //else
        //{
        //    break;
        //}
    }
    while (strlen(value)>0);

    free(dir);
    return NO_ERROR;
}

//process_QRdata_to_fragment完后执行
//含有路径信息和文件名
//函数功能：去掉报头，仅保留正文.                      DES_RECV_INI_LOCATION:生成到该目录
int fragmentProcess::des_prestart_content_receiver(char *QRdata)
{
    char *relative_dir  = new char[PATH_MAX];
    char *total_dir  = new char[PATH_MAX];
    char *name = new char[NAME_MAX];
    char *pureQRdata;//temp
    int *offset = (int *)malloc(sizeof(int));

    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    *offset = 0;
    //temp
    //pureQRdata = new char[QRDATA_SIZE];//temp
    memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp
    if(NO_ERROR != ret){
        return -1;
    }
    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;


    sprintf(total_dir, "%s%s", DES_RECV_INI_LOCATION, relative_dir); //生成到该目录

    //文件夹是否存在
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }

    strcat(total_dir, name);

    FILE *INI_Destination = fopen(total_dir, "wb"); //ab+;
    //测试读取二维码并生成文件，正式版删去
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), INI_Destination);

    fclose(INI_Destination); // 关闭文件


    free(relative_dir);
    free(total_dir);
    free(name);
    //free(pureQRdata);//temp
    free(offset);
    return NO_ERROR;
}

//process_QRdata_to_fragment完后执行
int fragmentProcess::des_prestart_content_receiver(char *QRdata, char *des_str)
{
    FILE *INI_Destination = fopen(des_str, "wb"); //ab+;
    //测试读取二维码并生成文件，正式版删去
    int size = fwrite(QRdata, 1, strlen(QRdata), INI_Destination);

    fclose(INI_Destination); // 关闭文件

    return NO_ERROR;
}

//process_QRdata_to_fragment完后执行
//含有路径信息和文件名
//函数功能：去掉报头，仅保留正文.                      DES_RECEIVE_LOCATION:生成到该目录
int fragmentProcess::des_start_content_receiver(char *QRdata)
{
    char *relative_dir  = new char[PATH_MAX];
    char *total_dir  = new char[PATH_MAX];
    char *name = new char[NAME_MAX];
    char *pureQRdata;//temp
    int *offset = (int *)malloc(sizeof(int));

    memset(relative_dir, 0, PATH_MAX);
    memset(total_dir, 0, PATH_MAX);
    memset(name, 0, NAME_MAX);

    *offset = 0;
    //temp
    //pureQRdata = new char[QRDATA_SIZE];//temp
    memset(pureQRdata, 0, QRDATA_SIZE);//temp

    //cutQRdata(QRdata, pureQRdata, relative_dir, name); //temp
    int ret = cutQRdata(QRdata, offset, relative_dir, name); //temp
    if(NO_ERROR != ret){
        return -1;
    }
    pureQRdata = QRdata;
    pureQRdata = pureQRdata + *offset;


    sprintf(total_dir, "%s%s", DES_RECEIVE_LOCATION, relative_dir);

    //文件夹是否存在
    if(0 != access(total_dir, F_OK))
    {
        mkdir(total_dir, S_IRWXU|S_IRWXG|S_IRWXO);
    }

    strcat(total_dir, name);

    FILE *INI_Destination = fopen(total_dir, "wb"); //ab+;
    //测试读取二维码并生成文件
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), INI_Destination);

    fclose(INI_Destination); // 关闭文件


    free(relative_dir);
    free(total_dir);
    free(name);
    //free(pureQRdata);//temp
    free(offset);
    return NO_ERROR;
}

//ini fragment traversal & process
void fragmentProcess::des_ini_fragment_traversal(string dir, int depth) //decode_base64_fragment
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
            //输出当前文件名
            printf("%*s%s/\n",depth," ",enty->d_name);

            //继续递归调用
            des_fragment_traversal(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输出当前目录名
            printf("%*s%s/\n",depth," ",enty->d_name);
            //get文件名
            /////vecString.push_back(total_dir);//NULL
            ///std::vector<std::array<char,255>>* vecString = reinterpret_cast<std::vector<std::array<char,255>>*>(total_dir);

            //拼接ini
            char *rename = new char[NAME_MAX];
            memset(rename, 0, NAME_MAX);
            cutFileName((char *)dir.c_str(), rename);
            //printf("%s\n", rename);
            cat((char *)dir.c_str(), DES_INI_LOCATION, rename);  //参数3为拼接完成后输出的目录（绝对路径）

            free(rename);


        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);
}

///process_QRdata_to_fragment完后执行,遍历碎片并恢复
void fragmentProcess::des_fragment_traversal(string dir, int depth) //decode_base64_fragment
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
            //输出当前文件名
            printf("%*s%s/\n",depth," ",enty->d_name);

            //继续递归调用
            des_fragment_traversal(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输出当前目录名
            //printf("%*s%s/\n",depth," ",enty->d_name);

            if(1)//is_base64_decode
            {
                char *des_str = new char[PATH_MAX];//home/montafan/QRcodeGrab/destination/2_base64_decode_location/nocolor.png/   //remeber free, flq
                char *diplay_content;
                memset(des_str, 0, PATH_MAX);
                strcat(des_str, DES_BASE64_DECODE_LOCATION);
                strcat(des_str, "nocolor.png/");
                strcat(des_str, enty->d_name);

                //dont forget mkdir fold
                FILE *infile = fopen(total_dir.c_str(), "rb");
                FILE *outfile = fopen(des_str, "w");

                decode(infile, outfile);//生成二进制文件百度首页设置登录

                free(des_str);
                fclose(infile);
                fclose(outfile);
            }
        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);
}


///逐帧识别,识别二维码并生成碎片
int fragmentProcess::process_QRdata_to_fragment(char *QRdata, char *des_str)
{
    //printf("process_QRdata_to_fragment\n");  //count
    FILE *Destination;

    if(0 == strlen(QRdata))
    {
        return -1;
    }

    ///if Idle mode
    if(0 == strcmp(QRdata, TRANSMIT_IDLE)){
        return NO_ERROR;
    }

    ///if Prestart mode
    if(0 == strcmp(QRdata, TRANSMIT_PRESTART)){
        ini_flag = true;  //置为ini状态
        return NO_ERROR;
    }

    ///if Start mode
    if(0 == strcmp(QRdata, TRANSMIT_START)){
        return NO_ERROR;
    }

    ///if End mode
    if(0 == strcmp(QRdata, TRANSMIT_END)){
        system("cat /home/montafan/QRcodeGrab/destination/2_base64_decode_location/nocolor.png/X* >>/home/montafan/QRcodeGrab/destination/3_cat_location/nocolor.png.lzo");
        ///processLZO(argc, argv, LZO_DECOMPRESS);
        return NO_ERROR;
    }

    ///在这里对目标路径收到的fragment做处理和还原 start
    //恢复ini文件
    if(ini_flag)
    {
        ini_flag = false;
    }

    //模拟传输完成，做base64解码
    printf("YYYYYYYYYYY strcmp(QRdata, TRANSMIT_END)=%d\n", strcmp(QRdata, TRANSMIT_END));
    static bool flag = true;
    if(0 == strcmp(QRdata, TRANSMIT_TEST) && flag){
        printf("===================逐帧接收二维码===========================\n");
        flag = false;
        des_fragment_traversal(DES_RECEIVE_LOCATION, 0);
        return NO_ERROR;
    }

    #if 1
    Destination = fopen(des_str, "wb"); //ab+;
    //测试读取二维码并生成文件，正式版删去
    int size = fwrite(QRdata, 1, strlen(QRdata), Destination);   //Temp delete
    printf("size=%d\n",size);

    fclose(Destination); // 关闭文件
    #endif

    ///在这里对目标路径收到的fragment做处理和还原 end

    return NO_ERROR;
}


//该函数废弃,在des_fragment_traversal()中做decode
int fragmentProcess::process_fragment_base64_decode(char *QRdata, char *des_str)
{
    FILE *Destination;

    if(0 == strlen(QRdata))
    {
        return NO_DATA;
    }

    ///if Idle mode
    ///if Start mode
    ///if End mode

    Destination = fopen(des_str, "wb"); //ab+;
    ///int size = fwrite(QRdata, 1, strlen(QRdata), Destination);   //Temp delete
    //printf("size=%d\n",size);

    fclose(Destination); // 关闭文件

    return NO_ERROR;
}


bool fragmentProcess::is_md5sum_match(char *QRdata)
{
    unsigned char *client_md5sum;///[MD5SUM_MAX_S];

    char *input_str = new char[512];

    memset(input_str, 0, 512);
    input_str = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111222222222";


    ///memset(client_md5sum, 0, MD5SUM_MAX_S);
    //client_md5sum = generate_md5sum_from_charstr(input_str);

    return NO_ERROR;
}

int fragmentProcess::setTransmitStatus(TransmitStatus status)
{
    mTransStatus = status;
    return NO_ERROR;
}

TransmitStatus fragmentProcess::getTransmitStatus()
{
    return mTransStatus;
}
