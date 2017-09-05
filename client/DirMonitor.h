#ifndef DIRMONITOR_H
#define DIRMONITOR_H

#include <dirent.h>
#include <sys/stat.h>
#include "include/fileParameters.h"

class DirMonitor{
public:
    DirMonitor();
    virtual ~DirMonitor();
    int startMonitoring();

private:
    int init();
    int bindWithInotify(char *dir,int fd);
    int watchingFiles(int fd);
    int inotifyEventHandler(struct inotify_event *event,int fd);


    struct watchSign{
        int dir_wd;       //to compare with event->wd
        char dir_fullPath[PATH_MAX];
    }sign[120];     //cannot over 500 packages!
    int signIndex;   //global index of the sign
    char fileType[5];
};


#endif // DIRMONITOR_H
