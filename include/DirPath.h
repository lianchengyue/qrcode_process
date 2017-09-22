#include "include/fileParameters.h"
#include "include/macros.h"
#if 0
static char SRC_BASE_LOCATION[] = "/home/montafan/QRcodeGrab/";
static char DES_BASE_LOCATION[] = "/home/montafan/QRcodeGrab/destination";

//SRC
static char SRC_LOCATION[PATH_MAX];
static char SRC_LZO_LOCATION[PATH_MAX];
static char SRC_SPLIT_LOCATION[PATH_MAX];
static char SRC_BASE64_ENCODE_LOCATION[PATH_MAX];

//SRC INI
static char SRC_INI_LOCATION[PATH_MAX];
static char SRC_INI_FILE_LOCATION[PATH_MAX];
static char SRC_INI_FOLD_LOCATION[PATH_MAX];
static char SRC_INI_FRAGMENT_LOCATION[PATH_MAX];
static char SRC_INI_FILE_FRAG_LOCATION[PATH_MAX];
static char SRC_INI_FOLD_FRAG_LOCATION[PATH_MAX];



//DES
static char* DES_LOCATION111;
static char DES_LOCATION[PATH_MAX];
static char DES_RECEIVE_LOCATION[PATH_MAX];
static char DES_BASE64_DECODE_LOCATION[PATH_MAX];
static char DES_CAT_LOCATION[PATH_MAX];

//DES INI
static char DES_RECV_INI_LOCATION[PATH_MAX];
static char DES_INI_LOCATION[PATH_MAX];
static char DES_INI_FILE_LOCATION[PATH_MAX];
static char DES_INI_FOLD_LOCATION[PATH_MAX];
static char DES_INI_FILE[PATH_MAX];
static char DES_INI_FOLD[PATH_MAX];

///========================================///
//SRC
static char SRC_LOCATION_REL[] = "/source/1_location/";
static char SRC_LZO_LOCATION_REL[] = "/source/2_lzo_location/";
static char SRC_SPLIT_LOCATION_REL[] = "/source/3_split_location/";
static char SRC_BASE64_ENCODE_LOCATION_REL[] = "/source/4_base64_encode_location/";

//SRC INI
static char SRC_INI_LOCATION_REL[] = "/source/INI/";
static char SRC_INI_FILE_LOCATION_REL[] = "/source/INI/config.ini";
static char SRC_INI_FOLD_LOCATION_REL[] = "/source/INI/folder.ini";
static char SRC_INI_FRAGMENT_LOCATION_REL[] = "/source/FRAG_INI/";
static char SRC_INI_FILE_FRAG_LOCATION_REL[] = "/source/INI/config/";
static char SRC_INI_FOLD_FRAG_LOCATION_REL[] = "/source/INI/folder/";



//DES
static char DES_LOCATION_REL[] = "/destination/4_location/";
static char DES_RECEIVE_LOCATION_REL[] = "/destination/1_receive_location/";
static char DES_BASE64_DECODE_LOCATION_REL[] = "/destination/2_base64_decode_location/";
static char DES_CAT_LOCATION_REL[] = "/destination/3_cat_location/";
//static char DES_BASE64_DECODE_LOCATION[] = "/destination/2_base64_decode_location/";

//DES INI
static char DES_RECV_INI_LOCATION_REL[] = "/destination/recvINI/";
static char DES_INI_LOCATION_REL[] = "/destination/INI/";
static char DES_INI_FILE_LOCATION_REL[] = "/destination/recvINI/config.ini/";
static char DES_INI_FOLD_LOCATION_REL[] = "/destination/recvINI/folder.ini/";
static char DES_INI_FILE_REL[] = "/destination/INI/config.ini";
static char DES_INI_FOLD_REL[] = "/destination/INI/folder.ini";


#else
static char SRC_LOCATION[] = "/home/montafan/QRcodeGrab/source/1_location/";
static char SRC_LZO_LOCATION[] = "/home/montafan/QRcodeGrab/source/2_lzo_location/";
static char SRC_SPLIT_LOCATION[] = "/home/montafan/QRcodeGrab/source/3_split_location/";
static char SRC_BASE64_ENCODE_LOCATION[] = "/home/montafan/QRcodeGrab/source/4_base64_encode_location/";
//static const char SRC_WHOLE_INI_LOCATION[] = "/home/montafan/QRcodeGrab/source/wholeINI/";

//SRC INI
static char SRC_INI_LOCATION[] = "/home/montafan/QRcodeGrab/source/INI/";
static char SRC_INI_FILE_LOCATION[] = "/home/montafan/QRcodeGrab/source/INI/config.ini";
static char SRC_INI_FOLD_LOCATION[] = "/home/montafan/QRcodeGrab/source/INI/folder.ini";
static char SRC_INI_FRAGMENT_LOCATION[] = "/home/montafan/QRcodeGrab/source/FRAG_INI/";
static char SRC_INI_FILE_FRAG_LOCATION[] = "/home/montafan/QRcodeGrab/source/INI/config/";
static char SRC_INI_FOLD_FRAG_LOCATION[] = "/home/montafan/QRcodeGrab/source/INI/folder/";



//DES
static char DES_LOCATION[] = "/home/montafan/QRcodeGrab/destination/4_location/";
static char DES_RECEIVE_LOCATION[] = "/home/montafan/QRcodeGrab/destination/1_receive_location/";
static char DES_BASE64_DECODE_LOCATION[] = "/home/montafan/QRcodeGrab/destination/2_base64_decode_location/";
static char DES_CAT_LOCATION[] = "/home/montafan/QRcodeGrab/destination/3_cat_location/";
//static char DES_BASE64_DECODE_LOCATION[] = "/home/montafan/QRcodeGrab/destination/2_base64_decode_location/";

//DES INI
static char DES_RECV_INI_LOCATION[] = "/home/montafan/QRcodeGrab/destination/recvINI/";
static char DES_INI_LOCATION[] = "/home/montafan/QRcodeGrab/destination/INI/";
static char DES_INI_FILE_LOCATION[] = "/home/montafan/QRcodeGrab/destination/recvINI/config.ini/";
static char DES_INI_FOLD_LOCATION[] = "/home/montafan/QRcodeGrab/destination/recvINI/folder.ini/";
static char DES_INI_FILE[] = "/home/montafan/QRcodeGrab/destination/INI/config.ini";
static char DES_INI_FOLD[] = "/home/montafan/QRcodeGrab/destination/INI/folder.ini";
#endif
