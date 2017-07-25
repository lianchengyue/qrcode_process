/*#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
*/


//show QRCode
/*
#include <QCoreApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

int main()
{
    //QCoreApplication a(argc, argv);
    String title = "img";
    namedWindow( title, WINDOW_AUTOSIZE );
    Mat src1 = imread( "/home/montafan/QRCode/zbar-0.10/QRcode1.jpg" );
    cv::namedWindow(title,cv::WINDOW_NORMAL);
    cv::imshow( title, src1 );
    cv::waitKey(0);
    cv::destroyAllWindows();

//    return a.exec();
}
*/

#if 0
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<pylon/PylonIncludes.h>

using namespace cv;
using namespace std;
using namespace Pylon;

int main(int argc, char* argv[])
{
    //basler相机初始化
    PylonInitialize();
    VideoCapture capture(1);
    //设置图片的大小
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);//2592
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 960);//1944
    while (1)
    {

        Mat frame;
        capture >> frame;
        //imshow("摄像头", frame);

        char c = cvWaitKey(33);
        if (c == 't')
        {
            PylonTerminate();//退出程序前要释放相机资源，不然下次运行时会出错
            break;
        }
    }

    return 0;
}
#endif



#if 0
#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 100;

int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;

    // Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
    // is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    try
    {
        // Create an instant camera object with the camera device found first.
        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());

        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        // The parameter MaxNumBuffer can be used to control the count of buffers
        // allocated for grabbing. The default value of this parameter is 10.
        camera.MaxNumBuffer = 5;

        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing( c_countOfImagesToGrab);

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

        // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
        // when c_countOfImagesToGrab images have been retrieved.
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                // Access the image data.
                cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;

                // Display the grabbed image.
                Pylon::DisplayImage(1, ptrGrabResult);
            }
            else
            {
                cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription();
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

    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    return exitCode;
}
#endif





















#if 0
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 100;

int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;

    // Automagically call PylonInitialize and PylonTerminate to ensure
    // the pylon runtime system is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;


    CGrabResultPtr ptrGrabResult;
    namedWindow("CV_Image",WINDOW_AUTOSIZE);
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

        while(camera.IsGrabbing()){
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);
            if (ptrGrabResult->GrabSucceeded()){
                     fc.Convert(image, ptrGrabResult);

                    //cv_img = cv::Mat(ptrGrabResult->GetHeight(),     ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());
                    //imshow("CV_Image",cv_img);
                    //  waitKey(1);
                    if(waitKey(300)==27){
                          camera.StopGrabbing();
                    }
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
}
#endif





































#if 1 //can modify
//#define DISPLAY_QRCODE

#include "gigegrab.h"
#include "qrgenerator.h"
#include <QApplication>


int main(int argc, char* argv[])
{
#ifdef DISPLAY_QRCODE
    QApplication a(argc, argv);
    QRGenerator w;
    w.show();

    return a.exec();
#else
    QApplication a(argc, argv);
    QRCodeZbar w;
    w.show();

    gigegrab *m_gigegrab = new gigegrab();
    m_gigegrab->grab();

#endif
    return 0;
}
#endif























#if 0
//#include <QCoreApplication>

#include "ScanCode.h"


int main()
{

    FILE* pFileIn;
    unsigned char* pPIcBuf;
    int w = 199;
    int h = 196;
    int bufLen;

    ScanCode *m_scancode = new ScanCode();

    pFileIn = fopen("/home/montafan/QRCode/zbar-0.10/QRcode1.jpg", "rb+");

    bufLen = w*h;
    pPIcBuf = new unsigned char[bufLen];

    fread(pPIcBuf, bufLen*sizeof(unsigned char), 1, pFileIn);

    char result[1024] = {0};


    m_scancode->scanimage((void*)pPIcBuf, result);
    //scanimage((void*)pPIcBuf, result);

    return 0;
}
#endif
