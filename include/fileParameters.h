#ifndef FILEPARAMETERS_H
#define FILEPARAMETERS_H

#include <string>
#include <vector>

#include "include/macros.h"

#define MD5SUM_MAX 64
#define NAME_MAX 255
#define PATH_MAX 255

#define BLOCK_SIZE 2048  //split_size suggest smaller than 2048, 1792

#define QRDATA_SIZE 3072  //3072,  2076

//#define INPUT_WIDTH 320
//#define INPUT_HEIGHT 240
#define INPUT_WIDTH 640
#define INPUT_HEIGHT 480
//#define INPUT_WIDTH 1280
//#define INPUT_HEIGHT 960

#define CV_WAITKEY_INTERVAL 3//3

#define LZO_SUFFIX ".lzo"

#ifdef USE_MUTIPLE_THREAD
#define THREAD_NUM 8
#define QUEUES 64
#endif

//二维码的生成,显示速度参数
#define WAIT_FRAME_COUNT 20
#define DISPLAY_INTERVAL 100000  //unit: us


typedef enum{
    FILE_FOLD=0,
    FRAGMENT_FULLSIZE,
    FRAGMENT_INSUFFICIENT,
    NEED_NOT_COMPRESS,
    EMPTY,
} fragment_status;

typedef enum{
    IDLE=1,
    PRESTART,
    PREEND,
    TRANSMITTING,
    END,
} TransmitStatus;

const char TRANSMIT_PRESTART[] = "1010101010"; //传输报头
const char TRANSMIT_PREEND[] = "2020202020"; //传输报头
//const char TRANSMIT_START[] = "http://baike.baidu.com";//传输开始
const char TRANSMIT_START[] = "s0s0s0s0s0";//传输开始
const char TRANSMIT_END[] = "http://baike.baidu.com";//传输结束
//const char TRANSMIT_END[] = "e0e0e0e0e0";//传输结束
const char TRANSMIT_IDLE[] = "3030303030";//空闲状态
//const char TRANSMIT_INI[] = "3030303030"; //传输配置文件


const char TRANSMIT_TEST[] = "http://baike.baidu.com";//传输TEST

typedef struct{
    unsigned char* md5sum;  //64
    int size;
    char* path; //512  /should be 255
    char name [NAME_MAX+1];
    fragment_status status;

} fragment_parm;//Or in stats.h

using namespace std;


class fileParameter
{
public:
    fileParameter();
    fileParameter(const string &params)
    {
        unflatten(params);
    }
    ~fileParameter();

    string flatten() const;
    void unflatten(const string &params);

    //KEYs
    static const char KEY_MD5SUM[];
    static const char KEY_SIZE[];
    static const char KEY_PATH[];
    static const char KEY_FILENAME[];
    static const char KEY_FRAGMENT_STATUS[];
    static const char KEY_FRAGMENT_TRANSMIT_STATUS[];

private:
    //DefaultKeyVector<String8, String8> mMap;
    //vector<string, string> mMap;
    vector<string> mMap;
};

#endif // FILEPARAMETERS_H
