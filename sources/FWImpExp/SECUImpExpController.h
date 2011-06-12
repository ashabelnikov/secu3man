/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

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

#include <vector>
#include "common/fastdelegate.h"

class CFirmwareDataMediator;
class CMapImpExpDlg;
struct FWMapsDataHolder;

class AFX_EXT_CLASS SECU3ImportController
{
  //<buffer, list_of_allowed_sizes, selected_size, filename, title, ret_value>
  typedef fastdelegate::FastDelegate6<BYTE*, const std::vector<int>&, _TSTRING*, int*, _TSTRING*, _TSTRING*, bool> Delegate1;
 public:
  SECU3ImportController(FWMapsDataHolder* ip_fwd);
  ~SECU3ImportController();

  int  DoImport(void);

  void OnOkPressed(void);
  void OnCancelPressed(void);
  void OnExchangePressed(void);
  void OnViewActivate(void);
  void OnCurrentListNameChanged(int item, CString text);
  void OnOtherListNameChanged(int item, CString text);
  bool IsExchangeButtonAllowed(void);

  void setFileReader(Delegate1 delegate);

 private:
  FWMapsDataHolder* mp_fwd;
  CMapImpExpDlg* mp_view;
  CFirmwareDataMediator* m_fwdm;
  _TSTRING m_secu3_file_name;
  Delegate1 m_FileReader;
};

class AFX_EXT_CLASS SECU3ExportController
{
  //<buffer, list_of_allowed_sizes, selected_size, filename, title, file_path, ret_value>
  typedef fastdelegate::FastDelegate6<BYTE*, const std::vector<int>&, _TSTRING*, int*, _TSTRING*, _TSTRING*, bool> Delegate1;
 public:
  SECU3ExportController(FWMapsDataHolder* ip_fwd);
  ~SECU3ExportController();

  int  DoExport(void);

  void OnOkPressed(void);
  void OnCancelPressed(void);
  void OnExchangePressed(void);
  void OnViewActivate(void);
  void OnCurrentListNameChanged(int item, CString text);
  void OnOtherListNameChanged(int item, CString text);
  bool IsExchangeButtonAllowed(void);

  void setFileReader(Delegate1 delegate);

 private:
  FWMapsDataHolder* mp_fwd;
  CMapImpExpDlg* mp_view;
  CFirmwareDataMediator* m_fwdm;
  _TSTRING m_secu3_file_name;
  _TSTRING m_secu3_file_path;
  Delegate1 m_FileReader;
};
