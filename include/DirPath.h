#include "include/fileParameters.h"
#include "include/macros.h"

///===================最终路径=====================///
static char ROOT_DIR[256];

static char SRC_BASE_LOCATION[256];  //"/home/montafan/QRcodeGrab";
static char DES_BASE_LOCATION[256];  //"/home/montafan/QRcodeGrab";

///////Normal
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
//static char* DES_LOCATION111;
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

///////UDP
//SRC
static char SRC_UDP_LOCATION[PATH_MAX];
static char SRC_UDP_LZO_LOCATION[PATH_MAX];
static char SRC_UDP_SPLIT_LOCATION[PATH_MAX];
static char SRC_UDP_BASE64_ENCODE_LOCATION[PATH_MAX];

//SRC INI
static char SRC_UDP_INI_LOCATION[PATH_MAX];
static char SRC_UDP_INI_FILE_LOCATION[PATH_MAX];
static char SRC_UDP_INI_FOLD_LOCATION[PATH_MAX];
static char SRC_UDP_INI_FRAGMENT_LOCATION[PATH_MAX];
static char SRC_UDP_INI_FILE_FRAG_LOCATION[PATH_MAX];
static char SRC_UDP_INI_FOLD_FRAG_LOCATION[PATH_MAX];


//DES
//static char* DES_LOCATION111;
static char DES_UDP_LOCATION[PATH_MAX];
static char DES_UDP_RECEIVE_LOCATION[PATH_MAX];
static char DES_UDP_BASE64_DECODE_LOCATION[PATH_MAX];
static char DES_UDP_CAT_LOCATION[PATH_MAX];

//DES INI
static char DES_UDP_RECV_INI_LOCATION[PATH_MAX];
static char DES_UDP_INI_LOCATION[PATH_MAX];
static char DES_UDP_INI_FILE_LOCATION[PATH_MAX];
static char DES_UDP_INI_FOLD_LOCATION[PATH_MAX];
static char DES_UDP_INI_FILE[PATH_MAX];
static char DES_UDP_INI_FOLD[PATH_MAX];



///===================相对路径=====================///
static char ROOT_DIR_REL[256];

static char SRC_BASE_LOCATION_REL[] = "/QRcodeGrab";
static char DES_BASE_LOCATION_REL[] = "/QRcodeGrab";

///////Normal
//SRC
static char SRC_LOCATION_REL[] = "/source/normal/1_location/";     //"/home/montafan/QRcodeGrab/source/normal/1_location/";
static char SRC_LZO_LOCATION_REL[] = "/source/normal/2_lzo_location/";
static char SRC_SPLIT_LOCATION_REL[] = "/source/normal/3_split_location/";
static char SRC_BASE64_ENCODE_LOCATION_REL[] = "/source/normal/4_base64_encode_location/";

//SRC INI
static char SRC_INI_LOCATION_REL[] = "/source/normal/INI/";
static char SRC_INI_FILE_LOCATION_REL[] = "/source/normal/INI/config.ini";
static char SRC_INI_FOLD_LOCATION_REL[] = "/source/normal/INI/folder.ini";
static char SRC_INI_FRAGMENT_LOCATION_REL[] = "/source/normal/FRAG_INI/";
static char SRC_INI_FILE_FRAG_LOCATION_REL[] = "/source/normal/INI/config/";
static char SRC_INI_FOLD_FRAG_LOCATION_REL[] = "/source/normal/INI/folder/";



//DES
static char DES_LOCATION_REL[] = "/destination/normal/4_location/";
static char DES_RECEIVE_LOCATION_REL[] = "/destination/normal/1_receive_location/";
static char DES_BASE64_DECODE_LOCATION_REL[] = "/destination/normal/2_base64_decode_location/";
static char DES_CAT_LOCATION_REL[] = "/destination/normal/3_cat_location/";

//DES INI
static char DES_RECV_INI_LOCATION_REL[] = "/destination/normal/recvINI/";
static char DES_INI_LOCATION_REL[] = "/destination/normal/INI/";
static char DES_INI_FILE_LOCATION_REL[] = "/destination/normal/recvINI/config.ini/";
static char DES_INI_FOLD_LOCATION_REL[] = "/destination/normal/recvINI/folder.ini/";
static char DES_INI_FILE_REL[] = "/destination/normal/INI/config.ini";
static char DES_INI_FOLD_REL[] = "/destination/normal/INI/folder.ini";


///////UDP
//SRC
static char SRC_UDP_LOCATION_REL[] = "/source/UDP/1_location/";     //"/home/montafan/QRcodeGrab/source/UDP/1_location/";
static char SRC_UDP_LZO_LOCATION_REL[] = "/source/UDP/2_lzo_location/";
static char SRC_UDP_SPLIT_LOCATION_REL[] = "/source/UDP/3_split_location/";
static char SRC_UDP_BASE64_ENCODE_LOCATION_REL[] = "/source/UDP/4_base64_encode_location/";

//SRC INI
static char SRC_UDP_INI_LOCATION_REL[] = "/source/UDP/INI/";
static char SRC_UDP_INI_FILE_LOCATION_REL[] = "/source/UDP/INI/config.ini";
static char SRC_UDP_INI_FOLD_LOCATION_REL[] = "/source/UDP/INI/folder.ini";
static char SRC_UDP_INI_FRAGMENT_LOCATION_REL[] = "/source/UDP/FRAG_INI/";
static char SRC_UDP_INI_FILE_FRAG_LOCATION_REL[] = "/source/UDP/INI/config/";
static char SRC_UDP_INI_FOLD_FRAG_LOCATION_REL[] = "/source/UDP/INI/folder/";



//DES
static char DES_UDP_LOCATION_REL[] = "/destination/UDP/4_location/";
static char DES_UDP_RECEIVE_LOCATION_REL[] = "/destination/UDP/1_receive_location/";
static char DES_UDP_BASE64_DECODE_LOCATION_REL[] = "/destination/UDP/2_base64_decode_location/";
static char DES_UDP_CAT_LOCATION_REL[] = "/destination/UDP/3_cat_location/";
//static char DES_BASE64_DECODE_LOCATION[] = "/destination/UDP/2_base64_decode_location/";

//DES INI
static char DES_UDP_RECV_INI_LOCATION_REL[] = "/destination/UDP/recvINI/";
static char DES_UDP_INI_LOCATION_REL[] = "/destination/UDP/INI/";
static char DES_UDP_INI_FILE_LOCATION_REL[] = "/destination/UDP/recvINI/config.ini/";
static char DES_UDP_INI_FOLD_LOCATION_REL[] = "/destination/UDP/recvINI/folder.ini/";
static char DES_UDP_INI_FILE_REL[] = "/destination/UDP/INI/config.ini";
static char DES_UDP_INI_FOLD_REL[] = "/destination/UDP/INI/folder.ini";
