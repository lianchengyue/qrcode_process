#include "ScanCode.h"
#include <QtGui>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "include/fileParameters.h"
#include "gigeGrab.h"
#include "ProcessInThread.h"

using namespace std;
using namespace zbar;
using namespace cv;


static zbar_image_scanner_t *scanner;
static zbar_image_t *image;

static ProcessInThread *mPinThread;
static RecvStateMachine *m_RecvstateMachine;

ScanCode::ScanCode()
{
    //scanner = NULL;
    initZbar();
}

ScanCode::~ScanCode()
{
    //zbar_image_destroy(image);
    //zbar_image_scanner_destroy(scanner);
}

int ScanCode::initZbar(){
    /* create a reader */
    scanner = zbar_image_scanner_create();

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1); //0

    mPinThread = new ProcessInThread();
    //mPinThread1 = new ProcessInThread();

    m_RecvstateMachine = RecvStateMachine::getInstance();
}

//return 0:fail 1:success
void ScanCode::scanimagefunc(/*const*/ void *raw/*, char *result*/)
{
    int width = INPUT_WIDTH, height = INPUT_HEIGHT;
    scanimageData *raw1 = reinterpret_cast<scanimageData *>(raw);

#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_lock(&raw1->lock);
#endif
    usleep(10);
    /* wrap image data */
    //zbar_image_t *image = zbar_image_create();

    image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    //zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
    zbar_image_set_data(image, raw1->imageGray.data, width * height, zbar_image_free_data);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
    printf("n=%d\n",n);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol))
    {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("decoded: %s symbol:%s\n", zbar_get_symbol_name(typ), data);
        ///传值
        strcpy(raw1->result, data);
        raw1->ret = n;

        delete(data);//added for flq
    }
    //printf("The %d Frame processing\n", raw1->framecnt);

    //对二维码的处理放入线程中
    //very important
    if(1 == n)
    {
        printf("The %d Frame processing start!\n", raw1->framecnt);
        mPinThread->QRdataProcess(raw1->result);
    }

#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_unlock(&raw1->lock);
#endif
}
void ScanCode::scanimage(/*const*/ void *raw/*, char *result*/)
{
    int width = INPUT_WIDTH, height = INPUT_HEIGHT;
    scanimageData *raw1 = reinterpret_cast<scanimageData *>(raw);

#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_lock(&raw1->lock);
#endif
    usleep(10);
    /* wrap image data */
    //zbar_image_t *image = zbar_image_create();

    image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    //zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
    zbar_image_set_data(image, raw1->imageGray.data, width * height, zbar_image_free_data);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
    printf("n=%d\n",n);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol))
    {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("decoded: %s symbol:%s\n", zbar_get_symbol_name(typ), data);
        ///传值
        strcpy(raw1->result, data);
        raw1->ret = n;

        delete(data);//added for flq
    }
    //printf("The %d Frame processing\n", raw1->framecnt);

#ifdef USE_MUTIPLE_THREAD
    pthread_mutex_unlock(&raw1->lock);
#endif
}

 void* ScanCode::canby(void *ptr)
{
    printf("11");
}
