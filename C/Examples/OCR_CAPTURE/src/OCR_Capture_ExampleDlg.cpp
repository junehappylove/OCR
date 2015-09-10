
// OCR_Capture_ExampleDlg.cpp : 实现文件
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
//以下路径可根据需要进行修改
//账号文件路径在AccountInfo.h中定义
#ifndef _WIN32_WCE
//日志文件保存路径
#define LOG_FILE_PATH "../../bin/win_x86"
//授权文件保存路径
#define AUTH_FILE_PATH "../../bin/win_x86"
//资源文件路径
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
    //获取一下授权中全部可用能力列表
    CAPABILITY_LIST capbility_list;
    HCI_ERR_CODE errCode = hci_get_capability_list( NULL, &capbility_list);
    if (errCode != HCI_ERR_NONE)
    {
        return false;
    }

    //判断传入的capbility_array 中的能力是否在可用能力列表中
    bool is_capkey_enable = false;
    for (size_t capbility_index = 0; capbility_index < capbility_list.uiItemCount; capbility_index++)
    {
        if (capkey == string(capbility_list.pItemList[capbility_index].pszCapKey))
        {
            is_capkey_enable = true;
            break;
        }
    }
    //释放可用能力列表
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
	//输出结果
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
			strTmp.AppendFormat("%s", "加载的图片错误，识别失败");
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
		//输出结果
		unsigned char* szGbkResult;
		UTF8ToGBK((unsigned char *)pRecogResult->pTextBuf, &szGbkResult);
		string strGBK = (char*)szGbkResult;
		free(szGbkResult);
		//修改换行符\n为\r\n,使用中间变量MIDTEMP
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
		AfxMessageBox("缺少本地资源");
    }
	else
	{
		AfxMessageBox("识别图片失败",MB_OK,NULL);
	}

}

////////////////////////////////////////////////////////////

bool DeleteDirectory(const char *DirName)//如删除DeleteDirectory("c:\\aaa") 
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
        //MessageBox(0,"删除目录失败！","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
        return FALSE;
    }
    return TRUE;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// COCR_Capture_ExampleDlg 对话框




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


// COCR_Capture_ExampleDlg 消息处理程序

