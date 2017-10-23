#include <sys/time.h>
#include "usbGrab.h"

#include "include/DirPath.h"
#include "include/Errors.h"
#include "include/macros.h"
#include "include/fileParameters.h"


#define OPENCV_WIN
#define FPS_LOG_FREQ 3

using namespace cv;
using namespace std;

Mat USBimageGray;

usbGrab::usbGrab()
{
    mPreviewFrames = 0;
    mFPSCount = 0;

    memset(&mScanImgData, 0, sizeof(mScanImgData));
    mScanImgData.framecnt = 0;
    mfragmentProcess = new fragmentProcess();

#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_init(&mScanImgData.lock, NULL);

    assert((pool = threadpool_create(THREAD_NUM, QUEUES, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and queue size of %d\n", THREAD_NUM, QUEUES);
#endif
}

usbGrab::~usbGrab()
{
#ifdef USE_MUTIPLE_THREAD
    assert(threadpool_destroy(pool, 0) == 0);

    pthread_mutex_destroy(&mScanImgData.lock);
#endif
}


int usbGrab::grab()
{
    int width = INPUT_WIDTH;
    int height = INPUT_HEIGHT;
    m_scancode = new ScanCode(); //added by flq
    namedWindow("usb camera",WINDOW_AUTOSIZE);

    VideoCapture capture(0);
    //设置图片的大小
    capture.set(CV_CAP_PROP_FRAME_WIDTH, width);//1280 INPUT_WIDTH
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);//960 INPUT_HEIGHT
    while (1)
    {
        Mat frame;
        capture >> frame;

        //帧数及帧率
        if(0 == mPreviewFrames)
        {
            printf("first usb frame arrives!\n");
        }

        ////printf("mPreviewFrames=%d,", mPreviewFrames);

        if(0 == mFPSCount)
        {
            gettimeofday(&mPreviewStartTime, NULL);
        }

        printfps(frame);

        char c = cv::waitKey(CV_WAITKEY_INTERVAL); //100 CV_WAITKEY_INTERVAL
        if (c == 't')
        {
            break;
        }


#ifdef USE_MUTIPLE_THREAD
        #ifdef IMAGEGRAY_DEBUG_FUNC
        Mat imageGray;
        cvtColor(frame,imageGray,CV_RGB2GRAY);
        imshow("usb camera",imageGray);
        threadpool_add(pool, m_scancode->scanimagefunc, (void*)imageGray.data, 0);

        #else
        mScanImgData.ret = 9;
        mScanImgData.framecnt = mPreviewFrames;
        cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
///        printf("the %d frame coming\n", mPreviewFrames);
        imshow("usb camera",mScanImgData.imageGray);

        /////关注为什么ASSERT失败
        //assert(threadpool_add(pool, m_scancode->scanimagefunc, (void*)&mScanImgData, 0) == 0);
        threadpool_add(pool, m_scancode->scanimagefunc, (void*)&mScanImgData, 0);
        //assert(threadpool_destroy(pool, threadpool_graceful) == 0);
        #endif
#else

        #ifdef IMAGEGRAY_DEBUG_FUNC
        Mat imageGray;
        cvtColor(frame,imageGray,CV_RGB2GRAY);
        imshow("usb camera",imageGray);
        m_scancode->scanimage((void*)imageGray.data);

        #else
        cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
        imshow("usb camera",mScanImgData.imageGray);
        m_scancode->scanimage((void*)&mScanImgData);   //if single process, delete
        #endif

#endif
        mPreviewFrames++;


        ///test
    #ifdef USE_DEBUG
        //for test验证接收文件
        char *QRcodebuf = new char[QRDATA_SIZE];  //模拟识别到的二维码
        FILE *infile = fopen("/home/montafan/QRcodeGrab/run.sh", "rb");
        fread(QRcodebuf,1,2772,infile);

        //mfragmentProcess->des_prestart_content_receiver(QRcodebuf); //OK already
        ///mfragmentProcess->des_start_content_receiver(QRcodebuf);  //OK already
        fclose(infile); // 关闭文件


        ////整个处理的入口函数，识别二维码成功后进入
        mfragmentProcess->QRdataProcess(QRcodebuf);

        free(QRcodebuf);
        //for test end

        ///===========================fragmentWrite=============================//
        //成功接收
        //if(*res)
        {
            mfragmentProcess->QRdataProcess(QRcodebuf);
        }

        //只执行一次就退出循环
        return 0; //temp, stop loop, for TEST
    #else
        //if(*res)
        {
        #ifdef USE_MUTIPLE_THREAD
        //if(mScanImgData.ret)
        //{
        //    mfragmentProcess->QRdataProcess(mScanImgData.result);
        //}
        #else
        if(mScanImgData.ret)
        {
            mfragmentProcess->QRdataProcess(mScanImgData.result);
        }
        #endif
        }
    #endif
    }
}

//print fps
void usbGrab::printfps(cv::Mat frame)
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

