#include "JSON.h"
#include "client/qrgenerator.h"

using namespace std;

#ifdef USE_ACTIVEMQ
extern vector<ActiveMQVec> UDPVec;
extern vector<ActiveMQVec> NormalVec;

extern vector<string> UDPrawVec;
extern vector<string> NormalrawVec;
//extern vector<ActiveMQVec> MessageVec;
#endif

int readJSON()
{
    // 解析json串
    //1:udp 2:tcp
    string message = "{\"filename\":\"hu.png\",\"date\":\"2017-3-3\",\"size\":1077,\"md5sum\":\"d54646434003246e49eac0f2abe211a4\",\"type\": 1}";

    Json::Reader reader(Json::Features::strictMode());
    Json::Value parseData;
    if (reader.parse(message.c_str(), parseData))
    {
        string filename = parseData.get("filename", 0).asString();
        string date = parseData.get("date", 0).asString();
        int size = parseData.get("size", 0).asInt();
        string md5sum = parseData.get("md5sum", 0).asString();
        int type = parseData.get("type", 0).asInt();
        printf("========content=start=======\n"
               "readJSONMessage(),\n"
               "filename:%s,\n"
               "date:%s,\n"
               "size:%d,\n"
               "md5sum:%s,\n"
               "type:%d\n"
               "========content=end========\n", filename.c_str(), date.c_str(), size, md5sum.c_str(), type);

        //将读取到的消息写到本地vector中
        ActiveMQVec receivedMessage;

        receivedMessage.filename = filename;
        receivedMessage.size = size;
        receivedMessage.date = date;
        receivedMessage.md5sum = md5sum;
        receivedMessage.type =type;
        if(1 == type)//UDP
        {
            UDPVec.push_back(receivedMessage);

        }
        else if(2 == type)
        {
            NormalVec.push_back(receivedMessage);
        }
        else
        {
            printf("/*******************WRONG TYPE OF MESSAGE****************/");
        }

         printf("UDPVec.size=%d,NormalVec.size=%d\n", UDPVec.size(), NormalVec.size());

    }
    else
    {
        printf("parse failed");
    }

    return 0;
}

int readJSONMessage(std::string message)
{
    // 解析json串
    //1:udp 2:tcp
    //string message = "{\"filename\":\"hu.png\",\"date\":\"2017-8-8\",\"size\":1077,\"md5sum\":\"d54646434003246e49eac0f2abe211a4\",\"type\": 1}";

    Json::Reader reader(Json::Features::strictMode());
    Json::Value parseData;
    if (reader.parse(message.c_str(), parseData))
    {
        string filename = parseData.get("filename", 0).asString();
        string date = parseData.get("date", 0).asString();
        int size = parseData.get("size", 0).asInt();
        string md5sum = parseData.get("md5sum", 0).asString();
        int type = parseData.get("type", 0).asInt();

        printf("\n\n\n========content=start=======\n"
               "readJSONMessage(),\n"
               "filename:%s,\n"
               "date:%s,\n"
               "size:%d,\n"
               "md5sum:%s,\n"
               "type:%d\n"
               "========content=end=======\n", filename.c_str(), date.c_str(), size, md5sum.c_str(), type);

        //将读取到的消息写到本地vector中
        ActiveMQVec receivedMessage;

        receivedMessage.filename = filename;
        receivedMessage.size = size;
        receivedMessage.date = date;
        receivedMessage.md5sum = md5sum;
        receivedMessage.type =type;
        if(UDP == type)//UDP
        {
            ///UDPVec.push_back(receivedMessage);
            UDPrawVec.push_back(message);

        }
        else if(NORMAL == type)
        {
            ///NormalVec.push_back(receivedMessage);
            NormalrawVec.push_back(message);
        }
        else
        {
            printf("/*******************WRONG TYPE OF MESSAGE****************/");
        }

         //printf("UDPVec.size=%d,NormalVec.size=%d\n", UDPVec.size(), NormalVec.size());
        printf("UDPrawVec.size=%d,NormalrawVec.size=%d\n", UDPrawVec.size(), NormalrawVec.size());

    }
    else
    {
        printf("readJSONMessage(), parse failed");
    }

    return 0;
}

string writeJSON_TransResult(const char *date, const char *d_name, int type, int Errno)
{
    // 生成json串
    Json::Value parseData;
    Json::FastWriter fastWriter;

    parseData["date"] = date;
    parseData["filename"] = d_name;
    parseData["type"] = type;
    parseData["errno"] = Errno;

    string result = fastWriter.write(parseData);
    printf("result: %s \n", result.c_str());

    return result;
}


string writeJSON_RecvResult(char *totalDir, char *date, char *d_name, int type, int Errno)
{
    // 生成json串
    Json::Value parseData;
    Json::FastWriter fastWriter;

    parseData["date"] = date;
    parseData["fliename"] = d_name;
    parseData["path"] = totalDir;
    parseData["type"] = type;
    parseData["errno"] = Errno;

    string result = fastWriter.write(parseData);
    printf("result: %s \n", result.c_str());

    return result;
}
