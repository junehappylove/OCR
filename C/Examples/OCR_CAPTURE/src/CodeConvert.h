#ifndef __CODE_CONVERT_H__
#define __CODE_CONVERT_H__

#if defined WIN32 || defined _WIN32_WCE
#include <windows.h>
#else
#include <iconv.h>
#endif

int GBKToUTF8(unsigned char * lpGBKStr,unsigned char ** lpUTF8Str);

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char ** lpGBKStr);

#endif // __FILE_UTIL_H__
