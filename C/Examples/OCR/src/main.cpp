#include "hci_ocr.h"
#include "FileBuf.h"
#include "CodeConvert.h"
#include "AccountInfo.h"
#ifdef __LINUX__
#include <stdio.h>
#include <malloc.h>
#define _tmain main
#else
#include <time.h>
#include <tchar.h>
#endif

//以下路径可根据需要进行修改
//账号文件路径在AccountInfo.h中定义
#ifndef _WIN32_WCE
//日志文件保存路径
#define LOG_FILE_PATH "../../bin/win_x86"
//授权文件保存路径
#define AUTH_FILE_PATH "../../bin/win_x86"
//资源文件路径
#define DATA_PATH "../../data"
//演示使用的识别图片
#define JPG_FILE "../../testdata/ocr.jpg"
#define IDCARD_JPG_FILE "../../testdata/IDCard.jpg"
#define BIZCARD_JPG_FILE "../../testdata/BizCard.jpg"
#define IDCARD_TEMPLATE_FILE "../../data/templates/IdCard/IDCard_EN.xml"

#else
#define LOG_FILE_PATH "/SDMMC/bin"
#define AUTH_FILE_PATH "/SDMMC/bin"
#define DATA_PATH "/SDMMC/data"
#define JPG_FILE "/SDMMC/testdata/ocr.jpg"
#define IDCARD_JPG_FILE "/SDMMC/testdata/IDCard.jpg"
#define BIZCARD_JPG_FILE "/SDMMC/testdata/BizCard.jpg"
#define IDCARD_TEMPLATE_FILE "/SDMMC/data/IDCard_EN.xml"
#endif

#ifdef _WIN32_WCE
time_t time( time_t *inTT ) {
	SYSTEMTIME sysTimeStruct;
	FILETIME fTime;
	ULARGE_INTEGER int64time;
	time_t locTT = 0;

	if ( inTT == NULL ) {
		inTT = &locTT;
	}

	GetSystemTime( &sysTimeStruct );
	if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) ) {
		memcpy( &int64time, &fTime, sizeof( FILETIME ) );
		/* Subtract the value for 1970-01-01 00:00 (UTC) */
		int64time.QuadPart -= 0x19db1ded53e8000;
		/* Convert to seconds. */
		int64time.QuadPart /= 10000000;
		*inTT = int64time.QuadPart;
	}

	return *inTT;
}
#endif

//////////////////////////////////////////////////////////////////////////
const char * g_szNodeType[] = {"PAGE", "RGN ", "CELL", "LINE", "CHAR" };
const char * g_szRegionType[] = {"HORZ", "VERT", "TABLE", "GRAPH", "AUTOTEXT" };


bool CheckAndUpdataAuth()
{
	/* 检查授权是否正确，必要时会下载云端授权，授权过期后7天内继续有效

	更新授权文件有如下两种做法：

	1. 在 hci_init() 时将 autoCloudAuth 配置项设为 yes，系统会启动一个后台线程，
	定期检查授权的过期时间， 如果授权过期时间已到，会自动更新授权文件。此为缺省配置。

	2. 如果 autoCloudAuth 设为 no，则需要开发者自行在适当的时机通过 hci_check_auth() 来更新授权。

	自动更新授权的方式在PC等联网条件不受限的情况下会工作得很好，但对于移动终端应用等对网络条件
	和流量比较敏感的情况来说， 最好由开发者自行决定更新授权的时机，这样可控性更强，例如可以在WiFi
	打开的时候才进行授权更新等等。

	开发者可以使用 hci_get_auth_expire_time() 获取当前授权过期时间，当此函数返回错误或者授权过期
	时间已经快到了或者已经过期的时候， 再调用 hci_check_auth()函数到云端下载授权文件。例如，下面
	的示例会在授权过期后检测并下载新的授权。
	*/
	
	int64 nExpireTime;
	int64 nCurTime = (int64)time( NULL );
	HCI_ERR_CODE errCode = hci_get_auth_expire_time( &nExpireTime );
	if( errCode == HCI_ERR_NONE )
	{
		if( nExpireTime < nCurTime )// 已经过期
		{
			errCode = hci_check_auth();
			if( errCode == HCI_ERR_NONE )
			{
				printf( "check auth success \n" );
				return true;
			}
			else
			{
				printf( "check auth failed %d \n", errCode );
				return false;
			}
		}
		else
		{
			printf( "check auth success \n" );
			return true;
		}
	}
	else if( errCode ==	HCI_ERR_SYS_AUTHFILE_INVALID )
	{
		errCode = hci_check_auth();
		if( errCode == HCI_ERR_NONE )
		{
			printf( "check auth success \n" );
			return true;
		}
		else
		{
			printf( "check auth failed: %d \n", errCode );
			return false;
		}
	}
	else
	{
		printf( "check auth failed: %d \n", errCode );
		return false;
	}
}

