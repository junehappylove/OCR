#ifndef __ACCOUNT_INFO_H__
#define __ACCOUNT_INFO_H__

#include <string>
using std::string;

//授权信息文件路径，可根据需要进行修改
#ifdef _WIN32_WCE
#define ACCOUNT_INFO_FILE "/SDMMC/testdata/AccountInfo.txt"
#else
#define ACCOUNT_INFO_FILE "../../testdata/AccountInfo.txt"
#endif

//从文件testdata/AccountInfo.txt获取从接通分配的应用账号信息
//pszAccountInfo形如："appKey=##,developerKey=###,cloudUrl=###,"；
bool GetAccountInfo(string &strAccountInfo);

bool GetCapkey(string &capkey);

#endif // __FILE_UTIL_H__
