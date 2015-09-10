
// OCR_Capture_ExampleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OCR_Capture_Example.h"
#include "OCR_Capture_ExampleDlg.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include "CodeConvert.h"
#include "tinyxml.h"
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
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
#else
#define LOG_FILE_PATH "/SDMMC/bin"
#define AUTH_FILE_PATH "/SDMMC/bin"
#define DATA_PATH "/SDMMC/data"
#endif

#define TEMPLATE_FILE "../../data/IDCard.xml"

using namespace std;


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
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    else if( errCode ==	HCI_ERR_SYS_AUTHFILE_INVALID )
    {
        errCode = hci_check_auth();
        if( errCode == HCI_ERR_NONE )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
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
        return false;
    }

    //�жϴ����capbility_array �е������Ƿ��ڿ��������б���
    bool is_capkey_enable = false;
    for (size_t capbility_index = 0; capbility_index < capbility_list.uiItemCount; capbility_index++)
    {
        if (capkey == string(capbility_list.pItemList[capbility_index].pszCapKey))
        {
            is_capkey_enable = true;
            break;
        }
    }
    //�ͷſ��������б�
    hci_free_capability_list(&capbility_list);
    return is_capkey_enable;
}

void EchoCell(const TiXmlElement* pElem,const string &key,CString &strMessage)
{
    unsigned char* gbk_name = NULL;
    UTF8ToGBK((unsigned char *)pElem->Attribute(key.c_str()), &gbk_name);
    strMessage.AppendFormat("%s:\r\n",gbk_name);
    free(gbk_name);
}


void EchoElement(const TiXmlElement* pElem,const string &subTag,const string &key,const string &value,CString &strMessage)
{
	const TiXmlElement* result = pElem->FirstChildElement(subTag.c_str());
	//������
	unsigned char* gbk_name = NULL;
	unsigned char* gbk_result = NULL;
	UTF8ToGBK((unsigned char *)pElem->Attribute(key.c_str()), &gbk_name);
	UTF8ToGBK((unsigned char *)result->Attribute(value.c_str()), &gbk_result);
	strMessage.AppendFormat("%s:\t%s\r\n",gbk_name,gbk_result);
	free(gbk_name);
	free(gbk_result);
}

////////////////////////////////////////////////////////////
void HCIAPI recogFinish(
                        _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                        _MUST_ _IN_ OCR_RECOG_RESULT *pRecogResult,
                        _OPT_ _IN_ void * pUsrParam )
{

    COCR_Capture_ExampleDlg *dlg = (COCR_Capture_ExampleDlg*)pUsrParam;
    
	if (dlg->m_capkey.find(".bizcard") != string::npos)
	{
		if (pRecogResult->pTextBuf == NULL)
		{
			CString strTmp;
			strTmp.AppendFormat("%s", "���ص�ͼƬ����ʶ��ʧ��");
			AfxMessageBox(strTmp);
			return;
		}
		string strXML = pRecogResult->pTextBuf;
		TiXmlDocument doc;
		doc.Parse( strXML.c_str(), 0, TIXML_ENCODING_UTF8 );

		TiXmlElement* rootElement = doc.RootElement();
		if(rootElement != NULL )
		{
			for( const TiXmlElement* pElem = rootElement->FirstChildElement( "cell" );pElem != NULL;pElem = pElem->NextSiblingElement( "cell" ) )
			{

				if (pElem->FirstChildElement("cell"))
				{

					for( const TiXmlElement* pSubElem = pElem->FirstChildElement( "cell" );pSubElem != NULL;pSubElem = pSubElem->NextSiblingElement( "cell" ) )
					{
						EchoElement(pSubElem,"result","item","text",dlg->m_strMessage);
					}
				}
				EchoElement(pElem,"result","item","text",dlg->m_strMessage);
			}
		}
		else
		{
			dlg->m_strMessage.AppendFormat("result format error!");
		}
	}
	else if(dlg->m_capkey.find(".local.template")  != string::npos)
	{
		string strXML = pRecogResult->pTextBuf;
		TiXmlDocument doc;
		doc.Parse( strXML.c_str(), 0, TIXML_ENCODING_UTF8 );

		TiXmlElement* rootElement = doc.RootElement();
		if(rootElement != NULL )
		{
			const TiXmlElement* form = rootElement->FirstChildElement( "form" );
			const TiXmlElement* page = form->FirstChildElement( "page" );
			if( page != NULL )
			{
				for( const TiXmlElement* pElem = page->FirstChildElement( "cell" );pElem != NULL;pElem = pElem->NextSiblingElement( "cell" ) )
				{
					if (pElem->FirstChildElement("cell"))
					{
                        EchoCell(pElem,"name",dlg->m_strMessage);
						for( const TiXmlElement* pSubElem = pElem->FirstChildElement( "cell" );pSubElem != NULL;pSubElem = pSubElem->NextSiblingElement( "cell" ) )
						{
							dlg->m_strMessage.AppendFormat("\t");
							EchoElement(pSubElem,"result","name","result",dlg->m_strMessage);
						}
					}
                    else
                    {
                        EchoElement(pElem,"result","name","result",dlg->m_strMessage);
                    }
				}
			}
		}
		else
		{
			dlg->m_strMessage.AppendFormat("result format error!");
		}
	}
	else
	{
		//������
		unsigned char* szGbkResult;
		UTF8ToGBK((unsigned char *)pRecogResult->pTextBuf, &szGbkResult);
		string strGBK = (char*)szGbkResult;
		free(szGbkResult);
		//�޸Ļ��з�\nΪ\r\n,ʹ���м����MIDTEMP
		int nPos = strGBK.find("\n");
		while(nPos != string::npos)
		{
			strGBK.replace(nPos,strlen("\n"),"MIDTEMP");
			nPos = strGBK.find("\n");
		}

		nPos = strGBK.find("MIDTEMP");
		while(nPos != string::npos)
		{
			strGBK.replace(nPos,strlen("MIDTEMP"),"\r\n");
			nPos = strGBK.find("MIDTEMP");
		}
		dlg->m_strMessage.AppendFormat("%s",strGBK.c_str());	
	}
    dlg->SetDlgItemText(RecogResult,dlg->m_strMessage);
	dlg->UpdateWindow();
}