bool IsCapkeyEnable(const string &capkey)
{
	//获取一下授权中全部可用能力列表
	CAPABILITY_LIST capbility_list;
	HCI_ERR_CODE errCode = hci_get_capability_list( NULL, &capbility_list);
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_get_capability_list failed return %d \n",errCode);
		return false;
	}

	//判断传入的capbility_array 中的能力是否在可用能力列表中
	bool is_capkey_enable = false;
	for (size_t capbility_index = 0; capbility_index < capbility_list.uiItemCount; capbility_index++)
	{
		//printf("capkey:%s\n",capbility_list.pItemList[capbility_index].pszCapKey);
		if (capkey.compare(string(capbility_list.pItemList[capbility_index].pszCapKey))==0)
		{
			is_capkey_enable = true;
			break;
		}
	}
	//释放可用能力列表
	hci_free_capability_list(&capbility_list);
	return is_capkey_enable;
}



//--------------------------工具函数-----------------------------
void PrintOcrXMLResult(OCR_RECOG_RESULT &sResult)
{
    char* pszGBK;
    UTF8ToGBK( (unsigned char*)sResult.pTextBuf, (unsigned char**)&pszGBK);
    printf( "RecogResult=%s\n", pszGBK );
    free( pszGBK );
}
void PrintOcrResult(OCR_RECOG_RESULT &sResult)
{
	char* pszGBK;
	UTF8ToGBK( (unsigned char*)sResult.pTextBuf, (unsigned char**)&pszGBK);
	printf( "RecogResult=%s\n", pszGBK );
	free( pszGBK );

	printf("请查看识别结果，按Enter键显示详细识别信息\n");
	getchar();

	for( int i = 0; i < sResult.nNodeCount; ++i )
	{
		// 识别结果节点类型
		printf( "%s: ", g_szNodeType[sResult.pNodes[i].wType]);
		switch( sResult.pNodes[i].wType )
		{
		case OCR_RECOG_NODE_PAGE:
			{
				// 页面宽带
				OCR_RECOG_PAGENODE &pn = sResult.pNodes[i].nodePage;
				printf( "(%d x %d), Skew:%d", 
					pn.nWidth, pn.nHeight,
					pn.nSkewAngle );
				break;
			}
		case OCR_RECOG_NODE_REGION:
			{
				// 区域类型
				OCR_RECOG_REGIONNODE & rn = sResult.pNodes[i].nodeRegion;
				// 区域相对于整个图像的位置以及区域的实际识别语言
				printf( "%s (%d, %d, %d, %d), Lang=%d",
					g_szRegionType[rn.wRegionType],
					rn.rcBound.left, rn.rcBound.top, rn.rcBound.right, rn.rcBound.bottom,
					rn.wRecogLang );
				break;
			}
		case OCR_RECOG_NODE_CELL:
			{
				OCR_RECOG_CELLNODE & cn = sResult.pNodes[i].nodeCell;
				// 此表格单元的位置
				printf( "(%d, %d, %d, %d)",
					cn.rcCellBound.left, cn.rcCellBound.top, cn.rcCellBound.right, cn.rcCellBound.bottom );
				break;
			}
		case OCR_RECOG_NODE_LINE:
			{
				OCR_RECOG_LINENODE & ln = sResult.pNodes[i].nodeLine;
				// 此行的基线
				printf( "baseline: %d, height: %d, avgCharSize: (%d x %d)",
					ln.nBaseLine, ln.nLineSize, ln.xChar, ln.yChar );
				break;
			}
		case OCR_RECOG_NODE_CHAR:
			{
				OCR_RECOG_CHARNODE & cn = sResult.pNodes[i].nodeChar;

				// 是否可信
				printf( "%s", cn.bAccept ? " " : "*" );

				char* pszGBK;
				UTF8ToGBK( (unsigned char*)cn.pszChar, (unsigned char**)&pszGBK);
				// 识别结果单字
				printf( "%s", pszGBK );
				free( pszGBK );

				// 多个候选字的识别结果
				printf( "[" );
				for(int j=0;j<CANDIDATE_NUM;j++) 
				{
					if( cn.pszCandidateWords[j] != NULL) 
					{
						UTF8ToGBK( (unsigned char*)cn.pszCandidateWords[j], (unsigned char**)&pszGBK);
						printf( "%s", pszGBK );
						free( pszGBK );
					}
					
				}
				printf( "]" );

				// 此字符在图像中区域
				printf( " (%d, %d, %d, %d)",
					cn.rcBlock.left, cn.rcBlock.top, cn.rcBlock.right, cn.rcBlock.bottom );

				// 字符前面的空格字符数
				if( cn.nSpaceBefore != 0)
				{
					printf( " LeadingSpace: %d", cn.nSpaceBefore);
				}

				break;
			}

		}

		printf( "\n" );
	}

}

