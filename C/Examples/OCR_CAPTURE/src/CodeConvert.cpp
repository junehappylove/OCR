#include "stdafx.h"
#include "CodeConvert.h"

//////////////////////////////////////////////////////////////////////////
#ifdef __LINUX__
#include <iconv.h> 
#include <stdlib.h>
#include <memory.h>

int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen) 
{ 
	iconv_t cd; 
	char **pin = &inbuf; 
	char **pout = &outbuf; 

	cd = iconv_open(to_charset,from_charset); 
	if (cd==0) 
		return -1; 
	memset(outbuf,0,outlen); 
	if (iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen)==-1) 
		return -1; 
	iconv_close(cd); 
	return 0; 
} 
//UNICODE码转为GB2312码 
int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen) 
{ 
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen); 
} 
//GB2312码转为UNICODE码 
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen) 
{ 
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen); 
} 
#endif

#ifdef _WIN32
int CTW(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpGBKStr)  //如果GBK字符串为NULL则出错退出
		return 0;

	nRetLen = MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,0);  //获取转换到Unicode编码后所需要的字符空间长度
	//lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	lpUnicodeStr = (WCHAR *)malloc((nRetLen + 1) * sizeof(WCHAR));  //为Unicode字符串空间
	if(lpUnicodeStr == NULL)
		return 0;


	nRetLen = MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
	if(!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度

	if(!lpUTF8Str)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if(lpUnicodeStr)       
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return nRetLen;
	}

	if(nUTF8StrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if(lpUnicodeStr)
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return 0;
	}

	nRetLen = WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //转换到UTF8编码

	if(lpUnicodeStr)
		//delete []lpUnicodeStr;
		free(lpUnicodeStr);
	return nRetLen;
}
int WTC(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str)  //如果GBK字符串为NULL则出错退出
		return 0;

	nRetLen = MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,0);  //获取转换到Unicode编码后所需要的字符空间长度
	//lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	lpUnicodeStr = (WCHAR *)malloc((nRetLen + 1) * sizeof(WCHAR));  //为Unicode字符串空间
	if(lpUnicodeStr == NULL)
		return 0;
	memset(lpUnicodeStr,0,(nRetLen + 1) * sizeof(WCHAR));
	nRetLen = MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
	if(!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度

	if(!lpGBKStr)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if(lpUnicodeStr)       
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return nRetLen;
	}

	if(nGBKStrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if(lpUnicodeStr)
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return 0;
	}

	nRetLen = WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nGBKStrLen,NULL,NULL);  //转换到UTF8编码

	if(lpUnicodeStr)
		//delete []lpUnicodeStr;
		free(lpUnicodeStr);

	return nRetLen;
}

#endif

int GBKToUTF8(unsigned char * lpGBKStr,unsigned char ** lpUTF8Str)
{
#ifdef _WIN32
	int nRetLen = 0;
	nRetLen = CTW(lpGBKStr,NULL,0);
	(* lpUTF8Str) = (unsigned char *)malloc((nRetLen + 1)*sizeof(char));
	if((* lpUTF8Str) == NULL)
		return 0;
	nRetLen = CTW(lpGBKStr,(* lpUTF8Str),nRetLen);
	return nRetLen;
#endif

#ifdef __LINUX__
	int nRetLen = strlen((char *)lpGBKStr) * 2 + 1;
	* lpUTF8Str = (unsigned char *)malloc(nRetLen);
	return g2u((char *)lpGBKStr,strlen((char *)lpGBKStr),(char *)*lpUTF8Str,nRetLen);
#endif

}

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char ** lpGBKStr)
{
#ifdef _WIN32
	int nRetLen = 0;
	nRetLen = WTC(lpUTF8Str,NULL,0);
	(* lpGBKStr) = (unsigned char *)malloc((nRetLen + 1)*sizeof(char));
	if((* lpGBKStr) == NULL)
		return 0;
	nRetLen = WTC(lpUTF8Str,(* lpGBKStr),nRetLen);
	return nRetLen;
#endif


#ifdef __LINUX__
	int nRetLen = strlen((char *)lpUTF8Str) + 1;
	* lpGBKStr = (unsigned char *)malloc(nRetLen);
	return u2g((char *)lpUTF8Str,strlen((char *)lpUTF8Str),(char *)*lpGBKStr,nRetLen);
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////////