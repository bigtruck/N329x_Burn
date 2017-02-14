
// N329x_BurnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "N329x_Burn.h"
#include "N329x_BurnDlg.h"
#include "afxdialogex.h"

#include <winioctl.h>
#include <Dbt.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MYMSG WM_USER+8



#define MYMSG_UPDATE_PROGRESS	0
#define MYMSG_DISABLE_CTRL		1
#define MYMSG_ENABLE_CTRL		2

typedef struct
{
	UINT32 msg;
	UINT32 param;
}__ST_MSG;


CHAR looderDataBuff[1024 * 1024 * 2];
DWORD looderFileSize;
CHAR appDataBuff[1024 * 1024 * 2];
DWORD appFileSize;
CHAR section33Buff[512];


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CN329x_BurnDlg 对话框



CN329x_BurnDlg::CN329x_BurnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_N329X_BURN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CN329x_BurnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CN329x_BurnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BULF, &CN329x_BurnDlg::OnBnClickedBuopenfile)
	ON_BN_CLICKED(IDC_BUAF, &CN329x_BurnDlg::OnBnClickedBuaf)
	ON_BN_CLICKED(IDC_BUBURN, &CN329x_BurnDlg::OnBnClickedBuburn)
	ON_MESSAGE(WM_MYMSG, OnUserThreadend)
END_MESSAGE_MAP()


// CN329x_BurnDlg 消息处理程序

BOOL CN329x_BurnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	ScanAllDisk();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CN329x_BurnDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CN329x_BurnDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CN329x_BurnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CN329x_BurnDlg::ScanAllDisk(void)
{
	TCHAR szBuf[100];
	DWORD len, i;
	UINT type;
	CString str;
	memset(szBuf, 0, 100);

	len = GetLogicalDriveStrings(sizeof(szBuf) / sizeof(TCHAR), szBuf);
	//m_CBDisk->ResetContent();
	CComboBox *m_CBDisk = (CComboBox *)GetDlgItem(IDC_CBDISK);
	m_CBDisk->ResetContent();

	for (i = 0; i<len; i++)
	{
		str = &szBuf[i];
		type = GetDriveType(str);
		if (type == DRIVE_REMOVABLE)
		{
			str.Delete(2, 1);
			m_CBDisk->AddString(str);
		}
	}
	i = m_CBDisk->GetCount();
	if (i > 0)
	{
		m_CBDisk->SetCurSel(i - 1);
	}
	else
	{
		m_CBDisk->SetCurSel(0);
	}
}

#define RDBUFF_SIZE	(1024*1024*2)
CHAR	rdBuffer[RDBUFF_SIZE], verBuffer[RDBUFF_SIZE];

