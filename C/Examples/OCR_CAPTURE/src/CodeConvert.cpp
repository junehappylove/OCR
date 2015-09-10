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
//UNICODE��תΪGB2312�� 
int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen) 
{ 
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen); 
} 
//GB2312��תΪUNICODE�� 
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

	if(!lpGBKStr)  //���GBK�ַ���ΪNULL������˳�
		return 0;

	nRetLen = MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,0);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
	//lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
	lpUnicodeStr = (WCHAR *)malloc((nRetLen + 1) * sizeof(WCHAR));  //ΪUnicode�ַ����ռ�
	if(lpUnicodeStr == NULL)
		return 0;


	nRetLen = MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //ת����Unicode����
	if(!nRetLen)  //ת��ʧ��������˳�
		return 0;

	nRetLen = WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //��ȡת����UTF8���������Ҫ���ַ��ռ䳤��

	if(!lpUTF8Str)  //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
	{
		if(lpUnicodeStr)       
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return nRetLen;
	}

	if(nUTF8StrLen < nRetLen)  //���������������Ȳ������˳�
	{
		if(lpUnicodeStr)
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return 0;
	}

	nRetLen = WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //ת����UTF8����

	if(lpUnicodeStr)
		//delete []lpUnicodeStr;
		free(lpUnicodeStr);
	return nRetLen;
}
int WTC(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str)  //���GBK�ַ���ΪNULL������˳�
		return 0;

	nRetLen = MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,0);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
	//lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
	lpUnicodeStr = (WCHAR *)malloc((nRetLen + 1) * sizeof(WCHAR));  //ΪUnicode�ַ����ռ�
	if(lpUnicodeStr == NULL)
		return 0;
	memset(lpUnicodeStr,0,(nRetLen + 1) * sizeof(WCHAR));
	nRetLen = MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen);  //ת����Unicode����
	if(!nRetLen)  //ת��ʧ��������˳�
		return 0;

	nRetLen = WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //��ȡת����UTF8���������Ҫ���ַ��ռ䳤��

	if(!lpGBKStr)  //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
	{
		if(lpUnicodeStr)       
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return nRetLen;
	}

	if(nGBKStrLen < nRetLen)  //���������������Ȳ������˳�
	{
		if(lpUnicodeStr)
			//delete []lpUnicodeStr;
			free(lpUnicodeStr);
		return 0;
	}

	nRetLen = WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nGBKStrLen,NULL,NULL);  //ת����UTF8����

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