void PrintDeskewResult(OCR_DESKEW_RESULT &sDeskew)
{
	printf( "需旋转角度: %d / 100\n", sDeskew.nSkewAngle );
}

char* g_szRegionName[] = {"横排文本", "竖排文本", "表格区域", "图片区域", "文本区域"};

void ShowTable( const OCR_RECOG_TABLE* pTable )
{
	printf( "\n**********************************************\n" );

	//hlines
	printf( "* count of hline is %d\n", pTable->nHLine );
	for( int i = 0; i < pTable->nHLine; ++i )
	{
		for( int j = 0; j < pTable->HLines[i].nSegmentCount; ++j )
		{
			OCR_RECOG_LINESEGMENT& ls = pTable->HLines[i].pSegment[j];
			printf( "* hline%d (%d,%d)->(%d,%d)\n", j, ls.nStartX, ls.nStartY, ls.nEndX, ls.nEndY );
		}
	}

	//vlines
	printf( "* count of vline is %d\n", pTable->nVLine );

	//Cell
	for( int i = 0; i < pTable->nCellNoCount; ++i )
	{
		OCR_RECOG_TABLECELL& tc = pTable->pCell[i];
		printf( "第%d个\n", tc.nCellNo );
	}

	printf( "*********************************************\n" );
}

void PrintLayoutResult(OCR_LAYOUT_RESULT &sLayout)
{

	if( sLayout.nRegionCount == 0 )
	{
		printf( " 0 regions found.\n" );
	}
	else
	{
		for( int i = 0; i < sLayout.nRegionCount; ++i )
		{
			OCR_RECOG_REGION& rgn = sLayout.pRegionList[i];
			printf( "region %d: 版面类型=%s,语言=%d,bound=(%d,%d,%d,%d)\n", i + 1, 
				g_szRegionName[rgn.wRegionType],
				rgn.wRecogLang,
				rgn.rcBound.left,
				rgn.rcBound.top,
				rgn.rcBound.right,
				rgn.rcBound.bottom );

			if( rgn.wRegionType == 2 && rgn.pTable != NULL )
			{
				ShowTable( rgn.pTable );
			}
		}
	}
}

//--------------------------不进行倾斜校正和版面分析,直接识别-----------------------------
bool AutoRecog(const string &capkey, const char* pszImageFile )
{
	printf( "\n-------------------不进行倾斜校正和版面分析,直接识别----------------\n" );
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	// 识别配置串
	int nSessionId = -1;
	string strSessionConfig = "capkey=" + capkey;

	// 启动 OCR Session
	printf( "hci_ocr_session_start config[ %s ]\n",strSessionConfig.c_str());
	errCode = hci_ocr_session_start(strSessionConfig.c_str(),&nSessionId);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_start return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_start success\n" );
	
	// --------------------------载入本次会话的图像-------------------------------------------------------------

	CFileBuf imageData;
	imageData.Load( pszImageFile );
	errCode = hci_ocr_set_image_buffer( nSessionId, imageData.m_pBuf, imageData.m_nLen);
	imageData.Free();

	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_set_image return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}

	// 识别配置串：输出详细结果，识别范围默认为all，且不进行自动倾斜校正和版面分析
	// 如已知识别内容，指定识别范围会获得更好的识别效果
	string strRecogConfig = "detailresult=yes,localprocess = no";
	// 如果传入的图片为截屏图片，可以添加参数imageType=screen，会有识别效果的提升，默认参数为imageType=normal
	// string strRecogConfig = "detailresult=yes,imageType=screen";
	// 在不传入版面分析结果的情况下可以另autoDeskew=yes,autoLayout=yes,即自动进行倾斜校正与版面分析后识别——自动识别

	// 识别
	OCR_RECOG_RESULT ocrRecogResult;
	errCode = hci_ocr_recog( nSessionId, strRecogConfig.c_str(), NULL, 0, &ocrRecogResult );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_recog return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_recog success\n" );

	// 输出识别结果
	PrintOcrResult( ocrRecogResult );

	// 释放识别结果
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// 终止 OCR Session
	errCode = hci_ocr_session_stop( nSessionId );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_stop return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_stop success\n" );
	printf( "\n-------------------直接识别完毕-------------------\n" );
	return true;
}

