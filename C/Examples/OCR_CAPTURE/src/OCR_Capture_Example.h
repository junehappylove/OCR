
// OCR_Capture_Example.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COCR_Capture_ExampleApp:
// �йش����ʵ�֣������ OCR_Capture_Example.cpp
//

class COCR_Capture_ExampleApp : public CWinAppEx
{
public:
	COCR_Capture_ExampleApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedGetpicturefold();
};

extern COCR_Capture_ExampleApp theApp;