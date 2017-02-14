
// N329x_BurnDlg.h : 头文件
//

#pragma once


// CN329x_BurnDlg 对话框
class CN329x_BurnDlg : public CDialogEx
{
// 构造
public:
	CN329x_BurnDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_N329X_BURN_DIALOG };
#endif

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
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void ScanAllDisk(void);
	static UINT WINAPI MainThread(LPVOID lParam);
	CWinThread* m_TFunWriteData;
	HANDLE m_HdSDCard;
	void ViewLastError(LPTSTR lpMsg);
	afx_msg LRESULT OnUserThreadend(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBuopenfile();
	afx_msg void OnBnClickedBuaf();
	afx_msg void OnBnClickedBuburn();
};