//--------------------------进行倾斜校正和版面分析，调整后识别-----------------------------
bool AdvancedRecog(const string &capkey, const char* pszImageFile )
{
	printf( "\n--------------进行倾斜校正和版面分析，调整后识别-----------------\n" );
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	// 识别配置串
	int nSessionId = -1;
	string strSessionConfig = "capkey=" + capkey;

	// 启动 OCR Session
	printf( "hci_ocr_session_start config[ %s ]\n",strSessionConfig.c_str());
	errCode = hci_ocr_session_start(strSessionConfig.c_str(),&nSessionId);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_start return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_start success\n" );
	
	// --------------------------载入本次会话的图像-------------------------------------------------------------

	CFileBuf imageData;
	imageData.Load( pszImageFile );
	errCode = hci_ocr_set_image_buffer( nSessionId, imageData.m_pBuf, imageData.m_nLen);
	imageData.Free();

	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_set_image return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}

	// 进行识别，首先进行倾斜校正分析
	OCR_DESKEW_RESULT sDeskew;
	// hci_ocr_deskew 的config为NULL或者adjustImage = no，不对数据进行倾斜校正，只返回倾斜角
	// 当ocr_deskew 的config参数为adjustImage = yes时，返回倾斜角并对数据进行校正
	string deskewconfig = "adjustImage = yes";
	errCode = hci_ocr_deskew( nSessionId,  deskewconfig.c_str(), &sDeskew );

	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_deskew return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_deskew success\n" );

	// 打印检测结果
	PrintDeskewResult( sDeskew );

	// 释放倾斜检测结果
	hci_ocr_free_deskew_result(&sDeskew);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_deskew_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_deskew_result success\n" );

	// 进行版面分析
	OCR_LAYOUT_RESULT sLayout;
	errCode = hci_ocr_layout_analysis( nSessionId, "", &sLayout );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_layout_analysis return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_layout_analysis success\n" );

	// 打印版面分析结果
	PrintLayoutResult(sLayout);
	
	/*
	此处可以对版面分析结果进行调整，识别时传入的版面可为修改后的版面列表
	*/

	// 进行识别
	// 识别配置串：输出详细结果，识别范围默认为all。如已知识别内容，指定识别范围会获得更好的识别效果
	string strRecogConfig = "detailresult=yes";
	// 如果传入的图片为截屏图片，可以添加参数imageType=screen，会有识别效果的提升，默认参数为imageType=normal
	// string strRecogConfig = "detailresult=yes,imageType=screen";

	OCR_RECOG_RESULT ocrRecogResult;
	// 在识别时传入之前的版面分析结果以进行更准确的识别
	// 此时不支持autoDeskew与antoLayout配置项，即高级识别
	errCode = hci_ocr_recog( nSessionId, strRecogConfig.c_str(), 
		sLayout.pRegionList, sLayout.nRegionCount, &ocrRecogResult );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_recog return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_recog success\n" );

	// 打印识别结果
	PrintOcrResult( ocrRecogResult );

	// 释放版面分析结果
	hci_ocr_free_layout_result(&sLayout);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_layout_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_layout_result success\n" );

	// 释放识别结果
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// 终止 OCR Session
	errCode = hci_ocr_session_stop( nSessionId );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_stop return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_stop success\n" );
	printf( "\n-------------------高级识别完毕-------------------\n" );
	return true;
}

