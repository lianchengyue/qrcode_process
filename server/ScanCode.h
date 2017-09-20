#ifndef SCAN_IMAGE_H
#define SCAN_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <zbar.h>

#include <pylon/PylonIncludes.h>

#include "ProcessInThread.h"
#include "RecvStateMachine.h"

using namespace zbar;

typedef void scanimage(void *raw);
typedef scanimage* scanimagefunc;

class RecvStateMachine;

class ScanCode
{
public:
    ScanCode();
    virtual ~ScanCode();

    //static void *scanimage(const void *pic, char *result);
    //static void* scanimage(void *raw);
    static void scanimagefunc(void *raw);
    static void *canby(void *ptr);
    friend class RecvStateMachine;

private:
    bool flag;
    //static zbar_image_scanner_t *scanner;
    //static zbar_image_t *image;

    static int initZbar();
    ///static ProcessInThread *mPinThread1;
    ///staticRecvStateMachine *m_RecvstateMachine;


};
#endif // SCAN_IMAGE_H
