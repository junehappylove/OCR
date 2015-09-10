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

//����·���ɸ�����Ҫ�����޸�
//�˺��ļ�·����AccountInfo.h�ж���
#ifndef _WIN32_WCE
//��־�ļ�����·��
#define LOG_FILE_PATH "../../bin/win_x86"
//��Ȩ�ļ�����·��
#define AUTH_FILE_PATH "../../bin/win_x86"
//��Դ�ļ�·��
#define DATA_PATH "../../data"
//��ʾʹ�õ�ʶ��ͼƬ
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
	/* �����Ȩ�Ƿ���ȷ����Ҫʱ�������ƶ���Ȩ����Ȩ���ں�7���ڼ�����Ч

	������Ȩ�ļ�����������������

	1. �� hci_init() ʱ�� autoCloudAuth ��������Ϊ yes��ϵͳ������һ����̨�̣߳�
	���ڼ����Ȩ�Ĺ���ʱ�䣬 �����Ȩ����ʱ���ѵ������Զ�������Ȩ�ļ�����Ϊȱʡ���á�

	2. ��� autoCloudAuth ��Ϊ no������Ҫ�������������ʵ���ʱ��ͨ�� hci_check_auth() ��������Ȩ��

	�Զ�������Ȩ�ķ�ʽ��PC���������������޵�����»Ṥ���úܺã��������ƶ��ն�Ӧ�õȶ���������
	�������Ƚ����е������˵�� ����ɿ��������о���������Ȩ��ʱ���������ɿ��Ը�ǿ�����������WiFi
	�򿪵�ʱ��Ž�����Ȩ���µȵȡ�

	�����߿���ʹ�� hci_get_auth_expire_time() ��ȡ��ǰ��Ȩ����ʱ�䣬���˺������ش��������Ȩ����
	ʱ���Ѿ��쵽�˻����Ѿ����ڵ�ʱ�� �ٵ��� hci_check_auth()�������ƶ�������Ȩ�ļ������磬����
	��ʾ��������Ȩ���ں��Ⲣ�����µ���Ȩ��
	*/
	
	int64 nExpireTime;
	int64 nCurTime = (int64)time( NULL );
	HCI_ERR_CODE errCode = hci_get_auth_expire_time( &nExpireTime );
	if( errCode == HCI_ERR_NONE )
	{
		if( nExpireTime < nCurTime )// �Ѿ�����
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
	//��ȡһ����Ȩ��ȫ�����������б�
	CAPABILITY_LIST capbility_list;
	HCI_ERR_CODE errCode = hci_get_capability_list( NULL, &capbility_list);
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_get_capability_list failed return %d \n",errCode);
		return false;
	}

	//�жϴ����capbility_array �е������Ƿ��ڿ��������б���
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
	//�ͷſ��������б�
	hci_free_capability_list(&capbility_list);
	return is_capkey_enable;
}



