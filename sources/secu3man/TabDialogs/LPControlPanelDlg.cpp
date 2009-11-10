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
#include "LPControlPanelDlg.h"

#include <limits>
#include "common\FastDelegate.h"

using namespace std;

#undef max //numeric_limits<>::max() cases warning C4003: not enough actual parameters for macro...

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fastdelegate;

const UINT CLPControlPanelDlg::IDD = IDD_LOG_PLAYER_CONTROL_PANEL;

CLPControlPanelDlg::CLPControlPanelDlg(CWnd* pParent /*=NULL*/)
: Super(CLPControlPanelDlg::IDD, pParent)
, m_next_button_state(false)
, m_prev_button_state(false)
, m_play_button_state(false)
, m_slider_state(false)
, m_all_enabled(false)
{
 //todo
}

void CLPControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
 Super::DoDataExchange(pDX);
 DDX_Control(pDX, IDC_LOG_PLAYER_TIME_FACTOR_COMBO, m_time_factor_combo);
 DDX_Control(pDX, IDC_LOG_PLAYER_PLAY_BUTTON, m_play_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_NEXT_BUTTON, m_next_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_PREV_BUTTON, m_prev_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_FILE_NAME_INDICATOR, m_file_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_INDICATOR, m_position_indicator);
 DDX_Control(pDX, IDC_LOG_PLAYER_OPEN_FILE_BUTTON, m_open_file_button);
 DDX_Control(pDX, IDC_LOG_PLAYER_POSITION_SLIDER, m_slider);
}

BEGIN_MESSAGE_MAP(CLPControlPanelDlg, Super) 
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PREV_BUTTON, OnUpdatePrevButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_NEXT_BUTTON, OnUpdateNextButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_PLAY_BUTTON, OnUpdatePlayButton)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_POSITION_SLIDER, OnUpdateSlider)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnUpdateControls)
 ON_UPDATE_COMMAND_UI(IDC_LOG_PLAYER_TIME_FACTOR_TEXT, OnUpdateControls)
 ON_BN_CLICKED(IDC_LOG_PLAYER_OPEN_FILE_BUTTON, OnOpenFileButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PLAY_BUTTON, OnPlayButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_NEXT_BUTTON, OnNextButton)
 ON_BN_CLICKED(IDC_LOG_PLAYER_PREV_BUTTON, OnPrevButton)
 ON_WM_HSCROLL()
 ON_CBN_SELCHANGE(IDC_LOG_PLAYER_TIME_FACTOR_COMBO, OnSelchangeTimeFactorCombo)
END_MESSAGE_MAP()

BOOL CLPControlPanelDlg::OnInitDialog() 
{
 Super::OnInitDialog();
	
 UpdateDialogControls(this,TRUE);	
 return TRUE;
}

void CLPControlPanelDlg::FillTimeFactorCombo(std::vector<_TSTRING>& i_factor_strings)
{
 for(std::vector<_TSTRING>::iterator it = i_factor_strings.begin(); it != i_factor_strings.end(); ++it)
 {
  int index = m_time_factor_combo.AddString((*it).c_str());
  ASSERT(index != LB_ERR);
  m_time_factor_combo.SetItemData(index, std::distance(i_factor_strings.begin(), it));
 }
}

size_t CLPControlPanelDlg::GetTimeFactor(void) const
{
 int index = m_time_factor_combo.GetCurSel(); 
 return (index != LB_ERR ? m_time_factor_combo.GetItemData(index) : numeric_limits<size_t>::max());
}

void CLPControlPanelDlg::SetTimeFactor(size_t i_factor_id)
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

void CLPControlPanelDlg::SetPlayButtonState(bool i_state)
{
 m_play_button.SetCheck(i_state ? BST_CHECKED : BST_UNCHECKED);
}

bool CLPControlPanelDlg::GetPlayButtonState(void) const
{
 return m_play_button.GetCheck() == BST_CHECKED;
}