UINT CN329x_BurnDlg::MainThread(LPVOID lParam)
{
#if 1
	BOOL bRtn;
	DWORD	outSize,looderSize,appSize;
//	UINT32 fileSize,sdIndex,dataIndex;
//	UINT  proVal;
//	__ST_MSG myMsg;

	CN329x_BurnDlg *myDlg = (CN329x_BurnDlg *)lParam;
	//::SendMessage(::AfxGetMainWnd()->m_hWnd, WM_MYMSG, 0, 0);

	looderSize = looderFileSize + (512 - looderFileSize % 512);
	appSize = appFileSize + (512 - appFileSize % 512);
	HANDLE m_HdSDCard;

	CString strDisk;
	CComboBox *m_CB = (CComboBox *)myDlg->GetDlgItem(IDC_CBDISK);
	m_CB->GetLBText(m_CB->GetCurSel(), strDisk);
	strDisk = "\\\\.\\" + strDisk;
	m_HdSDCard = CreateFile(strDisk, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (m_HdSDCard == INVALID_HANDLE_VALUE)
	{
		//MessageBox("打开SD卡出错", "错误", MB_OK | MB_ICONERROR);
		AfxMessageBox("打开SD卡出错1", MB_OK | MB_ICONERROR);
		goto __LAB_ERROR;
	}

	//DeviceIoControl(m_HdSDCard, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &outSize, NULL);
	if (DeviceIoControl(m_HdSDCard, IOCTL_DISK_DELETE_DRIVE_LAYOUT, NULL, 0, NULL, 0, &outSize, NULL) == FALSE)
	{
		AfxMessageBox("删除MBR信息错误", MB_OK | MB_ICONERROR);
		goto __LAB_ERROR;
	}
	CloseHandle(m_HdSDCard);
	Sleep(50);
	m_HdSDCard = CreateFile(strDisk, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (m_HdSDCard == INVALID_HANDLE_VALUE)
	{
		//MessageBox("打开SD卡出错", "错误", MB_OK | MB_ICONERROR);
		//AfxMessageBox("打开SD卡出错2", MB_OK | MB_ICONERROR);
		myDlg->ViewLastError("打开SD卡出错2");
		goto __LAB_ERROR;
	}


	UINT8 buff[512];
	memset(buff, 0, sizeof(buff));
	buff[0x1B0] = 0x20;
	buff[0x1B1] = 0x16;
	buff[0x1B2] = 0x09;
	buff[0x1B3] = 0x02;
	buff[0x1B4] = 0x41;
	buff[0x1BF] = 0x82;
	buff[0x1C0] = 0x03;
	buff[0x1C1] = 0x00;
	buff[0x1C2] = 0x0B;
	buff[0x1C3] = 0x0F;
	buff[0x1C4] = 0xFC;
	buff[0x1C5] = 0xD3;
	buff[0x1C6] = 0x00;
	buff[0x1C7] = 0x20;
	buff[0x1CB] = 0xE0;
	buff[0x1CC] = 0xEF;
	buff[0x1FE] = 0x55;
	buff[0x1FF] = 0xAA;

	SetFilePointer(m_HdSDCard, 0, NULL, FILE_BEGIN);
	bRtn = WriteFile(m_HdSDCard, buff, 512, &outSize, NULL);
	if (bRtn == FALSE)
	{
		//AfxMessageBox("写MBR失败", MB_OK | MB_ICONERROR);
		myDlg->ViewLastError("写MBR失败");
		goto __LAB_ERROR;
	}
	//写looder数据
	SetFilePointer(m_HdSDCard, 512, NULL, FILE_BEGIN);
	bRtn = WriteFile(m_HdSDCard, looderDataBuff, looderSize, &outSize, NULL);
	if (bRtn == FALSE)
	{
		//AfxMessageBox("写数据失败1", MB_OK | MB_ICONERROR);
		myDlg->ViewLastError("写数据1失败");
		goto __LAB_ERROR;
	}

	//写第33扇区
	SetFilePointer(m_HdSDCard, 33*512, NULL, FILE_BEGIN);
	bRtn = WriteFile(m_HdSDCard, section33Buff, 512, &outSize, NULL);
	if (bRtn == FALSE)
	{
		//AfxMessageBox("写数据失败2", MB_OK | MB_ICONERROR);
		myDlg->ViewLastError("写数据2失败");
		goto __LAB_ERROR;
	}

	//写APP数据
	SetFilePointer(m_HdSDCard, 34*512, NULL, FILE_BEGIN);
	bRtn = WriteFile(m_HdSDCard, appDataBuff, appSize, &outSize, NULL);
	if (bRtn == FALSE)
	{
		//AfxMessageBox("写数据失败3", MB_OK | MB_ICONERROR);
		myDlg->ViewLastError("写数据3失败");
		goto __LAB_ERROR;
	}

	//myMsg.msg = MYMSG_UPDATE_PROGRESS;
	//myMsg.param = 100;
	//::SendMessage(::AfxGetMainWnd()->m_hWnd, WM_MYMSG, 0, 0);
	AfxMessageBox("写入完成", MB_OK | MB_ICONERROR);
__LAB_ERROR:
	//CloseHandle(m_HdSDCard);
	CloseHandle(m_HdSDCard);

#endif
	AfxEndThread(0);
	return 0;
}

void CN329x_BurnDlg::ViewLastError(LPTSTR lpMsg)
{

	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();//获取错误代码

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	CString strErr;
	strErr.Format("%s-%s", lpMsg, lpMsgBuf);
	MessageBox(strErr, "错误", MB_OK | MB_ICONERROR);
	LocalFree(lpMsgBuf);
}



LRESULT CN329x_BurnDlg::OnUserThreadend(WPARAM wParam, LPARAM lParam)
{
	//MessageBox("hello");
	CProgressCtrl *proCtrl;

	__ST_MSG *Msg = (__ST_MSG *)lParam;

	switch (Msg->msg)
	{
	case MYMSG_UPDATE_PROGRESS:
		proCtrl = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
		proCtrl->SetPos(Msg->param);
		break;
	case MYMSG_DISABLE_CTRL:
		GetDlgItem(IDC_BULF)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUAF)->EnableWindow(FALSE);
		break;
	case MYMSG_ENABLE_CTRL:
		GetDlgItem(IDC_BULF)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUAF)->EnableWindow(TRUE);
		break;
	default:
		break;
	}

	return LRESULT();
}




