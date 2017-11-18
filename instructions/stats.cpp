//Auther fanlinqing
//获取文件属性，并遍历文件
//实现非常多业务的文件

#include <time.h>

#include "stats.h"
#include "inirw.h"
//http://bbs.csdn.net/topics/391842320
#include "md5sum.h"
//#include "qrgenerator.h"
#include "include/fileParameters.h"
#include "instructions/base64.h"
#include "include/DirPath.h"

#include "LZO/lzopack.h"//temp
#include "instructions/split.h"
#include "include/Errors.h"

//#define PRINT_MD5SUM

const char *HEAD = SRC_INI_FILE_LOCATION;//"/home/montafan/QRcodeGrab/source/INI/config.ini";
const char *folderHead = SRC_INI_FOLD_LOCATION;//"/home/montafan/QRcodeGrab/source/INI/folder.ini";
//const char *fragmentHEAD = "/home/montafan/QRcodeGrab/source/INI/nocolor.png/ini/config.ini"; //need modify soon
//const char *iniHEAD = "/home/montafan/QRcodeGrab/source/INI/nocolor.png/ini/config.ini"; //need modify soon

unsigned char md5sum_str[MD5SUM_MAX];
unsigned char md5sum_str_hex[MD5SUM_MAX];

bool is_base64 = true;

long file_cnt = 0;

