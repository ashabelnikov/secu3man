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

/** \file EnumPorts.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "EnumPorts.h"
#include "common\UnicodeSupport.h"
#include <vector>

#include "atlbase.h"
#include "setupapi.h"
#include "winioctl.h"


CEnumPorts::CEnumPorts()
{
 m_portList.reserve(16);
}

CEnumPorts::~CEnumPorts()
{
 //empty
}

bool CEnumPorts::Query(void)
{
 return _QueryPortList(GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, DIGCF_PRESENT);
 //return _QueryPortList(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
}

void CEnumPorts::getPortsList(PortDescList_t& portList)
{
 portList.clear();
 for(size_t i = 0; i < m_portList.size(); ++i)
 {
  const TCHAR* str = m_portList[i].first.c_str();
  if (_tcsnicmp(str, _T("COM"), 3) != 0)
   continue; //not a COM port
  
  str+= 3;
  size_t len = _tcslen(str);
  bool digits = true;
  for(size_t j = 0; j < len; ++j)
  {
   if (0 == _istdigit(str[j]))
   {
    digits = false;
    break;
   }
  }
  if (digits)
   portList.push_back(m_portList[i]);
 }
}

namespace {
bool _QueryDeviceName(ATL::CRegKey& key, LPCTSTR i_name, _TSTRING& o_value)
{
 ULONG charNum = 0;
 //When the method returns, charNum contains the size, in TCHARs, of the string retrieved, including a terminating null character.
 if (ERROR_SUCCESS != key.QueryStringValue(i_name, NULL, &charNum)) //get required size of buffer
  return false;

 o_value.resize(charNum);

 if (ERROR_SUCCESS != key.QueryStringValue(i_name, &o_value[0], &charNum)) //read value
  return false;

 if (charNum > 0)
  o_value.resize(charNum - 1); //remove terminating character

 return true; //OK
}


bool _QueryDeviceDescr(HDEVINFO devInfoSet, SP_DEVINFO_DATA& devInfoData, _TSTRING& o_description)
{
 DWORD regDataType = 0, reqSize = 0;
 if (!SetupDiGetDeviceRegistryProperty(devInfoSet, &devInfoData, SPDRP_DEVICEDESC, &regDataType, NULL, 0, &reqSize))
 {
  if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
   return false;
 }
 size_t charSize = reqSize / sizeof(TCHAR);
 o_description.resize(charSize);
 if (!SetupDiGetDeviceRegistryProperty(devInfoSet, &devInfoData, SPDRP_DEVICEDESC, &regDataType, (PBYTE)&o_description[0], reqSize, &reqSize))
  return false;
 if (regDataType != REG_SZ)
  return false;

 if (charSize > 0 && o_description[charSize - 1] == _T('\0'))
  o_description.resize(charSize - 1); //remove unnecessary null terminating character

 return true; //OK
}

} //ns

bool CEnumPorts::_QueryPortList(const GUID& guid, DWORD flags)
{
  HDEVINFO devInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, flags);
  if (devInfoSet == INVALID_HANDLE_VALUE)
   return false; //failed

  SP_DEVINFO_DATA devInfo;
  ZeroMemory(&devInfo, sizeof(SP_DEVINFO_DATA));

  int memberIndex = 0;
  m_portList.clear();
  for (;;)
  {
   devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
   if (!SetupDiEnumDeviceInfo(devInfoSet, memberIndex++, &devInfo))
    break; //end

   ATL::CRegKey deviceKey;
   deviceKey.Attach(SetupDiOpenDevRegKey(devInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE));
   if (deviceKey != INVALID_HANDLE_VALUE)
   {
    _TSTRING portName, portDesc;
    if (_QueryDeviceName(deviceKey, _T("PortName"), portName) && _QueryDeviceDescr(devInfoSet, devInfo, portDesc))
     m_portList.push_back(std::make_pair(portName, portDesc));
   }
  }

  SetupDiDestroyDeviceInfoList(devInfoSet); //The caller of SetupDiGetClassDevs must delete the returned device information set when it is no longer needed

  return true; //OK
}

