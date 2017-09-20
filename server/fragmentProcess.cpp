#include "fragmentProcess.h"
#include "../instructions/stats.h"
#include "../instructions/inirw.h"
#include "../instructions/cat.h"

#include "LZO/lzopack.h"

#include "include/DirPath.h"
#include "../instructions/base64.h"

#include "include/Errors.h"
#include "include/macros.h"
#include "ScanCode.h"

#include <mutex>

#include <string>
using namespace std;


fragmentProcess::fragmentProcess()
{
    init();
    ini_flag = false;

    is_folder_created = false;

    //m_stateMachine = new RecvStateMachine(this);  //flq
    m_stateMachine = RecvStateMachine::getInstance();

    //test for单例测试，是否成功
    /*
    RecvStateMachine* singleton1 = RecvStateMachine::getInstance();
    RecvStateMachine* singleton2 = RecvStateMachine::getInstance();

    if (singleton1 == singleton2)
        printf("singleton1 = singleton2\n");
    */
    //test for单例测试，是否成功end
}

fragmentProcess::~fragmentProcess()
{
    free(m_stateMachine);
}

int fragmentProcess::init(){

}

//整个处理的入口函数，识别二维码成功后进入
int fragmentProcess::QRdataProcess(char* QRdata)
{
#ifdef USE_DEBUG
    //TEST
    //cat后ini即完全恢复start
    ///*
    des_ini_fragment_traversal(DES_INI_FOLD_LOCATION, 0);
    if(1)
    {
        create_folder_tree_from_ini(); //only once;
    }

    des_ini_fragment_traversal(DES_INI_FILE_LOCATION, 0);
    //*/
    //cat后ini即完全恢复 end

    //对没有报头的碎片，做base64,并生成文件到2_base64文件夹 start
    des_fragment_traversal();
    //对没有报头的碎片，做base64,并生成文件到2_base64文件夹 end
    //TEST END
#endif
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
        ///遍历完后拼接fold.ini
        des_ini_fragment_traversal_imp(DES_INI_FOLD_LOCATION, 0);
        if(!is_folder_created)
        {
            printf("CREATE_FOLDER_FROM_INI\n");
            create_folder_tree_from_ini(); //only once;
            is_folder_created = true;
        }

        //遍历完后拼接config.ini
        des_ini_fragment_traversal_imp(DES_INI_FILE_LOCATION, 0);

        setTransmitStatus(TRANSMITTING);
    }

    //开始遍历1_receiver正文
    else if(0 == strcmp(QRdata, TRANSMIT_END))
    {
        setTransmitStatus(END);
        //delay(300)//ms
        ///遍历待拼接文件
        printf("FRAGMENT_TRAVERSAL\n");
        des_fragment_traversal();
    }

    //处理配置文件或文件正文
    else
    {
        //发送报头
        if(PRESTART == getTransmitStatus()){
            ///接收报头并放到目标目录
            printf("RECEIVE_INI\n");
            des_prestart_content_receiver(QRdata);
        }
        //发送内容
        else if(TRANSMITTING == getTransmitStatus()){
            ///接收正文并放到目标目录
            printf("RECEIVE_CONTENT\n");
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

            //mkdir 2_base64_location
            sprintf(dir, "%s%s", DES_BASE64_DECODE_LOCATION, value);
            //printf("%s\n", dir);
            mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO);

            //mkdir 3_cat_location
            sprintf(dir, "%s%s", DES_CAT_LOCATION, value);
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

/*函数功能:识别的二维码信息裁剪完后,放入1_receive文件夹.   做base64 decode完后,再放入2_base64文件夹*/
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
    pureQRdata = new char[QRDATA_SIZE];//temp
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

    FILE *cut_head_Destination = fopen(total_dir, "wb"); //ab+;

#if 1  //#ifdef DES_DECODE
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);
    //解码并生成文件
    memset(total_dir, 0, PATH_MAX);
    sprintf(total_dir, "%s%s%s", DES_BASE64_DECODE_LOCATION, relative_dir,name);
    FILE *base64_decode_Destination = fopen(total_dir, "w");

    decode(pureQRdata, base64_decode_Destination);
#else
    //1_receive 生成文件
    int size = fwrite(pureQRdata, 1, strlen(pureQRdata), cut_head_Destination);
