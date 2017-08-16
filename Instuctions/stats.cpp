//获取文件属性，并遍历文件
//author fanlinqing
//实现非常多业务的文件

#include<time.h>
#include "stats.h"
#include "inirw.h"
//http://bbs.csdn.net/topics/391842320
#include "md5sum.h"
//#include "qrgenerator.h"
#include "fileParameters.h"
#include "Instuctions/base64.h"
#include "DirPath.h"

///const char *HEAD = "/home/montafan/Qt5.6.2/project/zbar_gige/Instuctions/inirw/config.ini";
const char *HEAD = "/home/montafan/QRcodeGrab/source/WholeINI/config.ini";
const char *fragment_HEAD = "/home/montafan/QRcodeGrab/source/temp_location/nocolor.png/ini/config.ini";

unsigned char md5sum_str[MD5SUM_MAX];
unsigned char md5sum_str_hex[MD5SUM_MAX];

bool is_base64 = true;

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
            strcat(total_dir,"/");

            //输出当前目录名
            printf("%*s%s/\n",depth," ",enty->d_name);
          //added by flq
            //iniSetString("sect2", "str003", "value003");
            //iniSetInt("sect2", "int001", 100, 0);
          //added end

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
int file_traversal()
{
    ///char *topdir = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/";
    char *topDir = SRC_LOCATION;//"/home/montafan/QRcodeGrab/source/location/";

    printf("Directory scan of %s\n",topDir);

    is_base64 = false;
#if 0
    //生成配置文件
    char *HEAD = new char[PATH_MAX];
    strcpy(HEAD,topDir);
    strcat(HEAD,"HEAD.ini");
    freopen(HEAD, "a+", stdout);
#else
    iniFileLoad(HEAD);
#endif

    print_INI_Info(topDir, 0);
    //printf("Done\n");
    return 0;
}


int fragment_traversal()
{
    ///char *topdir = "/home/montafan/Qt5.6.2/project/zbar_gige/testFile/";
    char *fragmentDir = SRC_SPLIT_LOCATION;//"/home/montafan/QRcodeGrab/source/3_split_location/nocolor.png/";//文件夹

    printf("Directory fragement scan of %s\n",fragmentDir);

    is_base64 = true;
    iniFileLoad(fragment_HEAD);

    print_INI_Info(fragmentDir, 0);
    //printf("Done\n");
    return 0;
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
  for (i=0; i < 32; i++)
  {
    printf ("%c", md5sum_str_hex[i]);
  }
  printf ("\n");

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
