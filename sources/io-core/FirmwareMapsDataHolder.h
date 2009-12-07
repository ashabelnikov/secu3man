
#pragma once

#include "common/unicodesupport.h"
#include <vector>

//определяем количество узлов интерполяции для каждой функции
#define F_WRK_POINTS_F         16  
#define F_WRK_POINTS_L         16  
#define F_TMP_POINTS           16
#define F_STR_POINTS           16                            
#define F_IDL_POINTS           16     
#define F_NAME_SIZE            16

//количество наборов характеристик хранимых в памяти программ
#define TABLES_NUMBER          8   

struct SECU3FWMapsItem
{
  float f_str[F_STR_POINTS];          // функция УОЗ на старте
  float f_idl[F_IDL_POINTS];          // функция УОЗ для ХХ
  float f_wrk[F_WRK_POINTS_L * F_WRK_POINTS_F];     // основная функция УОЗ (3D)
  float f_tmp[F_TMP_POINTS];  // функция коррект. УОЗ по температуре
  _TSTRING name;              // имя набора характеристик
};

//Аппаратно независимое представление данных таблиц хранимых в прошивке SECU-3
struct FWMapsDataHolder
{
  SECU3FWMapsItem  maps[TABLES_NUMBER];

  std::vector<_TSTRING> GetListOfNames(void)
  {
   std::vector<_TSTRING> list;
   for (int i = 0; i < TABLES_NUMBER; i++)
    list.push_back(maps[i].name);
   return list;
  };
};