#endif

    fclose(cut_head_Destination); // 关闭文件
    fclose(base64_decode_Destination);


    free(relative_dir);
    free(total_dir);
    free(name);
    //free(pureQRdata);//temp
    free(offset);
    return NO_ERROR;
}

//说明：des_fragment_traversal_imp()的对外接口函数
int fragmentProcess::des_ini_traversal()
{
    des_ini_fragment_traversal_imp(DES_INI_FOLD_LOCATION, 0);

    printf("CREATE_FOLDER_FROM_INI\n");
    create_folder_tree_from_ini(); //创建文件夹

    //遍历完后拼接config.ini
    des_ini_fragment_traversal_imp(DES_INI_FILE_LOCATION, 0);
    return 0;
}

//ini fragment traversal & process
void fragmentProcess::des_ini_fragment_traversal_imp(string dir, int depth) //decode_base64_fragment
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
            des_ini_fragment_traversal_imp(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输出当前目录名
            printf("%*s%s/\n",depth," ",enty->d_name);

            //拼接ini
            char *rename = new char[NAME_MAX];
            memset(rename, 0, NAME_MAX);
            cutDirName((char *)dir.c_str(), rename);
            //printf("%s\n", rename);
            cat((char *)dir.c_str(), DES_INI_LOCATION, rename);  //参数23拼接完成为后输出的目录（xiangdui路径）

            free(rename);

        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);
}
#if 1
/*
int fragmentProcess::des_file_traversal()
{
    char *topDir = SRC_LOCATION;//"/home/montafan/QRcodeGrab/source/location/";
    char relativeDir[PATH_MAX] = {0};
    char *_2_dir = SRC_LZO_LOCATION;//"/home/montafan/QRcodeGrab/source/2_lzo_location/";
    char *_3_dir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
    char *_4_dir = SRC_BASE64_ENCODE_LOCATION;//"/home/montafan/QRcodeGrab/source/4_base64_encode_location/";

    printf("Directory scan of %s\n",topDir);

    is_base64 = false;//flag, dont neglect

    FILE *ini_file = fopen(HEAD, "w");
    fclose(ini_file);
    //iniFileLoad(HEAD);

    FILE *ini_folder = fopen(folderHead, "w");
    fclose(ini_folder);
    //iniFileLoad(folderHead);

    ///遍历源文件夹并生成所有的文件夹,处理完后，在3中生成碎片
    des_file_traversal_imp(topDir, relativeDir,_2_dir, _3_dir, _4_dir, 0);

    printf("Transmit Done\n");
    return 0;
}
*/


//说明：des_fragment_traversal_imp()的对外接口函数
int fragmentProcess::des_fragment_traversal()
{
    char *fragmentDir = DES_BASE64_DECODE_LOCATION;//DES_RECEIVE_LOCATION,    DES_BASE64_DECODE_LOCATION
    char *fragmentDes = DES_CAT_LOCATION;   //gigeGrab.cpp
    char relativeDir[PATH_MAX] = {0};

    printf("des_fragment_traversal, Directory fragement scan of %s\n",fragmentDir);

    des_fragment_traversal_imp(fragmentDir, relativeDir, fragmentDes, 0);
    return 0;
}
#endif

