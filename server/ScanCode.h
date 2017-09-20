#ifndef SCAN_IMAGE_H
#define SCAN_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <zbar.h>

#include <pylon/PylonIncludes.h>

#include "ProcessInThread.h"
#include "RecvStateMachine.h"
#include "include/macros.h"

#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)//#define PR(...) printf(__VA_ARGS__)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#endif

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
    static void scanimage(/*const*/ void *raw/*, char *result*/);
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
