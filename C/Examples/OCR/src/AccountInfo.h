#ifndef __ACCOUNT_INFO_H__
#define __ACCOUNT_INFO_H__

#include <string>
using std::string;

//��Ȩ��Ϣ�ļ�·�����ɸ�����Ҫ�����޸�
#ifdef _WIN32_WCE
#define ACCOUNT_INFO_FILE "/SDMMC/testdata/AccountInfo.txt"
#else
#define ACCOUNT_INFO_FILE "../../testdata/AccountInfo.txt"
#endif

//���ļ�testdata/AccountInfo.txt��ȡ�ӽ�ͨ�����Ӧ���˺���Ϣ
//pszAccountInfo���磺"appKey=##,developerKey=###,cloudUrl=###,"��
bool GetAccountInfo(string &strAccountInfo);

bool GetCapkey(string &capkey);

#endif // __FILE_UTIL_H__
