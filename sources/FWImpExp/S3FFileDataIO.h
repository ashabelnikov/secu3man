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

/** \file S3FFileDataIO.h
 * \author Alexey A. Shabelnikov
 */

#pragma once

#include <vector>
#include "common/unicodesupport.h"
#include "io-core/FirmwareMapsDataHolder.h"

struct S3FFileHdr;
class AFX_EXT_CLASS S3FFileDataIO
{
 public:
  S3FFileDataIO(bool sepmaps = true);
 ~S3FFileDataIO();

  bool Load(const _TSTRING i_file_name);
  bool Save(const _TSTRING i_file_name);

  const FWMapsDataHolder& GetData() const;
  FWMapsDataHolder& GetDataLeft();

  bool IsFileIntegrityOk(void) const;
  unsigned short GetVersion(void) const;
  bool HasSeparateMaps(void) const;

 private:  
  bool _ReadData(const BYTE*, const S3FFileHdr*);
  bool _ReadData_v0102(const BYTE*, const S3FFileHdr*); //used for reading of versions <= 1.02

  FWMapsDataHolder m_data;
  bool m_file_crc_ok;        //indicates that CRC of the loaded file is Ok
  unsigned short m_version;  //version of loaded file
  bool m_sepmaps;            //indicates presence of separate maps
};
