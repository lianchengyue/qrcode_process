#ifndef USBGRAB_H
#define USBGRAB_H

//opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

//zbar
#include "ScanCode.h"
#include "qrcodezbar.h"
#include <QApplication>

#include "include/fileParameters.h"

//pthread
#ifdef USE_MUTIPLE_THREAD
#include <pthread.h>
#endif

#include "server/fragmentProcess.h"  //fragment fwrite
#include "include/macros.h"
#ifdef USE_MUTIPLE_THREAD
#include "instructions/threadpool.h"
#endif

//#ifdef USE_MUTIPLE_THREAD
//#define THREAD_NUM 8
//#define QUEUES 64
//#endif

typedef struct scanimageData {
    cv::Mat imageGray;
    char result[QRDATA_SIZE];
    int ret;  //0:FAIL  1:SUCCESS
    int framecnt;
#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_t lock;
#endif
} ScanImage_Data;

class usbGrab
{
public:
    usbGrab();
    virtual ~usbGrab();

    int grab();

    int getViewerStatus();

private:
    void printfps(cv::Mat frame);

    ScanCode *m_scancode;
    int mPreviewFrames;
    int mFPSCount;
    struct timeval mPreviewStartTime;
    struct timeval mPreviewStopTime;

    fragmentProcess *mfragmentProcess;

    ScanImage_Data mScanImgData[MAT_BUF_SIZE]; //MAT_BUF_SIZE
    //ScanImage_Data mScanImgData;

#ifdef USE_MUTIPLE_THREAD
    threadpool_t *pool;
#endif

    int viewer_status;
};

#endif // USBGRAB_H
