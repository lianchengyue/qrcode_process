#ifndef FILEPARAMETERS_H
#define FILEPARAMETERS_H

#include <string>
#include <vector>

#include "include/macros.h"

#define DATE_MAX 16
#define MD5SUM_MAX 64
#define NAME_MAX 255
#define PATH_MAX 255

#define BLOCK_SIZE 1408//900//1408  //split_size suggest smaller than 2048, 1792, 1536   //1059

#define QRDATA_SIZE 3072  //3072,  2076

//#define INPUT_WIDTH 320
//#define INPUT_HEIGHT 240

#define INPUT_WIDTH 640
#define INPUT_HEIGHT 480

//#define INPUT_WIDTH 1280
//#define INPUT_HEIGHT 960

//#define INPUT_WIDTH 960
//#define INPUT_HEIGHT 640

//#define INPUT_WIDTH 960
//#define INPUT_HEIGHT 640

//#define INPUT_WIDTH 480
//#define INPUT_HEIGHT 480

//#define INPUT_WIDTH 600
//#define INPUT_HEIGHT 600

//#define INPUT_WIDTH 320
//#define INPUT_HEIGHT 240

//in gigeGrab, num of Mat buffer
#define MAT_BUF_SIZE 96 //16

#define CV_WAITKEY_INTERVAL 3//3

#define LZO_SUFFIX ".lzo"

#ifdef USE_MUTIPLE_THREAD
#if 0
#define THREAD_NUM 4
#define QUEUES 64
#elif 1
#define THREAD_NUM 96//96//64//16
#define QUEUES 384//384//256
#else
#define THREAD_NUM 32
#define QUEUES 128
#endif
#endif

#define CONFIG_FUNC_DEBUG
#ifdef CONFIG_FUNC_DEBUG
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...) printf(fmt,##args)
#define LOG_LOW(fmt,args...)
#else
#define LOG_ERR(fmt,args...) printf(fmt,##args)
#define LOG_DBG(fmt,args...)
#define LOG_LOW(fmt,args...)
#endif

//二维码的生成,显示速度参数
//状态码显示的次数
#define WAIT_FRAME_COUNT 5
//二维码显示15帧
#define DISPLAY_INTERVAL 66666//100000  //unit: us     30fps:33333  25fps:40000  16fps:62500  15fps:66666  12fps:83333  10fps:100000  8fps:125000  3fps:333333


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

#ifdef USE_ACTIVEMQ
typedef struct ActiveMQVec {
    std::string filename;
    std::string date;
    int size;
    std::string md5sum;
    int type;// 1: UDP 2:tcp

/*    void operator=(ActiveMQVec& other)
    {
        filename = other.filename;
        date = other.date;
        size =other.size;
        md5sum = other.md5sum;
        type = other.type;
    }
*/
} activeMQVec;

typedef enum{
    UNKNOWN = 0,
    UDP = 1,
    NORMAL = 2,
} MESSAGE_LEVEL;

typedef enum{
    TRANS_SUCCESS = 0,
    TRANS_FAILED = 1,
} TRANS_RESULT;

typedef enum{
    REV_SUCCESS = 0,
    REV_MD5SUM_NOT_MATCH = 1,
    REV_NOT_COMPLETE = 2,
} RECEIVE_RESULT;
#endif

const char TRANSMIT_PRESTART[] = "1010101010101010"; //传输报头
const char TRANSMIT_PREEND[] =   "2020202020202020"; //传输报头
const char TRANSMIT_START[] =    "s0s0s0s0s0s0s0s0";//传输开始
const char TRANSMIT_END[] =      "e0e0e0e0e0e0e0e0";//传输结束
const char TRANSMIT_IDLE[] =     "3030303030303030";//空闲状态
//const char TRANSMIT_INI[] =    "4040404040404040"; //传输配置文件
const char TRANSMIT_RESET[] =    "r0r0r0r0r0r0r0r0";//空闲状态


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
