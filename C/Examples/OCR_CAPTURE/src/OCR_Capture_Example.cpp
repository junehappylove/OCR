
// OCR_Capture_Example.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "OCR_Capture_Example.h"
#include "OCR_Capture_ExampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COCR_Capture_ExampleApp

BEGIN_MESSAGE_MAP(COCR_Capture_ExampleApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
    ON_BN_CLICKED(GetPictureFold, &COCR_Capture_ExampleApp::OnBnClickedGetpicturefold)
END_MESSAGE_MAP()


// COCR_Capture_ExampleApp ����

COCR_Capture_ExampleApp::COCR_Capture_ExampleApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� COCR_Capture_ExampleApp ����

COCR_Capture_ExampleApp theApp;


// COCR_Capture_ExampleApp ��ʼ��

BOOL COCR_Capture_ExampleApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	COCR_Capture_ExampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

void COCR_Capture_ExampleApp::OnBnClickedGetpicturefold()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}