LRESULT CN329x_BurnDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//WM_DEVICECHANGE，系统硬件改变发出的系统消息
	case WM_DEVICECHANGE:
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
		switch (wParam)
		{
		case WM_DEVICECHANGE:
			break;
		case DBT_DEVICEARRIVAL://DBT_DEVICEARRIVAL，设备检测结束，并且可以使用
		{
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)//逻辑卷
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				switch (lpdbv->dbcv_flags)
				{
				case 0:                //U盘
				{
					ScanAllDisk();
				}
				break;
				case DBTF_MEDIA:    //光盘
					break;
				}
			}
		}
		break;
		case DBT_DEVICEREMOVECOMPLETE://DBT_DEVICEREMOVECOMPLETE,设备卸载或者拔出
		{
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)//逻辑卷
			{
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				switch (lpdbv->dbcv_flags)
				{
				case 0:                //U盘
				{
					ScanAllDisk();
				}
				break;
				case DBTF_MEDIA:    //光盘

					break;
				}
			}
		}
		break;
		}
	}
	break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}





void CN329x_BurnDlg::OnBnClickedBuopenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString fPath = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		fPath = dlgFile.GetPathName();
		if (!fPath.IsEmpty())
		{
			GetDlgItem(IDC_EDLP)->SetWindowTextA(fPath);
		}
	}
}


void CN329x_BurnDlg::OnBnClickedBuaf()
{
	// TODO: 在此添加控件通知处理程序代码
	CString fPath = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		fPath = dlgFile.GetPathName();
		if (!fPath.IsEmpty())
		{
			GetDlgItem(IDC_EDAP)->SetWindowTextA(fPath);
		}
	}
}


typedef struct
{
	UINT16 RESERVE0;
	UINT16 num;
	UINT16 startSection;
	UINT16 endSection;
	UINT32 RESERVE1;
	UINT32 dataLength;
	UINT8  name[32];
}FILE_INFO;

typedef struct
{
	UINT32 mark0;
	UINT32 fileCount;
	UINT32 xxx;
	UINT32 mark1;
}SECTION33;



