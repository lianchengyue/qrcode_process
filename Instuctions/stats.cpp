//Auther fanlinqing
//获取文件属性，并遍历文件
//实现非常多业务的文件

#include<time.h>
#include "stats.h"
#include "inirw.h"
//http://bbs.csdn.net/topics/391842320
#include "md5sum.h"
//#include "qrgenerator.h"
#include "include/fileParameters.h"
#include "Instuctions/base64.h"
#include "include/DirPath.h"

#include "LZO/lzopack.h"//temp
#include "Instuctions/split.h"

//#define PRINT_MD5SUM

const char *HEAD = SRC_INI_FILE_LOCATION;//"/home/montafan/QRcodeGrab/source/INI/config.ini";
const char *folderHead = SRC_INI_FOLD_LOCATION;//"/home/montafan/QRcodeGrab/source/INI/folder.ini";
const char *fragmentHEAD = "/home/montafan/QRcodeGrab/source/INI/nocolor.png/ini/config.ini"; //need modify soon
const char *iniHEAD = "/home/montafan/QRcodeGrab/source/INI/nocolor.png/ini/config.ini"; //need modify soon

unsigned char md5sum_str[MD5SUM_MAX];
unsigned char md5sum_str_hex[MD5SUM_MAX];

bool is_base64 = true;

int src_init_topology()
{
    chdir("/home/montafan/");
    mkdir("QRcodeGrab", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("QRcodeGrab");

    mkdir("source", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("source");

    mkdir("1_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_lzo_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_split_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_base64_encode_location", S_IRWXU|S_IRWXG|S_IRWXO);

    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("FRAG_INI", S_IRWXU|S_IRWXG|S_IRWXO);

    return 0;
}

int des_init_topology()
{
    chdir("/home/montafan/");
    mkdir("QRcodeGrab", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("QRcodeGrab");

    mkdir("destination", S_IRWXU|S_IRWXG|S_IRWXO);
    chdir("destination");

    mkdir("1_receive_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("2_base64_decode_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("3_cat_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("4_location", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("INI", S_IRWXU|S_IRWXG|S_IRWXO);
    //所有完整文件的属性
    mkdir("config", S_IRWXU|S_IRWXG|S_IRWXO);
    mkdir("folder", S_IRWXU|S_IRWXG|S_IRWXO);

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
            iniFileLoad(folderHead);

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
            iniSetString(enty->d_name, "name", enty->d_name);//name
            iniSetString(enty->d_name, "path", relative_dir);//path
            iniSetInt(enty->d_name, "size", statbuf.st_size, 0);//size
            iniSetString(enty->d_name, "md5sum", (char*)generate_md5sum(total_dir));//md5sum   or (char*)md5sum_str_hex
            //getTimestamp();

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
                char *lzo_content =new char[PATH_MAX];
                memset(lzo_content, 0, PATH_MAX);
                    strcpy(lzo_content,_2_lzo_dir);
                    strcat(lzo_content,".lzo");
                processLZO(total_dir, lzo_content, LZO_COMPRESS);

                //3_split_location,切割文件
                //memset(des_buf, 0, statbuf.st_size);
                //fwrite(des_buf, 1, statbuf.st_size, out_3_file);
                mkdir(_3_split_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
                spilt(total_dir, _3_split_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
                mkdir(_4_base64_encode_dir, S_IRWXU|S_IRWXG|S_IRWXO);

                //4_base64_encode_location,base64 encode文件
                //memset(des_buf, 0, statbuf.st_size);
                //fwrite(des_buf, 1, statbuf.st_size, out_4_file);


                free(des_buf);

                free(lzo_content);
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
    //稍后判断文件是否为空end

    mkdir(config_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    spilt(SRC_INI_FILE_LOCATION, config_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    mkdir(folder_dir, S_IRWXU|S_IRWXG|S_IRWXO);///这里的_3_split_dir是目录，不是文件，存放切割后的碎片
    spilt(SRC_INI_FOLD_LOCATION, folder_dir, BLOCK_SIZE); ///这里的_3_split_dir是目录，不是文件，存放切割后的碎片

}



//遍历3目录的碎片文件，做base64 encode的
//src_fragment_traversal_imp
void src_fragment_traversal_imp(char *dir, char *des, int depth)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;
    //文件相对或绝对路径 addded by flq
    char *total_dir;
    char *des_str;

    //打开指定的目录，获得目录指针
    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"can not open dir:%s\n",dir);
        return;
    }

    total_dir = new char[PATH_MAX];
    des_str = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);
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

            strcpy(des_str,des);
            strcat(des_str,enty->d_name);
            strcat(des_str,"/");

            //I:碎片文件目录的INI
            iniFileLoad(fragmentHEAD);
            //继续递归调用
            /////////print_INI_Info(enty->d_name,depth+4);
            src_fragment_traversal_imp(total_dir, des_str, depth+4);//绝对路径递归调用错误 modify by flq
        }
        else
        {
            //added by flq, get absolute path
            strcpy(total_dir,dir);
            strcat(total_dir,enty->d_name);
            //printf("%s\n", total_dir);
            //generate_md5sum(total_dir);

            //输出文件名
            //printf("%*s%s",depth," ",enty->d_name);
            //printf(", statbuf.st_size=%d\n", statbuf.st_size);
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
            /////////src_fragment_traversal_imp(enty->d_name,depth+4);
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

    printf("Directory scan of %s\n",topDir);

    is_base64 = false;//flag, dont neglect

    FILE *ini_file = fopen(HEAD, "w");
    fclose(ini_file);
    //iniFileLoad(HEAD);

    FILE *ini_folder = fopen(folderHead, "w");
    fclose(ini_folder);
    //iniFileLoad(folderHead);

    ///遍历源文件夹并生成所有的文件夹,处理完后，在3中生成碎片
    src_file_traversal_imp(topDir, relativeDir,_2_dir, _3_dir, _4_dir, 0);

    printf("Transmit Done\n");
    return 0;
}


int ini_traversal()
{
    src_ini_traversal_imp();
    //printf("Done\n");
    return 0;
}

int fragment_traversal()
{
    char *fragmentDir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/";
    char *fragmentDes = SRC_BASE64_ENCODE_LOCATION;

    printf("Directory fragement scan of %s\n",fragmentDir);

    is_base64 = true;
    //待处理，寻找合适的位置
    ////iniFileLoad(fragmentHEAD);

    src_fragment_traversal_imp(fragmentDir, fragmentDes, 0);
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

    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "can't open `%s': %s\n", filename, strerror (errno));
        exit (1);
        //return -1;
    }

    memset(buffer, 0, 4096);

    md5_init (&ctx);
    while ( (n = fread (buffer, 1, sizeof buffer, fp)))
    md5_write (&ctx, (unsigned char*)buffer, n);

    if (ferror (fp))
    {
        fprintf (stderr, "error reading `%s': %s\n", filename,strerror (errno));
        exit (1);
        //return -1;
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