BOOL COCR_Capture_ExampleDlg::OnInitDialog()
{
    m_isNeedCutEdge=false;
	CDialog::OnInitDialog();
    // 初始化灵云sdk的sys模块
    HCI_ERR_CODE errCode = HCI_ERR_NONE;
    //从文件testdata/AccountInfo.txt获取从捷通分配的应用账号信息
    //strAccountInfo形如："appKey=##,developerKey=###,cloudUrl=###"
    //用户实际使用时可直接使用一个字符串代替
    string strAccountInfo;
    bool bRet = GetAccountInfo(strAccountInfo);
    if (!bRet)
    {
        MessageBox("请在AccountInfo.txt填写账号及能力");
        return -1;
    }

    //获取能力key,能力key决定了要运行的示例分支
    
    bRet = GetCapkey(m_capkey);
    if (!bRet)
    {
		MessageBox("请在AccountInfo.txt填写能力");
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
    
    // 初始化 灵云 系统
    //	appKey,developerKey: 捷通分配的应用账号信息
    //	logFileSize,logLevel,logFilePath,logFileCount: 日志相关配置
    //	authPath: 读写授权文件和用户信息文件的路径
    //	autoCloudAuth: 是否自动检测云端授权
    //	cloudUrl: 云端授权地址
    const char * pszConfig = "";
    unsigned int uiImageDataSize = 0;

    string authPath = path + string("\\") + AUTH_FILE_PATH;
    string logFilePath = path + string("\\") + LOG_FILE_PATH;
    string strConfig = strAccountInfo
        + string("logFileSize=500,logLevel=5,logFilePath=") + logFilePath +  string(",logFileCount=10,")
        + string("authPath=") +  authPath + string(",autoCloudAuth=no");
    pszConfig = strConfig.c_str();

    // 灵云sys初始化
    HCI_ERR_CODE nRet = hci_init(pszConfig);
    if( nRet != HCI_ERR_NONE )
    {
		CString strTmp;
		strTmp.AppendFormat("hci_init failed %d",nRet);
        MessageBox(strTmp);
        return -1;
    }
    //检测授权,必要时到云端下载授权。此处需要注意的是，这个函数只是通过检测授权是否过期来判断是否需要进行
    //获取授权操作，如果在开发调试过程中，授权账号中新增了灵云sdk的能力，请到hci_init传入的authPath路径中
    //删除HCI_AUTH文件。否则无法获取新的授权文件，从而无法使用新增的灵云能力。
    if (!CheckAndUpdataAuth())
    {
        MessageBox("检查授权失败");
        hci_release();
        return -1;
    }
    //判断capkey是否可用
    if (!IsCapkeyEnable(m_capkey))
    {
        //MessageBox("capkey [%s] is not enable\n",capkey.c_str());
        //如正确的果填写了capkey，但是capkey不可用，可以尝试性的做一次hci_check_auth()操作
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

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COCR_Capture_ExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        
		CDialog::OnPaint();
        //更新windows窗口，如果无这步调用，图片显示还会出现问题
        CDialog::UpdateWindow();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
    //模板识别加载模板
    if (is_tempalte_recog)
    {
		if (m_template_path.empty())
		{
			strTmp.Format("没有选择模板文件");
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
    //名片识别
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
    //选择单张图片识别
    if (m_recogType == kFileRecog)
    {
		if(m_file_path.empty())
		{
			MessageBox("请选择图片");
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
   // 批量识别
    else if (m_recogType == kFolderRecog)
    {
        long Handle;
        struct _finddata_t FileInfo;
        CString TempPath=m_folder_path+CString("/");
		CString FolderTempPath = m_folder_path + "/*.jpg";
        if((Handle=_findfirst(FolderTempPath ,&FileInfo))==-1L)
        {
            MessageBox("没有找到匹配的图片");
        }
        else
        {
            strConf+=string(",isFile=yes");
            int uiImageDataSize=0;
            
            CString pitcure =TempPath+FileInfo.name;
			UpdateData(false); //更新路径公共变量
			if (access(pitcure,0)!=-1)
			{
				Show_picture(pitcure);
			}

			m_strMessage.AppendFormat("识别文件: %s\r\n", pitcure.GetString());
            capture_err=hci_ocr_capture_recog(strConf.c_str(),(unsigned char *)(LPSTR)(LPCTSTR)pitcure,uiImageDataSize);

            while(_findnext(Handle,&FileInfo)==0)
            {
                CString pitcure=TempPath+FileInfo.name;
				UpdateData(false); //更新路径公共变量
				if (access(pitcure,0)!=-1)
				{
					Show_picture(pitcure);
				}   
				m_strMessage.AppendFormat("\r\n识别文件: %s\r\n", pitcure.GetString());
                capture_err=hci_ocr_capture_recog(strConf.c_str(),(unsigned char *)(LPSTR)(LPCTSTR)pitcure,uiImageDataSize);
            }
            _findclose(Handle);
        }
		MessageBox("批量识别已完成");
    }
    if (is_tempalte_recog && templateId !=-1)
    {
        hci_ocr_capture_unload_template(templateId); 
    }
}

//显示图片函数
void COCR_Capture_ExampleDlg::Show_picture(CString picture)
{
    if(picture=="")        //判断图片路径是否存在
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
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog cfd(true,_T(".jpg"),NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,_T("Executable   Files   (*.*)|*.*|All   Files   (*.*)|*.*||"),this);     //选项图片的约定
    if(cfd.DoModal()!=IDOK)     //判断是否获得图片
    { 
        return;
    }
    m_recogType = kFileRecog;
	m_strMessage.Empty();
    CString pitcure = cfd.GetPathName();      //获得图片路径
	m_file_path = pitcure.GetString();
    this->SetDlgItemText(UserPicturePath,pitcure);
    UpdateData(false);              //更新路径公共变量
    //调用显示图片函数
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
    sInfo.lpszTitle   = _T("请选择一个文件夹：");
    sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    sInfo.lpfn     = NULL;

    // 显示文件夹选择对话框
    LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo); 
    if (lpidlBrowse != NULL)
    {
        // 取得文件夹名
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
    // TODO: 在此添加控件通知处理程序代码
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
    // TODO: 在此添加控件通知处理程序代码

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
    //模板识别加载模板
    if (is_tempalte_recog)
    {
		if (m_template_path.empty())
		{
			strTmp.Format("没有选择模板文件");
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
    //名片识别
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
    //扫描识别
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
            //更新路径公共变量
            UpdateData(false);
            Show_picture(picture);
        }    
    }
    _findclose(Handle);
    
}

void COCR_Capture_ExampleDlg::OnBnClickedPicturecut()
{
    // TODO: 在此添加控件通知处理程序代码
    m_isNeedCutEdge=true;

}

void COCR_Capture_ExampleDlg::OnBnClickedPicturenocut()
{
    // TODO: 在此添加控件通知处理程序代码
    m_isNeedCutEdge=false;
}

void COCR_Capture_ExampleDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    hci_ocr_capture_release();
    hci_release();
    OnCancel();
}

void COCR_Capture_ExampleDlg::OnBnClickedTemplate()
{
	CFileDialog cfd(true,_T(".xml"),NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,_T("Executable   Files   (*.xml)|*.xml|All   Files   (*.*)|*.*||"),this);     //选项图片的约定
	if(cfd.DoModal()!=IDOK)     //判断是否获得图片
	{ 
		return;
	}
	m_template_path = cfd.GetPathName();      //获得图片路径
	this->SetDlgItemText(UserTemplatePath,cfd.GetPathName());
	UpdateData(false);              //更新路径公共变量            
}
