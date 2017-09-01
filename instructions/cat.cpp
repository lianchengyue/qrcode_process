#include "instructions/cat.h"
#include "include/fileParameters.h"

#include <string.h>
//#include <string>
#include <unistd.h>

int CopytoOutput(FILE *inf,FILE *outf)
{
    char *Xbuf;

    fseek(inf,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    uint32_t len=ftell(inf); //获取文件长度
    Xbuf=new char[len+1];
    memset(Xbuf, 0, len+1);
    rewind(inf); //把指针移动到文件开头

    fread(Xbuf,1,len,inf);
    fwrite(Xbuf, 1, len-1, outf);////////////////////////////-1,去掉多余的\n

    free(Xbuf);

    return 0;
}

int CopytoOutputLast(FILE *inf,FILE *outf)
{
    char *Xbuf;

    fseek(inf,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    uint32_t len=ftell(inf); //获取文件长度
    Xbuf=new char[len+1];
    memset(Xbuf, 0, len+1);
    //Xbuf[len+1] = 'X';
    rewind(inf); //把指针移动到文件开头

    fread(Xbuf,1,len,inf);
    fwrite(Xbuf, 1, len, outf);

    free(Xbuf);

    return 0;
}

int getUpperName(char *dir)
{
    /*
    DIR *Dp;
    //文件目录结构体
    struct dirent *enty;
    //详细文件信息结构体
    struct stat statbuf;


    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"getUpperName(),can not open dir:%s\n",dir);
        return;
    }
    */
}

//param1:输入的碎片文件夹的路径
//param2:输出的路径
//param3:输出的文件名
int cat(char *input, char *outputpath, char *outfilenname)
{
    FILE *inf;  //要连接的文件
    FILE *outf;
    char *filendir;
    char *outfilendir;
    int  Totalcount = 0;

    int  count = 0;//连接成功个数

    filendir = new char[PATH_MAX];
    memset(filendir, 0, PATH_MAX);

    outfilendir = new char[PATH_MAX];
    memset(outfilendir, 0, PATH_MAX);
    //拼接参数2，3
    sprintf(outfilendir, "%s%s", outputpath, outfilenname);
    //拼接碎片文件完整文件名
    sprintf(filendir, "%sX%d", input, Totalcount);

    //outf=fopen(outfilenname,"wb");
    outf=fopen(outfilendir,"wb");


    while(0 == access(filendir, F_OK))
    {
        Totalcount++;

        memset(filendir, 0, PATH_MAX);
        sprintf(filendir, "%sX%d", input, Totalcount);
    }

    memset(filendir, 0, PATH_MAX);
    sprintf(filendir, "%sX%d", input, count);


    while(count < Totalcount)
    {
        if((inf=fopen(filendir,"rb"))==NULL)
        {//如果某个文件无法打开则跳过
           printf("\ncat(3)can't open %s\n",filendir);
           continue;
        }
        count++;

        if(count == Totalcount)
            CopytoOutputLast(inf, outf);
        else
            CopytoOutput(inf, outf);

        memset(filendir, 0, PATH_MAX);
        sprintf(filendir, "%sX%d", input, count);


        fclose(inf);

    }

    printf("**Cat %d file**\n",count);

    free(filendir);
    free(outfilendir);
    fclose(outf);
    return 0;

}


int cat(char *input, char *output)
{
    FILE *inf;  //要连接的文件
    FILE *outf;
    char *filendir;
    char *outfilendir;

    int  Totalcount = 0;
    int  count = 0;//连接成功个数

    filendir = new char[PATH_MAX];
    memset(filendir, 0, PATH_MAX);

    outfilendir = new char[PATH_MAX];
    memset(outfilendir, 0, PATH_MAX);

    sprintf(filendir, "%sX%d", input, count);

    sprintf(outfilendir, "%sYoutput", output);
    outf=fopen(outfilendir,"wb");

    while(0 == access(filendir, F_OK))
    {
        memset(filendir, 0, PATH_MAX);
        sprintf(filendir, "%sX%d", input, Totalcount);
        Totalcount++;
    }

    //while(0 == access(filendir, F_OK))// F_OK  X_OK
    while(count < Totalcount)
    {
        if((inf=fopen(filendir,"rb"))==NULL)
        {//如果某个文件无法打开则跳过
           printf("\ncat(2)can't open %s\n",filendir);
           continue;
        }
        count++;

        if(count == Totalcount)
            CopytoOutputLast(inf, outf);
        else
            CopytoOutput(inf, outf);

        memset(filendir, 0, PATH_MAX);
        sprintf(filendir, "%sX%d", input, count);


        fclose(inf);

    }

    printf("**Cat %d file**",count);

    free(filendir);
    free(outfilendir);
    fclose(outf);
    return 0;

}


/*
int cat(char *input, char *output)
{
    FILE *inf;  //要连接的文件
    FILE *outf;
    char *filendir;
    char *outfilendir;

    int  count = 0;//连接成功个数

    filendir = new char[PATH_MAX];
    memset(filendir, 0, PATH_MAX);

    outfilendir = new char[PATH_MAX];
    memset(outfilendir, 0, PATH_MAX);

    sprintf(filendir, "%sX%d", input, count);

    sprintf(outfilendir, "%sYoutput", output);
    outf=fopen(outfilendir,"wb");


    while(0 == access(filendir, F_OK))// F_OK  X_OK
    {
        if((inf=fopen(filendir,"rb"))==NULL)
        {//如果某个文件无法打开则跳过
           printf("\ncan't open %s\n",filendir);
           continue;
        }
        count++;

        CopytoOutput(inf, outf);

        memset(filendir, 0, PATH_MAX);
        sprintf(filendir, "%sX%d", input, count);


        fclose(inf);

    }

    printf("**Cat %d file**",count);

    free(filendir);
    free(outfilendir);
    fclose(outf);
    return 0;

}
*/
