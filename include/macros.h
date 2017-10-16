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

//是否使用多线程加速
#ifndef USE_MUTIPLE_THREAD
#define USE_MUTIPLE_THREAD
#endif

//接收端模拟接收的处理，模拟已经成功接收完碎片
#ifndef USE_DEBUG
//#define  USE_DEBUG
#endif

//调试宏控，发送端,发送的ini与正文是否带报头(正常流程不打开)
#ifndef INI_FRAGMENT_WITHOUT_MASTHEAD
//#define INI_FRAGMENT_WITHOUT_MASTHEAD
#endif

//LOG级别开关
#ifndef CONFIG_FUNC_DEBUG
#define CONFIG_FUNC_DEBUG
#endif

//选择相机类型BASLER
#ifndef BASLER_GRAB_FUNC
#define BASLER_GRAB_FUNC
#endif

//选择相机类型Normal USB
#ifndef USB_GRAB_FUNC
//#define USB_GRAB_FUNC
#endif

//调试开关,Mat或结构体ScanImage_Data，正常使用时关闭
#ifndef IMAGEGRAY_DEBUG_FUNC
//#define IMAGEGRAY_DEBUG_FUNC
#endif

//使用activeMQ
#ifndef USE_ACTIVEMQ
#define USE_ACTIVEMQ
#endif

//打印识别内容
#ifndef PRINT_CONTENT
#define PRINT_CONTENT
#endif

//#define INI_ACCESS_MODE
#endif // MACROS_H
