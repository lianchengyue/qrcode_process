#ifndef JSON_H
#define JSON_H

#include <json/json.h>

using namespace std;

int readJSON();
int readJSONMessage(std::string message);

string writeJSON_TransResult(const char *date, const char *d_name, int type, int Errno);
string writeJSON_RecvResult(char *totalDir, char *date, char *d_name, int type, int Errno);
#endif // JSON_H
