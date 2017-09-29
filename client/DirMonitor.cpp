//Auther Fanlinqing
#include "DirMonitor.h"
#include "include/Errors.h"
#include "include/macros.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/inotify.h>
//#include <dirent.h>
//#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//文件监测prototype
//char dirMonitor[20];
char* dirMonitoring = "/home/montafan/QRcodeGrab/source/1_location/";

char * event_array[] = {
    "File was accessed",
    "File was modified",
    "File attributes were changed",
    "writtable file closed",
    "Unwrittable file closed",
    "File was opened",
    "File was moved from X",
    "File was moved to Y",
    "Subfile was created",
    "Subfile was deleted",
    "Self was deleted",
    "Self was moved",
    "",
    "Backing fs was unmounted",
    "Event queued overflowed",
    "File was ignored"
};

DirMonitor::DirMonitor()
{

}

//DirMonitor::DirMonitor(fragmentProcess *ctrl)
//{
//    m_parent = ctrl;
//    m_state = RECV_IDLE;
//}

DirMonitor::~DirMonitor()
{
    //int inotify_rm_watch(int fd, uint32_t wd);
}

int DirMonitor::init()
{
    signIndex = 0;
}

//开始监控,入口start
int DirMonitor::startMonitoring()
{
    //init inotify!
    int fd = inotify_init();
    if ( fd < 0 ) {
        perror( "inotify_init error!\n" );
        return -1;
    }

    bindWithInotify(dirMonitoring,fd);

    //waiting for a change comes
    watchingFiles(fd);

    return NO_ERROR;
}
//入口end

int DirMonitor::bindWithInotify(char *dir,int fd)
{
    DIR *Dp;
    //文件目录结构体
    struct dirent *entry;
    //详细文件信息结构体
    struct stat statbuf;
    char * total_dir;

    //打开指定的目录，获得目录指针
    //make sure the files which to be watched!
    if(NULL == (Dp = opendir(dir)))
    {
        fprintf(stderr,"bindWithInotify(),can not open dir:%s\n",dir);
        return -1;
    }

    total_dir = new char[PATH_MAX];
    memset(total_dir, 0, PATH_MAX);

#if 1
    //int wd = 0;
    int wd = inotify_add_watch(fd, dir, IN_ALL_EVENTS);  //Now my watch begins^-^
    printf("watch dir=%s\n", dir);
    if(wd<0)
    {
        perror("no such directory!\n");
        return -1;
    }
    strcpy(sign[signIndex].dir_fullPath,dir);
    sign[signIndex].dir_wd=wd;
    //if (signIndex < 35)
        signIndex++;
#endif
    //切换到这个目录
    chdir(dir);

    while(NULL != (entry = readdir(Dp)))
    {
        //当前目录和上一目录过滤掉
        if(0 == strcmp(".",entry->d_name) ||
                      0 == strcmp("..",entry->d_name))
        {
            continue;
        }

        //通过文件名，得到详细文件信息
        lstat(entry->d_name,&statbuf);

        if(45 == signIndex)
        {
            printf("xxxK, signIndex=%d\n", signIndex);
        }

        if (S_ISDIR(statbuf.st_mode))      //is a directory
        {
            strcpy(total_dir,dir);
            strcat(total_dir,entry->d_name);
            strcat(total_dir,"/");

            //bindWithInotify(total_dir,fd);
        }
        else{
            ;//continue;
        }
    }

    //切换到上一及目录
    chdir("..");
    //关闭文件指针
    closedir(Dp);


    free(total_dir);
}



int DirMonitor::watchingFiles(int fd)
{
    unsigned char buf[1024] = {0};
    struct inotify_event *event = {0};
    for (;;)
    {
        printf("for()\n");
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        if (select(fd + 1, &fds, NULL, NULL, NULL) > 0)
        {
            int len, index = 0;
            while (((len = read(fd, buf, sizeof(buf))) < 0) && (errno == EINTR));
            while (index < len)
            {
                event = (struct inotify_event *)(buf + index);
                //if(event->name[strlen(event->name)-1]==fileType[0] || fileType[0]=='a')    //identify the fileType to be watched
                {
                    inotifyEventHandler(event,fd);
                }
                index += sizeof(struct inotify_event) + event->len;
            }
        }
        ///else
        {
        ///    printf("else");
        }
    }
    return NO_ERROR;
}