void CN329x_BurnDlg::OnBnClickedBuburn()
{
	// TODO: 在此添加控件通知处理程序代码
#if 1
	ULONG fileSize;
	CFile hFile ;
	CString strPath;
	CHAR *pLooderName,looderNameLen, *pAppName,appNameLen;
	CEdit *m_EDLP = (CEdit *)GetDlgItem(IDC_EDLP);
	CEdit *m_EDAP = (CEdit *)GetDlgItem(IDC_EDAP);
	if (m_EDLP->GetWindowTextLengthA() == 0)
	{
		MessageBox("请选择Looder文件", "错误", MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		m_EDLP->GetWindowTextA(strPath);
		hFile.Open(strPath, CFile::modeRead | CFile::typeBinary);
		if (!hFile)
		{
			MessageBox("打开Looder文件错误", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		fileSize = hFile.GetLength();
		if (fileSize> (1024*15) || fileSize == 0 )
		{
			MessageBox("Looder文件大小错误", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		CString str = hFile.GetFileName();
		pLooderName = str.GetBuffer();
		looderNameLen = str.GetLength();
		looderDataBuff[0] = 0xA5;
		looderDataBuff[1] = 0x5A;
		looderDataBuff[2] = 0x42;
		looderDataBuff[3] = 0x57;
		looderDataBuff[4] = 0x00;
		looderDataBuff[5] = 0x00;
		looderDataBuff[6] = 0x70;
		looderDataBuff[7] = 0x00;
		looderDataBuff[8]  = (UINT8)((fileSize >> 0) & 0xFF);
		looderDataBuff[9]  = (UINT8)((fileSize >> 8) & 0xFF);
		looderDataBuff[10] = (UINT8)((fileSize >> 16) & 0xFF);
		looderDataBuff[11] = (UINT8)((fileSize >> 24) & 0xFF);
		looderDataBuff[12] = 0x57;
		looderDataBuff[13] = 0x42;
		looderDataBuff[14] = 0x5A;
		looderDataBuff[15] = 0xA5;

		looderDataBuff[16] = 0x10;
		looderDataBuff[17] = 0x10;
		looderDataBuff[18] = 0x00;
		looderDataBuff[19] = 0x00;
		looderDataBuff[20] = 0x00;
		looderDataBuff[21] = 0x88;
		looderDataBuff[22] = 0x88;
		looderDataBuff[23] = 0x00;
		looderDataBuff[24] = 0x00;
		looderDataBuff[25] = 0x00;
		looderDataBuff[26] = 0x00;
		looderDataBuff[27] = 0x00;
		looderDataBuff[28] = 0x00;
		looderDataBuff[29] = 0x00;
		looderDataBuff[30] = 0x00;
		looderDataBuff[31] = 0x00;

		looderFileSize = fileSize + 32;
		hFile.Read(&looderDataBuff[32], fileSize);
		hFile.Close();
	}
	if (m_EDAP->GetWindowTextLengthA() == 0)
	{
		MessageBox("请选择App文件", "错误", MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		m_EDAP->GetWindowTextA(strPath);
		hFile.Open(strPath, CFile::modeRead | CFile::typeBinary);
		if (!hFile)
		{
			MessageBox("打开Looder文件错误", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		fileSize = hFile.GetLength();
		if (fileSize > (1024 * 1024) || fileSize == 0)
		{
			MessageBox("app文件大小错误", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		CString str = hFile.GetFileName();
		pAppName = str.GetBuffer();
		appNameLen = str.GetLength();
		appFileSize = fileSize;
		hFile.Read(&appDataBuff[0], fileSize);
		hFile.Close();
	}
	UINT16 endSection;
	endSection = looderFileSize / 512;
	if (looderFileSize % 512)
	{
		endSection++;
	}

	SECTION33 section33;
	memset(section33Buff, 0, 512);
	section33.mark0 = 0x574255AA;
	section33.mark1 = 0x57425963;
	section33.fileCount = 2;
	section33.xxx = 0x00200000;
	memcpy(&section33Buff[0], (UINT8 *)&section33, 16);

	FILE_INFO fileInfo;
	fileInfo.RESERVE0 = 0;
	fileInfo.RESERVE1 = 0;
	fileInfo.num = 3;
	fileInfo.startSection = 1;
	fileInfo.endSection = looderFileSize / 512;
	if (looderFileSize % 512)
	{
		fileInfo.endSection++;
	}
	fileInfo.dataLength = looderFileSize;
	memcpy(fileInfo.name, pLooderName, looderNameLen > 32 ? 32 : looderNameLen);
	memcpy(&section33Buff[16], &fileInfo, sizeof(fileInfo));

	fileInfo.num = 1;
	fileInfo.startSection = 34;
	fileInfo.endSection = appFileSize / 512;
	if (appFileSize % 512)
	{
		fileInfo.endSection++;
	}
	fileInfo.dataLength = appFileSize;
	memcpy(fileInfo.name, pAppName, appNameLen > 32 ? 32 : appNameLen);
	memcpy(&section33Buff[16 + sizeof(fileInfo)], &fileInfo, sizeof(fileInfo));


	CComboBox *m_CB = (CComboBox *)GetDlgItem(IDC_CBDISK);
	if (m_CB->GetCount() == 0)
	{
		MessageBox("请插入磁盘", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	//CString strDisk;
	//m_CB->GetLBText(m_CB->GetCurSel(), strDisk);
	//strDisk = "\\\\.\\" + strDisk;
	//m_HdSDCard = CreateFile(strDisk, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	//if (m_HdSDCard == INVALID_HANDLE_VALUE)
	//{
	//	MessageBox("打开SD卡出错","错误", MB_OK | MB_ICONERROR);
	//	return;
	//}
	//
	m_TFunWriteData = AfxBeginThread((AFX_THREADPROC)MainThread, this, THREAD_PRIORITY_NORMAL, 1024 * 1024 * 1, CREATE_SUSPENDED, NULL);
	m_TFunWriteData->ResumeThread();
#endif
}
