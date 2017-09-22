#include <sys/time.h>
#include "gigeGrab.h"

#include "include/DirPath.h"
#include "include/Errors.h"
#include "include/macros.h"
#include "include/fileParameters.h"


#define OPENCV_WIN
#define FPS_LOG_FREQ 3

using namespace cv;
using namespace Pylon;
using namespace std;

Mat imageGray;

gigeGrab::gigeGrab()
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

gigeGrab::~gigeGrab()
{
#ifdef USE_MUTIPLE_THREAD
    assert(threadpool_destroy(pool, 0) == 0);

    pthread_mutex_destroy(&mScanImgData.lock);
#endif
}


int gigeGrab::grab()
{
    int exitCode = 0;

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
                     cout <<"SizeX: "<<ptrGrabResult->GetWidth()<<endl;
                     cout <<"SizeY: "<<ptrGrabResult->GetHeight()<<endl;

                     //帧数及帧率
                     if(0 == mPreviewFrames)
                     {
                         printf("first gige frame arrives!\n");
                     }

                     printf("mPreviewFrames=%d,", mPreviewFrames);

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

                    frame = cv::Mat(ptrGrabResult->GetHeight(),     ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)image.GetBuffer());
            #ifdef USE_MUTIPLE_THREAD
                    mScanImgData.ret = 9;
                    mScanImgData.framecnt = mPreviewFrames;
                    cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
                    #ifdef OPENCV_WIN
                    imshow("basler camera",mScanImgData.imageGray);
                    waitKey(1);
                    #endif

                    /////关注为什么ASSERT失败
                    //assert(threadpool_add(pool, m_scancode->scanimagefunc, (void*)&mScanImgData, 0) == 0);
                    threadpool_add(pool, m_scancode->scanimagefunc, (void*)&mScanImgData, 0);
                    //assert(threadpool_destroy(pool, threadpool_graceful) == 0);

            #else
                    cvtColor(frame,mScanImgData.imageGray,CV_RGB2GRAY);
                    #ifdef OPENCV_WIN
                    imshow("basler camera",mScanImgData.imageGray);
                    waitKey(1);
                    #endif

                    m_scancode->scanimage((void*)&mScanImgData);   //if single process, delete
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
        printf("preview frames: %d, fps: %d\n", mPreviewFrames, fps);
        mFPSCount = 0;
    }
}

