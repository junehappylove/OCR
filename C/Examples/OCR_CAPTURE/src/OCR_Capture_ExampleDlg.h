
// OCR_Capture_ExampleDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "hci_sys.h"
#include "hci_ocr.h"
#include "hci_ocr_capture.h"
#include "AccountInfo.h"

#ifdef __LINUX__
#include <string.h>
#include <stdio.h>
#define _tmain main
#else
#include <time.h>
#include <tchar.h>
#endif


// COCR_Capture_ExampleDlg �Ի���
class COCR_Capture_ExampleDlg : public CDialog
{
// ����
public:
	COCR_Capture_ExampleDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OCR_CAPTURE_EXAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDevicerecog();
	afx_msg void OnBnClickedPicturecut();
	afx_msg void OnBnClickedPicturenocut();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedTemplate();
	afx_msg void OnBnClickedChoosepicture();
	afx_msg void OnBnClickedGetpicturefold();

	void Show_picture(CString picture);

public:
	string m_capkey;
	CString m_strMessage;

private:
	enum RecogType
	{
		kDeviceRecog = 0,
		kFileRecog,
		kFolderRecog,
	};

    CString m_folder_path;
	string m_file_path;
    string m_scan_image_path;
	string m_template_path;
    bool m_isNeedCutEdge;
	RecogType m_recogType;

};
