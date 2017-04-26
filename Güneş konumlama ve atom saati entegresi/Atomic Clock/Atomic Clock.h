// Atomic Clock.h

#if !defined(AFX_ATOMICCLOCK_H__35FD047C_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_)
#define AFX_ATOMICCLOCK_H__35FD047C_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CAtomicClockApp : public CWinApp {
public:
  CAtomicClockApp();
  //{{AFX_VIRTUAL(CAtomicClockApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL
  //{{AFX_MSG(CAtomicClockApp)
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_ATOMICCLOCK_H__35FD047C_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_)
