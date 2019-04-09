// Tmd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Tmd.h"
#include "TmdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTmdApp

BEGIN_MESSAGE_MAP(CTmdApp, CWinApp)
	//{{AFX_MSG_MAP(CTmdApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTmdApp construction

CTmdApp::CTmdApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTmdApp object

CTmdApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTmdApp initialization

BOOL CTmdApp::InitInstance()
{
	// Standard initialization

	CTmdDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
