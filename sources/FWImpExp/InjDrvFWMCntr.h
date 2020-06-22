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

/** \file InjDrvFWMCntr.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <set>

class CInjDrvFWMDlg;

class AFX_EXT_CLASS InjDrvFWMCntr
{
 public:
  InjDrvFWMCntr();
  ~InjDrvFWMCntr();

  int  DoLoad(BYTE* p_data, size_t size);

  void OnOkPressed(void);
  void OnSavePressed(void);
  void OnCancelPressed(void);
  void OnViewActivate(void);
  void OnChangeFwmCheck(int id);

  bool GetStatus(void);

 private:
  void _BuildOptList(std::set<_TSTRING> &opts, int procId);
  bool _LoadFirmware(const std::set<_TSTRING>& opts, int procId, bool save);
  bool _FindMatchForOpts(const _TSTRING& name, const std::set<_TSTRING>& opts);
  bool _StoreResult(const _TSTRING& name, BYTE* buff, size_t size, bool save);

  CInjDrvFWMDlg* mp_view;
  BYTE* mp_data;
  size_t m_size;
  TCHAR m_current_directory[MAX_PATH+1];
  bool m_status;
  std::set<_TSTRING> m_allopts;
};