void AutoGainOnce(Camera_t& camera)
{
    // Check whether the gain auto function is available.
    if ( !IsWritable( camera.GainAuto))
    {
        cout << "The camera does not support Gain Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed image area of interest (Image AOI).
    if (IsWritable(camera.OffsetX))
    {
        camera.OffsetX.SetValue(camera.OffsetX.GetMin());
    }
    if (IsWritable(camera.OffsetY))
    {
        camera.OffsetY.SetValue(camera.OffsetY.GetMin());
    }
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    if(IsAvailable(camera.AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics
        if (IsWritable(camera.AutoFunctionROIUseBrightness))
        {
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera.AutoFunctionROIOffsetX.SetValue(0);
        camera.AutoFunctionROIOffsetY.SetValue(0);
        camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
        camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
    }


    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera.AutoTargetBrightness.SetValue(0.3);

    // We are going to try GainAuto = Once.

    cout << "Trying 'GainAuto = Once'." << endl;
    cout << "Initial Gain = " << camera.Gain.GetValue() << endl;

    // Set the gain ranges for luminance control.
    camera.AutoGainLowerLimit.SetValue(camera.Gain.GetMin());
    camera.AutoGainUpperLimit.SetValue(camera.Gain.GetMax());

    camera.GainAuto.SetValue(GainAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off" and the new parameter value will be applied to the
    // subsequently grabbed images.

    int n = 0;
    while (camera.GainAuto.GetValue() != GainAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera.GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;
        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto gain did not finish.");
        }
    }

    cout << "GainAuto went back to 'Off' after " << n << " frames." << endl;
    cout << "Final Gain = " << camera.Gain.GetValue() << endl << endl;
}


void gigeGrab::AutoGainContinuous(Camera_t& camera)
{
    // Check whether the Gain Auto feature is available.
    if ( !IsWritable( camera.GainAuto))
    {
        cout << "The camera does not support Gain Auto." << endl << endl;
        return;
    }
    // Maximize the grabbed image area of interest (Image AOI).
    if (IsWritable(camera.OffsetX))
    {
        camera.OffsetX.SetValue(camera.OffsetX.GetMin());
    }
    if (IsWritable(camera.OffsetY))
    {
        camera.OffsetY.SetValue(camera.OffsetY.GetMin());
    }
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    if(IsAvailable(camera.AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics.
        if (IsWritable(camera.AutoFunctionROIUseBrightness))
        {
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera.AutoFunctionROIOffsetX.SetValue(0);
        camera.AutoFunctionROIOffsetY.SetValue(0);
        camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
        camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
    }

    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera.AutoTargetBrightness.SetValue(0.3);

    // We are trying GainAuto = Continuous.
    cout << "Trying 'GainAuto = Continuous'." << endl;
    cout << "Initial Gain = " << camera.Gain.GetValue() << endl;

    camera.GainAuto.SetValue(GainAuto_Continuous);

    // When "continuous" mode is selected, the parameter value is adjusted repeatedly while images are acquired.
    // Depending on the current frame rate, the automatic adjustments will usually be carried out for
    // every or every other image unless the camera�s micro controller is kept busy by other tasks.
    // The repeated automatic adjustment will proceed until the "once" mode of operation is used or
    // until the auto function is set to "off", in which case the parameter value resulting from the latest
    // automatic adjustment will operate unless the value is manually adjusted.
    for (int n = 0; n < 20; n++)            // For demonstration purposes, we will grab "only" 20 images.
    {
        GrabResultPtr_t ptrGrabResult;
        camera.GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);
    }
    camera.GainAuto.SetValue(GainAuto_Off); // Switch off GainAuto.

    cout << "Final Gain = " << camera.Gain.GetValue() << endl << endl;
}


void gigeGrab::AutoExposureOnce(Camera_t& camera)
{
    // Check whether auto exposure is available
    if ( !IsWritable( camera.ExposureAuto))
    {
        cout << "The camera does not support Exposure Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed area of interest (Image AOI).
    if (IsWritable(camera.OffsetX))
    {
        camera.OffsetX.SetValue(camera.OffsetX.GetMin());
    }
    if (IsWritable(camera.OffsetY))
    {
        camera.OffsetY.SetValue(camera.OffsetY.GetMin());
    }
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    if(IsAvailable(camera.AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics.
        if (IsWritable(camera.AutoFunctionROIUseBrightness))
        {
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera.AutoFunctionROIOffsetX.SetValue(0);
        camera.AutoFunctionROIOffsetY.SetValue(0);
        camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
        camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
    }

    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera.AutoTargetBrightness.SetValue(0.3);

    // Try ExposureAuto = Once.
    cout << "Trying 'ExposureAuto = Once'." << endl;
    cout << "Initial exposure time = ";
    cout << camera.ExposureTime.GetValue() << " us" << endl;

    // Set the exposure time ranges for luminance control.
    camera.AutoExposureTimeLowerLimit.SetValue(camera.AutoExposureTimeLowerLimit.GetMin());
    camera.AutoExposureTimeUpperLimit.SetValue(camera.AutoExposureTimeLowerLimit.GetMax());

    camera.ExposureAuto.SetValue(ExposureAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off", and the new parameter value will be applied to the
    // subsequently grabbed images.
    int n = 0;
    while (camera.ExposureAuto.GetValue() != ExposureAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera.GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto exposure did not finish.");
        }
    }
    cout << "ExposureAuto went back to 'Off' after " << n << " frames." << endl;
    cout << "Final exposure time = ";
    cout << camera.ExposureTime.GetValue() << " us" << endl << endl;
}


void gigeGrab::AutoExposureContinuous(Camera_t& camera)
{
    // Check whether the Exposure Auto feature is available.
    if ( !IsWritable( camera.ExposureAuto))
    {
        cout << "The camera does not support Exposure Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed area of interest (Image AOI).
    if (IsWritable(camera.OffsetX))
    {
        camera.OffsetX.SetValue(camera.OffsetX.GetMin());
    }
    if (IsWritable(camera.OffsetY))
    {
        camera.OffsetY.SetValue(camera.OffsetY.GetMin());
    }
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    if(IsAvailable(camera.AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for luminance statistics.
        // We want to use ROI1 for gathering the statistics.
        if (IsWritable(camera.AutoFunctionROIUseBrightness))
        {
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
            camera.AutoFunctionROIUseBrightness.SetValue(true);   // ROI 1 is used for brightness control
            camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
            camera.AutoFunctionROIUseBrightness.SetValue(false);   // ROI 2 is not used for brightness control
        }

        // Set the ROI (in this example the complete sensor is used)
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
        camera.AutoFunctionROIOffsetX.SetValue(0);
        camera.AutoFunctionROIOffsetY.SetValue(0);
        camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
        camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
    }

    // Set the target value for luminance control.
    // A value of 0.3 means that the target brightness is 30 % of the maximum brightness of the raw pixel value read out from the sensor.
    // A value of 0.4 means 40 % and so forth.
    camera.AutoTargetBrightness.SetValue(0.3);

    cout << "ExposureAuto 'GainAuto = Continuous'." << endl;
    cout << "Initial exposure time = ";
    cout << camera.ExposureTime.GetValue() << " us" << endl;

    camera.ExposureAuto.SetValue(ExposureAuto_Continuous);

    // When "continuous" mode is selected, the parameter value is adjusted repeatedly while images are acquired.
    // Depending on the current frame rate, the automatic adjustments will usually be carried out for
    // every or every other image, unless the camera�s microcontroller is kept busy by other tasks.
    // The repeated automatic adjustment will proceed until the "once" mode of operation is used or
    // until the auto function is set to "off", in which case the parameter value resulting from the latest
    // automatic adjustment will operate unless the value is manually adjusted.
    for (int n = 0; n < 20; n++)    // For demonstration purposes, we will use only 20 images.
    {
        GrabResultPtr_t ptrGrabResult;
        camera.GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);
    }
    camera.ExposureAuto.SetValue(ExposureAuto_Off); // Switch off Exposure Auto.

    cout << "Final exposure time = ";
    cout << camera.ExposureTime.GetValue() << " us" << endl << endl;
}


void gigeGrab::AutoWhiteBalance(Camera_t& camera)
{
    // Check whether the Balance White Auto feature is available.
    if ( !IsWritable( camera.BalanceWhiteAuto))
    {
        cout << "The camera does not support Balance White Auto." << endl << endl;
        return;
    }

    // Maximize the grabbed area of interest (Image AOI).
    if (IsWritable(camera.OffsetX))
    {
        camera.OffsetX.SetValue(camera.OffsetX.GetMin());
    }
    if (IsWritable(camera.OffsetY))
    {
        camera.OffsetY.SetValue(camera.OffsetY.GetMin());
    }
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    if(IsAvailable(camera.AutoFunctionROISelector))
    {
        // Set the Auto Function ROI for white balance.
        // We want to use ROI2
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
        camera.AutoFunctionROIUseWhiteBalance.SetValue(false);   // ROI 1 is not used for white balance
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        camera.AutoFunctionROIUseWhiteBalance.SetValue(true);   // ROI 2 is used for white balance

        // Set the Auto Function AOI for white balance statistics.
        // Currently, AutoFunctionROISelector_ROI2 is predefined to gather
        // white balance statistics.
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        camera.AutoFunctionROIOffsetX.SetValue(0);
        camera.AutoFunctionROIOffsetY.SetValue(0);
        camera.AutoFunctionROIWidth.SetValue(camera.Width.GetMax());
        camera.AutoFunctionROIHeight.SetValue(camera.Height.GetMax());
    }

    cout << "Trying 'BalanceWhiteAuto = Once'." << endl;
    cout << "Initial balance ratio: ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
        cout << "R = " << camera.BalanceRatio.GetValue() << "   ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
        cout << "G = " << camera.BalanceRatio.GetValue() << "   ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
        cout << "B = " << camera.BalanceRatio.GetValue() << endl;

    camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Once);

    // When the "once" mode of operation is selected,
    // the parameter values are automatically adjusted until the related image property
    // reaches the target value. After the automatic parameter value adjustment is complete, the auto
    // function will automatically be set to "off" and the new parameter value will be applied to the
    // subsequently grabbed images.
    int n = 0;
    while (camera.BalanceWhiteAuto.GetValue() != BalanceWhiteAuto_Off)
    {
        GrabResultPtr_t ptrGrabResult;
        camera.GrabOne( 5000, ptrGrabResult);
#ifdef PYLON_WIN_BUILD
        Pylon::DisplayImage(1, ptrGrabResult);
#endif
        ++n;

        //For demonstration purposes only. Wait until the image is shown.
        WaitObject::Sleep(100);

        //Make sure the loop is exited.
        if (n > 100)
        {
            throw RUNTIME_EXCEPTION( "The adjustment of auto white balance did not finish.");
        }
    }
    cout << "BalanceWhiteAuto went back to 'Off' after ";
    cout << n << " frames." << endl;
    cout << "Final balance ratio: ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    cout << "R = " << camera.BalanceRatio.GetValue() << "   ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    cout << "G = " << camera.BalanceRatio.GetValue() << "   ";
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    cout << "B = " << camera.BalanceRatio.GetValue() << endl;
}


bool gigeGrab::IsColorCamera(Camera_t& camera)
{
    GenApi::NodeList_t Entries;
    camera.PixelFormat.GetEntries(Entries);
    bool Result = false;

    for (size_t i = 0; i < Entries.size(); i++)
    {
        GenApi::INode *pNode = Entries[i];
        if (IsAvailable(pNode->GetAccessMode()))
        {
            GenApi::IEnumEntry *pEnum = dynamic_cast<GenApi::IEnumEntry *>(pNode);
            const GenICam::gcstring sym(pEnum->GetSymbolic());
            if (sym.find(GenICam::gcstring("Bayer")) != GenICam::gcstring::_npos())
            {
                Result = true;
                break;
            }
        }
    }
    return Result;
}