////////////////////////////////////////////////////////////
void HCIAPI eventStateChange(
                             _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                             _OPT_ _IN_ void * pUsrParam )
{
}

////////////////////////////////////////////////////////////
void HCIAPI captureCapturing(
                             _MUST_ _IN_ void * pszImageData,
                             _MUST_ _IN_ unsigned int uiImageLen,
                             _MUST_ _IN_ bool bHasConers,
                             _OPT_ _IN_ OCR_CORNERS_RESULT *pCornersResult,
                             _OPT_ _IN_ void * pUsrParam
                             )
{
}

////////////////////////////////////////////////////////////
void HCIAPI captureEventError(
                              _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                              _MUST_ _IN_ int eErrorCode,
                              _OPT_ _IN_ void * pUsrParam )
{
	
    if ( eErrorCode == HCI_ERR_LOCAL_LIB_MISSING )
    {
		AfxMessageBox("ȱ�ٱ�����Դ");
    }
	else
	{
		AfxMessageBox("ʶ��ͼƬʧ��",MB_OK,NULL);
	}

}

////////////////////////////////////////////////////////////

bool DeleteDirectory(const char *DirName)//��ɾ��DeleteDirectory("c:\\aaa") 
{
    CFileFind tempFind;
    char tempFileFind[MAX_PATH];
    sprintf(tempFileFind,"%s\\*.*",DirName);
    BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
    while(IsFinded)
    {
        IsFinded=(BOOL)tempFind.FindNextFile();
        if(!tempFind.IsDots())
        {
            char foundFileName[MAX_PATH];
            strcpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
            if(tempFind.IsDirectory())
            {
                char tempDir[MAX_PATH];
                sprintf(tempDir,"%s\\%s",DirName,foundFileName);
                DeleteDirectory(tempDir);
            }
            else
            {
                char tempFileName[MAX_PATH];
                sprintf(tempFileName,"%s\\%s",DirName,foundFileName);
                DeleteFile(tempFileName);
            }
        }
    }
    tempFind.Close();
    if(!RemoveDirectory(DirName))
    {
        //MessageBox(0,"ɾ��Ŀ¼ʧ�ܣ�","������Ϣ",MB_OK);//����û���ҵ��ļ���,ɾ��ʧ�ܣ��ɰѴ˾�ɾ��
        return FALSE;
    }
    return TRUE;
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// COCR_Capture_ExampleDlg �Ի���




COCR_Capture_ExampleDlg::COCR_Capture_ExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COCR_Capture_ExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COCR_Capture_ExampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COCR_Capture_ExampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &COCR_Capture_ExampleDlg::OnBnClickedOk)
    ON_BN_CLICKED(ChoosePicture, &COCR_Capture_ExampleDlg::OnBnClickedChoosepicture)
    ON_BN_CLICKED(GetPictureFold, &COCR_Capture_ExampleDlg::OnBnClickedGetpicturefold)
    ON_BN_CLICKED(DEVICERECOG, &COCR_Capture_ExampleDlg::OnBnClickedDevicerecog)
    ON_BN_CLICKED(PictureCut, &COCR_Capture_ExampleDlg::OnBnClickedPicturecut)
    ON_BN_CLICKED(PictureNoCut, &COCR_Capture_ExampleDlg::OnBnClickedPicturenocut)
    ON_BN_CLICKED(IDCANCEL, &COCR_Capture_ExampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDTemplate, &COCR_Capture_ExampleDlg::OnBnClickedTemplate)