bool TemplateRecog(const string &capkey, const char* pszImageFile ,const char* pszTemplateFile)
{	
	// 载入模板，载入的模板可以存起来用以创建session，可多个session共用，
	// session结束无需卸载，到不用时再卸载即可
	int template_id = 0;
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	if (capkey.find("ocr.local.template")!=string::npos)
	{
		errCode = hci_ocr_load_template(pszTemplateFile, &template_id); //加载模板
		if (errCode != HCI_ERR_NONE)
		{
			printf("hci_ocr_load_template failed(%d)\n", errCode);  
			return -1;
		}
		printf("hci_ocr_load_template success!\n");
	}else{
		template_id = 1 ;
	}

	// 创建会话
	int nSessionId = -1;
	char sess_config[256] = {0};
	sprintf(sess_config, "capkey=%s, templateid=%d", capkey.c_str(),template_id);
    // 启动 OCR Session
    printf( "hci_ocr_session_start config [%s]\n", sess_config);
	errCode = hci_ocr_session_start(sess_config, &nSessionId);
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_session_start failed(%d)\n", errCode);
		return -1;
	}


	CFileBuf cfb;
	if(cfb.Load(pszImageFile) == false)
	{
		printf("load image file <%s> failed.",pszImageFile);
		return -1;
	}
	// 设置图片
	errCode = hci_ocr_set_image_buffer(nSessionId,cfb.m_pBuf,cfb.m_nLen);	//主测点
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_set_image_by_decode_buffer failed(%d)\n", errCode);
		hci_ocr_session_stop(nSessionId);
		return -1;
	}
		
	// 进行识别
	//识别配置中注意cutEdge的配置,如果图像已经是切边图像则设置为no，否则可以采用默认值yes
	//模板信息配置参考开发手册中对模板文件的说明
	string strRecogConfig = "cutEdge=no,templateIndex=0,templatePageIndex=0";
	if (capkey.find("ocr.local.template")==string::npos)
	{
		// 云端识别通过指定模板领域进行区分（只支持dlcard驾驶证、vlcard行驶证、idcard身份证）。
		strRecogConfig += ",cutEdge=no,domain=idcard";
	}

	OCR_RECOG_RESULT ocrRecogResult;
	errCode = hci_ocr_recog(nSessionId, strRecogConfig.c_str() , NULL, 0, &ocrRecogResult);
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_recog failed(%d)\n", errCode);
		hci_ocr_session_stop(nSessionId);
		return -1;
	}
	PrintOcrXMLResult( ocrRecogResult );
	// 释放识别结果
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if (errCode != HCI_ERR_NONE)
	{
		printf("\n hci_ocr_free_recog_result failed %d",errCode);
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// 结束会话
	errCode = hci_ocr_session_stop(nSessionId);
	if (errCode !=HCI_ERR_NONE)
	{
		printf("\n hci_ocr_session_stop failed %d",errCode);
	}
	// 卸载模板，不必每次会话都加载、卸载模板
	if (capkey.find("ocr.local.template")!=string::npos)
    {
		errCode = hci_ocr_unload_template(template_id);
		if (errCode !=HCI_ERR_NONE)
		{
			printf("\n unload template failed %d",errCode);
		}
        printf("\n unload template success\n");
	}
	return true ; 
}

bool BizcardRecog(const string &capkey, const char* pszImageFile)
{
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	// 识别配置串
	int nSessionId = 0;
	string strSessionConfig = "capkey=" + capkey;

	// 启动 OCR Session
	printf( "hci_ocr_session_start config[ %s ]\n",strSessionConfig.c_str());
	errCode = hci_ocr_session_start(strSessionConfig.c_str(),&nSessionId);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_start return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_start success\n" );


	CFileBuf cfb;
	cfb.Load(pszImageFile);
	// 设置图片
	errCode = hci_ocr_set_image_buffer(nSessionId,cfb.m_pBuf,cfb.m_nLen);	//主测点
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_set_image_buffer failed(%d)\n", errCode);
		hci_ocr_session_stop(nSessionId);
		return -1;
	}

	// 识别
	//识别配置中注意cutEdge的配置,如果图像已经是切边图像则设置为no，否则可以采用默认值yes
	string strRecogConfig ="cutEdge=yes";
	OCR_RECOG_RESULT ocrRecogResult;
	errCode = hci_ocr_recog( nSessionId, strRecogConfig.c_str(), NULL, 0, &ocrRecogResult );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_recog return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_recog success\n" );

	// 输出识别结果
	PrintOcrXMLResult( ocrRecogResult ); 

	// 释放识别结果
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// 终止 OCR Session
	errCode = hci_ocr_session_stop( nSessionId );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_stop return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_stop success\n" );
	return true;

}


