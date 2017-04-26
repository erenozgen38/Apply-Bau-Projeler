// WndAnimator.h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WndAnimatorh
#define WndAnimatorh

/*
Most of this class is code to find the System Tray Location - the rest is easy.
Normally when you Minimize, Maximize, or Restore a Window you see the Window's Title Bar animate across the screen
but when you use Window Messages such as ShowWindow(SW_RESTORE); the system doesn't animate the Title Bar any more.
This class add the Window's Title Bar Animation.
It was designed to be used with the TrayIcon class which Minimizes an Application's Main Window to the System Tray in the Task Bar.
Since the Minimize and Restore is then being done by SC_MINIMIZE and SC_RESTORE commands the animation is no longer shown for these operations.

To allow your application to animate its title bar when you Minimize and Restore (like Windows Explorer does)
Create an instance of CWndAnimator in your main window's header:

CWndAnimator WndAnimator;

Then if you are creating a Dialog Application, in OnInitDialog()

WndAnimator.SetWnd(this);

and call each animation function appropriately (this example is for a Dialog Application):

void CMyDlg::OnCancel() {WndAnimator.Close();  CDialog::OnCancel();}
void CMyDlg::OnShow  () {WndAnimator.Restore();}
void CMyDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  switch(nID & 0xFFF0) {
    case IDM_ABOUTBOX: CAboutDlg().DoModal (); return;
    case SC_MINIMIZE:  WndAnimator.Minimize(); return;
    case SC_RESTORE:   if(!IsZoomed()) {WndAnimator.Restore(); return;} //Only use our animation for restoring from the TaskBar
    default: CDialog::OnSysCommand(nID, lParam); return;
} }

If you've got your own menu entry to show your application you don't need the SC_RESTORE above, instead, use:
BOOL CMyDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
  if((wParam==ID_TRAY_SHOW) && !IsZoomed()) { // Only use our animation for restoring from the TaskBar
    WndAnimator.Restore();
    return TRUE;
  }
  return CDialog::OnCommand(wParam, lParam);
}

*/
class CWndAnimator {
  CWnd* pWnd;
public:
  CWndAnimator() : pWnd(0) {}
  CWndAnimator(CWnd* pWnd) : pWnd(pWnd) {}
  void SetWnd(CWnd* Wnd) {pWnd=Wnd;}
  virtual ~CWndAnimator() {}

private:
  void GetTrayWndRect(LPRECT lpTrayRect) {
  //This is not guaranteed to work on future versions of the shell.
    HWND hShellTrayWnd=FindWindowEx(NULL,NULL,TEXT("Shell_TrayWnd"),NULL);
    if(hShellTrayWnd) {
      HWND hTrayNotifyWnd=FindWindowEx(hShellTrayWnd,NULL,TEXT("TrayNotifyWnd"),NULL);
      if(hTrayNotifyWnd) {
        ::GetWindowRect(hTrayNotifyWnd,lpTrayRect);
        return;
    } }

  //Either explorer isn't running or it's a new version of the shell with the window class names.
    APPBARDATA appBarData;
    appBarData.cbSize=sizeof(appBarData);
    if(SHAppBarMessage(ABM_GETTASKBARPOS,&appBarData)) {
      switch(appBarData.uEdge) {
        case ABE_LEFT:
        case ABE_RIGHT: { // We want to minimize to the bottom of the taskbar
          lpTrayRect->top=appBarData.rc.bottom-100;
          lpTrayRect->bottom=appBarData.rc.bottom-16;
          lpTrayRect->left=appBarData.rc.left;
          lpTrayRect->right=appBarData.rc.right;
          break;
        }
        case ABE_TOP:
        case ABE_BOTTOM: { // We want to minimize to the right of the taskbar
          lpTrayRect->top=appBarData.rc.top;
          lpTrayRect->bottom=appBarData.rc.bottom;
          lpTrayRect->left=appBarData.rc.right-100;
          lpTrayRect->right=appBarData.rc.right-16;
          break;
      } }
      return;
    }
/*
It's possible that a third party shell is running instead of explorer.
This shell might provide support for the system tray,
by providing a Shell_TrayWnd window
(which receives the messages for the icons)
So, look for a Shell_TrayWnd window and work out the rect from that.
Remember that explorer's taskbar is the Shell_TrayWnd,
and stretches either the width or the height of the screen.
We can't rely on the 3rd party shell's Shell_TrayWnd doing the same,
in fact, we can't rely on it being any size.
The best we can do is just blindly use the window rect,
perhaps limiting the width and height to, say 150 square.
Note that if the 3rd party shell supports the same configuraion as explorer
(the icons hosted in NotifyTrayWnd, which is a child window of Shell_TrayWnd),
we would already have caught it above.
*/
    hShellTrayWnd=FindWindowEx(NULL,NULL,TEXT("Shell_TrayWnd"),NULL);
    if(hShellTrayWnd) {
      ::GetWindowRect(hShellTrayWnd,lpTrayRect);
      if(lpTrayRect->right -lpTrayRect->left>150) lpTrayRect->left=lpTrayRect->right -150;
      if(lpTrayRect->bottom-lpTrayRect->top > 30) lpTrayRect->top =lpTrayRect->bottom- 30;
      return;
    }

  // Provide a default rect based on the current work area
    SystemParametersInfo(SPI_GETWORKAREA,0,lpTrayRect,0);
    lpTrayRect->left=lpTrayRect->right-150;
    lpTrayRect->top=lpTrayRect->bottom- 30;
  }

  bool Animate() {
    ANIMATIONINFO ai;
    ai.cbSize=sizeof(ai);
    SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai ,0);
    return !!ai.iMinAnimate;
  }

public:
  void Minimize() {
    ASSERT(pWnd);
    if(Animate()) {
      CRect src,dst;
      pWnd->GetWindowRect(&src);
      GetTrayWndRect(&dst);
      DrawAnimatedRects(pWnd->GetSafeHwnd(), IDANI_CAPTION, &src, &dst);
    }
    pWnd->ShowWindow(SW_HIDE);
  }
  void Restore() {
    ASSERT(pWnd);
    if(Animate()) {
      CRect src,dst;
      GetTrayWndRect(&src);
      pWnd->GetWindowRect(&dst); // Still valid even though the window is hidden
      DrawAnimatedRects(pWnd->GetSafeHwnd(), IDANI_CAPTION, &src, &dst);
    }
    pWnd->ShowWindow(SW_SHOW);
    pWnd->SetForegroundWindow();
  }
  void Close() {
    ASSERT(pWnd);
    if(Animate()) { //Since the dialog is only getting smaller, the system shouldn't ask its children to redraw themselves.
      CRect Rect;
      pWnd->GetWindowRect(&Rect);
      while(Rect.Height()>16) {
        pWnd->MoveWindow(&Rect);
        int dy=Rect.Height()>>2;
        Rect.top+=dy;
        Rect.bottom-=dy;
        Sleep(25);
      }
      while(Rect.Width()>16) {
        pWnd->MoveWindow(&Rect);
        int dx=Rect.Width()>>2;
        Rect.left+=dx;
        Rect.right-=dx;
        Sleep(25);
  } } }
/* You may prefer to use the following:
      ShowWindow(SW_HIDE);
      Sleep(25);
      CRect src;
      GetWindowRect(&src);
      CRect dst(src.CenterPoint() ,CSize(0,0));
      DrawAnimatedRects(m_hWnd,IDANI_CAPTION,&src,&dst);
*/
};

#endif //ndef WndAnimatorh
