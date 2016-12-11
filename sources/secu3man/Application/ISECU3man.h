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

/** \file ISECU3man.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

class CAppSettingsManager;
class CCommunicationManager;
class CMainFrameManager;
class LogWriter;

class ISECU3Man
{
 public:
  static ISECU3Man* GetSECU3Man()
  {
   ISECU3Man* p_application = (dynamic_cast<ISECU3Man*>(AfxGetApp()));
   _ASSERTE(p_application);
   return p_application;
  };

  virtual CAppSettingsManager*    GetAppSettingsManager(void) const = 0;
  virtual CCommunicationManager*  GetCommunicationManager(void) const = 0;
  virtual CMainFrameManager*      GetMainFrameManager(void) const = 0;
  virtual LogWriter*              GetLogWriter(void) const = 0;
};
