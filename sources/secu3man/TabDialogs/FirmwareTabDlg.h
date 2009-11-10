
#pragma once

#include <memory>
#include <vector>
#include "common\FastDelegate.h"
#include "ui-core\TabDialog.h"

enum EMapTypes
{
 TYPE_MAP_DA_START = 0,
 TYPE_MAP_DA_IDLE,
 TYPE_MAP_DA_WORK,
 TYPE_MAP_DA_TEMP_CORR,
 TYPE_MAP_ATTENUATOR
};

class CHotKeysToCmdRouter;
class CFirmwareModeContextMenuManager;
class CParamDeskDlg;

/////////////////////////////////////////////////////////////////////////////
// CFirmwareTabDlg dialog

class CFirmwareTabDlg : public CTabDialog
{
  typedef CTabDialog Super;
  typedef fastdelegate::FastDelegate0<> EventHandler;
  typedef fastdelegate::FastDelegate0<bool> EventResult;
  typedef fastdelegate::FastDelegate1<int> EventWithCode;
  typedef fastdelegate::FastDelegate2<int,CString> EventWithCodeAndString;

 public:
  CFirmwareTabDlg(CWnd* pParent = NULL);   // standard constructor

  static const UINT IDD;

  virtual LPCTSTR GetDialogID(void) const;
	
  bool IsBLStartedEmergency(void);
  void SetBLStartedEmergency(bool state)
  { 
   m_bl_started_emergency.SetCheck(state ? BST_CHECKED : BST_UNCHECKED);
  };

  void EnableBLStartedEmergency(bool enable);
  bool IsBLStartedEmergencyEnabled(void) {return m_is_bl_started_emergency_available;};

  void EnableBLItems(bool enable);
  bool IsBLItemsEnabled(void) {return m_is_bl_items_available;};

  void EnableAppItems(bool enable);
  bool IsAppItemsEnabled(void) {return m_is_app_items_available;};

  void SetFWInformationText(CString i_text);
  CString GetFWInformationText(void);

  float* GetStartMap(bool i_original) 
  {
   if (i_original)
	return m_start_map_original;
   else
    return m_start_map_active;
  }

  float* GetIdleMap(bool i_original) 
  {
   if (i_original)
	return m_idle_map_original;
   else
    return m_idle_map_active;
  }

  float* GetWorkMap(bool i_original) 
  {
   if (i_original)
	return &m_work_map_original[0][0];
   else
    return &m_work_map_active[0][0];
  }

  float* GetTempMap(bool i_original) 
  {
   if (i_original)
	return m_temp_map_original;
   else
    return m_temp_map_active;
  }

  float* GetAttenuatorMap(bool i_original) 
  {
   if (i_original)
	return m_attenuator_map_original;
   else
    return m_attenuator_map_active;
  }

  void SetFunSetListBox(std::vector<_TSTRING> i_list_of_names);
  void SetFunSetListBoxSelection(int i_selected_index);
  void SetFirmwareName(_TSTRING i_name);
  void SetModified(bool i_modified);
  void SetFirmwareCRCs(unsigned int crc_stored_in_fw, unsigned int actual_crc_of_fw);

  void UpdateOpenedCharts(void);
  bool IsProgrammeOnlyCode(void);

  std::auto_ptr<CParamDeskDlg> mp_ParamDeskDlg;
  std::auto_ptr<CFirmwareModeContextMenuManager> mp_ContextMenuManager;

