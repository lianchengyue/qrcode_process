#ifndef FILEPARAMETERS_H
#define FILEPARAMETERS_H

#include <string>
#include <vector>

#define MD5SUM_MAX 64
#define NAME_MAX 255
#define PATH_MAX 255

typedef enum{
    FILE_FOLD=0,
    FRAGMENT_FULLSIZE,
    FRAGMENT_INSUFFICIENT,
    NEED_NOT_COMPRESS,
    EMPTY,
} fragment_status;

const char TRANSMIT_START[] = "0000000000";//传输开始
const char TRANSMIT_END[] = "1111111111";//传输结束
const char TRANSMIT_IDLE[] = "1010101010";//空闲状态
const char TRANSMIT_INI[] = "2020202020"; //传输配置文件

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
