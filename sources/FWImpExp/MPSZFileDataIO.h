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
#include "common\unicodesupport.h"

//Абстракции над данными таблиц MPSZ

#define MPSZ_START_MAP_SIZE  16
#define MPSZ_IDLE_MAP_SIZE   16
#define MPSZ_WORK_MAP_SIZE_L 16
#define MPSZ_WORK_MAP_SIZE_F 16
#define MPSZ_MAPS_NAME_SIZE  16    //LO level value
#define MPSZ_NUMBER_OF_MAPS  16    //not actual value for some files, e.g *.mpz

#define MPSZ_NUMBER_OF_MAPS_IN_MPZ_FILE 1  //damn declaration

/////////////////////////////////////////////////////////////////////////////////////////
struct MPSZMapsDataItem
{
 float f_str[MPSZ_START_MAP_SIZE];
 float f_idl[MPSZ_IDLE_MAP_SIZE];
 float f_wrk[MPSZ_WORK_MAP_SIZE_L * MPSZ_WORK_MAP_SIZE_F];
 _TSTRING name;
};

struct MPSZMapsDataHolder
{
 MPSZMapsDataItem maps[MPSZ_NUMBER_OF_MAPS];
 int m_actual_sets_num;

 MPSZMapsDataHolder() : m_actual_sets_num(0)
 {
  for(int i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
  {
   memset(maps[i].f_str,0,sizeof(float) * MPSZ_START_MAP_SIZE);
   memset(maps[i].f_idl,0,sizeof(float) * MPSZ_IDLE_MAP_SIZE);
   memset(maps[i].f_wrk,0,sizeof(float) * MPSZ_WORK_MAP_SIZE_L * MPSZ_WORK_MAP_SIZE_F);
  }
 };

 std::vector<_TSTRING> GetListOfNames(void) const;
};

/////////////////////////////////////////////////////////////////////////////////////////
class MPSZFileDataIO
{
 public:
  MPSZFileDataIO()  {};
  ~MPSZFileDataIO() {};

  enum EFileTypes
  {
   FILE_TYPE_MPX = 0,
   FILE_TYPE_MPZ
  };

  bool Load(const _TSTRING i_file_name, MPSZFileDataIO::EFileTypes i_file_type);
  bool Save(const _TSTRING i_file_name, MPSZFileDataIO::EFileTypes i_file_type);
  const MPSZMapsDataHolder& GetData();
  MPSZMapsDataHolder& GetDataLeft();
  void SetActualSetsNumber(int i_number) { m_data.m_actual_sets_num = i_number;};
  int GetActualSetsNumber(void) { return m_data.m_actual_sets_num;};

 private:
  MPSZMapsDataHolder m_data;
};

/////////////////////////////////////////////////////////////////////////////////////////
class MPSZDataBase
{
 public:
  MPSZDataBase() {};
  virtual ~MPSZDataBase() {};

  //binary -> abstract
  virtual void operator()(const BYTE* ip_rawdata, MPSZMapsDataHolder* op_data) = 0;

  //abstract -> bynary
  virtual void operator()(const MPSZMapsDataHolder* ip_data, BYTE* op_rawdata) = 0;

  virtual int GetRequiredRawSize(void)  = 0;

  virtual int GetActualSetsNumber(void) = 0;
};

class MPSZDataMPX_IO : public MPSZDataBase
{
 public:
  MPSZDataMPX_IO() {};
  virtual ~MPSZDataMPX_IO() {};

  enum
  {
   ACTUAL_SETS_NUMBER = MPSZ_NUMBER_OF_MAPS,
   SIZE_OF_RAW_DATA   = 9216
  };

  //binary -> abstract
  virtual void operator()(const BYTE* ip_rawdata, MPSZMapsDataHolder* op_data);

  //abstract -> bynary
  virtual void operator()(const MPSZMapsDataHolder* ip_data, BYTE* op_rawdata);

  virtual int GetRequiredRawSize(void)  {return SIZE_OF_RAW_DATA;};

  virtual int GetActualSetsNumber(void) {return ACTUAL_SETS_NUMBER;};
};

class MPSZDataMPZ_IO : public MPSZDataBase
{
 public:
  MPSZDataMPZ_IO() {};
  virtual ~MPSZDataMPZ_IO() {};

  enum
  {
   ACTUAL_SETS_NUMBER = MPSZ_NUMBER_OF_MAPS_IN_MPZ_FILE,
   SIZE_OF_RAW_DATA   = 304
  };

  virtual void operator()(const BYTE* ip_rawdata, MPSZMapsDataHolder* op_data);
  virtual void operator()(const MPSZMapsDataHolder* ip_data, BYTE* op_rawdata);

  virtual int GetRequiredRawSize(void)  {return SIZE_OF_RAW_DATA;};

  virtual int GetActualSetsNumber(void) {return ACTUAL_SETS_NUMBER;};
};

/////////////////////////////////////////////////////////////////////////////////////////
