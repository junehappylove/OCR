
// OCR_Capture_ExampleDlg.h : 头文件
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


// COCR_Capture_ExampleDlg 对话框
class COCR_Capture_ExampleDlg : public CDialog
{
// 构造
public:
	COCR_Capture_ExampleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_OCR_CAPTURE_EXAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