END_MESSAGE_MAP()


// COCR_Capture_ExampleDlg ��Ϣ�������

BOOL COCR_Capture_ExampleDlg::OnInitDialog()
{
    m_isNeedCutEdge=false;
	CDialog::OnInitDialog();
    // ��ʼ������sdk��sysģ��
    HCI_ERR_CODE errCode = HCI_ERR_NONE;
    //���ļ�testdata/AccountInfo.txt��ȡ�ӽ�ͨ�����Ӧ���˺���Ϣ
    //strAccountInfo���磺"appKey=##,developerKey=###,cloudUrl=###"
    //�û�ʵ��ʹ��ʱ��ֱ��ʹ��һ���ַ�������
    string strAccountInfo;
    bool bRet = GetAccountInfo(strAccountInfo);
    if (!bRet)
    {
        MessageBox("����AccountInfo.txt��д�˺ż�����");
        return -1;
    }

    //��ȡ����key,����key������Ҫ���е�ʾ����֧
    
    bRet = GetCapkey(m_capkey);
    if (!bRet)
    {
		MessageBox("����AccountInfo.txt��д����");
        return -1;
    }

	SetDlgItemText(IDC_CAPKEY,m_capkey.c_str());
	if(m_capkey.find("local.template") == string::npos)
	{
		GetDlgItem(IDTemplate)->EnableWindow(false);
		GetDlgItem(UserTemplatePath)->EnableWindow(false);
	}

    char pchPathName[256] = {0};
    GetModuleFileName(NULL, pchPathName, 256);
    string path = string(pchPathName);
    int pos = path.rfind("\\");
    path = path.substr(0,pos); 
    m_template_path.clear();
    
    // ��ʼ�� ���� ϵͳ
    //	appKey,developerKey: ��ͨ�����Ӧ���˺���Ϣ
    //	logFileSize,logLevel,logFilePath,logFileCount: ��־�������
    //	authPath: ��д��Ȩ�ļ����û���Ϣ�ļ���·��
    //	autoCloudAuth: �Ƿ��Զ�����ƶ���Ȩ
    //	cloudUrl: �ƶ���Ȩ��ַ
    const char * pszConfig = "";
    unsigned int uiImageDataSize = 0;

    string authPath = path + string("\\") + AUTH_FILE_PATH;
    string logFilePath = path + string("\\") + LOG_FILE_PATH;
    string strConfig = strAccountInfo
        + string("logFileSize=500,logLevel=5,logFilePath=") + logFilePath +  string(",logFileCount=10,")
        + string("authPath=") +  authPath + string(",autoCloudAuth=no");
    pszConfig = strConfig.c_str();

    // ����sys��ʼ��
    HCI_ERR_CODE nRet = hci_init(pszConfig);
    if( nRet != HCI_ERR_NONE )
    {
		CString strTmp;
		strTmp.AppendFormat("hci_init failed %d",nRet);
        MessageBox(strTmp);
        return -1;
    }
    //�����Ȩ,��Ҫʱ���ƶ�������Ȩ���˴���Ҫע����ǣ��������ֻ��ͨ�������Ȩ�Ƿ�������ж��Ƿ���Ҫ����
    //��ȡ��Ȩ����������ڿ������Թ����У���Ȩ�˺�������������sdk���������뵽hci_init�����authPath·����
    //ɾ��HCI_AUTH�ļ��������޷���ȡ�µ���Ȩ�ļ����Ӷ��޷�ʹ������������������
    if (!CheckAndUpdataAuth())
    {
        MessageBox("�����Ȩʧ��");
        hci_release();
        return -1;
    }
    //�ж�capkey�Ƿ����
    if (!IsCapkeyEnable(m_capkey))
    {
        //MessageBox("capkey [%s] is not enable\n",capkey.c_str());
        //����ȷ�Ĺ���д��capkey������capkey�����ã����Գ����Ե���һ��hci_check_auth()����
        hci_release();
        return -1;
    }
    CAPTURE_CALLBACK_PARAM param = {};
    param.pfnRecogFinish = recogFinish;
    param.pvRecogFinishUsrParam = this;
    param.pfnStateChange = eventStateChange;
    param.pfnError = captureEventError;
    param.pfnCapturing = captureCapturing;
    string ocr_data_path = path + "\\" + DATA_PATH;
	m_scan_image_path = ocr_data_path;
    string init_config = "dataPath=" + ocr_data_path;
    init_config += ",initCapkeys=";
    init_config += m_capkey;
    CAPTURE_ERR_CODE capture_err;
    capture_err=hci_ocr_capture_init(init_config.c_str(), &param);
    if (capture_err!=0)
    {
        MessageBox("hci_ocr_capture_init failed");
        return -1;
    }
    CButton* radio=(CButton*)GetDlgItem(PictureNoCut);
    radio->SetCheck(1);

	m_recogType = kFileRecog;

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void COCR_Capture_ExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COCR_Capture_ExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        
		CDialog::OnPaint();
        //����windows���ڣ�������ⲽ���ã�ͼƬ��ʾ�����������
        CDialog::UpdateWindow();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR COCR_Capture_ExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COCR_Capture_ExampleDlg::OnBnClickedOk()
{   

    bool is_tempalte_recog = false;
    if (m_capkey.find("local.template")!=string::npos)
    {
        is_tempalte_recog = true;
    }

    CString strTmp;
    int templateId = -1;
    CAPTURE_ERR_CODE capture_err;
    //ģ��ʶ�����ģ��
    if (is_tempalte_recog)
    {
		if (m_template_path.empty())
		{
			strTmp.Format("û��ѡ��ģ���ļ�");
			AfxMessageBox(strTmp);
			return;
		}
        capture_err=hci_ocr_capture_load_template(m_template_path.c_str(), &templateId);
        if (capture_err!=0)
        {

            strTmp.Format("hci_ocr_capture_load_template failed");
            AfxMessageBox(strTmp);
            return;
        }
    }
    //��Ƭʶ��
    char templateID[128] = { 0 };
    sprintf(templateID, "%d", templateId);
    string strConf;
    if (m_capkey.find("local.template")!=string::npos)
    {
        strConf = string("capkey=")+m_capkey+string(",templateID=") + string(templateID)+string(",templateIndex=0, templatePageIndex=0");
    }
	else if (m_capkey.find("cloud.template") != string::npos)
    {
        strConf = string("capkey=")+m_capkey+string(",domain=idcard, templateIndex=0, templatePageIndex=0");
    }
	else
	{
		strConf = string("capkey=")+m_capkey;
	}

    if (m_isNeedCutEdge==true)
    {
        strConf+=string(",cutEdge=yes");
    }
    else
    {
        strConf+=string(",cutEdge=no");
    }
    //ѡ����ͼƬʶ��
    if (m_recogType == kFileRecog)
    {
		if(m_file_path.empty())
		{
			MessageBox("��ѡ��ͼƬ");
		}
		else
		{
			strConf+=string(",isFile=yes");
			int uiImageDataSize=0;
			capture_err=hci_ocr_capture_recog(strConf.c_str(),(unsigned char *)m_file_path.c_str(),uiImageDataSize);
			if (capture_err != CAPTURE_ERR_NONE)
			{
				return;
			}
		}
    }
   // ����ʶ��
    else if (m_recogType == kFolderRecog)
    {
        long Handle;
        struct _finddata_t FileInfo;
        CString TempPath=m_folder_path+CString("/");
		CString FolderTempPath = m_folder_path + "/*.jpg";
        if((Handle=_findfirst(FolderTempPath ,&FileInfo))==-1L)
        {
            MessageBox("û���ҵ�ƥ���ͼƬ");
        }
        else
        {
            strConf+=string(",isFile=yes");
            int uiImageDataSize=0;
            
            CString pitcure =TempPath+FileInfo.name;
			UpdateData(false); //����·����������
			if (access(pitcure,0)!=-1)
			{
				Show_picture(pitcure);
			}

			m_strMessage.AppendFormat("ʶ���ļ�: %s\r\n", pitcure.GetString());
            capture_err=hci_ocr_capture_recog(strConf.c_str(),(unsigned char *)(LPSTR)(LPCTSTR)pitcure,uiImageDataSize);

            while(_findnext(Handle,&FileInfo)==0)
            {
                CString pitcure=TempPath+FileInfo.name;
				UpdateData(false); //����·����������
				if (access(pitcure,0)!=-1)
				{
					Show_picture(pitcure);
				}   
				m_strMessage.AppendFormat("\r\nʶ���ļ�: %s\r\n", pitcure.GetString());
                capture_err=hci_ocr_capture_recog(strConf.c_str(),(unsigned char *)(LPSTR)(LPCTSTR)pitcure,uiImageDataSize);
            }
            _findclose(Handle);
        }
		MessageBox("����ʶ�������");
    }
    if (is_tempalte_recog && templateId !=-1)
    {
        hci_ocr_capture_unload_template(templateId); 
    }
}

//��ʾͼƬ����
void COCR_Capture_ExampleDlg::Show_picture(CString picture)
{
    if(picture=="")        //�ж�ͼƬ·���Ƿ����
    {
        return;
    }
    CImage myImage;
    myImage.Load(picture);
    CWnd *pWnd = GetDlgItem(IDC_SHOWPIC);
    CDC *pDC = pWnd->GetDC();
	CRect rect;
    pWnd->GetClientRect(&rect);
	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	if (!myImage.IsNull())
	{
		myImage.Draw(pDC->m_hDC, rect);
	}
    ReleaseDC(pDC);
    myImage.Destroy();
}

void COCR_Capture_ExampleDlg::OnBnClickedChoosepicture()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CFileDialog cfd(true,_T(".jpg"),NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,_T("Executable   Files   (*.*)|*.*|All   Files   (*.*)|*.*||"),this);     //ѡ��ͼƬ��Լ��
    if(cfd.DoModal()!=IDOK)     //�ж��Ƿ���ͼƬ
    { 
        return;
    }
    m_recogType = kFileRecog;
	m_strMessage.Empty();
    CString pitcure = cfd.GetPathName();      //���ͼƬ·��
	m_file_path = pitcure.GetString();
    this->SetDlgItemText(UserPicturePath,pitcure);
    UpdateData(false);              //����·����������
    //������ʾͼƬ����
    if (access(pitcure,0)!=-1)
    {
        Show_picture(pitcure);
    }                 
}
static CString ChooseFolder()
{
    TCHAR			szFolderPath[MAX_PATH] = {0};
    CString			strFolderPath = TEXT("");
    BROWSEINFO		sInfo;
    ::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
    sInfo.pidlRoot   = 0;
    sInfo.lpszTitle   = _T("��ѡ��һ���ļ��У�");
    sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    sInfo.lpfn     = NULL;

    // ��ʾ�ļ���ѡ��Ի���
    LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo); 
    if (lpidlBrowse != NULL)
    {
        // ȡ���ļ�����
        if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))  
        {
            strFolderPath = szFolderPath;
        }
    }
    if(lpidlBrowse != NULL)
    {
        ::CoTaskMemFree(lpidlBrowse);
    }
    return strFolderPath;
}