void CLPControlPanelDlg::EnableNextButton(bool i_enable)
{
 m_next_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLPControlPanelDlg::EnablePrevButton(bool i_enable)
{
 m_prev_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}
	
void CLPControlPanelDlg::SetSliderPosition(unsigned long i_position)
{
 m_slider.SetPos(i_position);
}

unsigned long CLPControlPanelDlg::GetSliderPosition(void) const
{
 return m_slider.GetPos();
}

void CLPControlPanelDlg::SetSliderRange(unsigned long i_begin, unsigned long i_end)
{
 m_slider.SetRangeMin(i_begin);
 m_slider.SetRangeMax(i_end);
}

void CLPControlPanelDlg::SetFileIndicator(const _TSTRING& i_string)
{
 m_file_indicator.SetWindowText(i_string.c_str());
}

void CLPControlPanelDlg::SetPositionIndicator(const _TSTRING& i_string)
{
 m_position_indicator.SetWindowText(i_string.c_str());
}

void CLPControlPanelDlg::OnUpdateControls(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_all_enabled);  
}

void CLPControlPanelDlg::OnUpdateNextButton(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_next_button_state && m_all_enabled);  
}

void CLPControlPanelDlg::OnUpdatePrevButton(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_prev_button_state && m_all_enabled);  
}

void CLPControlPanelDlg::OnUpdatePlayButton(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_play_button_state && m_all_enabled);  
}

void CLPControlPanelDlg::OnUpdateSlider(CCmdUI* pCmdUI) 
{
 pCmdUI->Enable(m_slider_state && m_all_enabled);  
}

void CLPControlPanelDlg::EnableAll(bool i_enable)
{
 m_all_enabled = i_enable;
 UpdateDialogControls(this,TRUE);
}

void CLPControlPanelDlg::OnOpenFileButton()
{
 if (m_on_open_file_button)
  m_on_open_file_button();
}

void CLPControlPanelDlg::OnPlayButton()
{
 if (m_on_play_button)
  m_on_play_button();
}

void CLPControlPanelDlg::OnNextButton()
{
 if (m_on_next_button)
  m_on_next_button();
}

void CLPControlPanelDlg::OnPrevButton()
{
 if (m_on_prev_button)
  m_on_prev_button();
}

void CLPControlPanelDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 if (m_on_slider_moved)
  m_on_slider_moved(nSBCode, nPos);
}

void CLPControlPanelDlg::OnSelchangeTimeFactorCombo()
{
 if (m_on_time_factor_combo)
 {
  size_t factor = GetTimeFactor();
  m_on_time_factor_combo(factor); 
 }
}

void CLPControlPanelDlg::setOnPlayButton(EventHandler i_callback)
{
 m_on_play_button = i_callback;
}

void CLPControlPanelDlg::setOnNextButton(EventHandler i_callback)
{
 m_on_next_button = i_callback;
}

void CLPControlPanelDlg::setOnPrevButton(EventHandler i_callback)
{
 m_on_prev_button = i_callback;
}

void CLPControlPanelDlg::setOnOpenFileButton(EventHandler i_callback)
{
 m_on_open_file_button = i_callback;
}

void CLPControlPanelDlg::setOnTimeFactorCombo(EventWithCode i_callback)
{
 m_on_time_factor_combo = i_callback;
}

void CLPControlPanelDlg::setOnSliderMoved(EventHScroll i_callback)
{
 m_on_slider_moved = i_callback; 
}

void CLPControlPanelDlg::SetOpenFileButtonText(const _TSTRING& i_text)
{
 m_open_file_button.SetWindowText(i_text.c_str());
}

bool CLPControlPanelDlg::IsNextButtonEnabled(void) const
{
 return m_next_button_state;
}

bool CLPControlPanelDlg::IsPrevButtonEnabled(void) const
{
 return m_prev_button_state;
}

void CLPControlPanelDlg::EnablePlayButton(bool i_enable)
{
 m_play_button_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CLPControlPanelDlg::IsPlayButtonEnabled(void) const
{
 return m_play_button_state;
}

void CLPControlPanelDlg::EnableSlider(bool i_enable)
{
 m_slider_state = i_enable;
 UpdateDialogControls(this,TRUE);
}

bool CLPControlPanelDlg::IsSliderEnabled(void) const
{
 return m_slider_state;
}

unsigned long CLPControlPanelDlg::GetSliderLineSize(void) const
{
 return m_slider.GetLineSize();
}

void CLPControlPanelDlg::SetSliderLineSize(unsigned long i_line_size)
{
 m_slider.SetLineSize(i_line_size);
}

unsigned long CLPControlPanelDlg::GetSliderPageSize(void) const
{
 return m_slider.GetPageSize();
}

void CLPControlPanelDlg::SetSliderPageSize(unsigned long i_page_size)
{
 m_slider.SetPageSize(i_page_size);
}
