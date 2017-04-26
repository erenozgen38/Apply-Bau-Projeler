// Atomic ClockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Atomic Clock.h"
#include "Atomic ClockDlg.h"
#include <Direct.h>
#include <locale.h>
#include <sys\timeb.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAtomicClockDlg::CAtomicClockDlg(CWnd* pParent /*=NULL*/) : MTI("Online Atomic Clock", AfxGetApp()->LoadIcon(IDR_MAINFRAME), IDR_PopUps, ID_TRAY_SHOW), CDialog(CAtomicClockDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CAtomicClockDlg)
  //}}AFX_DATA_INIT
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BEGIN_MESSAGE_MAP(CAtomicClockDlg, CDialog)
  //{{AFX_MSG_MAP(CAtomicClockDlg)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_TRAY_GETTIME, OnGetTime)
	ON_COMMAND(ID_TRAY_SHOW, OnTrayShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAtomicClockDlg::OnInitDialog(){
  CDialog::OnInitDialog();

  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  WndAnimator.SetWnd(this);

  SetTimer(IDT_1Day, 86400000, NULL);
  PostMessage(WM_TIMER, IDT_1Day); // Check the time now
  SetTimer(IDT_ShowTime, 4000, NULL);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below to draw the icon.
// For MFC applications using the document/view model, this is automatically done for you by the framework.
void CAtomicClockDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting
    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width () - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;
    dc.DrawIcon(x, y, m_hIcon);
  }else CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags the minimized window.
HCURSOR CAtomicClockDlg::OnQueryDragIcon() {return (HCURSOR) m_hIcon;}

void CAtomicClockDlg::OnTrayShow() {ShowWindow(SW_SHOW);}

void CAtomicClockDlg::OnGetTime() {
  Say(Atomic.Synch());
  ShowWindow(SW_SHOW);
}

void CAtomicClockDlg::OnTimer(UINT nIDEvent) {
  switch(nIDEvent) {
    case IDT_1Day: Say(Atomic.Synch()); break;
    case IDT_ShowTime: { // When the dialog is first shown, minimise it after ShowTime:
      KillTimer(IDT_ShowTime);
      ShowWindow(SW_HIDE);
      WndAnimator.Minimize();
      break;
  } }
  CDialog::OnTimer(nIDEvent);
}

void CAtomicClockDlg::OnSize(UINT nType, int cx, int cy) {
  if(nType==SIZE_RESTORED) {
    CRect Rect;
    GetWindowRect(&Rect);
    Rect.bottom=Rect.top+10;
    MoveWindow(Rect);
  }else CDialog::OnSize(nType, cx, cy);
}

void CAtomicClockDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0)==SC_CLOSE) {
    if(lParam) ShowWindow(SW_HIDE); else SetForegroundWindow();
    WndAnimator.Minimize();
  }else CDialog::OnSysCommand(nID, lParam);
}

void CAtomicClockDlg::OnCancel() {
  WndAnimator.Close();
	CDialog::OnCancel();
}

BOOL CAtomicClockDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
  if((wParam==ID_TRAY_SHOW) && !IsZoomed()) { // Only use our animation for restoring from the TaskBar
    WndAnimator.Restore();
    return TRUE;
  }
  return CDialog::OnCommand(wParam, lParam);
}
