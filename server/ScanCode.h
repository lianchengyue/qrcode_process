#ifndef SCAN_IMAGE_H
#define SCAN_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <zbar.h>

#include <pylon/PylonIncludes.h>

using namespace zbar;

class ScanCode
{
public:
    ScanCode();
    virtual ~ScanCode();

    //static void *scanimage(const void *pic, char *result);
    static void *scanimage(void *pic);
    static void *canby(void *ptr);

    //static zbar_image_scanner_t *scanner;
    //static zbar_image_t *image;

private:
    bool flag;
    //static zbar_image_scanner_t *scanner;
    //static zbar_image_t *image;

    static int initZbar();

};
#endif // SCAN_IMAGE_H
