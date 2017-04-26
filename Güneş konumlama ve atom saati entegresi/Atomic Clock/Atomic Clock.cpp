// Atomic Clock.cpp

#include "stdafx.h"
#include "Atomic Clock.h"
#include "Atomic ClockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAtomicClockApp, CWinApp)
  //{{AFX_MSG_MAP(CAtomicClockApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAtomicClockApp::CAtomicClockApp() {}

CAtomicClockApp theApp;

BOOL CAtomicClockApp::InitInstance() {
  CAtomicClockDlg Dlg;
  m_pMainWnd = &Dlg;
  Dlg.DoModal();

  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
}

