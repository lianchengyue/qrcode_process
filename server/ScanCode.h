#ifndef SCAN_IMAGE_H
#define SCAN_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <zbar.h>

#include <pylon/PylonIncludes.h>

using namespace zbar;

typedef void scanimage(void *raw);
typedef scanimage* scanimagefunc;

class ScanCode
{
public:
    ScanCode();
    virtual ~ScanCode();

    //static void *scanimage(const void *pic, char *result);
    //static void* scanimage(void *raw);
    static void scanimagefunc(void *raw);
    static void *canby(void *ptr);

private:
    bool flag;
    //static zbar_image_scanner_t *scanner;
    //static zbar_image_t *image;

    static int initZbar();

};
#endif // SCAN_IMAGE_H
