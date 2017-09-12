#ifndef GIGEGRAB_H
#define GIGEGRAB_H

#include <pylon/PylonIncludes.h>

//opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

//zbar
#include "ScanCode.h"
#include "qrcodezbar.h"
#include <QApplication>

//pthread
#ifdef USE_MUTIPLE_THREAD
#include <pthread.h>
#endif

#include "server/fragmentProcess.h"  //fragment fwrite
#include "include/macros.h"
#ifdef USE_MUTIPLE_THREAD
#include "instructions/threadpool.h"
#endif

#ifdef USE_MUTIPLE_THREAD
#define THREAD_NUM 8
//#define SIZE   8192
#define QUEUES 64
#endif

typedef struct scanimageData {
    cv::Mat imageGray;
    char result[QRDATA_SIZE];
    int ret;  //0:FAIL  1:SUCCESS
    int framecnt;
#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_t lock;
#endif
} ScanImage_Data;

class gigegrab
{
public:
    gigegrab();
    virtual ~gigegrab();

    int grab();

private:
    void printfps(cv::Mat frame);

    ScanCode *m_scancode;
    int mPreviewFrames;
    int mFPSCount;
    struct timeval mPreviewStartTime;
    struct timeval mPreviewStopTime;

    fragmentProcess *mfragmentProcess;

    ScanImage_Data mScanImgData;

#ifdef USE_MUTIPLE_THREAD
    threadpool_t *pool;
#endif

};
#endif // GIGEGRAB_H
