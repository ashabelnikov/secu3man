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

/** \file s3lrecord.h
 * \author Alexey A. Shabelnikov
 */

namespace s3l {

/**Data structure which represents one record in binary format log file*/
struct s3lRecord
{
 BYTE  hh;
 BYTE  mm;
 BYTE  ss;
 WORD  ms;
 WORD  frequen;
 float adv_angle;
 float pressure;
 float voltage;
 float temperat;
 float knock_k;
 float knock_retard;
 BYTE  air_flow;
 WORD flags;             //bits of flags
 float tps;
 float add_i1;
 float add_i2;
 float choke_pos;
 float gasdose_pos;
 float speed;
 float distance;
 float inj_ffd;
 float inj_fff;
 float air_temp;
 float strt_aalt;
 float idle_aalt;
 float work_aalt;
 float temp_aalt;
 float airt_aalt;
 float idlreg_aac;
 float octan_aac;
 float lambda_corr;
 float inj_pw;
 int   tpsdot;
 float map2;
 float tmp2;
 float mapd;
 float afr;
 float load;
 float baro_press;
 float inj_tim_begin;
 float inj_tim_end;
 float grts;
 float ftls;
 float egts;
 float ops; 
 float inj_duty;
 float rigid_arg;
 int   rxlaf;
 float maf;
 float vent_duty;
 BYTE  uniout_flags; 
 int   mapdot; 
 float fts;
 float cons_fuel;                       
 float lambda_corr2;
 float afr2;
 float afrmap;
 float tchrg;
 BYTE  log_marks;
 WORD  service_flags;
 DWORD ce_errors;
};

}
