 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "Resources\resource.h"
#include "LogPlayerTabDlg.h"

#include "common\FastDelegate.h"
#include "common\MathHelpers.h"
#include "LPControlPanelDlg.h"
#include "MIDesk\MIDeskDlg.h"
#include "ui-core\OScopeCtrl.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

const UINT CLogPlayerTabDlg::IDD = IDD_LOG_PLAYER;

BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, CDialog)
END_MESSAGE_MAP()

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CLogPlayerTabDlg::IDD, pParent)
, mp_MIDeskDlg(new CMIDeskDlg())
, mp_LPPanelDlg(new CLPControlPanelDlg)
, mp_OScopeCtrl(new COScopeCtrl())
{
 //todo
}

void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX); 
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}

BOOL CLogPlayerTabDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 mp_MIDeskDlg->Create(CMIDeskDlg::IDD, this);
 mp_MIDeskDlg->SetWindowPos(NULL,280,0,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
 mp_MIDeskDlg->Show(true);

 mp_LPPanelDlg->Create(CLPControlPanelDlg::IDD, this);
 mp_LPPanelDlg->SetWindowPos(NULL,0,0,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

 //инициализируем осциллограф
 _InitializeOscilloscopeControl();

 UpdateDialogControls(this,TRUE);	
 return TRUE;
}

void CLogPlayerTabDlg::EnlargeMonitor(bool i_enlarge)
{
 CRect rect;
 GetClientRect(rect);
 
 if (i_enlarge)
 {
  mp_MIDeskDlg->GetWindowRect(m_original_mi_rect);
  ScreenToClient(m_original_mi_rect);  
  CRect cp_rect;
  mp_LPPanelDlg->GetWindowRect(cp_rect);
  rect.left+=cp_rect.Width();
  CRect mi_rect = m_original_mi_rect;
  _ResizeRect(rect, mi_rect);
  mp_MIDeskDlg->Resize(mi_rect);
 }
 else
  mp_MIDeskDlg->Resize(m_original_mi_rect);
}

void CLogPlayerTabDlg::_ResizeRect(const CRect& i_external, CRect& io_victim)
{
 float Xf = ((float)i_external.Width()) / io_victim.Width();
 float Yf = ((float)i_external.Height()) / io_victim.Height(); 
 float factor = min(Xf, Yf);
 //масштабируем
 io_victim.right = MathHelpers::Round((io_victim.Width() * factor));
 io_victim.bottom = MathHelpers::Round((io_victim.Height() * factor));
 io_victim.left = 0;
 io_victim.top = 0;
 //центрируем
 CPoint center_external = i_external.CenterPoint();
 CPoint center_victim = io_victim.CenterPoint();
 io_victim.OffsetRect(center_external - center_victim);
}

//инициализация осциллографа для сигнала ДД
void CLogPlayerTabDlg::_InitializeOscilloscopeControl(void)
{
 CRect rect;
 GetDlgItem(IDC_LP_SIGNAL_OSCILLOSCOPE_HOLDER)->GetWindowRect(rect);
 ScreenToClient(rect);

 // create the control
 mp_OScopeCtrl->Create(WS_VISIBLE | WS_CHILD, rect, this); 

 // customize the control
 mp_OScopeCtrl->SetRange(0.0, 5.0, 1);
 mp_OScopeCtrl->SetYUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_V_UNIT));
 mp_OScopeCtrl->SetXUnits(MLL::LoadString(IDS_KC_OSCILLOSCOPE_H_UNIT));
 mp_OScopeCtrl->SetBackgroundColor(RGB(0, 0, 64));
 mp_OScopeCtrl->SetGridColor(RGB(192, 192, 255));
 mp_OScopeCtrl->SetPlotColor(RGB(255, 255, 255));
}

void CLogPlayerTabDlg::AppendKnockValue(double i_value, bool i_reverse)
{
 mp_OScopeCtrl->AppendPoint(i_value, i_reverse);
}

void CLogPlayerTabDlg::ResetKnockOscilloscope(void)
{
 mp_OScopeCtrl->Reset();
}

