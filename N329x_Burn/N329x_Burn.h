
// N329x_Burn.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CN329x_BurnApp: 
// �йش����ʵ�֣������ N329x_Burn.cpp
//

class CN329x_BurnApp : public CWinApp
{
public:
	CN329x_BurnApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CN329x_BurnApp theApp;