//--------------------------���ߺ���-----------------------------
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

	printf("��鿴ʶ��������Enter����ʾ��ϸʶ����Ϣ\n");
	getchar();

	for( int i = 0; i < sResult.nNodeCount; ++i )
	{
		// ʶ�����ڵ�����
		printf( "%s: ", g_szNodeType[sResult.pNodes[i].wType]);
		switch( sResult.pNodes[i].wType )
		{
		case OCR_RECOG_NODE_PAGE:
			{
				// ҳ����
				OCR_RECOG_PAGENODE &pn = sResult.pNodes[i].nodePage;
				printf( "(%d x %d), Skew:%d", 
					pn.nWidth, pn.nHeight,
					pn.nSkewAngle );
				break;
			}
		case OCR_RECOG_NODE_REGION:
			{
				// ��������
				OCR_RECOG_REGIONNODE & rn = sResult.pNodes[i].nodeRegion;
				// �������������ͼ���λ���Լ������ʵ��ʶ������
				printf( "%s (%d, %d, %d, %d), Lang=%d",
					g_szRegionType[rn.wRegionType],
					rn.rcBound.left, rn.rcBound.top, rn.rcBound.right, rn.rcBound.bottom,
					rn.wRecogLang );
				break;
			}
		case OCR_RECOG_NODE_CELL:
			{
				OCR_RECOG_CELLNODE & cn = sResult.pNodes[i].nodeCell;
				// �˱��Ԫ��λ��
				printf( "(%d, %d, %d, %d)",
					cn.rcCellBound.left, cn.rcCellBound.top, cn.rcCellBound.right, cn.rcCellBound.bottom );
				break;
			}
		case OCR_RECOG_NODE_LINE:
			{
				OCR_RECOG_LINENODE & ln = sResult.pNodes[i].nodeLine;
				// ���еĻ���
				printf( "baseline: %d, height: %d, avgCharSize: (%d x %d)",
					ln.nBaseLine, ln.nLineSize, ln.xChar, ln.yChar );
				break;
			}
		case OCR_RECOG_NODE_CHAR:
			{
				OCR_RECOG_CHARNODE & cn = sResult.pNodes[i].nodeChar;

				// �Ƿ����
				printf( "%s", cn.bAccept ? " " : "*" );

				char* pszGBK;
				UTF8ToGBK( (unsigned char*)cn.pszChar, (unsigned char**)&pszGBK);
				// ʶ��������
				printf( "%s", pszGBK );
				free( pszGBK );

				// �����ѡ�ֵ�ʶ����
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

				// ���ַ���ͼ��������
				printf( " (%d, %d, %d, %d)",
					cn.rcBlock.left, cn.rcBlock.top, cn.rcBlock.right, cn.rcBlock.bottom );

				// �ַ�ǰ��Ŀո��ַ���
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
	printf( "����ת�Ƕ�: %d / 100\n", sDeskew.nSkewAngle );
}

char* g_szRegionName[] = {"�����ı�", "�����ı�", "�������", "ͼƬ����", "�ı�����"};

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
		printf( "��%d��\n", tc.nCellNo );
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
			printf( "region %d: ��������=%s,����=%d,bound=(%d,%d,%d,%d)\n", i + 1, 
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

//--------------------------��������бУ���Ͱ������,ֱ��ʶ��-----------------------------
bool AutoRecog(const string &capkey, const char* pszImageFile )
{
	printf( "\n-------------------��������бУ���Ͱ������,ֱ��ʶ��----------------\n" );
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	// ʶ�����ô�
	int nSessionId = -1;
	string strSessionConfig = "capkey=" + capkey;

	// ���� OCR Session
	printf( "hci_ocr_session_start config[ %s ]\n",strSessionConfig.c_str());
	errCode = hci_ocr_session_start(strSessionConfig.c_str(),&nSessionId);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_start return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_start success\n" );
	
	// --------------------------���뱾�λỰ��ͼ��-------------------------------------------------------------

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

	// ʶ�����ô��������ϸ�����ʶ��ΧĬ��Ϊall���Ҳ������Զ���бУ���Ͱ������
	// ����֪ʶ�����ݣ�ָ��ʶ��Χ���ø��õ�ʶ��Ч��
	string strRecogConfig = "detailresult=yes,localprocess = no";
	// ��������ͼƬΪ����ͼƬ��������Ӳ���imageType=screen������ʶ��Ч����������Ĭ�ϲ���ΪimageType=normal
	// string strRecogConfig = "detailresult=yes,imageType=screen";
	// �ڲ��������������������¿�����autoDeskew=yes,autoLayout=yes,���Զ�������бУ������������ʶ�𡪡��Զ�ʶ��

	// ʶ��
	OCR_RECOG_RESULT ocrRecogResult;
	errCode = hci_ocr_recog( nSessionId, strRecogConfig.c_str(), NULL, 0, &ocrRecogResult );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_recog return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_recog success\n" );

	// ���ʶ����
	PrintOcrResult( ocrRecogResult );

	// �ͷ�ʶ����
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// ��ֹ OCR Session
	errCode = hci_ocr_session_stop( nSessionId );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_stop return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_stop success\n" );
	printf( "\n-------------------ֱ��ʶ�����-------------------\n" );
	return true;
}

//--------------------------������бУ���Ͱ��������������ʶ��-----------------------------
bool AdvancedRecog(const string &capkey, const char* pszImageFile )
{
	printf( "\n--------------������бУ���Ͱ��������������ʶ��-----------------\n" );
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	// ʶ�����ô�
	int nSessionId = -1;
	string strSessionConfig = "capkey=" + capkey;

	// ���� OCR Session
	printf( "hci_ocr_session_start config[ %s ]\n",strSessionConfig.c_str());
	errCode = hci_ocr_session_start(strSessionConfig.c_str(),&nSessionId);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_start return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_start success\n" );
	
	// --------------------------���뱾�λỰ��ͼ��-------------------------------------------------------------

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

	// ����ʶ�����Ƚ�����бУ������
	OCR_DESKEW_RESULT sDeskew;
	// hci_ocr_deskew ��configΪNULL����adjustImage = no���������ݽ�����бУ����ֻ������б��
	// ��ocr_deskew ��config����ΪadjustImage = yesʱ��������б�ǲ������ݽ���У��
	string deskewconfig = "adjustImage = yes";
	errCode = hci_ocr_deskew( nSessionId,  deskewconfig.c_str(), &sDeskew );

	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_deskew return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_deskew success\n" );

	// ��ӡ�����
	PrintDeskewResult( sDeskew );

	// �ͷ���б�����
	hci_ocr_free_deskew_result(&sDeskew);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_deskew_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_deskew_result success\n" );

	// ���а������
	OCR_LAYOUT_RESULT sLayout;
	errCode = hci_ocr_layout_analysis( nSessionId, "", &sLayout );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_layout_analysis return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_layout_analysis success\n" );

	// ��ӡ����������
	PrintLayoutResult(sLayout);
	
	/*
	�˴����Զ԰������������е�����ʶ��ʱ����İ����Ϊ�޸ĺ�İ����б�
	*/

	// ����ʶ��
	// ʶ�����ô��������ϸ�����ʶ��ΧĬ��Ϊall������֪ʶ�����ݣ�ָ��ʶ��Χ���ø��õ�ʶ��Ч��
	string strRecogConfig = "detailresult=yes";
	// ��������ͼƬΪ����ͼƬ��������Ӳ���imageType=screen������ʶ��Ч����������Ĭ�ϲ���ΪimageType=normal
	// string strRecogConfig = "detailresult=yes,imageType=screen";

	OCR_RECOG_RESULT ocrRecogResult;
	// ��ʶ��ʱ����֮ǰ�İ����������Խ��и�׼ȷ��ʶ��
	// ��ʱ��֧��autoDeskew��antoLayout��������߼�ʶ��
	errCode = hci_ocr_recog( nSessionId, strRecogConfig.c_str(), 
		sLayout.pRegionList, sLayout.nRegionCount, &ocrRecogResult );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_recog return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_recog success\n" );

	// ��ӡʶ����
	PrintOcrResult( ocrRecogResult );

	// �ͷŰ���������
	hci_ocr_free_layout_result(&sLayout);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_layout_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_layout_result success\n" );

	// �ͷ�ʶ����
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// ��ֹ OCR Session
	errCode = hci_ocr_session_stop( nSessionId );
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_session_stop return %d\n", errCode );
		return false;
	}
	printf( "hci_ocr_session_stop success\n" );
	printf( "\n-------------------�߼�ʶ�����-------------------\n" );
	return true;
}

bool TemplateRecog(const string &capkey, const char* pszImageFile ,const char* pszTemplateFile)
{	
	// ����ģ�壬�����ģ����Դ��������Դ���session���ɶ��session���ã�
	// session��������ж�أ�������ʱ��ж�ؼ���
	int template_id = 0;
	HCI_ERR_CODE errCode = HCI_ERR_NONE;

	if (capkey.find("ocr.local.template")!=string::npos)
	{
		errCode = hci_ocr_load_template(pszTemplateFile, &template_id); //����ģ��
		if (errCode != HCI_ERR_NONE)
		{
			printf("hci_ocr_load_template failed(%d)\n", errCode);  
			return -1;
		}
		printf("hci_ocr_load_template success!\n");
	}else{
		template_id = 1 ;
	}

	// �����Ự
	int nSessionId = -1;
	char sess_config[256] = {0};
	sprintf(sess_config, "capkey=%s, templateid=%d", capkey.c_str(),template_id);
    // ���� OCR Session
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
	// ����ͼƬ
	errCode = hci_ocr_set_image_buffer(nSessionId,cfb.m_pBuf,cfb.m_nLen);	//�����
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_set_image_by_decode_buffer failed(%d)\n", errCode);
		hci_ocr_session_stop(nSessionId);
		return -1;
	}
		
	// ����ʶ��
	//ʶ��������ע��cutEdge������,���ͼ���Ѿ����б�ͼ��������Ϊno��������Բ���Ĭ��ֵyes
	//ģ����Ϣ���òο������ֲ��ж�ģ���ļ���˵��
	string strRecogConfig = "cutEdge=no,templateIndex=0,templatePageIndex=0";
	if (capkey.find("ocr.local.template")==string::npos)
	{
		// �ƶ�ʶ��ͨ��ָ��ģ������������֣�ֻ֧��dlcard��ʻ֤��vlcard��ʻ֤��idcard���֤����
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
	// �ͷ�ʶ����
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if (errCode != HCI_ERR_NONE)
	{
		printf("\n hci_ocr_free_recog_result failed %d",errCode);
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// �����Ự
	errCode = hci_ocr_session_stop(nSessionId);
	if (errCode !=HCI_ERR_NONE)
	{
		printf("\n hci_ocr_session_stop failed %d",errCode);
	}
	// ж��ģ�壬����ÿ�λỰ�����ء�ж��ģ��
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

	// ʶ�����ô�
	int nSessionId = 0;
	string strSessionConfig = "capkey=" + capkey;

	// ���� OCR Session
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
	// ����ͼƬ
	errCode = hci_ocr_set_image_buffer(nSessionId,cfb.m_pBuf,cfb.m_nLen);	//�����
	if (errCode != HCI_ERR_NONE)
	{
		printf("hci_ocr_set_image_buffer failed(%d)\n", errCode);
		hci_ocr_session_stop(nSessionId);
		return -1;
	}

	// ʶ��
	//ʶ��������ע��cutEdge������,���ͼ���Ѿ����б�ͼ��������Ϊno��������Բ���Ĭ��ֵyes
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

	// ���ʶ����
	PrintOcrXMLResult( ocrRecogResult ); 

	// �ͷ�ʶ����
	errCode = hci_ocr_free_recog_result(&ocrRecogResult);
	if( errCode != HCI_ERR_NONE )
	{
		printf( "hci_ocr_free_recog_result return %d\n", errCode );
		hci_ocr_session_stop( nSessionId );
		return false;
	}
	printf( "hci_ocr_free_recog_result success\n" );

	// ��ֹ OCR Session
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
	/******************************��ʼ��sysģ��********************************/
	HCI_ERR_CODE errCode = HCI_ERR_NONE;
	//���ļ�testdata/AccountInfo.txt��ȡ�ӽ�ͨ�����Ӧ���˺���Ϣ
	//strAccountInfo���磺"appKey=##,developerKey=###,cloudUrl=###"
	//�û�ʵ��ʹ��ʱ��ֱ��ʹ��һ���ַ�������
	string strAccountInfo;
	bool bRet = GetAccountInfo(strAccountInfo);
	if (!bRet)
	{
		printf("\ninput any char to quit and check!\n");
		getchar();
		return -1;
	}

	//��ȡ����key,����key������Ҫ���е�ʾ����֧
	string capkey;
	bRet = GetCapkey(capkey);
	if (!bRet)
	{
		printf("\ninput any char to quit and check!\n");
		getchar();
		return -1;
	}

	// ��ʼ�� ���� ϵͳ
	//	appKey,developerKey: ��ͨ�����Ӧ���˺���Ϣ
	//	logFileSize,logLevel,logFilePath,logFileCount: ��־�������
	//	authPath: ��д��Ȩ�ļ����û���Ϣ�ļ���·��
	//	autoCloudAuth: �Ƿ��Զ�����ƶ���Ȩ
	//	cloudUrl: �ƶ���Ȩ��ַ

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

	//�����Ȩ,��Ҫʱ���ƶ�������Ȩ���˴���Ҫע����ǣ��������ֻ��ͨ�������Ȩ�Ƿ�������ж��Ƿ���Ҫ����
	//��ȡ��Ȩ����������ڿ������Թ����У���Ȩ�˺�������������sdk���������뵽hci_init�����authPath·����
	//ɾ��HCI_AUTH�ļ��������޷���ȡ�µ���Ȩ�ļ����Ӷ��޷�ʹ������������������
	if (!CheckAndUpdataAuth())
	{
		hci_release();
		printf("CheckAndUpdateAuth failed");
		getchar();
		return -1;
	}

	//�ж�capkey�Ƿ����
	if (!IsCapkeyEnable(capkey))
	{
		printf("capkey [%s] is not enable\n",capkey.c_str());
		//����ȷ�Ĺ���д��capkey������capkey�����ã����Գ����Ե���һ��hci_check_auth()����
		hci_release();
		getchar();
		return -1;
	}

	/******************************��ʼ��OCRģ��*******************************/

	// ��ʼ�� OCR ���������ʹ�ñ���������������������
	// ��ֻʹ���ƶ�������������ÿ���ΪNULL��""
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


	///******************************ʶ��*******************************/
	if (capkey.find("template") != string::npos)
 	{
 		//ģ��ʶ��
		TemplateRecog(capkey, IDCARD_JPG_FILE,IDCARD_TEMPLATE_FILE);
 	}
	else if (capkey.find("bizcard") != string::npos)
 	{
 		//��Ƭʶ��
 		BizcardRecog(capkey, BIZCARD_JPG_FILE);
 	}
 	else
	{
		//�Զ�ʶ�𣬲�������б�����Ͱ��������ֱ��ʶ��
		AutoRecog(capkey, JPG_FILE);
		printf( "\n��Enter�����и߼�ʶ��\n" );
		getchar();

		// �߼�ʶ��,������ʶ�𡪡��ȵ�����бУ������������ӿڣ�Ȼ����ʶ��ӿ��д�����������������ʶ��
		// ����бУ���Ͱ����������󣬿�����תͼƬ�����ֶ��޸İ�������Ľ����Ȼ���ٽ���ʶ��
		// ����ʶ��ӿڴ�������������������Զ�ʶ����ʹ��autoDeskew��autoLayout������
		// ��б�����Ͱ�������Ǳ�����õĽӿ�
		AdvancedRecog(capkey, JPG_FILE);
	}


	/******************************����ʼ��OCRģ��*******************************/
	hci_ocr_release();

	/******************************����ʼ��sysģ��********************************/
	hci_release();

	printf("Press Enter to Exit");
	getchar();

	return 0;
}


