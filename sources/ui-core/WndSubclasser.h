#pragma once

//Original code and article at http://www.codeproject.com. Author: emilio_grv
//Some changes made by A. Shabelnikov. 22/02/2009.

/*
Provide a way to sublass a window.
Override the virtual OnWndProcSub to process messages.
ex. if the subclasser is associateed to a CFrameWnd or CView, WM_NCDESTROY will cause
the deletion of the CFrameWnd or CView. However, there are other message on the stack
(for ex. WM_SYSCOMMAND) that still need to be processed.
Alwaus allocate on the heap and manage with strong pointers

NOTE:
 - the object must be always on heap to keep track of message recursion.
 - define _USE_SETWINDOWLONGPTR if you want to use SetWindowLongPtr() instead SetWindowLong().
*/

class AFX_EXT_CLASS CWndSubclasser
{
 public:
  CWndSubclasser();
  virtual ~CWndSubclasser(void);
  bool Subclass(HWND hWnd);
  bool Clear();
  static LRESULT Call(LPVOID token);
  LRESULT Default();
  LRESULT Default(WPARAM wParam, LPARAM lParam);
  CWnd* GetWnd() {return CWnd::FromHandle(_hWnd);}

 protected:
  virtual LRESULT WndProcSub(UINT uMsg, WPARAM wParam, LPARAM lParam);
  HWND _hWnd; //referred window

 private:
  LPVOID _token;
  bool _bCleared;
};
