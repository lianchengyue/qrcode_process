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

class gigegrab
{
public:
    gigegrab();
    virtual ~gigegrab();

    int grab();

private:
    int mPreviewFrames;
    int mFPSCount;
    struct timeval mPreviewStartTime;
    struct timeval mPreviewStopTime;

    void printfps(cv::Mat frame);

};
#endif // GIGEGRAB_H
