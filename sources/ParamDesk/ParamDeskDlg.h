
#pragma once

#include <map>
#include <vector>

#include "common\unicodesupport.h"
#include "FunSetPageDlg.h"
#include "IParamDeskView.h"
#include "ui-core\ITabControllerEvent.h"
#include "ui-core\TabController.h"
#include "ui-core\UpdatableDialog.h"

class CHotKeysToCmdRouter;

/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg dialog

class AFX_EXT_CLASS CParamDeskDlg : public CModelessUpdatableDialog, public IParamDeskView, private ITabControllerEvent
{
 typedef CModelessUpdatableDialog Super;

 public:
  CParamDeskDlg(CWnd* pParent = NULL, bool i_show_knock_page = false);
  virtual ~CParamDeskDlg();
  static UINT IDD;
  static UINT IDD_F;

  void SetPosition(int x_pos, int y_pos);
  void SetTitle(const CString& i_str);
  void GetTitle(CString& o_str);

  //----------interface implementation---------------------------
  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);
  virtual void ShowSaveButton(bool i_show);

  virtual bool SetValues(BYTE i_descriptor, const void* i_values);
  virtual bool GetValues(BYTE i_descriptor, void* o_values);

  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_names);
  virtual const std::vector<_TSTRING>& GetFunctionsNames(void);

  virtual BYTE GetCurrentDescriptor(void);

  virtual void SetOnTabActivate(EventHandler OnTabActivate);
  virtual void SetOnChangeInTab(EventHandler OnChangeInTab);
  virtual void SetOnSaveButton(EventHandler OnSaveButton);

  //Get/Set current selection
  virtual bool SetCurSel(int sel);
  virtual int GetCurSel(void);
  //-------------------------------------------------------------

 // Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
  afx_msg void OnSaveButton();
  afx_msg void OnHK_STARTR_PAR();
  afx_msg void OnHK_ANGLES_PAR();
  afx_msg void OnHK_IDLREG_PAR();
  afx_msg void OnHK_FUNSET_PAR();
  afx_msg void OnHK_TEMPER_PAR();
  afx_msg void OnHK_CARBUR_PAR();
  afx_msg void OnHK_ADCCOR_PAR();
  afx_msg void OnHK_CKPS_PAR();
  afx_msg void OnHK_KNOCK_PAR(); // <--optional
  afx_msg void OnHK_MISCEL_PAR();
  DECLARE_MESSAGE_MAP()

 private:
  // ITabControllerEvent
  virtual void OnSelchangeTabctl(void);
  virtual void OnSelchangingTabctl(void);

  void OnChangeInTab(void);
  int  _GetTabIndex(unsigned i_descriptor);
  void _RegisterHotKeys(void);

  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;

  //указатели на диалоги всех вкладок
  class CStarterPageDlg*   m_pStarterPageDlg;
  class CAnglesPageDlg*    m_pAnglesPageDlg;
  class CIdlRegPageDlg*    m_pIdlRegPageDlg;
  class CFunSetPageDlg*    m_pFunSetPageDlg;
  class CTemperPageDlg*    m_pTemperPageDlg;
  class CCarburPageDlg*    m_pCarburPageDlg;
  class CADCCompenPageDlg* m_pADCCompenPageDlg;
  class CCKPSPageDlg*      m_pCKPSPageDlg;
  class CKnockPageDlg*     m_pKnockPageDlg; //<--optional
  class CMiscPageDlg*      m_pMiscPageDlg;

  CStatic m_pd_title;
  CButton m_save_button;
  CTabController m_tab_control;
  CImageList* m_pImgList;

  BOOL m_enabled;
  const bool m_show_knock_page;
  typedef std::map<int,unsigned> TabDescriptor;
  TabDescriptor m_tab_descriptors;

  //обработчики событий (делегаты)
  EventHandler m_OnTabActivate;
  EventHandler m_OnChangeInTab;
  EventHandler m_OnSaveButton;
};

/////////////////////////////////////////////////////////////////////////////
