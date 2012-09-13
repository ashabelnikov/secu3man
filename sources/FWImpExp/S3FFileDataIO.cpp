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

#include "stdafx.h"
#include "resource.h"
#include "S3FFileDataIO.h"

#include <vector>
#include "common/MathHelpers.h"
#include "io-core/FirmwareMapsDataHolder.h"


/////////////////////////////////////////////////////////////////////////////////////
S3FFileDataIO::S3FFileDataIO()
{
 //empty
};

/////////////////////////////////////////////////////////////////////////////////////
S3FFileDataIO::~S3FFileDataIO()
{
 //empty
};

/////////////////////////////////////////////////////////////////////////////////////
bool S3FFileDataIO::Load(const _TSTRING i_file_name)
{
 CFile file;
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(),CFile::modeRead,&ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }

 BYTE rawdata[65536];
// int size = ;
// if (file.GetLength()!=size)
// {
//  file.Close();
//  return false;  //некорректный размер файла
// }

// file.Read(rawdata, size);
 file.Close();

 return true;
}

/////////////////////////////////////////////////////////////////////////////////////
bool S3FFileDataIO::Save(const _TSTRING i_file_name)
{
 CFile file;
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(), CFile::modeWrite|CFile::modeCreate, &ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }

// BYTE rawdata[65536];
// file.Write(rawdata, size);
 file.Close();
 return true;
}

const FWMapsDataHolder& S3FFileDataIO::GetData()
{
 return m_data;
}

FWMapsDataHolder& S3FFileDataIO::GetDataLeft()
{
 return m_data;
}

int S3FFileDataIO::GetMapSetsNumber(void)
{
 return 0;
}
