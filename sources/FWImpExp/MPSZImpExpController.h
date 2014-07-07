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

#pragma once

#include "io-core/FirmwareMapsDataHolder.h"

class CMapImpExpDlg;
class MPSZFileDataIO;

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
  void OnCurrentListNameChanged(int item, CString text);
  void OnOtherListNameChanged(int item, CString text);
  bool IsExchangeButtonAllowed(void);

 private:
  FWMapsDataHolder* mp_fwd;
  CMapImpExpDlg* mp_view;
  MPSZFileDataIO* mp_mpsz_io;
  _TSTRING m_mpsz_file_name;
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
  void OnCurrentListNameChanged(int item, CString text);
  void OnOtherListNameChanged(int item, CString text);
  bool IsExchangeButtonAllowed(void);

 private:
  FWMapsDataHolder* mp_fwd;
  CMapImpExpDlg* mp_view;
  MPSZFileDataIO* mp_mpsz_io;
  _TSTRING m_mpsz_file_name;
};
