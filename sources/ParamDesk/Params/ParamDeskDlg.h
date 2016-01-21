/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

/** \file ParamDeskDlg.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <map>
#include <vector>

#include "common/unicodesupport.h"
#include "IParamDeskView.h"
#include "ui-core/ITabControllerEvent.h"
#include "ui-core/TabController.h"
#include "ui-core/UpdatableDialog.h"

class CHotKeysToCmdRouter;

/////////////////////////////////////////////////////////////////////////////
// CParamDeskDlg dialog

class AFX_EXT_CLASS CParamDeskDlg : public CModelessUpdatableDialog, public IParamDeskView, private ITabControllerEvent
{
 typedef CModelessUpdatableDialog Super;

 public:
  CParamDeskDlg(CWnd* pParent = NULL, bool i_show_knock_page = false);
  virtual ~CParamDeskDlg();
  static const UINT IDD;
  static const UINT IDD_F;

  //note: wnd_insert_after parameter may be used to fix tab order!
  void SetPosition(int x_pos, int y_pos, CWnd* wnd_insert_after = NULL);
  void SetTitle(const CString& i_str);
  void GetTitle(CString& o_str);

  //----------interface implementation---------------------------
  virtual bool IsEnabled(void);
  virtual void Enable(bool enable);
  virtual void Show(bool show);
  virtual void ShowSaveButton(bool i_show);

  virtual bool SetValues(BYTE i_descriptor, const void* i_values);
  virtual bool GetValues(BYTE i_descriptor, void* o_values);
  virtual bool LockUIUpdate(BYTE i_descriptor, bool lock);

  virtual void SetFunctionsNames(const std::vector<_TSTRING>& i_names);
  virtual const std::vector<_TSTRING>& GetFunctionsNames(void);

  virtual void EnableIgnitionCogs(bool enable);
  virtual void EnableUseVentPwm(bool enable);
  virtual void EnableUseCTSCurveMap(bool enable);
  virtual void EnableHallOutputParams(bool enable);
  virtual void SetMaxCylinders(int number);
  virtual void EnableOddCylinders(bool enable);
  virtual void EnableSECU3TItems(bool i_enable);
  virtual void EnableChokeTesting(bool i_enable);
  virtual void EnableChokeManPos(bool i_enable);
  virtual void EnableCKPSItems(bool i_enable);
  virtual void EnableBTNameAndPass(bool i_enable);
  virtual void EnableInputsMerging(bool i_enable);
  virtual void EnableFuelInjection(bool i_enable);
  virtual void EnableLambda(bool i_enable);
  virtual void EnableGasdoseTesting(bool i_enable); //GD
  virtual void EnableGasdoseManPos(bool i_enable);  //GD
  virtual void EnableGasdose(bool i_enable);        //GD (entire tab)
  virtual void EnableChoke(bool i_enable);

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
  afx_msg void OnSysCommand(UINT, LONG);
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
  afx_msg void OnHK_CHOKE_PAR();
  afx_msg void OnHK_SECUR_PAR();
  afx_msg void OnHK_UNIOUT_PAR();
  DECLARE_MESSAGE_MAP()

 private:
  // ITabControllerEvent
  virtual void OnSelchangeTabctl(void);
  virtual bool OnSelchangingTabctl(void);

  void OnChangeInTab(void);
  int  _GetTabIndex(unsigned i_descriptor);
  void _RegisterHotKeys(void);

  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;

  //Pointers to dialogs of all tabs
  class CStarterPageDlg*   m_pStarterPageDlg;
  class CAnglesPageDlg*    m_pAnglesPageDlg;
  class CIdlRegPageDlg*    m_pIdlRegPageDlg;
  class CFunSetPageDlg*    m_pFunSetPageDlg;
  class CTemperPageDlg*    m_pTemperPageDlg;
  class CCarburPageDlg*    m_pCarburPageDlg;
  class CADCCompenPageDlg* m_pADCCompenPageDlg;
  class CCKPSPageDlg*      m_pCKPSPageDlg;
  class CKnockPageDlg*     m_pKnockPageDlg;    //<--optional
  class CMiscPageDlg*      m_pMiscPageDlg;
  class CChokePageDlg*     m_pChokePageDlg;
  class CGasdosePageDlg*   m_pGasdosePageDlg;  //GD
  class CSecurPageDlg*     m_pSecurPageDlg;
  class CUniOutPageDlg*    m_pUniOutPageDlg;
  class CInjectorPageDlg*  m_pInjectorPageDlg; //<--fuel injection
  class CLambdaPageDlg*    m_pLambdaPageDlg;   //<--lambda
  class CAccelEnrPageDlg*  m_pAccelEnrPageDlg; //<--fuel injection

  CStatic m_pd_title;
  CButton m_save_button;
  CTabController m_tab_control;
  CImageList* m_pImgList;

  bool m_enabled;
  bool m_fuel_injection;
  bool m_lambda;
  bool m_gasdose;
  bool m_choke;
  const bool m_show_knock_page;
  typedef std::map<int,unsigned> TabDescriptor;
  TabDescriptor m_tab_descriptors;
  int m_fuel_inject_idx;
  int m_lambda_tab_idx;
  int m_accenr_tab_idx;
  int m_gasdose_tab_idx;
  int m_choke_tab_idx;

  //Event handlers (delegates)
  EventHandler m_OnTabActivate;
  EventHandler m_OnChangeInTab;
  EventHandler m_OnSaveButton;
};

/////////////////////////////////////////////////////////////////////////////
