#ifndef MACROS_H
#define MACROS_H

//打开为发送端，屏蔽为接收端
#ifndef DISPLAY_QRCODE
//#define DISPLAY_QRCODE  //grab pic
#endif

//是否使用LZO压缩算法
#ifndef USE_LZO_COMPRESSION
#define USE_LZO_COMPRESSION
#endif


//调试宏控，发送端,发送的ini与正文是否带报头(正常流程不打开)
#ifndef INI_FRAGMENT_WITHOUT_MASTHEAD
//#define INI_FRAGMENT_WITHOUT_MASTHEAD
#endif

#endif // MACROS_H
