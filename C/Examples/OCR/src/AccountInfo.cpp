#include "AccountInfo.h"
#include <fstream>
#ifdef __LINUX__
#include <string.h>
#endif
using std::ifstream;

string trim(const string& str) {
	string t = str;
	t.erase(0, t.find_first_not_of(" \t\n\r"));
	t.erase(t.find_last_not_of(" \t\n\r") + 1);
	return t;
}

#define APP_KEY_STR "appKey="
#define DEVELOPER_KEY_STR "developerKey="
#define CLOUD_URL_STR "cloudUrl="

bool GetAccountInfo(string &strAccountInfo)
{	
	strAccountInfo.clear();
	ifstream fin;
	fin.open(ACCOUNT_INFO_FILE);
	if (!fin)
	{
		printf("get account info failed \n\t may be the file %s not exist!\n",ACCOUNT_INFO_FILE);
		return false;
	}

	string strTmp;
	while(getline(fin,strTmp))
	{
		strTmp = trim(strTmp);
		if (strTmp.empty() || strTmp[0] == '#' )
		{
			continue;
		}
		if (
			(strTmp.find(APP_KEY_STR) == 0 && strTmp.length() > strlen(APP_KEY_STR))
			|| (strTmp.find(DEVELOPER_KEY_STR) == 0 && strTmp.length() >strlen(DEVELOPER_KEY_STR) )
			|| (strTmp.find(CLOUD_URL_STR) == 0 && strTmp.length() >strlen(CLOUD_URL_STR) )
			)
		{
			strAccountInfo += strTmp;
			strAccountInfo += ",";	
		}
	}

	fin.close();
	if ( strAccountInfo.find(APP_KEY_STR) == string::npos 
		|| strAccountInfo.find(DEVELOPER_KEY_STR) == string::npos 
		|| strAccountInfo.find(CLOUD_URL_STR) == string::npos 
		)
	{
		printf("get account info failed \n\t account info(%s,%s,%s) \n\t some record info may be missed,please check the file %s!\n",
			APP_KEY_STR,DEVELOPER_KEY_STR,CLOUD_URL_STR,ACCOUNT_INFO_FILE);
		return false;
	}

	return true;
}

#define CAP_KEY "capKey="
bool GetCapkey(string &capkey)
{
	capkey.clear();
	ifstream fin;
	fin.open(ACCOUNT_INFO_FILE);
	if (!fin)
	{
		printf("get capkey info failed \n\t may be the file %s not exist!\n",ACCOUNT_INFO_FILE);
		return false;
	}

	string strTmp;
	while(getline(fin,strTmp))
	{
		strTmp = trim(strTmp);
		if (strTmp.empty() || strTmp[0] == '#' )
		{
			continue;
		}
		if (strTmp.find(CAP_KEY) == 0 && strTmp.length() > strlen(CAP_KEY))
		{
			capkey= strTmp;
		}
	}

	fin.close();
	if (capkey.find(CAP_KEY) == string::npos)
	{
		printf("get capkey info failed \n\t capkey info(%s) \n\t some record info may be empty,please check the file %s!\n",
			CAP_KEY,ACCOUNT_INFO_FILE);
		return false;
	}

	capkey = capkey.substr(strlen(CAP_KEY),capkey.length() - strlen(CAP_KEY));
	return true;
}
