#include <sys/time.h>
#include "gigeGrab.h"

#include "include/DirPath.h"
#include "include/Errors.h"
#include "include/macros.h"
#include "include/fileParameters.h"

#define OPENCV_WIN
#define FPS_LOG_FREQ  3

using namespace cv;
using namespace Pylon;
// Namespace for using GenApi objects.
using namespace GenApi;
using namespace std;

gigeGrab::gigeGrab()
{
    mPreviewFrames = 0;
    mFPSCount = 0;

////    memset(&mScanImgData, 0, sizeof(mScanImgData));
    mfragmentProcess = new fragmentProcess();

#ifdef USE_MUTIPLE_THREAD
////    pthread_mutex_init(&mScanImgData.lock, NULL);

    assert((pool = threadpool_create(THREAD_NUM, QUEUES, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and queue size of %d\n", THREAD_NUM, QUEUES);
#endif
}

gigeGrab::~gigeGrab()
{
#ifdef USE_MUTIPLE_THREAD
    assert(threadpool_destroy(pool, 0) == 0);

////    pthread_mutex_destroy(&mScanImgData.lock);
#endif
}


int gigeGrab::grab()
{
    int exitCode = 0;

    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();

    // Automagically call PylonInitialize and PylonTerminate to ensure
    // the pylon runtime system is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    ScanCode *m_scancode = new ScanCode();


    CGrabResultPtr ptrGrabResult;
#ifdef OPENCV_WIN
    namedWindow("basler camera",WINDOW_AUTOSIZE);
#endif
    try
    {
        // Only look for cameras supported by Camera_t.
         CDeviceInfo info;
         info.SetDeviceClass( Camera_t::DeviceClass());

         // Create an instant camera object with the first found camera device that matches the specified device class.
         Camera_t camera( CTlFactory::GetInstance().CreateFirstDevice( info));

        //CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
         camera.Open();

        //set Parameters
        char path[256];
        memset(path, 0, 256);
        sprintf(path, "%s/flq.pfs", getenv("HOME"));
        LOG_DBG("pfs PATH:%s\n",path);
        CFeaturePersistence::Load(path, &camera.GetNodeMap(), true );
        //CFeaturePersistence::Load("/home/montafan/flq.pfs", &camera.GetNodeMap(), true );


        INodeMap& nodemap = camera.GetNodeMap();
        GenApi::CIntegerPtr width(nodemap.GetNode("Width"));
        GenApi::CIntegerPtr height(nodemap.GetNode("Height"));
        //GenApi::CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
        //GenApi::CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));

        //手动设置分辨率
        //width->SetValue(INPUT_WIDTH);    //设置水平分辨率640，可根据自己需要修改
        //height->SetValue(INPUT_HEIGHT);  //设置水平分辨率480，可根据自己需要修改

        //输入帧的格式转换，转为Mat格式
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
                     //cout <<"SizeX: "<<ptrGrabResult->GetWidth()<<endl;
                     //cout <<"SizeY: "<<ptrGrabResult->GetHeight()<<endl;

                     //帧数及帧率
                     if(0 == mPreviewFrames)
                     {
                         printf("first gige frame arrives!\n");
                     }

                     ////////////////////////////////printf("mPreviewFrames=%d,", mPreviewFrames);

                     if(0 == mFPSCount)
                     {
                         gettimeofday(&mPreviewStartTime, NULL);
                     }

                     printfps(frame);

                    if('t' == cv::waitKey(CV_WAITKEY_INTERVAL))
                    {
                          camera.StopGrabbing();
                          break;
                    }

                    frame = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());
            #ifdef USE_MUTIPLE_THREAD

                    #ifdef IMAGEGRAY_DEBUG_FUNC
                    Mat imageGray;
                    cvtColor(frame,imageGray,CV_RGB2GRAY);
                    imshow("basler camera",imageGray);
                    threadpool_add(pool, m_scancode->scanimagefunc, (void*)imageGray.data, 0);

                    #else
                    /*
                    mScanImgData.framecnt = mPreviewFrames;
                    cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
                    #ifdef OPENCV_WIN
///                    printf("the %d frame coming\n", mPreviewFrames);
                    imshow("basler camera",mScanImgData.imageGray);
                    #endif
                    */
                    int i = mPreviewFrames % MAT_BUF_SIZE;//MAT_BUF_SIZE
                    mScanImgData[i].framecnt = mPreviewFrames;
                    cvtColor(frame,mScanImgData[i].imageGray,CV_RGB2GRAY);
                    #ifdef OPENCV_WIN
                    imshow("basler camera",mScanImgData[i].imageGray);
                    #endif
                    threadpool_add(pool, m_scancode->scanimagefunc, (void*)&mScanImgData[i], 0);

                    #endif

            #else
                    #ifdef IMAGEGRAY_DEBUG_FUNC
                    //OK, imageGray
                    Mat imageGray;
                    cvtColor(frame,imageGray,CV_RGB2GRAY);
                    imshow("basler camera",imageGray);
                    m_scancode->scanimage((void*)imageGray.data);

                    #else
                    //ok, mScanImgData
                    cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
                    #ifdef OPENCV_WIN
                    imshow("basler camera",mScanImgData.imageGray);
                    //waitKey(1);
                    #endif

                    m_scancode->scanimage((void*)&mScanImgData);   //if single process, delete
                    #endif
            #endif
                    mPreviewFrames++;


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

//print fps
void gigeGrab::printfps(cv::Mat frame)
{
    //不显示帧率
    int elapse;
    int fps;

    mFPSCount++;
    gettimeofday(&mPreviewStopTime, NULL);
    elapse = ((mPreviewStopTime.tv_sec - mPreviewStartTime.tv_sec)*1000) + ((mPreviewStopTime.tv_usec - mPreviewStartTime.tv_usec)/1000);
    if(elapse/(1000*FPS_LOG_FREQ) > 0)
    {
        fps = mFPSCount / FPS_LOG_FREQ;
        LOG_ERR("preview frames: %d, fps: %d\n", mPreviewFrames, fps);
        mFPSCount = 0;
    }
}