 public: //установка обработчиков событий от меню
  void setOnBootLoaderInfo(EventHandler OnFunction) {m_OnBootLoaderInfo = OnFunction;};
  void setOnReadEepromToFile(EventHandler OnFunction) {m_OnReadEepromToFile = OnFunction;};
  void setOnWriteEepromFromFile(EventHandler OnFunction) {m_OnWriteEepromFromFile = OnFunction;};
  void setOnReadFlashToFile(EventHandler OnFunction) {m_OnReadFlashToFile = OnFunction;};
  void setOnWriteFlashFromFile(EventHandler OnFunction) {m_OnWriteFlashFromFile = OnFunction;};
  void setOnOpenFlashFromFile(EventHandler OnFunction) {m_OnOpenFlashFromFile = OnFunction;}
  void setOnFWInformationTextChanged(EventHandler OnFunction) {m_OnFWInformationTextChanged = OnFunction;}
  void setOnSaveFlashToFile(EventHandler OnFunction) {m_OnSaveFlashToFile = OnFunction;}
  void setIsFirmwareOpened(EventResult IsFunction) {m_IsFirmwareOpened = IsFunction;}
  void setOnMapChanged(EventWithCode OnFunction) {m_OnMapChanged = OnFunction;}
  void setOnFunSetSelectionChanged(EventWithCode OnFunction) {m_OnFunSetSelectionChanged = OnFunction;}
  void setOnFunSetNamechanged(EventWithCodeAndString OnFunction) {m_OnFunSetNamechanged = OnFunction;}
  void setOnImportDataFromAnotherFW(EventHandler OnFunction) {m_OnImportDataFromAnotherFW = OnFunction;}	
  void setOnReadFlashFromSECU(EventHandler OnFunction) {m_OnReadFlashFromSECU = OnFunction;}
  void setOnWriteFlashToSECU(EventHandler OnFunction) {m_OnWriteFlashToSECU = OnFunction;}
  void setOnImportDataFromSECU3(EventHandler OnFunction) {m_OnImportDataFromSECU3 = OnFunction;}
  void setOnImportMapsFromMPSZ(EventHandler OnFunction) {m_OnImportMapsFromMPSZ = OnFunction;}
  void setOnExportMapsToMPSZ(EventHandler OnFunction) {m_OnExportMapsToMPSZ = OnFunction;}
  void setOnFirmwareInfo(EventHandler OnFunction) {m_OnFirmwareInfo = OnFunction;}
  //... от кнопок и чек боксов
  void setOnBLStartedEmergency(EventHandler OnFunction) {m_OnBLStartedEmergency = OnFunction;}

// Implementation
 protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnUpdatePopupMenu_bl(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_file1(CCmdUI* pCmdUI);
  afx_msg void OnUpdatePopupMenu_app(CCmdUI* pCmdUI);
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  afx_msg void OnFirmwareSupportViewStartMap();
  afx_msg void OnFirmwareSupportViewIdleMap();
  afx_msg void OnFirmwareSupportViewWorkMap();
  afx_msg void OnFirmwareSupportViewTempMap();
  afx_msg void OnUpdateFirmwareSupportViewStartMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewIdleMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewWorkMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewTempMap(CCmdUI* pCmdUI);
  afx_msg void OnUpdateFirmwareSupportViewAttenuatorMap(CCmdUI* pCmdUI);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnDestroy();
  afx_msg void OnBootLoaderInfo();
  afx_msg void OnReadEepromToFile();
  afx_msg void OnWriteEepromFromFile();
  afx_msg void OnReadFlashToFile();
  afx_msg void OnWriteFlashFromFile();
  afx_msg void OnUpdateBLStartedEmergency(CCmdUI* pCmdUI);
  afx_msg void OnFirmwareSupportBlStartedEmergency();	
  afx_msg void OnOpenFlashFromFile();
  afx_msg void OnSaveFlashToFile();
  afx_msg void OnChangeFirmwareSupportFwInformation();
  afx_msg void OnUpdateFirmwareControls(CCmdUI* pCmdUI);
  afx_msg void OnChangeFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnEndLabelEditFirmwareSupportFunsetList(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnImportDataFromAnotherFW();
  afx_msg void OnReadFlashFromSECU();
  afx_msg void OnWriteFlashToSECU();
  afx_msg void OnImportDataFromSECU3();
  afx_msg void OnUpdateProgOnlyCode(CCmdUI* pCmdUI);
  afx_msg void OnImportMapsFromMPSZ();
  afx_msg void OnExportMapsToMPSZ();
  afx_msg void OnFirmwareSupportViewAttenuatorMap();		
  afx_msg void OnWirmwareInfo();
  DECLARE_MESSAGE_MAP()

 private:
  CListCtrl	m_funset_listbox;
  CButton	m_bl_started_emergency;
  CButton	m_view_work_map_btn;
  CButton	m_view_temp_map_btn;
  CButton	m_view_start_map_btn;
  CButton	m_view_idle_map_btn;
  CButton m_prog_only_code_checkbox;
  CButton m_view_attenuator_map_btn;
  CEdit   m_fw_information_edit;
  CEdit m_fw_name;
  CStatic m_fw_crc;
  CStatic m_modification_flag;

 private: 
  EventWithCode m_OnMapChanged;
  EventWithCode m_OnFunSetSelectionChanged;
  EventHandler  m_OnBootLoaderInfo;
  EventHandler  m_OnReadEepromToFile;
  EventHandler  m_OnWriteEepromFromFile;
  EventHandler  m_OnReadFlashToFile;
  EventHandler  m_OnWriteFlashFromFile;
  EventHandler  m_OnBLStartedEmergency;
  EventHandler  m_OnOpenFlashFromFile;
  EventHandler  m_OnSaveFlashToFile;
  EventHandler  m_OnFWInformationTextChanged;
  EventResult   m_IsFirmwareOpened;
  EventWithCodeAndString m_OnFunSetNamechanged; 
  EventHandler  m_OnImportDataFromAnotherFW;
  EventHandler  m_OnReadFlashFromSECU;
  EventHandler  m_OnWriteFlashToSECU;
  EventHandler  m_OnImportDataFromSECU3;
  EventHandler  m_OnImportMapsFromMPSZ;
  EventHandler  m_OnExportMapsToMPSZ;
  EventHandler  m_OnFirmwareInfo;
  
  //for C - functions
  int m_work_map_chart_state;
  int m_temp_map_chart_state;
  int m_start_map_chart_state;
  int m_idle_map_chart_state;
  int m_attenuator_map_chart_state;
 
  HWND m_start_map_wnd_handle;
  HWND m_idle_map_wnd_handle;
  HWND m_work_map_wnd_handle;
  HWND m_temp_map_wnd_handle;
  HWND m_attenuator_map_wnd_handle;
   
  int m_attenuator_table_slots[128];

  static void __cdecl OnChangeStartMap(void* i_param);
  static void __cdecl OnCloseStartMap(void* i_param);
  static void __cdecl OnChangeIdleMap(void* i_param);
  static void __cdecl OnCloseIdleMap(void* i_param);
  static void __cdecl OnChangeWorkMap(void* i_param);
  static void __cdecl OnCloseWorkMap(void* i_param);
  static void __cdecl OnChangeTempMap(void* i_param);
  static void __cdecl OnCloseTempMap(void* i_param);
  static void __cdecl OnChangeAttenuatorTable(void* i_param);
  static void __cdecl OnCloseAttenuatorTable(void* i_param);

  bool IsFirmwareOpened(void) 
  {
   if (m_IsFirmwareOpened)
    return m_IsFirmwareOpened();
   return false; 
  };

  bool m_is_bl_started_emergency_available;
  bool m_is_bl_items_available;
  bool m_is_app_items_available;

  ///////////////////////////////////////////////////////
  float m_start_map_active[16];
  float m_start_map_original[16];

  float m_idle_map_active[16];
  float m_idle_map_original[16];

  float m_work_map_active[16][16];
  float m_work_map_original[16][16];
	
  float m_temp_map_active[16];
  float m_temp_map_original[16];

  float m_attenuator_map_active[128];
  float m_attenuator_map_original[128];
  ///////////////////////////////////////////////////////

  void _RegisterHotKeys(void);
  std::auto_ptr<CHotKeysToCmdRouter> m_hot_keys_supplier;
};