int DirMonitor::inotifyEventHandler(struct inotify_event *event,int fd)
{
    int flag=0;
    char dirOrFile[15]="",method[15]="";

    printf("event->len = %d, event->mask=%d\n", event->len, event->mask);
    ///if ( event->len )
    {
        if ( event->mask & IN_CREATE )
        {
            printf("IN_CREATE\n");
            flag=1;
            strcpy(method,"created");
            if ( event->mask & IN_ISDIR )
            {
                strcpy(dirOrFile,"directory");

                //add a new dir to watch when the program is running.
                int i=0;
                for(;i<signIndex;i++)
                {
                    if(event->wd==sign[i].dir_wd)
                        break;
                }
                char fullPathTemp[100]="";
                strcpy(fullPathTemp,sign[i].dir_fullPath);
                strcat(fullPathTemp,"/");
                strcat(fullPathTemp,event->name);
                DIR *pDir;
                if ((pDir=opendir(fullPathTemp))==NULL)
                {
                    printf("Dynamic open directory falied!\n");
                    return -1;
                }
                bindWithInotify(fullPathTemp,fd);
            }
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_DELETE )
        {
            printf("IN_DELETE\n");
            flag=1;
            strcpy(method,"deleted");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_ACCESS )
        {
            printf("IN_ACCESS\n");
            flag=1;
            strcpy(method,"accessed");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_MODIFY )
        {
            printf("IN_MODIFY\n");
            flag=1;
            strcpy(method,"modified");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        ////////////////
        else if ( event->mask & IN_MOVED_FROM)
        {
            printf("IN_MODIFY_SELF\n");
        }
        else if ( event->mask & IN_MOVED_TO)
        {
            printf("IN_MOVED_TO\n");
        }

        else if ( event->mask & IN_CLOSE)  // 	IN_CLOSE_WRITE|IN_CLOSE_NOWRITE
        {
            printf("IN_CLOSE\n");
        }

        else if ( event->mask & IN_CLOSE_WRITE)  // 	IN_CLOSE_WRITE|IN_CLOSE_NOWRITE
        {
            printf("IN_CLOSE_WRITE\n");
        }

        else if ( event->mask & IN_CLOSE_NOWRITE)  // 	IN_CLOSE_WRITE|IN_CLOSE_NOWRITE
        {
            printf("IN_CLOSE_NOWRITE\n");
        }

        //if(flag==1)
        //    output(method,dirOrFile,event,sockfd);
    }
}

#if 0
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define LOGFILE "./dir_log"
#define CONFIGRATION "./config"
char ip[20];
char port[10];
char dirMonitor[20];
char fileType[5];
struct watchSign{
    int dir_wd;       //to compare with event->wd
    char dir_fullPath[100];
}sign[100];     //cannot over 500 packages!
int signIndex=0;   //global index of the sign

void bindWithInotify(char*,int,DIR *);
int getParameter(int,char **);
void watchingFiles(int,int);
void inotifyEventHandler(struct inotify_event *,int,int);
void output(char *,char *,struct inotify_event *,int);
int creatConnection();
int getConfig();

int main(int argc, char *argv[])
{
    if(getParameter(argc,argv)<0)
    {
        if(getConfig()<0)
            {
                printf("Parameters are not right!%s\n");
                return 1;
            }
    }

    int sockfd=creatConnection();
    if(sockfd<0)
        return 1;
    //init inotify!
    int fd = inotify_init();
    if ( fd < 0 ) {
        perror( "inotify_init error!\n" );
        return 1;
    }
    //make sure the files which to be watched!
    DIR *pDir;
    pDir=opendir(dirMonitor);
    if(pDir==NULL)
    {
        printf("Open directory falied!\n");
        return 1;
    }
    bindWithInotify(dirMonitor,fd,pDir);

    //waiting for a change comes
    watchingFiles(fd,sockfd);
    return 0;
}

int getParameter(int argc, char **argv)
{
    if(argc==5)
    {
        printf("Parameters are from console!\n");
        strcpy(ip,argv[1]);
        strcpy(port,argv[2]);
        strcpy(dirMonitor,argv[3]);
        strcpy(fileType,argv[4]);
        return 0;
    }
    return -1;
}

void bindWithInotify(char *dir,int fd,DIR *pDir)
{
    int wd = inotify_add_watch(fd, dir, IN_ALL_EVENTS);
    if(wd<0)
    {
        perror("no such directory!\n");
        return;
    }
    strcpy(sign[signIndex].dir_fullPath,dir);
    sign[signIndex].dir_wd=wd;
    signIndex++;
    struct dirent *entry=NULL;
    struct stat buf;
    while((entry=readdir(pDir))!=NULL)
    {
        if ((strcmp(entry->d_name,".")==0)||(strcmp(entry->d_name,"..")==0))
            continue;
        char path[100]="";
        strcpy(path,dir);
        strcat(path,"/");
        strcat(path,entry->d_name);
        stat(path,&buf);
        if (S_ISDIR(buf.st_mode))      //is a directory
        {
            DIR *cDir;
            cDir=opendir(path);
            bindWithInotify(path,fd,cDir);
        }
        else
            continue;
    }
}

void watchingFiles(int fd,int sockfd)
{
    unsigned char buf[1024] = {0};
    struct inotify_event *event = {0};
    for (;;)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        if (select(fd + 1, &fds, NULL, NULL, NULL) > 0)
        {
            int len, index = 0;
            while (((len = read(fd, &buf, sizeof(buf))) < 0) && (errno == EINTR));
            while (index < len)
            {
                event = (struct inotify_event *)(buf + index);
                if(event->name[strlen(event->name)-1]==fileType[0] || 						fileType[0]=='a')    //identify the fileType to be watched
                {
                    inotifyEventHandler(event,fd,sockfd);
                }
                index += sizeof(struct inotify_event) + event->len;
            }
        }
    }
}

