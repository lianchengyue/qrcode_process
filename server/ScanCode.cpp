#include "ScanCode.h"
#include <QtGui>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "include/fileParameters.h"

using namespace std;
using namespace zbar;
using namespace cv;


static zbar_image_scanner_t *scanner;
static zbar_image_t *image;

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

}

//return 0:fail 1:success
void* ScanCode::scanimage(/*const*/ void *raw/*, char *result*/)
{
    /* obtain image data */
#if 0//CvMat
    CvMat *img1 = cvLoadImageM("/home/montafan/QRCode/zbar-0.10/barcode.png", CV_LOAD_IMAGE_GRAYSCALE);//QRcode3.png
    int width = img1->width;
    int height = img1->height;
    raw = (char*)img1->data.ptr;;
#elif 0
    //cv::Mat
    Mat img1 = imread("/home/montafan/QRCode/zbar-0.10/barcode.png", CV_LOAD_IMAGE_GRAYSCALE);  //IMREAD_GRAYSCALE   IMREAD_COLOR  //barcode long2
    int width = img1.cols;
    int height = img1.rows;
    raw = img1.data;
    //uchar* raw = img1.ptr<uchar>(0);
#else
    int width = INPUT_WIDTH, height = INPUT_HEIGHT;
#endif

//added by flq
    /////////QImage *img=new QImage("/home/montafan/QRCode/zbar-0.10/QRcode1.jpg");
    //ui->label->setPixmap(QPixmap::fromImage(*img));
//added end


    /* wrap image data */
    //zbar_image_t *image = zbar_image_create();
    image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);

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
        ////strcpy(result, data);

        delete(data);//added for flq
    }
    ///printf("result:%s\n", result);


    /* clean up */
    //zbar_image_destroy(image);
    //zbar_image_scanner_destroy(scanner);

    ///return &n;
}

 void* ScanCode::canby(void *ptr)
{
    printf("11");
}
