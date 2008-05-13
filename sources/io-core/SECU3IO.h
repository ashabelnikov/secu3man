 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/


//Представление данных передаваемых между SECU и менеджером
#include <string>

#pragma once

namespace SECU3IO
{
//описывает все входы системы - их производные и интегральные величины
 struct SensorDat
 {
  int frequen;                                        //частота вращения коленвала (усредненная)
  float pressure;                                     //давление во впускном коллекторе (усредненное)
  float voltage;                                      //напряжение бортовой сети (усредненное)
  float temperat;                                     //температура охлаждающей жидкости (усредненная)
  float adv_angle;                                    //текущий УОЗ
  unsigned char carb;                                 //состояние концевика карбюратора 
  unsigned char gas;                                  //состояние газового клапана 
  unsigned char air_flow;                             //расход воздуха
  unsigned char ephh_valve;                           //состояние клапана ЭПХХ
 };
 
 struct FnNameDat
 {
  unsigned char tables_num;
  unsigned char index;
  std::string   name;
 };

 struct StartrPar
 {
   int  starter_off;                                //порог выключения стартера (обороты)
   int  smap_abandon;                               //обороты перехода с пусковой карты на рабочую   
 };
 
struct AnglesPar
{
  float  max_angle;
  float  min_angle;
  float  angle_corr;
};


struct FunSetPar
{
  unsigned char fn_benzin;
  unsigned char fn_gas;
  float map_grad;
  float press_swing;
};

struct IdlRegPar

{
  unsigned char idl_regul;
  float  ifac1;
  float  ifac2;
  int  MINEFR;
  int  idl_turns;
};


struct CarburPar
{
  int  ephh_lot;
  int  ephh_hit;
  unsigned char carb_invers;
};

struct TemperPar
{
  unsigned char tmp_use;
  float  vent_on;
  float  vent_off;
};

};