int src_init_topology()
{
    CompletePath();

    chdir(ROOT_DIR);
    mkdir("QRcodeGrab", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("QRcodeGrab");

    //normal
    mkdir("source", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("source");
    mkdir("normal", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("normal");

    mkdir("1_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_lzo_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_split_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_base64_encode_location", S_IRWXU|S_IRWXG|S_IRWXO);

    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    ///mkdir("FRAG_INI", S_IRWXU|S_IRWXG|S_IRWXO);

    //UDP
    chdir("..");

    mkdir("UDP", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("UDP");

    mkdir("1_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_lzo_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_split_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_base64_encode_location", S_IRWXU|S_IRWXG|S_IRWXO);

    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    return 0;
}

int des_init_topology()
{
    CompletePath();

    chdir(ROOT_DIR);
    mkdir("QRcodeGrab", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("QRcodeGrab");

    //normal
    mkdir("destination", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("destination");
    mkdir("normal", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("normal");

    mkdir("1_receive_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_base64_decode_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_cat_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("recvINI", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    //所有完整文件的属性
//    mkdir("recvINI/config.ini", S_IRWXU|S_IRWXG|S_IRWXO);
//    mkdir("recvINI/folder.ini", S_IRWXU|S_IRWXG|S_IRWXO);

    //UDP
    //normal
    chdir("..");

    mkdir("UDP", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("UDP");

    mkdir("1_receive_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_base64_decode_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_cat_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("recvINI", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    //所有完整文件的属性
//    mkdir("recvINI/config.ini", S_IRWXU|S_IRWXG|S_IRWXO);
//    mkdir("recvINI/folder.ini", S_IRWXU|S_IRWXG|S_IRWXO);

    return 0;
}

//遍历完整文件，生成未做base64 encode的碎片
void src_file_traversal_imp(char *dir, char* _short_dir, char *_2_dir, char *_3_dir, char *_4_dir, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;
    char *relative_dir;

    char *_2_lzo_dir;
    char *_3_split_dir;
    char *_4_base64_encode_dir;

    char value[34];

    memset(value, 0, 34);

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
    _2_lzo_dir = new char[PATH_MAX];
    memset(_2_lzo_dir, 0, PATH_MAX);
    _3_split_dir = new char[PATH_MAX];
    memset(_3_split_dir, 0, PATH_MAX);
    _4_base64_encode_dir = new char[PATH_MAX];
    memset(_4_base64_encode_dir, 0, PATH_MAX);

    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

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

            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            strcat(total_dir,"/");

            //相对路径拼接
            sprintf(relative_dir, "%s%s/", _short_dir, enty->d_name);

            //输出当前目录名
            printf("%*s%s/\n",depth," ",enty->d_name);

            //ini中记录所有的文件夹路径
/////////////////////////////////            iniFileLoad(folderHead);

            static int count = 0;
            char str[256]= {0};
            sprintf(str,"folder%d",count++);
            iniSetString("FOLDER", str, relative_dir);//局部路径

            //2_lzo_location
            strcpy(_2_lzo_dir,_2_dir);
            strcat(_2_lzo_dir,enty->d_name);
            mkdir(_2_lzo_dir, S_IRWXU|S_IRWXG|S_IRWXO);
            strcat(_2_lzo_dir,"/");
            //3_split_location
            strcpy(_3_split_dir,_3_dir);
            strcat(_3_split_dir,enty->d_name);
            mkdir(_3_split_dir, S_IRWXU|S_IRWXG|S_IRWXO);
            strcat(_3_split_dir,"/");
            //4_base64_encode_location
            strcpy(_4_base64_encode_dir,_4_dir);
            strcat(_4_base64_encode_dir,enty->d_name);
            mkdir(_4_base64_encode_dir, S_IRWXU|S_IRWXG|S_IRWXO);
            strcat(_4_base64_encode_dir,"/");

            //继续递归调用
            src_file_traversal_imp(total_dir, relative_dir, _2_lzo_dir, _3_split_dir, _4_base64_encode_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        //是文件
        else
        {
            //added by flq, get absolute path
            ///获取其他文件夹下的文件绝对路径
            //1的绝对路径
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            printf("%s\n", total_dir);
            //generate_md5sum(total_dir);
            //1的相对路径
            sprintf(relative_dir, "%s%s", _short_dir, enty->d_name);

            //2,3,4的绝对路径
            strcpy(_2_lzo_dir,_2_dir);
            strcat(_2_lzo_dir,enty->d_name);
            strcpy(_3_split_dir,_3_dir);
            strcat(_3_split_dir,enty->d_name);
            strcpy(_4_base64_encode_dir,_4_dir);
            strcat(_4_base64_encode_dir,enty->d_name);

            //输出文件名,后续可屏蔽
            printf("%*s%s",depth," ",enty->d_name);
            printf(", statbuf.st_size=%d\n", statbuf.st_size);
            //I:原文件目录的INI
            iniFileLoad(HEAD);

            sprintf(value, "%d", file_cnt); //将file_cnt转为10进制表示的字符串  %x:16进制
            //参数1为sect
            iniSetString(value/*enty->d_name*/, "name", enty->d_name);//name
            iniSetString(value, "path", relative_dir);//path
            iniSetInt(value, "size", statbuf.st_size, 0);//size
            iniSetString(value, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();
            file_cnt++;

            //LZO压缩
            FILE *in_file = fopen(total_dir, "rb");
            //FILE *out_2_file = fopen(_2_lzo_dir, "wb");
            //FILE *out_3_file = fopen(_3_split_dir, "wb");
            //FILE *out_4_file = fopen(_4_base64_encode_dir, "wb"); //temp delete

            if((statbuf.st_size > 0) && (statbuf.st_size < BLOCK_SIZE)){
                //copy to des
                FILE *out_2_file = fopen(_2_lzo_dir, "wb");
                FILE *out_3_file = fopen(_3_split_dir, "wb");

                char *des_buf = new char[statbuf.st_size];///statbuf.st_size+1
                memset(des_buf, 0, statbuf.st_size);
                fread(des_buf,1,statbuf.st_size,in_file);
                //写小文件
                fwrite(des_buf, 1, statbuf.st_size, out_2_file);

                ///WHY CRASH???????
                //memset(des_buf, 0, statbuf.st_size);
                //fread(des_buf,1,statbuf.st_size,in_file);
                fwrite(des_buf, 1, statbuf.st_size, out_3_file);

                //memset(des_buf, 0, statbuf.st_size);
                //fread(des_buf,1,statbuf.st_size,in_file);
                //fwrite(des_buf, 1, statbuf.st_size, out_4_file);

                free(des_buf);
                fclose(out_2_file);
                fclose(out_3_file);
            }
            else {
            //else if((statbuf.st_size > BLOCK_SIZE) && (statbuf.st_size < BLOCK_SIZE*512)) {//<1M
                //copy to des
                char *des_buf = new char[statbuf.st_size];

                //2_lzo_location中,压缩文件
                memset(des_buf, 0, statbuf.st_size);
                fread(des_buf,1,statbuf.st_size,in_file);
                //写中等文件,有压缩包即可，实际不需要fwrite
                //fwrite(des_buf, 1, statbuf.st_size, out_2_file);

                //压缩des_buf的内容到文件夹,到3_split_location
#ifdef USE_LZO_COMPRESSION
                char *lzo_dir =new char[PATH_MAX];
                memset(lzo_dir, 0, PATH_MAX);
                    strcpy(lzo_dir,_2_lzo_dir);
                    strcat(lzo_dir,LZO_SUFFIX);  //LZO_SUFFIX = ".lzo"

                LOG_DBG("%s, processLZO start\n", __func__);
                processLZO(total_dir, lzo_dir, LZO_COMPRESS);
                LOG_DBG("%s, processLZO end\n", __func__);
#endif
                //3_split_location,切割文件
                //memset(des_buf, 0, statbuf.st_size);
                //fwrite(des_buf, 1, statbuf.st_size, out_3_file);
                if(0 != access(_3_split_dir, F_OK))    // _3_split_dir=/home/montafan/QRcodeGrab/source/3_split_location/zbar_gige
                {
                    LOG_DBG("mkdir _3_split_dir\n");
                    mkdir(_3_split_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
                    //在最后会添加一个额外的过程来显示碎片路径的ini,
                    //在此添加虽然代码显得麻烦，但会在接收端的每次接收都减少一次判断
                    //consider whether need to add code to generate a ini of fragment for mkdir
                    //...
                    //...
                    //...
                } else{
                    LOG_DBG("exist, need not mkdir _3_split_dir\n");
                }
#ifdef USE_LZO_COMPRESSION
                char *outputDir = new char[PATH_MAX];
                memset(outputDir, 0, PATH_MAX);
                sprintf(outputDir, "%s%s%s%s", SRC_LZO_LOCATION, _short_dir, enty->d_name, LZO_SUFFIX);

                LOG_DBG("%s, split start, outputDir=%s, _3_split_dir=%s, blocksize=%d\n", __func__, outputDir, _3_split_dir ,BLOCK_SIZE);
                split(outputDir, _3_split_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
                LOG_DBG("%s, split end\n", __func__);

                free(outputDir);
                free(lzo_dir);

#else
                split(total_dir, _3_split_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
#endif
                if(0 != access(_4_base64_encode_dir, F_OK))
                {
                    mkdir(_4_base64_encode_dir, S_IRWXU|S_IRWXG|S_IRWXO);
                }

                //4_base64_encode_location,base64 encode文件
                //memset(des_buf, 0, statbuf.st_size);
                //fwrite(des_buf, 1, statbuf.st_size, out_4_file);


                free(des_buf);
            }/* else if(statbuf.st_size > 2048*512){ //>1M
                ///支持对1M以上文件的传输

                //split();
                //LZO
                //base64
            }*/
            fclose(in_file);
            //fclose(out_2_file);
            //fclose(out_3_file);
            //fclose(out_4_file);
            //LZO压缩 end
        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

    //free
    free(total_dir);
    free(_2_lzo_dir);
    free(_3_split_dir);
    free(_4_base64_encode_dir);


}

//报头文件碎片化
void src_ini_traversal_imp(/*char *dir*/)
{
    //目录位置
    char *config_dir;
    char *folder_dir;
    //文件位置

    config_dir = new char[PATH_MAX];
    memset(config_dir, 0, PATH_MAX);
    folder_dir = new char[PATH_MAX];
    memset(folder_dir, 0, PATH_MAX);

    sprintf(config_dir,"%sconfig/",SRC_INI_LOCATION);
    sprintf(folder_dir,"%sfolder/",SRC_INI_LOCATION);

    //稍后判断文件是否为空start
    //flq, to be added
    //稍后判断文件是否为空end

    //稍后判断文件是否exist
    //access(config_dir, F_OK);  //got=0, none or without permission

    ///后续在此添加报头
    //方法:遍历时读取，并添加内容
    if(0 != access(config_dir, F_OK))
    {
        mkdir(config_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    }
    #ifdef INI_FRAGMENT_WITHOUT_MASTHEAD
    split(SRC_INI_FILE_LOCATION, config_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    #else
    //split_ini(SRC_INI_FILE_LOCATION, config_dir, "config/", BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    split_ini(SRC_INI_FILE_LOCATION, config_dir, "config.ini/", BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    #endif
    if(0 != access(folder_dir, F_OK))
    {
        mkdir(folder_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    }
    #ifdef INI_FRAGMENT_WITHOUT_MASTHEAD
    split(SRC_INI_FOLD_LOCATION, folder_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    #else
    //split_ini(SRC_INI_FOLD_LOCATION, folder_dir, "folder/", BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    split_ini(SRC_INI_FOLD_LOCATION, folder_dir, "folder.ini/", BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    #endif

    free(config_dir);
    free(folder_dir);
}



//遍历3目录的碎片文件，做base64 encode的
//src_fragment_traversal_imp
void src_fragment_traversal_imp(char *dir, char* _short_dir, char *des, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;
    char *relative_dir;
    char *des_str;

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

    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

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

            #if 0
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            //strcat(total_dir,"/");

            //输出当前目录名 3_split_location
            printf("%*s%s/\n",depth," ",enty->d_name);
            mkdir(enty->d_name, S_IRWXU|S_IRWXG|S_IRWXO);//int mkdir(const char *path, mode_t mode); //#include <sys/stat.h>

            strcat(total_dir,"/");
            #else
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            strcat(total_dir,"/");
            #endif

            //相对路径拼接
            sprintf(relative_dir, "%s%s/", _short_dir, enty->d_name);

            strcpy(des_str,des);
            strcat(des_str,enty->d_name);
            strcat(des_str,"/");

            //I:碎片文件目录的INI
//////////////////            iniFileLoad(fragmentHEAD);
            //继续递归调用
            src_fragment_traversal_imp(total_dir, relative_dir, des_str, depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            //printf("%s\n", total_dir);
            //generate_md5sum(total_dir);
            //相对路径拼接
            sprintf(relative_dir, "%s%s", _short_dir, enty->d_name);

            //输出文件名
            //added by flq
            iniSetString(enty->d_name, "name", enty->d_name);//name
            iniSetString(enty->d_name, "path", total_dir);//path
            iniSetInt(enty->d_name, "size", statbuf.st_size, 0);//size
            //碎片不需要MD5SUM
            //iniSetString(enty->d_name, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();
            //added end
            /////////////////////////BASE64 ENCODE////////////////////////////
            if(is_base64){
                #if 0
                char *des_str = new char[PATH_MAX];///home/montafan/QRcodeGrab/source/4_base64_encode_location/   //remeber free, flq
                memset(des_str, 0, PATH_MAX);
                strcat(des_str, SRC_BASE64_ENCODE_LOCATION);
                strcat(des_str, "nocolor.png/");
                strcat(des_str, enty->d_name);
                #else
                strcpy(des_str,des);
                strcat(des_str,enty->d_name);
                #endif
                char *diplay_content;

                //dont forget mkdir fold, already done
                FILE *infile = fopen(total_dir, "rb");
                FILE *outfile = fopen(des_str, "wb");

                #if 1
                //encode(infile, outfile);//OK
                encode(infile, outfile, _short_dir, enty->d_name);//添加路径信息
                #else
                //first length. then new, then
                int length = get_length_after_base64(infile);
                diplay_content = new char[length];
                encode(infile, diplay_content);
                #endif
                ///===============后续在此生成二维码===============//
                ///here qrgenrator

                //free(des_str);
                fclose(infile);
                fclose(outfile);
            }
        }
    }

    ///***********************************通知到显示模块**********************************//
    ////QRGenerator w;
    ////w.StartTimer();

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

    //free
    free(total_dir);
    free(des_str);
}




// 遍历完整文件，prototype，后续会删除
void print_INI_Info(char *dir, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"can not open dir:%s\n",dir);
        return;
    }

    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

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

            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            //strcat(total_dir,"/");

            //输出当前目录名 3_split_location
            printf("%*s%s/\n",depth," ",enty->d_name);
            mkdir(enty->d_name, S_IRWXU|S_IRWXG|S_IRWXO);//int mkdir(const char *path, mode_t mode); //#include <sys/stat.h>

            strcat(total_dir,"/");

            //继续递归调用
            print_INI_Info(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            printf("%s\n", total_dir);
            //generate_md5sum(total_dir);

            //输出文件名
            printf("%*s%s",depth," ",enty->d_name);
            printf(", statbuf.st_size=%d\n", statbuf.st_size);
            //added by flq
            ///iniSetString(enty->d_name, "name", enty->d_name);//name
            ///iniSetString(enty->d_name, "path", total_dir);//path
            ///iniSetInt(enty->d_name, "size", statbuf.st_size, 0);//size
            ///iniSetString(enty->d_name, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();
            //added end
            /////////////////////////BASE64 ENCODE////////////////////////////
            if(is_base64){
                char *des_str = new char[PATH_MAX];///home/montafan/QRcodeGrab/source/4_base64_encode_location/   //remeber free, flq
                char *diplay_content;
                memset(des_str, 0, PATH_MAX);
                strcat(des_str, SRC_BASE64_ENCODE_LOCATION);
                strcat(des_str, "nocolor.png/");
                strcat(des_str, enty->d_name);

                //dont forget mkdir fold
                FILE *infile = fopen(total_dir, "rb");
                FILE *outfile = fopen(des_str, "wb");

                #if 1
                encode(infile, outfile);//OK
                #else
                //first length. then new, then
                int length = get_length_after_base64(infile);
                diplay_content = new char[length];
                encode(infile, diplay_content);
                #endif
                ///===============后续在此生成二维码===============//
                ///here qrgenrator

                free(des_str);
                fclose(infile);
                fclose(outfile);
            }
        }
    }

    ///***********************************通知到显示模块**********************************//
    ////QRGenerator w;
    ////w.StartTimer();

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

    //free
    free(total_dir);
}


#if 0
//后续会评估是否移动到src中
void des_ini_traversal_imp(char *dir, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"can not open dir:%s\n",dir);
        return;
    }

    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

    //切换到这个目录
    chdir(dir);
    //printf("NAME_MAX=%d\n\n\n\n\n\n", NAME_MAX);
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

            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            //strcat(total_dir,"/");

            //输出当前目录名 3_split_location
            printf("%*s%s/\n",depth," ",enty->d_name);
            mkdir(enty->d_name, S_IRWXU|S_IRWXG|S_IRWXO);//int mkdir(const char *path, mode_t mode); //#include <sys/stat.h>

            strcat(total_dir,"/");

            //继续递归调用
            /////////print_INI_Info(enty->d_name,depth+4);
            print_INI_Info(total_dir,depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            printf("%s\n", total_dir);
            //generate_md5sum(total_dir);

            //输出文件名
            printf("%*s%s",depth," ",enty->d_name);
            printf(", statbuf.st_size=%d\n", statbuf.st_size);
            //added by flq
            iniSetString(enty->d_name, "name", enty->d_name);//name
            iniSetString(enty->d_name, "path", total_dir);//path
            iniSetInt(enty->d_name, "size", statbuf.st_size, 0);//size
            iniSetString(enty->d_name, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();
            //added end
            /////////////////////////BASE64 ENCODE////////////////////////////
            if(is_base64){
                char *des_str = new char[PATH_MAX];///home/montafan/QRcodeGrab/source/4_base64_encode_location/   //remeber free, flq
                char *diplay_content;
                memset(des_str, 0, PATH_MAX);
                strcat(des_str, SRC_BASE64_ENCODE_LOCATION);
                strcat(des_str, "nocolor.png/");
                strcat(des_str, enty->d_name);

                //dont forget mkdir fold
                FILE *infile = fopen(total_dir, "rb");
                FILE *outfile = fopen(des_str, "w");

                #if 1
                encode(infile, outfile);//OK
                #else
                //first length. then new, then
                int length = get_length_after_base64(infile);
                diplay_content = new char[length];
                encode(infile, diplay_content);
                #endif
                ///===============后续在此生成二维码===============//
                ///here qrgenrator

                free(des_str);
                fclose(infile);
                fclose(outfile);
            }
        }
    }

    ///***********************************通知到显示模块**********************************//
    ////QRGenerator w;
    ////w.StartTimer();

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

    //free
    free(total_dir);
}
#endif


int file_traversal()
{
    char *topDir = SRC_LOCATION;//"/home/montafan/QRcodeGrab/source/location/";
    char relativeDir[PATH_MAX] = {0};
    char *_2_dir = SRC_LZO_LOCATION;//"/home/montafan/QRcodeGrab/source/2_lzo_location/";
    char *_3_dir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
    char *_4_dir = SRC_BASE64_ENCODE_LOCATION;//"/home/montafan/QRcodeGrab/source/4_base64_encode_location/";

    printf("file_traversal(), Directory scan of %s\n",topDir);

    is_base64 = false;//flag, dont neglect

    FILE *ini_file = fopen(HEAD, "wb");
    fclose(ini_file);

    FILE *ini_folder = fopen(folderHead, "wb");
    fclose(ini_folder);

    ///遍历源文件夹并生成所有的文件夹,处理完后，在3中生成碎片
    ///如果不遍历文件，改为解析JSON中的文件名  flq

    src_file_traversal_imp(topDir, relativeDir,_2_dir, _3_dir, _4_dir, 0);
    printf("src_file_traversal_imp(), Done\n");

    return 0;
}

#ifdef USE_ACTIVEMQ
int file_select(activeMQVec msg, const char* jsonStr)
{
    char *config_ini_dir; //生成的ini文件的绝对路径
    char *symbol = "/";

    config_ini_dir = new char[PATH_MAX];
    memset(config_ini_dir, 0, PATH_MAX);

    if(UDP == msg.type)
    {
        char *topDir = SRC_UDP_LOCATION;//"/home/montafan/QRcodeGrab/source/location/";
        char relativeDir[PATH_MAX] = {0};  //tqq
        char *_2_dir = SRC_UDP_LZO_LOCATION;//"/home/montafan/QRcodeGrab/source/2_lzo_location/";
        char *_3_dir = SRC_UDP_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
        char *_4_dir = SRC_UDP_BASE64_ENCODE_LOCATION;//"/home/montafan/QRcodeGrab/source/4_base64_encode_location/";
        ///后续改为读解析出的JSON信息
        //char date[PATH_MAX] = "2017-3-3";
        //char d_name[NAME_MAX] = "hu.png";
        char date[PATH_MAX] ={0};
        char d_name[PATH_MAX] ={0};

        //避免传const
        strcpy(date, msg.date.c_str());
        strcpy(d_name, msg.filename.c_str());

        printf("Directory scan of %s\n",topDir);

        is_base64 = false;//flag, dont neglect

        //创建ini相关文件夹与文件start
        strcpy(config_ini_dir, SRC_UDP_INI_LOCATION);
        strcat(config_ini_dir, date);
        mkdir(config_ini_dir, S_IRWXU|S_IRWXG|S_IRWXO);  //date
        strcat(config_ini_dir, symbol);
        strcat(config_ini_dir, d_name);
        mkdir(config_ini_dir, S_IRWXU|S_IRWXG|S_IRWXO);  //name

        memset(config_ini_dir, 0, PATH_MAX);

        sprintf(config_ini_dir,"%s%s/%s/config.ini", SRC_UDP_INI_LOCATION, date, d_name);
        //创建ini相关文件夹与文件end

        FILE *ini_file = fopen(config_ini_dir, "wb");
        fclose(ini_file);

        //FILE *ini_folder = fopen(folderHead, "wb");
        //fclose(ini_folder);

        ///不遍历文件，改为解析JSON中的文件名
        src_file_select_imp(topDir, relativeDir,_2_dir, _3_dir, _4_dir, date, d_name, msg.type, jsonStr);
        printf("src_file_select_imp(), Done\n");
    }
    else if(NORMAL == msg.type)
    {
        char *topDir = SRC_LOCATION;//"/home/montafan/QRcodeGrab/source/location/";
        char relativeDir[PATH_MAX] = {0};  //tqq
        char *_2_dir = SRC_LZO_LOCATION;//"/home/montafan/QRcodeGrab/source/2_lzo_location/";
        char *_3_dir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
        char *_4_dir = SRC_BASE64_ENCODE_LOCATION;//"/home/montafan/QRcodeGrab/source/4_base64_encode_location/";
        ///后续改为读解析出的JSON信息
        //char date[PATH_MAX] = "2017-3-3";
        //char d_name[NAME_MAX] = "hu.png";
        char date[PATH_MAX] ={0};
        char d_name[PATH_MAX] ={0};

        //避免传const
        strcpy(date, msg.date.c_str());
        strcpy(d_name, msg.filename.c_str());

        printf("Directory scan of %s\n",topDir);

        is_base64 = false;//flag, dont neglect

        //创建ini相关文件夹与文件start
        strcpy(config_ini_dir, SRC_INI_LOCATION);
        strcat(config_ini_dir, date);
        mkdir(config_ini_dir, S_IRWXU|S_IRWXG|S_IRWXO);  //date
        strcat(config_ini_dir, symbol);
        strcat(config_ini_dir, d_name);
        mkdir(config_ini_dir, S_IRWXU|S_IRWXG|S_IRWXO);  //name

        memset(config_ini_dir, 0, PATH_MAX);

        sprintf(config_ini_dir,"%s%s/%s/config.ini",SRC_INI_LOCATION, date, d_name);
        //创建ini相关文件夹与文件end

        FILE *ini_file = fopen(config_ini_dir, "wb");
        fclose(ini_file);

        //FILE *ini_folder = fopen(folderHead, "wb");
        //fclose(ini_folder);

        ///不遍历文件，改为解析JSON中的文件名
        src_file_select_imp(topDir, relativeDir,_2_dir, _3_dir, _4_dir, date, d_name, msg.type, jsonStr);
        printf("Nomal, src_file_select_imp(), Done\n");
    }

    return 0;
}

void src_file_select_imp(char *dir, char* _short_dir, char *_2_dir, char *_3_dir, char *_4_dir, char *date, char *d_name, int type, const char* jsonStr)
{
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;
    char *relative_dir;

    char *_2_lzo_dir;
    char *_3_split_dir;
    char *_4_base64_encode_dir;

    char *symbol = "/";

    char *config_ini_dir; //生成的ini文件的绝对路径

    char value[34];

    memset(value, 0, 34);


    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
    relative_dir = new char[PATH_MAX];
    memset(relative_dir, 0, PATH_MAX);
    _2_lzo_dir = new char[PATH_MAX];
    memset(_2_lzo_dir, 0, PATH_MAX);
    _3_split_dir = new char[PATH_MAX];
    memset(_3_split_dir, 0, PATH_MAX);
    _4_base64_encode_dir = new char[PATH_MAX];
    memset(_4_base64_encode_dir, 0, PATH_MAX);

    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

    //生成的ini文件的绝对路径
    config_ini_dir = new char[PATH_MAX];
    memset(config_ini_dir, 0, PATH_MAX);

    //切换到这个目录
    chdir(dir);
    mkdir(date, S_IRWXU|S_IRWXG|S_IRWXO);//日期目录
    chdir(_2_dir);
    mkdir(date, S_IRWXU|S_IRWXG|S_IRWXO);//日期目录
    chdir(_3_dir);
    mkdir(date, S_IRWXU|S_IRWXG|S_IRWXO);//日期目录
    chdir(_4_dir);
    mkdir(date, S_IRWXU|S_IRWXG|S_IRWXO);//日期目录

    chdir(dir);

    //处理待发送的文件
    {
        //added by flq, get absolute path
        ///获取其他文件夹下的文件绝对路径
        //1的绝对路径
        strcpy(total_dir,dir);
        strcat(total_dir,date);
        strcat(total_dir,symbol);
        strcat(total_dir,d_name);
        printf("%s\n", total_dir);

        //通过文件名，得到详细文件信息
        lstat(total_dir,&statbuf);

        //判断文件是否存在，不存在返回
        if(0 != access(total_dir, F_OK)) {  //待发送文件不存在
            return;
        }

        //generate_md5sum(total_dir);
        //1的相对路径
        sprintf(relative_dir, "%s%s/%s", _short_dir, date, d_name);

        //2,3,4的绝对路径
        strcpy(_2_lzo_dir,_2_dir);
        mkdir(_2_lzo_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        strcat(_2_lzo_dir,date);
        strcat(_2_lzo_dir,symbol);
        strcat(_2_lzo_dir,d_name);

        strcpy(_3_split_dir,_3_dir);
        mkdir(_3_split_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        strcat(_3_split_dir,date);
        strcat(_3_split_dir,symbol);
        strcat(_3_split_dir,d_name);

        strcpy(_4_base64_encode_dir,_4_dir);
        mkdir(_4_base64_encode_dir, S_IRWXU|S_IRWXG|S_IRWXO);
        strcat(_4_base64_encode_dir,date);
        strcat(_4_base64_encode_dir,symbol);//'/'
        strcat(_4_base64_encode_dir,d_name);

        //输出文件名,后续可屏蔽
        printf("enty->d_name:%s"," ",d_name);
        printf(", statbuf.st_size=%d\n", statbuf.st_size);
        //I:原文件目录的INI

        if(UDP == type){
            sprintf(config_ini_dir,"%s%s/%s/config.ini",SRC_UDP_INI_LOCATION,date,d_name); //whole
        }else if(NORMAL == type){
            sprintf(config_ini_dir,"%s%s/%s/config.ini",SRC_INI_LOCATION,date,d_name); //whole
        }

        iniFileLoad(config_ini_dir);  //iniFileLoad  //"/home/montafan/QRcodeGrab/source/INI/config.ini"

        sprintf(value, "%d", file_cnt); //将file_cnt转为10进制表示的字符串  %x:16进制
        //参数1为sect
        iniSetString(value, "name", d_name);//name
        iniSetInt(value, "type", type, 0);//size
        iniSetString(value/*enty->d_name*/, "date", date);//name
        iniSetString(value, "path", relative_dir);//path
        iniSetInt(value, "size", statbuf.st_size, 0);//size
        iniSetString(value, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
        ///将JSON消息通过config.ini传给接收端
        iniSetString(value, "JSON", jsonStr);//JSON消息体

        //getTimestamp();
        file_cnt++;

        //LZO压缩
        FILE *in_file = fopen(total_dir, "rb");
        //FILE *out_2_file = fopen(_2_lzo_dir, "wb");
        //FILE *out_3_file = fopen(_3_split_dir, "wb");
        //FILE *out_4_file = fopen(_4_base64_encode_dir, "wb"); //temp delete

        if((statbuf.st_size > 0) && (statbuf.st_size < BLOCK_SIZE)){
            //copy to des
            FILE *out_2_file = fopen(_2_lzo_dir, "wb");
            FILE *out_3_file = fopen(_3_split_dir, "wb");

            char *des_buf = new char[statbuf.st_size];///statbuf.st_size+1
            memset(des_buf, 0, statbuf.st_size);
            fread(des_buf,1,statbuf.st_size,in_file);
            //写小文件
            fwrite(des_buf, 1, statbuf.st_size, out_2_file);

            ///WHY CRASH???????
            //memset(des_buf, 0, statbuf.st_size);
            //fread(des_buf,1,statbuf.st_size,in_file);
            fwrite(des_buf, 1, statbuf.st_size, out_3_file);

            //memset(des_buf, 0, statbuf.st_size);
            //fread(des_buf,1,statbuf.st_size,in_file);
            //fwrite(des_buf, 1, statbuf.st_size, out_4_file);

            free(des_buf);
            fclose(out_2_file);
            fclose(out_3_file);
        }
        else {
        //else if((statbuf.st_size > BLOCK_SIZE) && (statbuf.st_size < BLOCK_SIZE*512)) {//<1M
            //copy to des
            char *des_buf = new char[statbuf.st_size];

            //2_lzo_location中,压缩文件
            memset(des_buf, 0, statbuf.st_size);
            fread(des_buf,1,statbuf.st_size,in_file);

            //写中等文件,有压缩包即可，实际不需要fwrite
            //fwrite(des_buf, 1, statbuf.st_size, out_2_file);

            //压缩des_buf的内容到文件夹,到3_split_location
#ifdef USE_LZO_COMPRESSION
            char *lzo_dir =new char[PATH_MAX];
            memset(lzo_dir, 0, PATH_MAX);
                strcpy(lzo_dir,_2_lzo_dir);
                strcat(lzo_dir,LZO_SUFFIX);  //LZO_SUFFIX = ".lzo"

            LOG_DBG("%s, processLZO start\n", __func__);
            processLZO(total_dir, lzo_dir, LZO_COMPRESS);
            LOG_DBG("%s, processLZO end\n", __func__);
#endif
            //3_split_location,切割文件
            //memset(des_buf, 0, statbuf.st_size);
            //fwrite(des_buf, 1, statbuf.st_size, out_3_file);
            if(0 != access(_3_split_dir, F_OK))    // _3_split_dir=/home/montafan/QRcodeGrab/source/3_split_location/zbar_gige
            {
                LOG_DBG("mkdir _3_split_dir\n");
                mkdir(_3_split_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
                //在最后会添加一个额外的过程来显示碎片路径的ini,
                //在此添加虽然代码显得麻烦，但会在接收端的每次接收都减少一次判断
                //consider whether need to add code to generate a ini of fragment for mkdir
                //...
                //...
                //...
            } else{
                LOG_DBG("exist, need not mkdir _3_split_dir\n");
            }
#ifdef USE_LZO_COMPRESSION
            char *outputDir = new char[PATH_MAX];
            memset(outputDir, 0, PATH_MAX);
            if(UDP == type)
            {
                sprintf(outputDir, "%s%s%s/%s%s", SRC_UDP_LZO_LOCATION, _short_dir, date, d_name, LZO_SUFFIX); //moidfied by flq
            }
            else if(NORMAL == type)
            {
                sprintf(outputDir, "%s%s%s/%s%s", SRC_LZO_LOCATION, _short_dir, date, d_name, LZO_SUFFIX); //moidfied by flq
            }else
            {
                printf("ERROR, Wrong data type");
            }

            LOG_DBG("%s, split start, outputDir=%s, _3_split_dir=%s, blocksize=%d\n", __func__, outputDir, _3_split_dir ,BLOCK_SIZE);
            split(outputDir, _3_split_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
            LOG_DBG("%s, split end\n", __func__);

            free(outputDir);
            free(lzo_dir);

#else
            split(total_dir, _3_split_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
#endif
            if(0 != access(_4_base64_encode_dir, F_OK))
            {
                mkdir(_4_base64_encode_dir, S_IRWXU|S_IRWXG|S_IRWXO);
            }

            //4_base64_encode_location,base64 encode文件
            //memset(des_buf, 0, statbuf.st_size);
            //fwrite(des_buf, 1, statbuf.st_size, out_4_file);


            free(des_buf);
        }/* else if(statbuf.st_size > 2048*512){ //>1M
            ///支持对1M以上文件的传输

            //split();
            //LZO
            //base64
        }*/
        fclose(in_file);
        //fclose(out_2_file);
        //fclose(out_3_file);
        //fclose(out_4_file);
        //LZO压缩 end
    }

    //free
    free(total_dir);
    free(_2_lzo_dir);
    free(_3_split_dir);
    free(_4_base64_encode_dir);


}

int ini_select(activeMQVec msg)
{
    char date[PATH_MAX] = {0};
    char d_name[NAME_MAX] = {0};
    char md5sum[MD5SUM_MAX] = {0};

    //避免传const
    strcpy(date, msg.date.c_str());
    strcpy(d_name, msg.filename.c_str());
    strcpy(md5sum, msg.md5sum.c_str());


    src_ini_select_imp(date, d_name, msg.type, md5sum);
    //printf("ini_traversalDone\n");

    return 0;
}

//报头文件碎片化
void src_ini_select_imp(char *date, char *d_name, int type ,char *md5sum)
{
    //目录位置
    char *config_dir;
    char *folder_dir;

    //生成的ini文件的绝对路径
    char *config_ini_dir;
    char *folder_ini_dir;

    //待写入碎片的相对位置
    char *relative_path;

    //目录位置
    config_dir = new char[PATH_MAX];
    memset(config_dir, 0, PATH_MAX);
    folder_dir = new char[PATH_MAX];
    memset(folder_dir, 0, PATH_MAX);

    //生成的ini文件的绝对路径
    config_ini_dir = new char[PATH_MAX];
    memset(config_ini_dir, 0, PATH_MAX);
    folder_ini_dir = new char[PATH_MAX];
    memset(folder_ini_dir, 0, PATH_MAX);

    //待写入碎片的相对位置
    relative_path = new char[PATH_MAX];
    memset(relative_path, 0, PATH_MAX);

    if(UDP == type){
        sprintf(config_dir,"%s%s/%s/config/",SRC_UDP_INI_LOCATION,date,d_name);
        sprintf(folder_dir,"%s%s/%s/folder/",SRC_UDP_INI_LOCATION,date,d_name);

        sprintf(config_ini_dir,"%s%s/%s/config.ini",SRC_UDP_INI_LOCATION,date,d_name);
        sprintf(folder_ini_dir,"%s%s/%s/folder.ini",SRC_UDP_INI_LOCATION,date,d_name);

    }else if (NORMAL == type)
    {
        sprintf(config_dir,"%s%s/%s/config/",SRC_INI_LOCATION,date,d_name);
        sprintf(folder_dir,"%s%s/%s/folder/",SRC_INI_LOCATION,date,d_name);

        sprintf(config_ini_dir,"%s%s/%s/config.ini",SRC_INI_LOCATION,date,d_name);
        sprintf(folder_ini_dir,"%s%s/%s/folder.ini",SRC_INI_LOCATION,date,d_name);
    }
    //待写入碎片的相对位置
    ///sprintf(relative_path, "%s/%s/config.ini/",date,d_name);
    sprintf(relative_path, "%s/%s/config.ini/%d/%s",date,d_name,type, md5sum); //加入type类型，1或者2

    ///后续在此添加报头
    //方法:遍历时读取，并添加内容
    if(0 != access(config_dir, F_OK))
    {
        mkdir(config_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    }

    split_ini(config_ini_dir, config_dir, relative_path/*"config.ini/2/md5sum"*/, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片

#if 0
    //单个文件不需要folder信息
    if(0 != access(folder_dir, F_OK))
    {
        mkdir(folder_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    }

    split_ini(folder_ini_dir, folder_dir, "folder.ini/", BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
#endif
    free(config_dir);
    free(folder_dir);
    free(config_ini_dir);
    free(folder_ini_dir);
}

int fragment_selected_traversal(activeMQVec msg)
{
    if(UDP == msg.type)
    {
        char *fragmentDir = SRC_UDP_SPLIT_LOCATION;
        char *fragmentDes = SRC_UDP_BASE64_ENCODE_LOCATION;
        char relativeDir[PATH_MAX] = {0};
        ///后续改为读解析出的JSON信息
        //char date[PATH_MAX] = "2017-3-3";
        //char d_name[NAME_MAX] = "hu.png";
        char date[PATH_MAX] = {0};
        char d_name[NAME_MAX] = {0};

        //避免传const
        strcpy(date, msg.date.c_str());
        strcpy(d_name, msg.filename.c_str());

        is_base64 = true;

        src_fragment_selected_traversal_imp(fragmentDir, relativeDir, fragmentDes, date, d_name);
    }
    else if(NORMAL == msg.type)
    {
        char *fragmentDir = SRC_SPLIT_LOCATION;
        char *fragmentDes = SRC_BASE64_ENCODE_LOCATION;
        char relativeDir[PATH_MAX] = {0};
        ///后续改为读解析出的JSON信息
        //char date[PATH_MAX] = "2017-3-3";
        //char d_name[NAME_MAX] = "hu.png";
        char date[PATH_MAX] = {0};
        char d_name[NAME_MAX] = {0};

        //避免传const
        strcpy(date, msg.date.c_str());
        strcpy(d_name, msg.filename.c_str());

        is_base64 = true;

        src_fragment_selected_traversal_imp(fragmentDir, relativeDir, fragmentDes, date, d_name);
    }

    return 0;
}

void src_fragment_selected_traversal_imp(char *dir, char* _short_dir, char *des, char *date, char *d_name)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;
    char *relative_dir;
    char *des_str;

    char *symbol = "/";

    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
    relative_dir = new char[PATH_MAX];
    memset(relative_dir, 0, PATH_MAX);

    des_str = new char[PATH_MAX];
    memset(des_str, 0, PATH_MAX);

    memset(md5sum_str, 0, MD5SUM_MAX);
    memset(md5sum_str_hex, 0, MD5SUM_MAX);

    //碎片的绝对路径
    strcpy(total_dir,dir);
    strcat(total_dir,date);
    strcat(total_dir,symbol);
    strcat(total_dir,d_name);
//    printf("current dir, %s\n", total_dir);

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(total_dir)))
    {
        fprintf(stderr,"can not open dir:%s\n",dir);
        return;
    }

    //切换到这个目录
    chdir(total_dir);

    memset(total_dir, 0, strlen(total_dir));
    sprintf(relative_dir, "%s/%s/", date, d_name);

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
        /*
            strcpy(total_dir,dir);
            strcat(total_dir,date);
            strcat(total_dir,symbol);
            strcat(total_dir,enty->d_name);

            //相对路径拼接
            sprintf(relative_dir, "%s%s/", _short_dir, enty->d_name);

            strcpy(des_str,des);
            strcat(total_dir,date);
            strcat(total_dir,symbol);
            strcat(des_str,enty->d_name);

            //I:碎片文件目录的INI
            iniFileLoad(fragmentHEAD);
            //继续递归调用
            src_fragment_selected_traversal_imp(total_dir, relative_dir, des_str, date, d_name);//绝对路径递归调用错误 modify by flq
        */
        }
        else
        {
            //碎片的绝对路径
            strcpy(total_dir,dir);
            strcat(total_dir,date);
            strcat(total_dir,symbol);
            strcat(total_dir,d_name);
            //printf("current dir, %s\n", total_dir);

            //generate_md5sum(total_dir);
            //相对路径拼接
            //sprintf(relative_dir, "%s%s", _short_dir, enty->d_name);

            //输出文件名
            //added by flq
            #if 0
            iniSetString(enty->d_name, "name", enty->d_name);//name
            iniSetString(enty->d_name, "path", total_dir);//path
            iniSetInt(enty->d_name, "size", statbuf.st_size, 0);//size
            #endif
            //碎片不需要MD5SUM
            //iniSetString(enty->d_name, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();
            //added end
            /////////////////////////BASE64 ENCODE////////////////////////////
            if(is_base64)
            {
                //3_拼接为碎片名
                strcpy(total_dir,dir);
                strcat(total_dir,date);
                strcat(total_dir,symbol);
                strcat(total_dir,d_name);
                strcat(total_dir,symbol);
                strcat(total_dir,enty->d_name);
                //4_拼接为碎片名
                strcpy(des_str,des);
                strcat(des_str,date);
                strcat(des_str,symbol);
                strcat(des_str,d_name);
                strcat(des_str,symbol);
                strcat(des_str,enty->d_name);

                char *diplay_content;

                //dont forget mkdir fold, already done
                FILE *infile = fopen(total_dir, "rb");
                FILE *outfile = fopen(des_str, "wb");

                #if 1
                //encode(infile, outfile);//OK
                encode(infile, outfile, relative_dir, enty->d_name);//添加路径信息
                #else
                //first length. then new, then
                int length = get_length_after_base64(infile);
                diplay_content = new char[length];
                encode(infile, diplay_content);
                #endif
                ///===============后续在此生成二维码===============//
                ///here qrgenrator

                //free(des_str);
                fclose(infile);
                fclose(outfile);
            }
        }
    }

    ///***********************************通知到显示模块**********************************//
    ////QRGenerator w;
    ////w.StartTimer();

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);

    //free
    free(total_dir);
    free(des_str);
}
#endif  //USE_ACTIVEMQ

int ini_traversal()
{
    src_ini_traversal_imp();
    //printf("ini_traversalDone\n");
    return 0;
}

int fragment_traversal()
{
    char *fragmentDir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
    char *fragmentDes = SRC_BASE64_ENCODE_LOCATION;
    char relativeDir[PATH_MAX] = {0};

    //printf("\nDirectory fragement scan of %s\n",fragmentDir);

    is_base64 = true;

    src_fragment_traversal_imp(fragmentDir, relativeDir, fragmentDes, 0);
    //printf("Done\n");
    return 0;
}

//获取文件大小
unsigned long get_file_size(char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

unsigned char* generate_md5sum(char *filename)
{
    FILE *fp;
    char buffer[4096];
    size_t n;
    MD5_CONTEXT ctx;
    int i;
    unsigned char err_md5sum[MD5SUM_MAX] = {0};

    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "can't open `%s': %s\n", filename, strerror (errno));
        //exit (1);
        return err_md5sum;
    }

    memset(buffer, 0, 4096);

    md5_init (&ctx);
    while ( (n = fread (buffer, 1, sizeof buffer, fp)))
    md5_write (&ctx, (unsigned char*)buffer, n);

    if (ferror (fp))
    {
        fprintf (stderr, "error reading `%s': %s\n", filename,strerror (errno));
        //exit (1);
        return err_md5sum;
    }
    md5_final (&ctx);
    fclose (fp);
/*
    printf("MD5SUM:");

    for (i=0; i < 16; i++)
    {
      //printf ("%02x", ctx.buf[i]);
    }
*/
    HexToStr(md5sum_str_hex, ctx.buf, 16);
#ifdef PRINT_MD5SUM
    for (i=0; i < 32; i++)
    {
        printf ("%c", md5sum_str_hex[i]);
    }
    printf ("\n");
#endif
    return md5sum_str_hex;
}

//识别二维码后进行md5sum check，时减少一次读文件的操作，提高效率 , not sure
unsigned char* generate_md5sum_from_charstr(char *QRdata)
{
    size_t n;
    MD5_CONTEXT ctx;
    int i;
    int j=0;

    md5_init (&ctx);
    while (EOF != QRdata[j++])
    md5_write (&ctx, (unsigned char*)QRdata, n);

    md5_final (&ctx);

    HexToStr(md5sum_str_hex, ctx.buf, 16);

    for (i=0; i < 32; i++)
    {
        printf ("%c", md5sum_str_hex[i]);
    }
    printf ("\n");

    return md5sum_str_hex;
}

//http://www.cnblogs.com/nio-nio/p/3309367.html
/*
// C prototype : void StrToHex(char *pbDest, char *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
// [IN] pbSrc - 字符串
// [IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
*/
static void StrToHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
    char h1,h2;
    char s1,s2;
    int i;

    for (i=0; i<nLen; i++)
    {
        h1 = pbSrc[2*i];
        h2 = pbSrc[2*i+1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
        s1 -= 7;

        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
        s2 -= 7;

        pbDest[i] = s1*16 + s2;
    }
}

/*
// C prototype : void HexToStr(char *pbDest, char *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
// [IN] pbSrc - 输入16进制数的起始地址
// [IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
static void HexToStr(unsigned char *pbDest, unsigned char *pbSrc, int nLen) //unsigned char才能得到正确的值
{
    char ddl,ddh;
    int i;

    for (i=0; i<nLen; i++)
    {
        ddh = '0' + pbSrc[i] / 16;//48
        ddl = '0' + pbSrc[i] % 16;
        if (ddh > 57)
            ddh = ddh + 39; //7
        if (ddl > 57)
            ddl = ddl + 39;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
        }

    pbDest[nLen*2] = '\0';
}

//在参数中赋值,获取时间
//http://blog.csdn.net/openswc/article/details/53082715
static void getTimestamp()
//static void getTimestamp(char *des_time)
{
    struct tm *t;
    time_t tt;
    //time_t ts;

    struct tm tr = {0};

    time(&tt);
    t = localtime(&tt);
    printf("localtime %4d%02d%02d %02d:%02d:%02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    //time
    //localtime_r(&tt, &tr);
    //printf("localtime_r %4d%02d%02d %02d:%02d:%02d\n", tr.tm_year + 1900, tr.tm_mon + 1, tr.tm_mday, tr.tm_hour, tr.tm_min, tr.tm_sec);

    //change ints to char[]
    char ctime[40];
    memset(ctime, 0, 40);
    sprintf(ctime, "%02:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    printf("ctime:%s", ctime);
}

/*函数功能:摘取报头信息与正文*/
//instr: 输入内容
//pureQRdata: 数据信息
//relative_path:碎片中的碎片中的相对路径信息
//filename:碎片中的文件名信息
int cutQRdata(char *instr, int *offset, char *relative_path,char *filename)
{//要求截取的字符串不可以改变，但指向字符串的指针可以改变
    char *pp = instr;
    int i = 0;
    int j = 0;
    int cnt = 0;

    if(pp==NULL)
    {//如果截取的字符串是空的直接返回
        return -1;
    }

    while(*pp!='\0')
    {
        if(*pp=='\n')
        {
            pp++;
            cnt++;
            break;
        }

        relative_path[i] = instr[i];
        pp++;
        i++;
    }

    while(*pp!='\0')
    {
        if(*pp=='\n')
        {
            pp++;
            cnt++;
            break;
        }

        filename[j] = instr[i+1+j];
        pp++;
        j++;
    }

    if(cnt<2)
    {
        //*offset = i+1+j;
        LOG_ERR("%s, cutQRdata err, relative_path=%s, filename=%s\n",__func__, relative_path, filename);
        return REV_CONTENT_ERROR;
    }

    *offset = i+2+j;

    return NO_ERROR;
}

/*函数功能:摘取报头信息与正文*/
//instr: 输入内容
//pureQRdata: 数据信息
//relative_path:碎片中的相对路径信息
//filename:碎片中的文件名信息
int cutQRdata(char *instr, char *pureQRdata, char *relative_path,char *filename)
{//要求截取的字符串不可以改变，但指向字符串的指针可以改变
    char *pp = instr;
    int i = 0;
    int j = 0;
    int cnt = 0;

    if(pp==NULL)
    {//如果截取的字符串是空的直接返回
        return -1;
    }

    while(*pp!='\0')
    {
        if(*pp=='\n')
        {
            pp++;
            cnt++;
            break;
        }

        relative_path[i] = instr[i];
        pp++;
        i++;
    }

    while(*pp!='\0')
    {
        if(*pp=='\n')
        {
            pp++;
            cnt++;
            break;
        }

        filename[j] = instr[i+1+j];
        pp++;
        j++;
    }

    if(cnt<2)
    {
        LOG_ERR("%s, cutQRdata err, relative_path=%s, filename=%s\n",__func__, relative_path, filename);
        return REV_CONTENT_ERROR;
    }

    strcpy(pureQRdata, pp);

    return NO_ERROR;
}

/*函数功能:反转文件名*/
void reverse(char *filename)
{
    int i=0;
    int j=0;
    char *rename;

    j = strlen(filename)-1;

    rename = new char[strlen(filename) + 1];
    memset(rename, 0, strlen(filename) + 1);

    for (/*j = strlen(filename)-1*/; j>=0; j--)
    {
         rename[strlen(filename)-1-j] = filename[j];
    }

    strncpy(filename, rename, strlen(rename));
}

/*函数功能:获取碎片文件夹中文件的原文件名,作为相对路径*/
//param1: 输入：完整的文件夹名,绝对路径
//param1:输入的目录，如/home/montafan/QRcodeGrab/destination/INI/config.ini/
//param2: 输出：文件名
int cutDirName(char *instr, char *filename)
{
    int cnt;
    char *pp;
    int i = 0;
    bool is_start_getting = false;

    cnt = strlen(instr) - 1;

    pp = instr + cnt;

    while(cnt > 0)
    {
        if('/' == pp[0]){
            //遇到第1个‘/’
            if(!is_start_getting)
            {
                cnt--;
                pp = instr + cnt;
                is_start_getting = true;
            }
            //遇到第2个‘/’
            else
            {
                //反转文件名
                reverse(filename);
                break;
            }

        }
        //开始获取传入路径的内容,
        else if('/' != pp[0]){
            filename[i++] = pp[0];
            //printf("%c",pp[0]);
            cnt--;
            pp = instr + cnt;
        }
        else
        {
            cnt--;
            printf("cutDirName error!!");
        }

    }

    return NO_ERROR;
}
/*函数功能:获取上一级目录完整路径*/
int getUpperTotalDir(char *instr)
{
    int cnt;
    char *pp;
    int i = 0;
    bool is_start_getting = false;

    cnt = strlen(instr) - 1;

    pp = instr + cnt;

    while(cnt > 0)
    {
        if('/' == pp[0]){
            //遇到第1个‘/’
            if(!is_start_getting)
            {
                cnt--;
                pp = instr + cnt;
                is_start_getting = true;
                pp[0] = '\0';
            }
            //遇到第2个‘/’
            else
            {

                break;
            }

        }
        //开始获取传入路径的内容,
        else if('/' != pp[0]){
            pp[0] = '\0';
            //printf("%c",pp[0]);
            cnt--;
            pp = instr + cnt;
        }
        else
        {
            cnt--;
            printf("cutDirName error!!");
        }

    }
}

/*函数功能:获取文件的文件名*/
//param1: 输入：完整的文件名,绝对路径
//输入的目录，如/home/montafan/QRcodeGrab/destination/INI/config.ini
//param2: 输出：文件名
int cutFileName(char *instr, char *filename)
{
    int cnt;
    char *pp;
    int i = 0;
    bool is_start_getting = false;

    cnt = strlen(instr) - 1;

    pp = instr + cnt;

    while(cnt > 0)
    {
        if('/' == pp[0]){
            //遇到第1个‘/’
            //反转文件名
            reverse(filename);
            break;

        }
        //开始获取传入路径的内容,
        else if('/' != pp[0]){
            filename[i++] = pp[0];
            //printf("%c",pp[0]);
            cnt--;
            pp = instr + cnt;
        }
        else
        {
            cnt--;
            printf("cutDirName error!!");
        }

    }

    return NO_ERROR;
}

////拆分relative_dir，获取日期，文件名与配置文件名
int cutINIHeadData(char *relative_dir, char *date, char *name, char *ini_name, char *typeStr, char *md5sum)
{
    int cnt;
    int len;
    char *pp;
    int i;
    int j;

    cnt = 0;
    j=0;

    len = strlen(relative_dir) - 1;

    ////added by flq,防止接收不到配置文件时，碎片泄漏到根目录
    if(len <= 0){
        return -6;
    }
    ////added end,防止接收不到配置文件时，碎片泄漏到根目录

    pp = relative_dir;

    for(i=0; i < len; i++)
    {
        if('/' == pp[i])
        {
            cnt++;
            j = 0;
            continue;
        }
        else
        {
            if (0 == cnt)
            {
                date[j] = pp[i];
                j++;
            }
            else if (1 == cnt)
            {
                name[j] = pp[i];
                j++;
            }
            else if (2 == cnt)
            {
                ini_name[j] = pp[i];
                j++;
            }
            else if (3 == cnt){
                typeStr[j] = pp[i];
                j++;
            }
            else if (4 == cnt){
                md5sum[j] = pp[i];
                j++;
            }
        }
    }

    return NO_ERROR;
}

////拆分relative_dir，获取日期，文件名与配置文件名
int cutHeadData(char *relative_dir, char *date, char *name)
{
    int cnt;
    int len;
    char *pp;
    int i;
    int j;

    cnt = 0;
    j=0;

    len = strlen(relative_dir) - 1;

    ////added by flq,防止接收不到配置文件时，碎片泄漏到根目录
    if(len <= 0){
        return -6;
    }
    ////added end,防止接收不到配置文件时，碎片泄漏到根目录

    pp = relative_dir;

    for(i=0; i < len; i++)
    {
        if('/' == pp[i])
        {
            cnt++;
            j = 0;
            continue;
        }
        else
        {
            if (0 == cnt)
            {
                date[j] = pp[i];
                j++;
            }
            else if (1 == cnt)
            {
                name[j] = pp[i];
                j++;
            }
        }
    }

    return NO_ERROR;
}

int CompletePath()
{
    sprintf(ROOT_DIR, "%s", getenv("HOME"));
    sprintf(SRC_BASE_LOCATION, "%s%s", ROOT_DIR ,SRC_BASE_LOCATION_REL);
    sprintf(DES_BASE_LOCATION, "%s%s", ROOT_DIR ,DES_BASE_LOCATION_REL);


    ///==============NORMAL path================
    //SRC
    sprintf(SRC_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_LOCATION_REL);
    sprintf(SRC_LZO_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_LZO_LOCATION_REL);
    sprintf(SRC_SPLIT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_SPLIT_LOCATION_REL);
    sprintf(SRC_BASE64_ENCODE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_BASE64_ENCODE_LOCATION_REL);
    //SRC INI
    sprintf(SRC_INI_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_LOCATION_REL);
    sprintf(SRC_INI_FILE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FILE_LOCATION_REL);
    sprintf(SRC_INI_FOLD_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FOLD_LOCATION_REL);
    sprintf(SRC_INI_FRAGMENT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FRAGMENT_LOCATION_REL);
    sprintf(SRC_INI_FILE_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FILE_FRAG_LOCATION_REL);
    sprintf(SRC_INI_FOLD_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_INI_FOLD_FRAG_LOCATION_REL);

    //DES
    sprintf(DES_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_LOCATION_REL);
    sprintf(DES_RECEIVE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_RECEIVE_LOCATION_REL);
    sprintf(DES_BASE64_DECODE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_BASE64_DECODE_LOCATION_REL);
    sprintf(DES_CAT_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_CAT_LOCATION_REL);
    //DES INI
    sprintf(DES_RECV_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_RECV_INI_LOCATION_REL);
    sprintf(DES_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_LOCATION_REL);
    sprintf(DES_INI_FILE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_FILE_LOCATION_REL);
    sprintf(DES_INI_FOLD_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_INI_FOLD_LOCATION_REL);
    sprintf(DES_INI_FILE, "%s%s", DES_BASE_LOCATION ,DES_INI_FILE_REL);
    sprintf(DES_INI_FOLD, "%s%s", DES_BASE_LOCATION ,DES_INI_FOLD_LOCATION_REL);


    ///==============UDP path================
    //SRC
    sprintf(SRC_UDP_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_LOCATION_REL);
    sprintf(SRC_UDP_LZO_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_LZO_LOCATION_REL);
    sprintf(SRC_UDP_SPLIT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_SPLIT_LOCATION_REL);
    sprintf(SRC_UDP_BASE64_ENCODE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_BASE64_ENCODE_LOCATION_REL);
    //SRC INI
    sprintf(SRC_UDP_INI_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_LOCATION_REL);
    sprintf(SRC_UDP_INI_FILE_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FILE_LOCATION_REL);
    sprintf(SRC_UDP_INI_FOLD_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FOLD_LOCATION_REL);
    sprintf(SRC_UDP_INI_FRAGMENT_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FRAGMENT_LOCATION_REL);
    sprintf(SRC_UDP_INI_FILE_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FILE_FRAG_LOCATION_REL);
    sprintf(SRC_UDP_INI_FOLD_FRAG_LOCATION, "%s%s", SRC_BASE_LOCATION ,SRC_UDP_INI_FOLD_FRAG_LOCATION_REL);

    //DES
    sprintf(DES_UDP_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_LOCATION_REL);
    sprintf(DES_UDP_RECEIVE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_RECEIVE_LOCATION_REL);
    sprintf(DES_UDP_BASE64_DECODE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_BASE64_DECODE_LOCATION_REL);
    sprintf(DES_UDP_CAT_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_CAT_LOCATION_REL);
    //DES INI
    sprintf(DES_UDP_RECV_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_RECV_INI_LOCATION_REL);
    sprintf(DES_UDP_INI_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_LOCATION_REL);
    sprintf(DES_UDP_INI_FILE_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FILE_LOCATION_REL);
    sprintf(DES_UDP_INI_FOLD_LOCATION, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FOLD_LOCATION_REL);
    sprintf(DES_UDP_INI_FILE, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FILE_REL);
    sprintf(DES_UDP_INI_FOLD, "%s%s", DES_BASE_LOCATION ,DES_UDP_INI_FOLD_LOCATION_REL);

    return NO_ERROR;
}
