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

/** \file FirmwareMasterCntr.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <set>

class CFirmwareMasterDlg;

class AFX_EXT_CLASS FirmwareMasterCntr
{
 public:
  FirmwareMasterCntr();
  ~FirmwareMasterCntr();

  int  DoLoad(const _TSTRING& procStr, BYTE* p_data, size_t size, _TSTRING* o_file_name = NULL);

  void OnOkPressed(void);
  void OnCancelPressed(void);
  void OnViewActivate(void);
  void OnChangeFwmCheck(int id);
  void OnChangeUnit(void);
  void OnChangeSync(void);

  bool GetStatus(void);

 private:
  void _BuildOptList(std::set<_TSTRING> &opts);
  bool _LoadFirmware(const std::set<_TSTRING>& opts);
  bool _FindMatchForOpts(const _TSTRING& name, const std::set<_TSTRING>& opts);
  bool _StoreResult(const _TSTRING& name, BYTE* buff, size_t size);

  CFirmwareMasterDlg* mp_view;
  BYTE* mp_data;
  size_t m_size;
  _TSTRING* mp_file_name;
  _TSTRING m_procStr;
  TCHAR m_current_directory[MAX_PATH+1];
  bool m_status;
  std::set<_TSTRING> m_allopts;
};
