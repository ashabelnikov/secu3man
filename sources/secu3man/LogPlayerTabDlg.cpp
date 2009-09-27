 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "LogPlayerTabDlg.h"
#include "common/FastDelegate.h"

#include <limits>

using namespace std;

#undef max //numeric_limits<>::max() cases warning C4003: not enough actual parameters for macro...

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

CLogPlayerTabDlg::CLogPlayerTabDlg(CWnd* pParent /*=NULL*/)
: CTabDialog(CLogPlayerTabDlg::IDD, pParent)
, m_next_button_state(false)
, m_prev_button_state(false)
, m_all_enabled(false)
{
 //todo
}

void CLogPlayerTabDlg::DoDataExchange(CDataExchange* pDX)
{
 CDialog::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_LOG_PLAYER_TIME_FACTOR_COMBO, m_time_factor_combo);
 DDX_Control(pDX, IDC_LOG_PLAYER_PLAY_BUTTON, m_play_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_NEXT_BUTTON, m_next_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_PREV_BUTTON, m_prev_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_FILE_NAME_INDICATOR, m_file_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_INDICATOR, m_position_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_OPEN_FILE_BUTTON, m_open_file_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_SLIDER, m_slider);
}

LPCTSTR CLogPlayerTabDlg::GetDialogID(void) const
{
 return (LPCTSTR)IDD; 
}

BEGIN_MESSAGE_MAP(CLogPlayerTabDlg, CDialog)
 ON_WM_CLOSE()
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PREV_BUTTON, OnUpdatePrevButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_NEXT_BUTTON, OnUpdateNextButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PLAY_BUTTON, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_POSITION_SLIDER, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_TEXT, OnUpdateControls)
 ON_BN_CLICKED(IDC_LOG_PLAYER_OPEN_FILE_BUTTON, OnOpenFileButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PLAY_BUTTON, OnPlayButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_NEXT_BUTTON, OnNextButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PREV_BUTTON, OnPrevButton)
 ON_WM_HSCROLL()
 ON_CBN_SELCHANGE(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnSelchangeTimeFactorCombo)
END_MESSAGE_MAP()

BOOL CLogPlayerTabDlg::OnInitDialog() 
{
 CDialog::OnInitDialog();
	
 m_MIDeskDlg.Create(CMIDeskDlg::IDD, this);
 m_MIDeskDlg.MoveWindow(280,0,427,312);
 m_MIDeskDlg.ShowWindow(SW_SHOWNORMAL);
 m_MIDeskDlg.Show(true);

 UpdateDialogControls(this,TRUE);	
 return TRUE;
}

void CLogPlayerTabDlg::OnClose() 
{	
//	CDialog::OnClose();
}

void CLogPlayerTabDlg::FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings)
{
 for(std::vector<_TSTRING>::iterator it = i_factor_strings.begin(); it != i_factor_strings.end(); ++it)
 {
  int index = m_time_factor_combo.AddString((*it).c_str());
  ASSERT(index != LB_ERR);
  m_time_factor_combo.SetItemData(index, std::distance(i_factor_strings.begin(), it));
 }

}

size_t CLogPlayerTabDlg::GetTimeFactor(void) const
{
 int index = m_time_factor_combo.GetCurSel(); 
 return (index != LB_ERR ? m_time_factor_combo.GetItemData(index) : numeric_limits<size_t>::max());
}

void CLogPlayerTabDlg::SetTimeFactor(size_t i_factor_id)
{
 int count = m_time_factor_combo.GetCount();
 for(int i = 0; i < count; ++i)
 {
  size_t id = m_time_factor_combo.GetItemData(i);
  if (id==i_factor_id)
  {
   m_time_factor_combo.SetCurSel(i);
   return;
  }
 }
 ASSERT(0); //unknown item id!
}

void CLogPlayerTabDlg::SetPlayButtonState(bool i_state)
{
 m_play_button.SetCheck(i_state ? BST_CHECKED : BST_UNCHECKED);
}

bool CLogPlayerTabDlg::GetPlayButtonState(void) const
{
 return m_play_button.GetCheck() == BST_CHECKED;
}

void CLogPlayerTabDlg::EnableNextButton(bool i_enable)
{
 m_next_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLogPlayerTabDlg::EnablePrevButton(bool i_enable)
{
 m_prev_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}
	
void CLogPlayerTabDlg::SetSliderPosition(unsigned long i_position)
{
 m_slider.SetPos(i_position);
}

unsigned long CLogPlayerTabDlg::GetSliderPosition(void) const
{
 return m_slider.GetPos();
}

void CLogPlayerTabDlg::SetSliderRange(unsigned long i_begin, unsigned long i_end)
{
 m_slider.SetRangeMin(i_begin);
 m_slider.SetRangeMax(i_end);
}

void CLogPlayerTabDlg::SetFileIndicator(const _TSTRING& i_string)
{
 m_file_indicator.SetWindowText(i_string.c_str());
}

void CLogPlayerTabDlg::SetPositionIndicator(const _TSTRING& i_string)
{
 m_position_indicator.SetWindowText(i_string.c_str());
}

void CLogPlayerTabDlg::OnUpdateControls(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_all_enabled);  
}

void CLogPlayerTabDlg::OnUpdateNextButton(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_next_button_state && m_all_enabled);  
}

void CLogPlayerTabDlg::OnUpdatePrevButton(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_prev_button_state && m_all_enabled);  
}

void CLogPlayerTabDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLogPlayerTabDlg::OnOpenFileButton()
{
 if (m_on_open_file_button)
  m_on_open_file_button();
}

void CLogPlayerTabDlg::OnPlayButton()
{
 if (m_on_play_button)
  m_on_play_button();
}

void CLogPlayerTabDlg::OnNextButton()
{
 if (m_on_next_button)
  m_on_next_button();
}

void CLogPlayerTabDlg::OnPrevButton()
{
 if (m_on_prev_button)
  m_on_prev_button();
}

void CLogPlayerTabDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 if (m_on_slider_moved)
  m_on_slider_moved(nSBCode, nPos);
}

void CLogPlayerTabDlg::OnSelchangeTimeFactorCombo()
{
 if (m_on_time_factor_combo)
 {
  size_t factor = GetTimeFactor();
  m_on_time_factor_combo(factor); 
 }
}

void CLogPlayerTabDlg::setOnPlayButton(EventHandler i_callback)
{
 m_on_play_button = i_callback;
}

void CLogPlayerTabDlg::setOnNextButton(EventHandler i_callback)
{
 m_on_next_button = i_callback;
}

void CLogPlayerTabDlg::setOnPrevButton(EventHandler i_callback)
{
 m_on_prev_button = i_callback;
}

void CLogPlayerTabDlg::setOnOpenFileButton(EventHandler i_callback)
{
 m_on_open_file_button = i_callback;
}

void CLogPlayerTabDlg::setOnTimeFactorCombo(EventWithCode i_callback)
{
 m_on_time_factor_combo = i_callback;
}

void CLogPlayerTabDlg::setOnSliderMoved(EventHScroll i_callback)
{
 m_on_slider_moved = i_callback; 
}

void CLogPlayerTabDlg::SetOpenFileButtonText(const _TSTRING& i_text)
{
 m_open_file_button.SetWindowText(i_text.c_str());
}

