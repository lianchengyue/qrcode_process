#include <stdio.h>
#include <string.h>
#include <stdint.h>  //uint32
#include "include/fileParameters.h"
//Auther Fanlinqing

#include "split.h"

using namespace std;

//param1:待拆分文件
//param2:目标目录
//param3:待拆分文件的大小
int split(char *file, char*outpath, int blocksize)
{
    FILE *InFile=fopen(file,"rb"); //获取二进制文件的指针,rb二进制, rt文本文件

    char *pdesBuf;  //定义文件指针
    fseek(InFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    uint32_t len=ftell(InFile); //获取文件长度

    LOG_DBG("%s In,\n", __func__);
    LOG_DBG("file=%s,outpath=%s\n", file, outpath);  //tmp

    if(0 == len)
    {
        LOG_ERR("split(), empty fold,outpath=%s\n",outpath);
        return -1;
    }

    pdesBuf=new char[len+1];
    memset(pdesBuf, 0, len+1);
    rewind(InFile); //把指针移动到文件开头

    uint32_t count= len/blocksize + 1;

    LOG_DBG("%s count=%d,\n", __func__, count);

    for(int i=0; i < count; i++)
    {
        char *str = new char[PATH_MAX];
        memset(str, 0, PATH_MAX);
        //strcpy(str, outpath);
        //printf("1,str=%s\n", str);  //tmp
//        sprintf(str,"%s/X%d",str,i);/////////////////////////different gcc will lead different result
        sprintf(str,"%s/X%d",outpath,i);
        //LOG_DBG("2,str=%s\n", str);  //tmp

        FILE *OutFile=fopen(str,"wb"); //获取二进制文件的指针,rb二进制, rt文本文件

#if 0
        //0<len<blocksize, 目前<blocksize不做压缩，不会进入这里
        if (count = 1)
        {
            //<blocksize, only XO
            printf("flq1, only X0,%s\n", file);
            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }
        else{
            if(i < count-1){
                fread(pdesBuf,1,blocksize,InFile);
                fwrite(pdesBuf, 1, blocksize, OutFile);
            }
            else if (i = count-1)
            {
                fread(pdesBuf,1,len%blocksize,InFile); //读文件
                fwrite(pdesBuf, 1, len%blocksize, OutFile);
            }
        }
#else
        if(i < count-1){
            fread(pdesBuf,1,blocksize,InFile);
            fwrite(pdesBuf, 1, blocksize, OutFile);
        }
        else if (i = count-1)
        {
            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }
        //0<len<blocksize, 目前<blocksize不做压缩，不会进入这里
        else
        {
            //<blocksize, only XO
            printf("flq1, only X0,%s\n", file);
            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }
#endif

        delete(str);
        fclose(OutFile); // 关闭文件

    }
    //pdesBuf[len]=0;

    delete(pdesBuf);
    fclose(InFile); // 关闭文件

    LOG_DBG("%s Out,\n", __func__);
}

/*拆分ini文件*/
//param1:待拆分文件
//param2:目标目录
//param3:待拆分文件的大小
int split_ini(char *file, char*outpath, char *dir,int blocksize)
{
    FILE *InFile=fopen(file,"rb"); //获取二进制文件的指针,rb二进制, rt文本文件

    char *pdesBuf;  //定义文件指针
    char * filename;
    fseek(InFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    uint32_t len=ftell(InFile); //获取文件长度

    if(0 == len)
    {
        printf("spilt_ini(), empty fold,outpath=%s\n",outpath);
        //后续如果需要加空白的辨认信息，在这里加 added
        //...
        //...
        //end
        return -1;
    }


    pdesBuf=new char[len+1];
    memset(pdesBuf, 0, len+1);
    rewind(InFile); //把指针移动到文件开头

    filename = new char[NAME_MAX];

    uint32_t count= len/blocksize + 1;
    for(int i=0; i < count; i++)
    {
        char *str = new char[PATH_MAX];
        memset(str, 0, PATH_MAX);
        //strcpy(str, outpath);
        sprintf(str,"%s/X%d",outpath,i);

        sprintf(filename, "X%d", i);

        FILE *OutFile=fopen(str,"wb"); //获取二进制文件的指针,rb二进制, rt文本文件

        if(i < count-1){
            //相对路径
            fwrite(dir, sizeof(char), strlen(dir), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);

            //文件名
            fwrite(filename, 1, strlen(filename), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);

            //文件内容
            fread(pdesBuf,1,blocksize,InFile);
            fwrite(pdesBuf, 1, blocksize, OutFile);
        }
        else if (i = count-1)
        {
            //相对路径
            fwrite(dir, 1, strlen(dir), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);

            //文件名
            fwrite(filename, 1, strlen(filename), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);

            //文件内容
            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }
        //0<len<blocksize
        else
        {
            //<blocksize, only XO
            //如果ini仅拆分成1个，也需要加报头信息
            printf("flq2, only X0,%s\n", file);

            //报头信息 start
            //相对路径
            fwrite(dir, 1, strlen(dir), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);

            //文件名
            fwrite(filename, 1, strlen(filename), OutFile);
            fwrite("\n", sizeof(char), 1, OutFile);
            //报头信息 end

            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }

        delete(str);
        fclose(OutFile); // 关闭文件

    }
    //pdesBuf[len]=0;

    delete(pdesBuf);
    delete(filename);
    fclose(InFile); // 关闭文件
}