///process_QRdata_to_fragment完后执行,遍历碎片并恢复
//遍历相对目录
void fragmentProcess::des_fragment_traversal_imp(char *dir, char* _short_dir, char *des, int depth) //decode_base64_fragment
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径
    char *total_dir;
    char *relative_dir;
    char *des_str;
    bool need_cat = false;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"can not open dir:%s\n",dir);
        return;
    }

    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
    relative_dir = new char[PATH_MAX];
    memset(relative_dir, 0, PATH_MAX);

    des_str = new char[PATH_MAX];
    memset(des_str, 0, PATH_MAX);

    //切换到这个目录
    chdir(dir);

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

            //total_dir = dir + enty->d_name + "/";
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            strcat(total_dir,"/");

            //相对路径拼接
            sprintf(relative_dir, "%s%s/", _short_dir, enty->d_name);

            strcpy(des_str,des);
            strcat(des_str,enty->d_name);
            strcat(des_str,"/");

            //输出当前文件名
            ///////printf("des_fragment_traversal_imp(),FOLDER,%*s%s/\n",depth," ",enty->d_name);

            //继续递归调用
            des_fragment_traversal_imp(total_dir, relative_dir, des_str, depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);

            //输出当前目录名
            printf("des_fragment_traversal_imp(),FILE,%*s%s/\n",depth," ",enty->d_name);
            if(enty->d_name[0] !='X')
            {
                #if 1  ///flq    copy file
                //不需裁剪，从目录2拷贝到目录3 & 4 start
                char *content_buf = new char[statbuf.st_size];  //statbuf.st_size + 1
                char *output3Dir = new char[PATH_MAX];
                char *output4Dir = new char[PATH_MAX];
                memset(content_buf, 0, statbuf.st_size);
                memset(output3Dir, 0, PATH_MAX);
                memset(output4Dir, 0, PATH_MAX);
#if 0
                sprintf(output3Dir, "%s%s%s%s", DES_CAT_LOCATION, _short_dir, enty->d_name, LZO_SUFFIX); //xxx.c
                sprintf(output4Dir, "%s%s%s%s", DES_LOCATION, _short_dir, enty->d_name, LZO_SUFFIX); //xxx.c
#else
                //此处的完整文件均是小于blocksize的,所以直接拷贝
                sprintf(output3Dir, "%s%s%s", DES_CAT_LOCATION, _short_dir, enty->d_name); //xxx.c
                sprintf(output4Dir, "%s%s%s", DES_LOCATION, _short_dir, enty->d_name); //xxx.c
#endif

                FILE *infile = fopen(total_dir, "rb");
                FILE *outfile = fopen(output3Dir, "wb");
                FILE *outfile4 = fopen(output4Dir, "wb");

                fread(content_buf,1,statbuf.st_size,infile);
                fwrite(content_buf, 1, statbuf.st_size, outfile);
                fwrite(content_buf, 1, statbuf.st_size, outfile4);

                fclose(infile);
                fclose(outfile);
                fclose(outfile4);

                free(content_buf);
                free(output3Dir);
                free(output4Dir);
                //不需裁剪，从目录2拷贝到目录3 end
                #endif

                need_cat = false;
            } else
            {
                need_cat = true;
            }
        }
    }

    ///遍历完目录下的文件，开始做拼接
    //待拼接正文碎片的目录
    if(true == need_cat)
    {
        char *output3Dir = new char[PATH_MAX];
        char *purename = new char[NAME_MAX];
        char *rename = new char[NAME_MAX];

        memset(output3Dir, 0, PATH_MAX);
        memset(purename, 0, NAME_MAX);
        memset(rename, 0, NAME_MAX);

        sprintf(output3Dir, "%s%s", DES_CAT_LOCATION, _short_dir);
        cutDirName(output3Dir, purename);//input should be a filefold

        //printf("%s\n", purename);
        getUpperTotalDir(output3Dir);

#ifdef USE_LZO_COMPRESSION
        //add .lzo
        strcpy(rename, purename);
        strcat(rename, LZO_SUFFIX);
        cat(dir, output3Dir, rename);  //参数23拼接完成为后输出的目录（xiangdui路径
        strcat(output3Dir, rename);//将output3Dir作为LZO解压缩的输入
#else
        cat(dir, output3Dir, rename);  //参数23拼接完成为后输出的目录（xiangdui路径）
#endif

        //裁剪的文件，还需要做LZO解压缩 start
#ifdef USE_LZO_COMPRESSION
        char *lzo_dir =new char[PATH_MAX];
        memset(lzo_dir, 0, PATH_MAX);
        sprintf(lzo_dir, "%s%s", DES_LOCATION, _short_dir);
        getUpperTotalDir(lzo_dir);//返回上一级目录路径
        strcat(lzo_dir, purename);
        //strcat(lzo_dir,LZO_SUFFIX);  //LZO_SUFFIX = ".lzo"
        processLZO(output3Dir, lzo_dir, LZO_DECOMPRESS);  //flq
        free(lzo_dir);
#endif
        //裁剪的文件，还需要做LZO解压缩 end

        need_cat = false;

        free(output3Dir);
        free(purename);
        free(rename);
    }


    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);
}

//遍历绝对目录
void fragmentProcess::des_fragment_traversal_imp(string dir, int depth) //decode_base64_fragment
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
            des_fragment_traversal_imp(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            total_dir = dir + enty->d_name;

            //输出当前目录名
            //printf("%*s%s/\n",depth," ",enty->d_name);

            if(0)//is_base64_decode
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
        des_fragment_traversal_imp(DES_RECEIVE_LOCATION, 0);
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
