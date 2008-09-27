/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MapImpExpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg dialog


CMapImpExpDlg::CMapImpExpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapImpExpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapImpExpDlg)
	m_current_fwd_title_string = _T("");
	m_other_fwd_title_string = _T("");
		
	m_fwd_flags[FLAG_START_MAP] = FALSE;
	m_fwd_flags[FLAG_IDLE_MAP] = FALSE;
	m_fwd_flags[FLAG_WORK_MAP] = FALSE;
	m_fwd_flags[FLAG_TEMP_MAP] = FALSE;
	//}}AFX_DATA_INIT
}


void CMapImpExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapImpExpDlg)
	DDX_Control(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_LIST, m_current_fwd_list);
	DDX_Control(pDX, IDC_MAP_IMPEXP_OTHER_FWD_LIST, m_other_fwd_list);
	DDX_Text(pDX, IDC_MAP_IMPEXP_CURRENT_FWD_TITLE, m_current_fwd_title_string);
	DDX_Text(pDX, IDC_MAP_IMPEXP_OTHER_FWD_TITLE, m_other_fwd_title_string);
	DDX_Check(pDX, IDC_MAP_IMPEXP_STARTMAP_FLAG,m_fwd_flags[FLAG_START_MAP]);
	DDX_Check(pDX, IDC_MAP_IMPEXP_IDLEMAP_FLAG, m_fwd_flags[FLAG_IDLE_MAP]);
	DDX_Check(pDX, IDC_MAP_IMPEXP_WORKMAP_FLAG, m_fwd_flags[FLAG_WORK_MAP]);
	DDX_Check(pDX, IDC_MAP_IMPEXP_TEMPMAP_FLAG, m_fwd_flags[FLAG_TEMP_MAP]);

	DDX_Control(pDX, IDC_MAP_IMPEXP_STARTMAP_FLAG,m_fwd_flags_buttons[FLAG_START_MAP]);
	DDX_Control(pDX, IDC_MAP_IMPEXP_IDLEMAP_FLAG, m_fwd_flags_buttons[FLAG_IDLE_MAP]);
	DDX_Control(pDX, IDC_MAP_IMPEXP_WORKMAP_FLAG, m_fwd_flags_buttons[FLAG_WORK_MAP]);
	DDX_Control(pDX, IDC_MAP_IMPEXP_TEMPMAP_FLAG, m_fwd_flags_buttons[FLAG_TEMP_MAP]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapImpExpDlg, CDialog)
	//{{AFX_MSG_MAP(CMapImpExpDlg)
	ON_BN_CLICKED(IDC_MAP_IMPEXP_EXCHANGE_BUTTON, OnMapImpexpExchangeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapImpExpDlg message handlers

void CMapImpExpDlg::OnMapImpexpExchangeButton() 
{
 if (m_OnExchangeButton)
   m_OnExchangeButton();	 	
}

//----------------------------------------------------------------------------
//изменилось выделение в спимке семейств характеристик
void CMapImpExpDlg::OnChangeFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;
  
  if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED)) 
  {
   //состояние выделения изменилось
   int selected_index = 0;

   //will return -1 if there are no selected items, at all
   selected_index = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);

   if (m_OnFWDCurrentSelectionChanged)
	 m_OnFWDCurrentSelectionChanged(selected_index);     
  }
 
  *pResult = 0;
}

void CMapImpExpDlg::OnEndLabelEditFWDCurrentList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

  if (pdi->item.mask == LVIF_TEXT) 
  {
   m_current_fwd_list.SetItem(&pdi->item);
   if (m_OnFWDCurrentListNamechanged)
     m_OnFWDCurrentListNamechanged(pdi->item.iItem,pdi->item.pszText); 
  }
}

void CMapImpExpDlg::FillFWDCurrentList(std::vector<_TSTRING> i_list_of_names)
{
  m_current_fwd_list.DeleteAllItems();
  for (size_t i = 0; i < i_list_of_names.size(); i++)
  {
   m_current_fwd_list.InsertItem(i,i_list_of_names[i].c_str());	
  }
}

void CMapImpExpDlg::SetFWDCurrentListSelection(int i_selected_index)
{
  if (i_selected_index!=-1)
    m_current_fwd_list.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);

  int i = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);
}

void CMapImpExpDlg::SetFWDCurrentListTitle(const _TSTRING& i_title)
{
  m_current_fwd_title_string = i_title.c_str(); 
  UpdateData(FALSE);
}



//----------------------------------------------------------------------------
//изменилось выделение в спимке семейств характеристик
void CMapImpExpDlg::OnChangeFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLISTVIEW *pnmv = (NM_LISTVIEW FAR *) pNMHDR;
  
  if (pnmv->uChanged == LVIF_STATE && (pnmv->uNewState & LVIS_SELECTED)) 
  {
   //состояние выделения изменилось
   int selected_index = 0;

   //will return -1 if there are no selected items, at all
   selected_index = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);

   if (m_OnFWDCurrentSelectionChanged)
	 m_OnFWDCurrentSelectionChanged(selected_index);     
  }
 
  *pResult = 0;
}

void CMapImpExpDlg::OnEndLabelEditFWDOtherList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVDISPINFO *pdi = (LV_DISPINFO FAR *) pNMHDR;

  if (pdi->item.mask == LVIF_TEXT) 
  {
   m_current_fwd_list.SetItem(&pdi->item);
   if (m_OnFWDCurrentListNamechanged)
     m_OnFWDCurrentListNamechanged(pdi->item.iItem,pdi->item.pszText); 
  }
}

void CMapImpExpDlg::FillFWDOtherList(std::vector<_TSTRING> i_list_of_names)
{
  m_current_fwd_list.DeleteAllItems();
  for (size_t i = 0; i < i_list_of_names.size(); i++)
  {
   m_current_fwd_list.InsertItem(i,i_list_of_names[i].c_str());	
  }
}

void CMapImpExpDlg::SetFWDOtherListSelection(int i_selected_index)
{
  if (i_selected_index!=-1)
    m_current_fwd_list.SetItemState(i_selected_index, LVIS_SELECTED, LVIS_SELECTED);

  int i = m_current_fwd_list.GetNextItem(-1, LVNI_SELECTED);
}

void CMapImpExpDlg::SetFWDOtherListTitle(const _TSTRING& i_title)
{
  m_other_fwd_title_string = i_title.c_str();
  UpdateData(FALSE);
}


void CMapImpExpDlg::SetFWDFlag(EFWDFlags i_flag_type, bool i_state)
{
 m_fwd_flags[i_flag_type] = i_state ? TRUE : FALSE;
 UpdateData(FALSE);
}

bool CMapImpExpDlg::GetFWDFlag(EFWDFlags i_flag_type)
{
  return m_fwd_flags[i_flag_type] ? true : false;
}

void CMapImpExpDlg::EnableFWDFlag(EFWDFlags i_flag_type, bool i_enable)
{
  BOOL enable = i_enable ? TRUE : FALSE;
  m_fwd_flags_buttons[i_flag_type].EnableWindow(enable);
}
