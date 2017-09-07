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

class gigegrab
{
public:
    gigegrab();
    virtual ~gigegrab();

    int grab();

private:
    void printfps(cv::Mat frame);
#ifdef USE_MUTIPLE_THREAD
    int takeFirstProcess();
    int takeSecondProcess();
    int takeThirdProcess();
    int takeFourthProcess();
#endif

    ScanCode *m_scancode;
    int mPreviewFrames;
    int mFPSCount;
    struct timeval mPreviewStartTime;
    struct timeval mPreviewStopTime;

    fragmentProcess *mfragmentProcess;

#ifdef USE_MUTIPLE_THREAD
    pthread_t mFirstThread;
    pthread_t mSecondThread;
    pthread_t mThirdThread;
    pthread_t mFourthThread;
#endif

};
#endif // GIGEGRAB_H
