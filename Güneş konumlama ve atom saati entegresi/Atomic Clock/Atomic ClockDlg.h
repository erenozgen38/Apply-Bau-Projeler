// Atomic ClockDlg.h : header file
//

#if !defined(AFX_ATOMICCLOCKDLG_H__35FD047E_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_)
#define AFX_ATOMICCLOCKDLG_H__35FD047E_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Atomic.h"
#include "TrayIcon.h"
#include "WndAnimator.h"

/////////////////////////////////////////////////////////////////////////////
// CAtomicClockDlg dialog

class CAtomicClockDlg : public CDialog {
  CMenuTrayIcon MTI;
  CWndAnimator WndAnimator;
public:
  CAtomicClockDlg(CWnd* pParent = NULL);  // standard constructor

  void Say(const CString& Msg) {
    CString S("Online Atomic Clock - "+Msg);
    SetWindowText(S);
    MTI.SetTooltipText(S);
  }
  CAtomic Atomic;

  //{{AFX_DATA(CAtomicClockDlg)
  enum { IDD = IDD_ATOMICCLOCK_DIALOG };
  //}}AFX_DATA
  //{{AFX_VIRTUAL(CAtomicClockDlg)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:
  HICON m_hIcon;
  //{{AFX_MSG(CAtomicClockDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetTime();
	afx_msg void OnTrayShow();
	virtual void OnCancel();
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_ATOMICCLOCKDLG_H__35FD047E_B0C0_11D5_BF17_00A0C99A06A5__INCLUDED_)