void inotifyEventHandler(struct inotify_event *event,int fd,int sockfd)
{
    int flag=0;
    char dirOrFile[15]="",method[15]="";
    if ( event->len )
    {
        if ( event->mask & IN_CREATE )
        {
            flag=1;
            strcpy(method,"created");
            if ( event->mask & IN_ISDIR )
            {
                strcpy(dirOrFile,"directory");

                //add a new dir to watch when the program is running.
                int i=0;
                for(;i<signIndex;i++)
                {
                    if(event->wd==sign[i].dir_wd)
                        break;
                }
                char fullPathTemp[100]="";
                strcpy(fullPathTemp,sign[i].dir_fullPath);
                strcat(fullPathTemp,"/");
                strcat(fullPathTemp,event->name);
                DIR *pDir;
                if ((pDir=opendir(fullPathTemp))==NULL)
                {
                    printf("Dynamic open directory falied!\n");
                    return;
                }
                bindWithInotify(fullPathTemp,fd,pDir);
            }
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_DELETE )
        {
            flag=1;
            strcpy(method,"deleted");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_ACCESS )
        {
            flag=1;
            strcpy(method,"accessed");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        else if ( event->mask & IN_MODIFY )
        {
            flag=1;
            strcpy(method,"modified");
            if ( event->mask & IN_ISDIR )
                strcpy(dirOrFile,"directory");
            else
                strcpy(dirOrFile,"file");
        }
        if(flag==1)
            output(method,dirOrFile,event,sockfd);
    }
}

void output(char method[],char dirOrFile[],struct inotify_event *event,int sockfd)
{
    int i=0;
    for(;i<signIndex;i++)
    {
        if(event->wd==sign[i].dir_wd)
            break;
    }
    char temp1[256]="";
    strcpy(temp1,"The ");
    strcat(temp1,dirOrFile);
    strcat(temp1," ");
    strcat(temp1,event->name);
    strcat(temp1," was ");
    strcat(temp1,method);
    strcat(temp1,"\n");
    char temp2[256]="";
    strcpy(temp2,"The location is ");
    strcat(temp2,sign[i].dir_fullPath);
    strcat(temp2,"\n");

    time_t ct;
    ct=time(NULL);
    //printf("It happened at %s\n",ctime(&ct));
    char temp3[256]="";
    strcpy(temp3,"It happened at ");
    strcat(temp3,ctime(&ct));
    strcat(temp3,"\n");

    //output to console
    printf("%s",temp1);
    printf("%s",temp2);
    printf("%s",temp3);

    //output to file
    FILE* fp;
    fp=fopen(LOGFILE,"a+");
    if (fp==NULL)
        return;
    fputs(temp1,fp);
    fputs(temp2,fp);
    fputs(temp3,fp);
    fclose(fp);

    //output to another terminal
    char bufTemp[800]="";
    strcpy(bufTemp,temp1);
    strcat(bufTemp,temp2);
    strcat(bufTemp,temp3);
    if (write(sockfd, bufTemp, sizeof(bufTemp)) != sizeof(bufTemp)) {
            perror("write error!\n");
        }
}

int creatConnection()
{
    //char* to int
    int arrtemp[4]={0},port_int=0;
    int i=0,j=0;
    for(;i<sizeof(port);i++)
    {
        arrtemp[j]=port[i]-'0';
        j++;
    }
    j=4;
    int k=0;
    for(;k<4;k++)
    {
        int s=j;
        while(s>1)
        {
            arrtemp[k]=arrtemp[k]*10;
            s--;
        }
        port_int+=arrtemp[k];
        j--;
    }

    int sockfd;
    struct sockaddr_in target_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket error\n");
        return -1;
    }

    bzero(&target_addr, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(ip);
    target_addr.sin_port = htons(port_int);
    if (connect(sockfd, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
        perror("connect error\n");
        return -1;
    }
    printf("Connect successfully!\n");
    return sockfd;
}

int getConfig()
{
    FILE* fp;
    char temp[256];
    int i,tempint;
    fp=fopen(CONFIGRATION,"r");
    if (fp==NULL)
    {
        printf("Cannot find the file!\n");
        return -1;
    }
    int fileIndex=0;
    while (fgets(temp,sizeof(temp),fp)!=NULL)
    {
        fileIndex++;
        switch(fileIndex)
        {
            case 1:
            {
                strcpy(ip,temp);
                ip[strlen(temp)-1]='\0';
                break;
            }
            case 2:
            {
                strcpy(port,temp);
                port[strlen(temp)-1]='\0';
                break;
            }
            case 3:
            {
                strcpy(dirMonitor,temp);
                dirMonitor[strlen(temp)-1]='\0';
                break;
            }
            case 4:
            {
                strcpy(fileType,temp);
                fileType[strlen(temp)-1]='\0';
                break;
            }
            default:break;
        }
        memset(temp, '\0', sizeof(temp));
    }
    if(fileIndex==5)
    {
        printf("Parameters are from configration!\n");
        return 0;
    }
    return -1;
}


#endif






