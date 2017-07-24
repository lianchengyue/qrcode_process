#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

//opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

//zbar
#include "ScanCode.h"
#include "qrcodezbar.h"
#include <QApplication>

#define OPENCV_WIN


using namespace cv;

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 100;

int grab()
{
#if 1
    // The exit code of the sample application.
    int exitCode = 0;

    // Automagically call PylonInitialize and PylonTerminate to ensure
    // the pylon runtime system is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    ScanCode *m_scancode = new ScanCode(); //added by flq


    CGrabResultPtr ptrGrabResult;
#ifdef OPENCV_WIN
    namedWindow("CV_Image",WINDOW_AUTOSIZE);
#endif
    try
    {

        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
         camera.Open();

        GenApi::CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
         GenApi::CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
         Mat cv_img(width->GetValue(), height->GetValue(), CV_8UC3);

        camera.StartGrabbing();
        CPylonImage image;
        CImageFormatConverter fc;
         fc.OutputPixelFormat = PixelType_RGB8packed;

        while(camera.IsGrabbing())
        {
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            if (ptrGrabResult->GrabSucceeded()){
                     fc.Convert(image, ptrGrabResult);
#ifdef OPENCV_WIN
                    cv_img = cv::Mat(ptrGrabResult->GetHeight(),     ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());  //image.GetBuffer() ptr
                    imshow("CV_Image",cv_img);
                    waitKey(1);
#endif
                    if(waitKey(30)==27){
                          camera.StopGrabbing();
                    }

                    //added by flq
                    char result[1024] = {0};
                    m_scancode->scanimage((void*)image.GetBuffer(), result);
                    //added end
            }
        }

    }

    catch (GenICam::GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
        << e.GetDescription() << endl;
        exitCode = 1;
    }

    // Comment the following two lines to disable waiting on exit
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    return exitCode;
#endif
}
