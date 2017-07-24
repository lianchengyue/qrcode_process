#ifndef SCAN_IMAGE_H
#define SCAN_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <zbar.h>

using namespace zbar;

class ScanCode
{
public:
    ScanCode();
    virtual ~ScanCode();

    void abc();
    void scanimage(const void *pic, char *result);

    zbar_image_scanner_t *scanner;

private:
    bool flag;

};
#endif // SCAN_IMAGE_H
