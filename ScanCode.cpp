#include "ScanCode.h"
#include <QtGui>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace zbar;
using namespace cv;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ScanCode::ScanCode()
{
    //Distortion_init();
    scanner = NULL;
}

ScanCode::~ScanCode()
{

}

void ScanCode::abc(){

}

void ScanCode::scanimage(const void *raw, char *result)
{
    /* create a reader */
    scanner = zbar_image_scanner_create();

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1); //0

    /* obtain image data */
    int width = 1280, height = 960;
#if 0//CvMat
    CvMat *img1 = cvLoadImageM("/home/montafan/QRCode/zbar-0.10/barcode.png", CV_LOAD_IMAGE_GRAYSCALE);//QRcode3.png
    int width = img1->width;
    int height = img1->height;
    raw = (char*)img1->data.ptr;;
#elif 0
    //cv::Mat
    Mat img1 = imread("/home/montafan/QRCode/zbar-0.10/barcode.png", CV_LOAD_IMAGE_GRAYSCALE);  //IMREAD_GRAYSCALE   IMREAD_COLOR
    //Mat imageGray;
    //cvtColor(img1,imageGray,CV_RGB2GRAY);
    int width = img1.cols;
    int height = img1.rows;
    raw = img1.data;
    //uchar* data = img1.ptr<uchar>(0);
#endif


    //void *raw = NULL;
    /////////////////////////////////////////////////get_data(name, &width, &height, &raw);
//added by flq
    /////////QImage *img=new QImage("/home/montafan/QRCode/zbar-0.10/QRcode1.jpg");
    //ui->label->setPixmap(QPixmap::fromImage(*img));

//added end


    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
    printf("n=%d\n",n);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        printf("decoded %s symbol \"%s\"\n",    zbar_get_symbol_name(typ), data);
        strcpy(result, data);
    }
    printf("result:%s\n", result);

    /* clean up */
    //zbar_image_destroy(image);
    //zbar_image_scanner_destroy(scanner);
    //return(0);
}
