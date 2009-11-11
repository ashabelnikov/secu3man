
#pragma once

#include "io-core\FirmwareMapsDataHolder.h"
#include "MPSZFileDataIO.h"

class CMapImpExpDlg;

class AFX_EXT_CLASS MPSZImportController
{
 public:
  MPSZImportController(FWMapsDataHolder* ip_fwd);
  ~MPSZImportController();

  int  DoImport(void);

  void OnOkPressed(void);
  void OnCancelPressed(void);
  void OnExchangePressed(void); 
  void OnViewActivate(void);
  bool IsExchangeButtonAllowed(void);

 private:
  CMapImpExpDlg* mp_view;
  _TSTRING m_mpsz_file_name;
  FWMapsDataHolder* mp_fwd;
  MPSZFileDataIO m_mpsz_io;
};

class AFX_EXT_CLASS MPSZExportController
{
 public:
  MPSZExportController(FWMapsDataHolder* ip_fwd);
  ~MPSZExportController();

  int  DoExport(void);

  void OnOkPressed(void);
  void OnCancelPressed(void);
  void OnExchangePressed(void);
  void OnViewActivate(void);
  bool IsExchangeButtonAllowed(void);

 private:
  CMapImpExpDlg* mp_view;
  _TSTRING m_mpsz_file_name;
  FWMapsDataHolder* mp_fwd;
  MPSZFileDataIO m_mpsz_io;
};
