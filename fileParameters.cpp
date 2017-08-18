#include "include/fileParameters.h"

#include <sys/mman.h>
//#include <utils/String8.h>


const char fileParameter::KEY_MD5SUM[] = "md5sum";
const char fileParameter::KEY_SIZE[] = "target_size";
const char fileParameter::KEY_PATH[] = "target_total_path";
const char fileParameter::KEY_FILENAME[] = "target_name";
const char fileParameter::KEY_FRAGMENT_STATUS[] = "target_status";
const char fileParameter::KEY_FRAGMENT_TRANSMIT_STATUS[] = "target_status";

fileParameter::fileParameter()
           // : mMap()
{
}

fileParameter::~fileParameter()
{
}

string fileParameter::flatten() const
{
    string flattened("");
    size_t size = mMap.size();

    for (size_t i=0; i<size; i++) {
        string k,v;
        //k = mMap.keyAt(i);
        //v = mMap.valueAt(i);

        flattened += k;
        flattened += "=";
        flattened += v;
        if(i != size-1)
            flattened += ";";
    }

    return flattened;
}

void fileParameter::unflatten(const string &params)
{
    //const char *a = params;//params.string();
    //const char *b;

    //mMap.clear();

}
