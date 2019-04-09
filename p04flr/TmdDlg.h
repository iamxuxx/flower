// TmdDlg.h : header file
//

#if !defined(AFX_TMDDLG_H__3D1B8877_4DA8_41E2_9BEC_B4E85CFBC989__INCLUDED_)
#define AFX_TMDDLG_H__3D1B8877_4DA8_41E2_9BEC_B4E85CFBC989__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTmdDlg dialog

class CTmdDlg : public CDialog
{
// Construction
public:
	CTmdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTmdDlg)
	enum { IDD = IDD_TMD_DIALOG };
	CComboBox	m_dot;
	CComboBox	m_cmb;
	CListBox	m_lst;
	float	m_id;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTmdDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChangeEdit1();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMDDLG_H__3D1B8877_4DA8_41E2_9BEC_B4E85CFBC989__INCLUDED_)
