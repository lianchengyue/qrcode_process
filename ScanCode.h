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

    void scanimage(const void *pic, char *result);

private:
    bool flag;
    zbar_image_scanner_t *scanner;
    zbar_image_t *image;

    int initZbar();

};
#endif // SCAN_IMAGE_H