int _tmain(int argc,char * argv[])
{
	/******************************初始化sys模块********************************/
	HCI_ERR_CODE errCode = HCI_ERR_NONE;
	//从文件testdata/AccountInfo.txt获取从捷通分配的应用账号信息
	//strAccountInfo形如："appKey=##,developerKey=###,cloudUrl=###"
	//用户实际使用时可直接使用一个字符串代替
	string strAccountInfo;
	bool bRet = GetAccountInfo(strAccountInfo);
	if (!bRet)
	{
		printf("\ninput any char to quit and check!\n");
		getchar();
		return -1;
	}

	//获取能力key,能力key决定了要运行的示例分支
	string capkey;
	bRet = GetCapkey(capkey);
	if (!bRet)
	{
		printf("\ninput any char to quit and check!\n");
		getchar();
		return -1;
	}

	// 初始化 灵云 系统
	//	appKey,developerKey: 捷通分配的应用账号信息
	//	logFileSize,logLevel,logFilePath,logFileCount: 日志相关配置
	//	authPath: 读写授权文件和用户信息文件的路径
	//	autoCloudAuth: 是否自动检测云端授权
	//	cloudUrl: 云端授权地址

	string strConfig = strAccountInfo
		+ string("logFileSize=500,logLevel=5,logFilePath=" LOG_FILE_PATH ",logFileCount=10,")
		+ string("authPath=" AUTH_FILE_PATH ",autoCloudAuth=no");

	errCode = hci_init( strConfig.c_str() );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_init return %d\n", errCode );
		getchar();
		return -1;
	}
	printf( "hci_init success\n" );

	//检测授权,必要时到云端下载授权。此处需要注意的是，这个函数只是通过检测授权是否过期来判断是否需要进行
	//获取授权操作，如果在开发调试过程中，授权账号中新增了灵云sdk的能力，请到hci_init传入的authPath路径中
	//删除HCI_AUTH文件。否则无法获取新的授权文件，从而无法使用新增的灵云能力。
	if (!CheckAndUpdataAuth())
	{
		hci_release();
		printf("CheckAndUpdateAuth failed");
		getchar();
		return -1;
	}

	//判断capkey是否可用
	if (!IsCapkeyEnable(capkey))
	{
		printf("capkey [%s] is not enable\n",capkey.c_str());
		//如正确的果填写了capkey，但是capkey不可用，可以尝试性的做一次hci_check_auth()操作
		hci_release();
		getchar();
		return -1;
	}

	/******************************初始化OCR模块*******************************/

	// 初始化 OCR 能力，如果使用本地能力，会有如下配置
	// 若只使用云端能力，则此配置可以为NULL或""
	string init_config = "dataPath=" DATA_PATH;
	init_config += ",initCapkeys=";
	init_config += capkey;
	errCode = hci_ocr_init(init_config.c_str());
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_init failed return %d\n",errCode);
		hci_release();
		getchar();
		return -1;
	}


	///******************************识别*******************************/
	if (capkey.find("template") != string::npos)
 	{
 		//模板识别
		TemplateRecog(capkey, IDCARD_JPG_FILE,IDCARD_TEMPLATE_FILE);
 	}
	else if (capkey.find("bizcard") != string::npos)
 	{
 		//名片识别
 		BizcardRecog(capkey, BIZCARD_JPG_FILE);
 	}
 	else
	{
		//自动识别，不进行倾斜矫正和版面分析，直接识别
		AutoRecog(capkey, JPG_FILE);
		printf( "\n按Enter键进行高级识别\n" );
		getchar();

		// 高级识别,即交互识别——先调用倾斜校正、版面分析接口，然后在识别接口中传入版面分析结果后进行识别
		// 在倾斜校正和版面分析结果后，可以旋转图片或者手动修改版面分析的结果，然后再进行识别
		// 即在识别接口传入版面分析结果，而非自动识别中使用autoDeskew与autoLayout配置项
		// 倾斜矫正和版面分析非必须调用的接口
		AdvancedRecog(capkey, JPG_FILE);
	}


	/******************************反初始化OCR模块*******************************/
	hci_ocr_release();

	/******************************反初始化sys模块********************************/
	hci_release();

	printf("Press Enter to Exit");
	getchar();

	return 0;
}


