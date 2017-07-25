#include "gigegrab.h"

#define OPENCV_WIN

using namespace cv;

// Namespace for using pylon objects.
using namespace Pylon;

using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 100;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
gigegrab::gigegrab()
{

}

gigegrab::~gigegrab()
{

}


int gigegrab::grab()
{
#if 0
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
                    cv_img = cv::Mat(ptrGrabResult->GetHeight(),     ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());
                    imshow("CV_Image",cv_img);
                    waitKey(1);
#endif
                    if(waitKey(30)==27){
                          camera.StopGrabbing();
                    }

                    //added by flq
                    char result[1024] = {0};
                    Mat imageGray;
                    cvtColor(cv_img,imageGray,CV_RGB2GRAY);
                    m_scancode->scanimage((void*)imageGray.data, result);
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
#elif 1
    ScanCode *m_scancode = new ScanCode(); //added by flq
    namedWindow("CV_Image",WINDOW_AUTOSIZE);

    VideoCapture capture(1);
    //设置图片的大小
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);//1280
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 960);//960
    while (1)
    {

        Mat frame;
        capture >> frame;
        //imshow("摄像头", frame);

        char c = cvWaitKey(33);
        if (c == 't')
        {
            break;
        }

        //Mat imageGray;
        //cvtColor(frame,imageGray,CV_RGB2GRAY);
        //imshow("CV_Image",imageGray);
        //waitKey(1);

        //added by flq
        char result[1024] = {0};
        //////////m_scancode->scanimage((void*)frame.data, result);   //if single process, delete
        //added end

    }
#endif
}
