#include <sys/time.h>
#include "gigeGrab.h"

#include "include/DirPath.h"
#include "include/Errors.h"
#include "include/macros.h"
#include "include/fileParameters.h"

//#define GIGE_GRAB_FUNC

#define OPENCV_WIN
#define FPS_LOG_FREQ 3

using namespace cv;
using namespace Pylon;
using namespace std;

Mat imageGray;

gigegrab::gigegrab()
{
    mPreviewFrames = 0;
    mFPSCount = 0;

    mfragmentProcess = new fragmentProcess();
}

gigegrab::~gigegrab()
{

}


int gigegrab::grab()
{
#ifdef GIGE_GRAB_FUNC
    int exitCode = 0;

    // Automagically call PylonInitialize and PylonTerminate to ensure
    // the pylon runtime system is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    ScanCode *m_scancode = new ScanCode();


    CGrabResultPtr ptrGrabResult;
#ifdef OPENCV_WIN
    namedWindow("gige camera",WINDOW_AUTOSIZE);
#endif
    try
    {

        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
         camera.Open();

        GenApi::CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
         GenApi::CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
         Mat frame(width->GetValue(), height->GetValue(), CV_8UC3);

        camera.StartGrabbing();
        CPylonImage image;
        CImageFormatConverter fc;
         fc.OutputPixelFormat = PixelType_RGB8packed;

        while(camera.IsGrabbing())
        {
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            if (ptrGrabResult->GrabSucceeded()){
                     fc.Convert(image, ptrGrabResult);

                     //帧数及帧率
                     if(0 == mPreviewFrames)
                     {
                         printf("first gige frame arrives!\n");
                     }
                     mPreviewFrames++;

                     if(0 == mFPSCount)
                     {
                         gettimeofday(&mPreviewStartTime, NULL);
                     }

                     printfps(frame);

#ifdef OPENCV_WIN
                    frame = cv::Mat(ptrGrabResult->GetHeight(),     ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());
                    imshow("gige camera",frame);
                    waitKey(1);
#endif
                    //if(waitKey(30)==27)
                    if(waitKey(100)==27)
                    {
                          camera.StopGrabbing();
                    }

                    //added by flq
                    char result[1024] = {0};
                    Mat imageGray;
                    cvtColor(frame,imageGray,CV_RGB2GRAY);
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
#else //USB相机
    m_scancode = new ScanCode(); //added by flq
    namedWindow("usb camera",WINDOW_AUTOSIZE);

    VideoCapture capture(0);
    //设置图片的大小
    capture.set(CV_CAP_PROP_FRAME_WIDTH, INPUT_WIDTH);//1280
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, INPUT_HEIGHT);//960
    while (1)
    {   
        Mat frame;
        capture >> frame;

        //帧数及帧率
        if(0 == mPreviewFrames)
        {
            printf("first usb frame arrives!\n");
        }

        printf("mPreviewFrames=%d\n", mPreviewFrames);

        if(0 == mFPSCount)
        {
            gettimeofday(&mPreviewStartTime, NULL);
        }

        printfps(frame);

        //imshow("usb camera", frame);

        char c = cv::waitKey(3); //100 CV_WAITKEY_INTERVAL
        if (c == 't')
        {
            break;
        }

        ////Mat imageGray;
        cvtColor(frame,imageGray,CV_RGB2GRAY);

        imshow("usb camera",imageGray);
        waitKey(1);

#ifdef USE_MUTIPLE_THREAD
        if (0 == mPreviewFrames%4)
        {
           takeFirstProcess();
           pthread_join(mFirstThread, 0);
        }

        if (1 == mPreviewFrames%4)
        {
            takeSecondProcess();
            pthread_join(mSecondThread, 0);
        }

        if (2 == mPreviewFrames%4)
        {
           takeThirdProcess();
           pthread_join(mThirdThread, 0);
        }

        if (3 == mPreviewFrames%4)
        {
            takeFourthProcess();
            pthread_join(mFourthThread, 0);
        }

#else
        //added by flq
        char *result = new char[QRDATA_SIZE];
        memset(result, 0 , QRDATA_SIZE);
        //int *res = ScanCode::scanimage((void*)imageGray.data, result);   //if single process, delete
        //int *res = m_scancode->scanimage((void*)imageGray.data, result);   //if single process, delete

        int *res =0;
                m_scancode->scanimage((void*)imageGray.data);   //if single process, delete

        //printf("out result=%s,length=%d\n", result, strlen(result));
#endif

        ///test
        #if 0
        //for test验证接收文件
        char *QRcodebuf = new char[2772];  //模拟识别到的二维码
        FILE *infile = fopen("/home/montafan/QRcodeGrab/run.sh", "rb");
        fread(QRcodebuf,1,2772,infile);

        //mfragmentProcess->des_prestart_content_receiver(QRcodebuf); //OK already
        ///mfragmentProcess->des_start_content_receiver(QRcodebuf);  //OK already
        fclose(infile); // 关闭文件


        ////整个处理的入口函数，识别二维码成功后进入
        mfragmentProcess->QRdataProcess(QRcodebuf);

        free(QRcodebuf);
        //for test end
        #endif

        ///===========================fragmentWrite=============================//
        //成功接收
/*
#ifndef USE_MUTIPLE_THREAD
        if(*res)
        {
            mfragmentProcess->QRdataProcess(result);
        }

        mPreviewFrames++;
        free(result);
#endif
*/
        mPreviewFrames++;//temp

#ifdef USE_DEBUG
        //只执行一次就退出循环
        return 0; //temp, stop loop, for TEST
#endif
    }
#endif
}

#ifdef USE_MUTIPLE_THREAD
int gigegrab::takeFirstProcess()
{
    int rc = 0;
    rc = pthread_create(&mFirstThread, NULL, m_scancode->scanimage, (void*)imageGray.data);// ScanCode::canby
    if(0 != rc)
    {
        printf("Thread1 creation failed, rc=%d\n",rc);
    }

    return 0;
}
int gigegrab::takeSecondProcess()
{
    int rc = 0;
    rc = pthread_create(&mSecondThread, NULL, m_scancode->scanimage, (void*)imageGray.data);
    if(0 != rc)
    {
        printf("Thread1 creation failed, rc=%d\n",rc);
    }

    return 0;
}
int gigegrab::takeThirdProcess()
{
    int rc = 0;
    rc = pthread_create(&mThirdThread, NULL, m_scancode->scanimage, (void*)imageGray.data);
    if(0 != rc)
    {
        printf("Thread1 creation failed, rc=%d\n",rc);
    }

    return 0;
}
int gigegrab::takeFourthProcess()
{
    int rc = 0;
    rc = pthread_create(&mFourthThread, NULL, m_scancode->scanimage, (void*)imageGray.data);
    if(0 != rc)
    {
        printf("Thread1 creation failed, rc=%d\n",rc);
    }

    return 0;
}
#endif

//print fps
void gigegrab::printfps(cv::Mat frame)
{
#if 0
    //显示帧率
    int elapse;
    int fps;
    char string[10];  //存放帧率的字符串

    mFPSCount++;
    gettimeofday(&mPreviewStopTime, NULL);
    elapse = ((mPreviewStopTime.tv_sec - mPreviewStartTime.tv_sec)*1000) + ((mPreviewStopTime.tv_usec - mPreviewStartTime.tv_usec)/1000);
    if(elapse/(1000*FPS_LOG_FREQ) > 0)
    {
        fps = mFPSCount / FPS_LOG_FREQ;
        printf("preview frames: %d, fps: %d\n", mPreviewFrames, fps);
        mFPSCount = 0;
    }

    sprintf(string, "%d", fps);
    std::string fpsString("FPS:");
    fpsString += string;
    // 将帧率信息写在输出帧上
    putText(frame, // 图像矩阵
            fpsString,                  // string型文字内容
            cv::Point(5, 20),           // 文字坐标，以左下角为原点
            cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
            0.5, // 字体大小
            cv::Scalar(255, 255, 0));       // 字体颜色
#else
    //不显示帧率
    int elapse;
    int fps;

    mFPSCount++;
    gettimeofday(&mPreviewStopTime, NULL);
    elapse = ((mPreviewStopTime.tv_sec - mPreviewStartTime.tv_sec)*1000) + ((mPreviewStopTime.tv_usec - mPreviewStartTime.tv_usec)/1000);
    if(elapse/(1000*FPS_LOG_FREQ) > 0)
    {
        fps = mFPSCount / FPS_LOG_FREQ;
        printf("preview frames: %d, fps: %d\n", mPreviewFrames, fps);
        mFPSCount = 0;
    }
#endif
}