void COCR_Capture_ExampleDlg::OnBnClickedGetpicturefold()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
	m_recogType = kFolderRecog;
	m_strMessage.Empty();
    m_folder_path = ChooseFolder();
    this->SetDlgItemText(UserPictureFolder,m_folder_path);
    long Handle;
    struct _finddata_t FileInfo;
    CString TempPath=m_folder_path+CString("/");
    CString TemFoldPath=m_folder_path+CString("/*.jpg");
    if((Handle=_findfirst(TemFoldPath ,&FileInfo))!=-1L)
    {
        CString pitcture=TempPath+FileInfo.name;
        if (access(pitcture,0)!=-1)
        {
            Show_picture(pitcture);
        }    
    }
    _findclose(Handle);
}


void COCR_Capture_ExampleDlg::OnBnClickedDevicerecog()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������

    SetDlgItemText(UserPicturePath,"");
    SetDlgItemText(UserPictureFolder,"");
    m_recogType = kDeviceRecog;
	m_strMessage.Empty();
    int pos=m_scan_image_path.find("tmp");  
    string DelPath=m_scan_image_path.substr(0,pos)+string("/tmp");
    DeleteDirectory(DelPath.c_str());
    bool is_tempalte_recog = false;
    if (m_capkey.find("local.template")!=string::npos)
    {
        is_tempalte_recog = true;
    }
    CString strTmp;
    int templateId = -1;
    CAPTURE_ERR_CODE capture_err;
    //ģ��ʶ�����ģ��
    if (is_tempalte_recog)
    {
		if (m_template_path.empty())
		{
			strTmp.Format("û��ѡ��ģ���ļ�");
			AfxMessageBox(strTmp);
			return;
		}
        capture_err=hci_ocr_capture_load_template(m_template_path.c_str(), &templateId);
        if (capture_err!=0)
        {
            strTmp.Format("load template failed");
            AfxMessageBox(strTmp);
            return;
        }
    }
    //��Ƭʶ��
    char templateID[128] = { 0 };
    sprintf(templateID, "%d", templateId);
    string strConf;
    if (m_capkey.find("local.template")!=string::npos)
    {
        strConf = string("capkey=")+m_capkey+string(",templateID=") + string(templateID)+string(",templateIndex=0, templatePageIndex=0");
    }
    else
    {
        strConf = string("capkey=")+m_capkey;
    }
    if (m_isNeedCutEdge==true)
    {
        strConf+=string(",cutEdge=yes");
    }
    else
    {
        strConf+=string(",cutEdge=no");
    }
	strConf+=string(",showSettingUi=yes");
    //ɨ��ʶ��
    if(m_recogType == kDeviceRecog)
    {
        capture_err= hci_ocr_capture_start(strConf.c_str());
        if (capture_err!=0)
        {
			CString strTmp;
			strTmp.AppendFormat("hci_ocr_capture_start failed: %d",capture_err);
			MessageBox(strTmp);
			return;
        }
    }   
    if (is_tempalte_recog && templateId !=-1)
    {
        hci_ocr_capture_unload_template(templateId); 
    }  
    pos=m_scan_image_path.find("tmp");
    string datapath=m_scan_image_path.substr(0,pos);
    datapath +=string("tmp");
    long Handle;
    struct _finddata_t FileInfo;
    
    datapath += string("/*.tiff");
    if((Handle=_findfirst(datapath.c_str() ,&FileInfo))!=-1L)
    {
		string tmppath= datapath + string("/")+string(FileInfo.name);
        CString picture= tmppath.c_str();
        if (access(picture,0)!=-1)
        {
            //����·����������
            UpdateData(false);
            Show_picture(picture);
        }    
    }
    _findclose(Handle);
    
}

void COCR_Capture_ExampleDlg::OnBnClickedPicturecut()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_isNeedCutEdge=true;

}

void COCR_Capture_ExampleDlg::OnBnClickedPicturenocut()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_isNeedCutEdge=false;
}

void COCR_Capture_ExampleDlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    hci_ocr_capture_release();
    hci_release();
    OnCancel();
}

void COCR_Capture_ExampleDlg::OnBnClickedTemplate()
{
	CFileDialog cfd(true,_T(".xml"),NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,_T("Executable   Files   (*.xml)|*.xml|All   Files   (*.*)|*.*||"),this);     //ѡ��ͼƬ��Լ��
	if(cfd.DoModal()!=IDOK)     //�ж��Ƿ���ͼƬ
	{ 
		return;
	}
	m_template_path = cfd.GetPathName();      //���ͼƬ·��
	this->SetDlgItemText(UserTemplatePath,cfd.GetPathName());
	UpdateData(false);              //����·����������            
}
