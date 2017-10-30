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

#include "include/fileParameters.h"

//pylon
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
//pthread
#ifdef USE_MUTIPLE_THREAD
#include <pthread.h>
#endif

#include "server/fragmentProcess.h"  //fragment fwrite
#include "include/macros.h"
#ifdef USE_MUTIPLE_THREAD
#include "instructions/threadpool.h"
#endif

typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;

typedef Camera_t::GrabResultPtr_t GrabResultPtr_t;


typedef struct scanimageData {
    cv::Mat imageGray;
    char result[QRDATA_SIZE];
    int ret;  //0:FAIL  1:SUCCESS
    int framecnt;
#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_t lock;
#endif
} ScanImage_Data;

class gigeGrab
{
public:
    gigeGrab();
    virtual ~gigeGrab();

    int grab();

private:
    bool IsColorCamera(Camera_t& camera);
    void AutoGainOnce(Camera_t& camera);
    void AutoGainContinuous(Camera_t& camera);
    void AutoExposureOnce(Camera_t& camera);
    void AutoExposureContinuous(Camera_t& camera);
    void AutoWhiteBalance(Camera_t& camera);

    void printfps(cv::Mat frame);

    ScanCode *m_scancode;
    int mPreviewFrames;
    int mFPSCount;
    struct timeval mPreviewStartTime;
    struct timeval mPreviewStopTime;

    fragmentProcess *mfragmentProcess;

    ScanImage_Data mScanImgData[MAT_BUF_SIZE];
    //ScanImage_Data mScanImgData;

#ifdef USE_MUTIPLE_THREAD
    threadpool_t *pool;
#endif

};
#endif // GIGEGRAB_H
