#include <stdio.h>
#include <string.h>
#include <stdint.h>  //uint32
#include "include/fileParameters.h"
//Auther Fanlinqing

#include "split.h"

using namespace std;

//param1:待拆分文件 param2:目标目录
int spilt(char *file, char*outpath, int blocksize)
{
    FILE *InFile=fopen(file,"rb"); //获取二进制文件的指针,rb二进制, rt文本文件

    char *pdesBuf;  //定义文件指针
    fseek(InFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    uint32_t len=ftell(InFile); //获取文件长度
    pdesBuf=new char[len+1];
    memset(pdesBuf, 0, len+1);
    rewind(InFile); //把指针移动到文件开头

    uint32_t count= len/blocksize + 1;
    for(int i=0; i < count; i++)
    {
        char *str = new char[PATH_MAX];
        memset(str, 0, PATH_MAX);
        strcpy(str, outpath);
        sprintf(str,"%s/X%d",str,i);

        FILE *OutFile=fopen(str,"wb"); //获取二进制文件的指针,rb二进制, rt文本文件

        if(i < count-1){
            fread(pdesBuf,1,blocksize,InFile);
            fwrite(pdesBuf, 1, blocksize, OutFile);
        }
        else if (i = count-1)
        {
            fread(pdesBuf,1,len%blocksize,InFile); //读文件
            fwrite(pdesBuf, 1, len%blocksize, OutFile);
        }
        else
        {
            //Never get here
            printf("XXXXXXXXXXXXXXXXXXXXXXXXXXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN");
        }

        free(str);
        fclose(OutFile); // 关闭文件

    }
    //pdesBuf[len]=0;

    free(pdesBuf);
    fclose(InFile); // 关闭文件
}
