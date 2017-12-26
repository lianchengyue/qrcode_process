#ifndef JSON_H
#define JSON_H

#include <json/json.h>

using namespace std;

int readJSON();
int readJSONMessage(std::string message);

string writeJSON_TransResult(const char *date, const char *d_name, int type, int Errno, const char *username);
string writeJSON_RecvResult(char *totalDir, char *date, char *d_name, int type, int Errno, const char *username);
#endif // JSON_H
