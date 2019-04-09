// Tmd.h : main header file for the TMD application
//

#if !defined(AFX_TMD_H__A734BA97_BD97_4837_B5F9_C577466D737D__INCLUDED_)
#define AFX_TMD_H__A734BA97_BD97_4837_B5F9_C577466D737D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTmdApp:
// See Tmd.cpp for the implementation of this class
//

class CTmdApp : public CWinApp
{
public:
	CTmdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTmdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTmdApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TMD_H__A734BA97_BD97_4837_B5F9_C577466D737D__INCLUDED_)
