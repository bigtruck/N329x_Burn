
// N329x_BurnDlg.h : ͷ�ļ�
//

#pragma once


// CN329x_BurnDlg �Ի���
class CN329x_BurnDlg : public CDialogEx
{
// ����
public:
	CN329x_BurnDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_N329X_BURN_DIALOG };
#endif

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
