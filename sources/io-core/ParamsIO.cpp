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

/** \file ParamsIO.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "ParamsIO.h"
#include "SECU3ParametersDef.h"
#include "ufcodes.h"
#include "SECU3IO.h"
#include "common/MathHelpers.h"
#include "Magnitude.h"
#include "BitMask.h"
#include "ControlApp.h"
#include "PlatformParamHolder.h"

using namespace SECU3IO;

ParamsIO::ParamsIO()
: m_period_distance(0.166666f)//for speed sensor calculations
, m_quartz_frq(20000000)      //default clock is 20mHz
{
 //empty
}

ParamsIO::~ParamsIO()
{
 //empty
}

bool ParamsIO::SetDefParamValues(BYTE i_descriptor, const void* ip_values)
{
 using namespace SECU3IO;

 //получаем адрес структуры параметров
 SECU3IO::params_t* p_params = GetParamsPtr();

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
 {
  case TEMPER_PAR:
   {
    TemperPar* p_in = (TemperPar*)ip_values;
    p_params->tmp_use  = p_in->tmp_use;
    p_params->vent_pwm = p_in->vent_pwm;
    p_params->cts_use_map = p_in->cts_use_map;
    p_params->vent_on  = MathHelpers::Round(p_in->vent_on * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->vent_off = MathHelpers::Round(p_in->vent_off * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->vent_pwmfrq = MathHelpers::Round((1.0/p_in->vent_pwmfrq) * 524288.0);
    //air conditioner related:
    p_params->cond_pvt_on = MathHelpers::Round(((float)p_in->cond_pvt_on) / ADC_DISCRETE);
    p_params->cond_pvt_off = MathHelpers::Round(((float)p_in->cond_pvt_off) / ADC_DISCRETE);    
   }
   break;
  case CARBUR_PAR:
   {
    CarburPar* p_in = (CarburPar*)ip_values;
    p_params->ephh_hit    = p_in->ephh_hit;
    p_params->ephh_lot    = p_in->ephh_lot;
    p_params->carb_invers = p_in->carb_invers;
    p_params->epm_on_threshold = MathHelpers::Round(p_in->epm_ont * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->ephh_hit_g  = p_in->ephh_hit_g;
    p_params->ephh_lot_g  = p_in->ephh_lot_g;
    p_params->shutoff_delay = MathHelpers::Round(p_in->shutoff_delay * 100); //переводим в десятки мс
    p_params->tps_threshold = MathHelpers::Round(p_in->tps_threshold * TPS_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->fuelcut_map_thrd = MathHelpers::Round(p_in->fuelcut_map_thrd * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->fuelcut_cts_thrd = MathHelpers::Round(p_in->fuelcut_cts_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->revlim_lot  = p_in->revlim_lot;
    p_params->revlim_hit  = p_in->revlim_hit;
   }
   break;
  case IDLREG_PAR:
   {
    IdlRegPar* p_in = (IdlRegPar*)ip_values;
    WRITEBIT8(p_params->idl_flags, 0, p_in->idl_regul);
    WRITEBIT8(p_params->idl_flags, 1, p_in->use_regongas);
    WRITEBIT8(p_params->idl_flags, 2, p_in->closed_loop);   
    WRITEBIT8(p_params->idl_flags, 3, p_in->preg_mode);   
    WRITEBIT8(p_params->idl_flags, 4, p_in->idl_useiacclongas);
    p_params->idling_rpm = p_in->idling_rpm;
    p_params->MINEFR     = p_in->MINEFR;
    p_params->ifac1      = MathHelpers::Round(p_in->ifac1 * 256.0f);
    p_params->ifac2      = MathHelpers::Round(p_in->ifac2 * 256.0f);
    p_params->idlreg_min_angle = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLIER);
    p_params->idlreg_max_angle = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLIER);
    p_params->idlreg_turn_on_temp = MathHelpers::Round(p_in->turn_on_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    //closed loop parameters:
    p_params->idl_to_run_add = MathHelpers::Round(p_in->idl_to_run_add * 2.0f);
    p_params->rpm_on_run_add = MathHelpers::Round(p_in->rpm_on_run_add / 10.0f);
    p_params->idl_reg_p = MathHelpers::Round(p_in->idl_reg_p * 256.0f);
    p_params->idl_reg_i = MathHelpers::Round(p_in->idl_reg_i * 256.0f);
    p_params->idl_coef_thrd1 = MathHelpers::Round((p_in->idl_coef_thrd1 - 1.0f) * 128.0f);
    p_params->idl_coef_thrd2 = MathHelpers::Round((p_in->idl_coef_thrd2 - 1.0f) * 128.0f);
    p_params->idl_intrpm_lim = MathHelpers::Round(p_in->idl_intrpm_lim / 10.0f);
    p_params->idl_map_value = MathHelpers::Round(p_in->idl_map_value * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->idl_iacminpos = MathHelpers::Round(p_in->idl_iacminpos * 2.0f);
    p_params->idl_iacmaxpos = MathHelpers::Round(p_in->idl_iacmaxpos * 2.0f);
   }
   break;
  case ANGLES_PAR:
   {
    AnglesPar* p_in = (AnglesPar*)ip_values;
    p_params->angle_corr = MathHelpers::Round(p_in->angle_corr * ANGLE_MULTIPLIER);
    p_params->max_angle  = MathHelpers::Round(p_in->max_angle * ANGLE_MULTIPLIER);
    p_params->min_angle  = MathHelpers::Round(p_in->min_angle * ANGLE_MULTIPLIER);
    p_params->angle_dec_speed = MathHelpers::Round(p_in->dec_speed * ANGLE_MULTIPLIER);
    p_params->angle_inc_speed = MathHelpers::Round(p_in->inc_speed * ANGLE_MULTIPLIER);
    p_params->zero_adv_ang = p_in->zero_adv_ang;
   }
   break;
  case FUNSET_PAR:
   {
    FunSetPar* p_in = (FunSetPar*)ip_values;
    p_params->fn_benzin = p_in->fn_benzin;
    p_params->fn_gas    = p_in->fn_gas;
    p_params->map_lower_pressure = MathHelpers::Round(p_in->map_lower_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->map_upper_pressure = MathHelpers::Round(p_in->map_upper_pressure * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->map_curve_offset = MathHelpers::Round(p_in->map_curve_offset / ADC_DISCRETE);
    p_params->map_curve_gradient = MathHelpers::Round(128.0f * p_in->map_curve_gradient * MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * ADC_DISCRETE);
    p_params->tps_curve_offset = MathHelpers::Round(p_in->tps_curve_offset / ADC_DISCRETE);
    p_params->tps_curve_gradient = MathHelpers::Round(128.0f * p_in->tps_curve_gradient * (TPS_PHYSICAL_MAGNITUDE_MULTIPLIER*64) * ADC_DISCRETE);
    p_params->load_src_cfg = (p_params->load_src_cfg & 0xF0) | p_in->load_src_cfg;
    int uni_gas = (p_in->uni_gas==UNI_OUTPUT_NUM) ? 0xF : p_in->uni_gas;
    int uni_benzin = (p_in->uni_benzin==UNI_OUTPUT_NUM) ? 0xF : p_in->uni_benzin;
    p_params->mapsel_uni = MAKEBYTE(uni_gas, uni_benzin);
   }
   break;
  case STARTR_PAR:
   {
    StartrPar* p_in = (StartrPar*)ip_values;
    p_params->starter_off  = p_in->starter_off;
    p_params->smap_abandon = p_in->smap_abandon;
    p_params->inj_cranktorun_time = MathHelpers::Round(p_in->inj_cranktorun_time * 100.0f);
    p_params->inj_aftstr_strokes = MathHelpers::Round(p_in->inj_aftstr_strokes / 2.0f);
    float discrete = (GetPlatformId() == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
    p_params->inj_prime_cold = MathHelpers::Round((p_in->inj_prime_cold * 1000.0f) / discrete);
    p_params->inj_prime_hot = MathHelpers::Round((p_in->inj_prime_hot * 1000.0f) / discrete);
    p_params->inj_prime_delay = MathHelpers::Round(p_in->inj_prime_delay * 10.0f);
   }
   break;
  case ADCCOR_PAR:
   {
    ADCCompenPar* p_in = (ADCCompenPar*)ip_values;
    p_params->map_adc_factor      = MathHelpers::Round(p_in->map_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long map_correction_d  = MathHelpers::Round((-p_in->map_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->map_adc_correction  = MathHelpers::Round(16384 * (0.5f - map_correction_d * p_in->map_adc_factor));
    //-------------------------------------------------------------------------

    p_params->temp_adc_factor     = MathHelpers::Round(p_in->temp_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long temp_correction_d = MathHelpers::Round((-p_in->temp_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->temp_adc_correction = MathHelpers::Round(16384 * (0.5f - temp_correction_d * p_in->temp_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ubat_adc_factor     = MathHelpers::Round(p_in->ubat_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ubat_correction_d = MathHelpers::Round((-p_in->ubat_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ubat_adc_correction = MathHelpers::Round(16384 * (0.5f - ubat_correction_d * p_in->ubat_adc_factor));
    //-------------------------------------------------------------------------

    p_params->tps_adc_factor     = MathHelpers::Round(p_in->tps_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long tps_correction_d = MathHelpers::Round((-p_in->tps_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->tps_adc_correction = MathHelpers::Round(16384 * (0.5f - tps_correction_d * p_in->tps_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai1_adc_factor     = MathHelpers::Round(p_in->ai1_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai1_correction_d = MathHelpers::Round((-p_in->ai1_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai1_adc_correction = MathHelpers::Round(16384 * (0.5f - ai1_correction_d * p_in->ai1_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai2_adc_factor     = MathHelpers::Round(p_in->ai2_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai2_correction_d = MathHelpers::Round((-p_in->ai2_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai2_adc_correction = MathHelpers::Round(16384 * (0.5f - ai2_correction_d * p_in->ai2_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai3_adc_factor     = MathHelpers::Round(p_in->ai3_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai3_correction_d = MathHelpers::Round((-p_in->ai3_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai3_adc_correction = MathHelpers::Round(16384 * (0.5f - ai3_correction_d * p_in->ai3_adc_factor));
    //-------------------------------------------------------------------------

    p_params->ai4_adc_factor     = MathHelpers::Round(p_in->ai4_adc_factor * 16384);
    //-------------------------------------------------------------------------
    signed long ai4_correction_d = MathHelpers::Round((-p_in->ai4_adc_correction) / ADC_DISCRETE); //переводим из вольтов в дискреты АЦП
    p_params->ai4_adc_correction = MathHelpers::Round(16384 * (0.5f - ai4_correction_d * p_in->ai4_adc_factor));
    //-------------------------------------------------------------------------
   }
   break;
  case CKPS_PAR:
   {
    CKPSPar* p_in = (CKPSPar*)ip_values;
    p_params->ckps_cogs_btdc  = p_in->ckps_cogs_btdc;
    p_params->ckps_ignit_cogs = p_in->ckps_ignit_cogs;
    p_params->ckps_edge_type  = p_in->ckps_edge_type;
    p_params->ref_s_edge_type  = p_in->ref_s_edge_type;
    p_params->ckps_engine_cyl = p_in->ckps_engine_cyl;
    p_params->ckps_merge_ign_outs = p_in->ckps_merge_ign_outs;
    p_params->ckps_cogs_num = p_in->ckps_cogs_num;
    p_params->ckps_miss_num = p_in->ckps_miss_num;
    WRITEBIT8(p_params->hall_flags, 0, p_in->ckps_rising_spark);
    WRITEBIT8(p_params->hall_flags, 1, p_in->ckps_use_cam_ref);
    p_params->hall_wnd_width = MathHelpers::Round(p_in->hall_wnd_width * ANGLE_MULTIPLIER);
    p_params->hall_degrees_btdc = MathHelpers::Round(p_in->hall_degrees_btdc * ANGLE_MULTIPLIER);
   }
   break;
  case KNOCK_PAR:
   {
    KnockPar* p_in = (KnockPar*)ip_values;
    p_params->knock_use_knock_channel = p_in->knock_use_knock_channel;
    p_params->knock_k_wnd_begin_angle = MathHelpers::Round(p_in->knock_k_wnd_begin_angle * ANGLE_MULTIPLIER);
    p_params->knock_k_wnd_end_angle = MathHelpers::Round(p_in->knock_k_wnd_end_angle  * ANGLE_MULTIPLIER);
    ASSERT(p_in->knock_bpf_frequency >= 0.0f);
    p_params->knock_bpf_frequency = MathHelpers::Round(p_in->knock_bpf_frequency);
    p_params->knock_int_time_const = p_in->knock_int_time_const;

    p_params->knock_retard_step = MathHelpers::Round(p_in->knock_retard_step * ANGLE_MULTIPLIER);
    p_params->knock_advance_step = MathHelpers::Round(p_in->knock_advance_step * ANGLE_MULTIPLIER);
    p_params->knock_max_retard = MathHelpers::Round(p_in->knock_max_retard * ANGLE_MULTIPLIER);
    p_params->knock_threshold = MathHelpers::Round(p_in->knock_threshold / ADC_DISCRETE);
    p_params->knock_recovery_delay = p_in->knock_recovery_delay;
   }
   break;
  case MISCEL_PAR:
   {
    MiscelPar* p_in = (MiscelPar*)ip_values;

    int divisor = 0;
    for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
     if (SECU3IO::secu3_allowable_uart_divisors[i].first == p_in->baud_rate)
      divisor = SECU3IO::secu3_allowable_uart_divisors[i].second;
    if (0==divisor)
    {
     secu3_allowable_uart_divisors[0].second;
     ASSERT(0);
    }

    p_params->uart_divisor = divisor;
    p_params->uart_period_t_ms = p_in->period_ms / 10;
    p_params->ign_cutoff = p_in->ign_cutoff;
    p_params->ign_cutoff_thrd = p_in->ign_cutoff_thrd;
    p_params->hop_start_cogs = p_in->hop_start_cogs;
    p_params->hop_durat_cogs = p_in->hop_durat_cogs;
    WRITEBIT8(p_params->flpmp_flags, 0, p_in->flpmp_offongas);
    WRITEBIT8(p_params->flpmp_flags, 1, p_in->inj_offongas);
    p_params->evap_afbegin = MathHelpers::Round(p_in->evap_afbegin / 32.0f);
    p_params->evap_afslope = MathHelpers::Round(p_in->evap_afslope * 1048576.0f * 32.0f);
   }
   break;
  case CHOKE_PAR:
   {
    ChokePar* p_in = (ChokePar*)ip_values;
    p_params->sm_steps  = p_in->sm_steps;
    p_params->choke_rpm[0] = p_in->choke_rpm[0];
    p_params->choke_rpm[1] = p_in->choke_rpm[1];
    p_params->choke_startup_corr = MathHelpers::Round(p_in->strt_add * 2.0f);
    p_params->choke_rpm_if = MathHelpers::Round(p_in->choke_rpm_if * 1024.0f);
    p_params->choke_corr_time = MathHelpers::Round(p_in->choke_corr_time * 100.0f);
    p_params->choke_corr_temp = MathHelpers::Round(p_in->choke_corr_temp * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    WRITEBIT8(p_params->choke_flags, 0, p_in->offstrtadd_ongas);
    WRITEBIT8(p_params->choke_flags, 1, p_in->offrpmreg_ongas);
    WRITEBIT8(p_params->choke_flags, 2, p_in->usethrottle_pos);
    WRITEBIT8(p_params->choke_flags, 3, p_in->sm_maxfreqinit);
    p_params->sm_freq = p_in->sm_freq;
   }
   break;
  case GASDOSE_PAR:
   {
    GasdosePar* p_in = (GasdosePar*)ip_values;
    p_params->gd_steps  = p_in->gd_steps;
    p_params->gd_fc_closing = MathHelpers::Round(p_in->fc_closing * 2.0f);
    p_params->gd_lambda_corr_limit_p = MathHelpers::Round(p_in->lam_corr_limit_p * 512.0f / 100.0f);
    p_params->gd_lambda_corr_limit_m = MathHelpers::Round(p_in->lam_corr_limit_m * 512.0f / 100.0f);
    p_params->gd_lambda_stoichval = MathHelpers::Round(p_in->lam_stoichval * 128.0f);
    p_params->gd_freq = p_in->gd_freq;
    p_params->gd_maxfreqinit = p_in->gd_maxfreqinit;
   }
   break;
  case SECUR_PAR:
   {
    SecurPar* p_in = (SecurPar*)ip_values;
    WRITEBIT8(p_params->bt_flags, 0, p_in->use_bt);
    WRITEBIT8(p_params->bt_flags, 2, p_in->use_imm);
    WRITEBIT8(p_params->bt_flags, 3, p_in->use_respar);
    for(int j = 0; j < SECU3IO::IBTN_KEYS_NUM; ++j)
     memcpy(p_params->ibtn_keys[j], p_in->ibtn_keys[j], SECU3IO::IBTN_KEY_SIZE);
   }
   break;
  case UNIOUT_PAR:
   {
    CondEncoder cen(m_quartz_frq, m_period_distance);
    UniOutPar* p_in = (UniOutPar*)ip_values;
    for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
    {
     p_params->uni_output[oi].flags = ((p_in->out[oi].logicFunc) << 4) | (p_in->out[oi].invers_2?2:0) | (p_in->out[oi].invers_1?1:0);
     p_params->uni_output[oi].condition1 = p_in->out[oi].condition1;
     p_params->uni_output[oi].condition2 = p_in->out[oi].condition2;
     p_params->uni_output[oi].on_thrd_1 = cen.UniOutEncodeCondVal(p_in->out[oi].on_thrd_1, p_in->out[oi].condition1);
     p_params->uni_output[oi].off_thrd_1 = cen.UniOutEncodeCondVal(p_in->out[oi].off_thrd_1, p_in->out[oi].condition1);
     p_params->uni_output[oi].on_thrd_2 = cen.UniOutEncodeCondVal(p_in->out[oi].on_thrd_2, p_in->out[oi].condition2);
     p_params->uni_output[oi].off_thrd_2 = cen.UniOutEncodeCondVal(p_in->out[oi].off_thrd_2, p_in->out[oi].condition2);
    }
    p_params->uniout_12lf = p_in->logicFunc12;
   }
   break;
  case INJCTR_PAR:
   {
    InjctrPar* p_in = (InjctrPar*)ip_values;
    WRITEBIT8(p_params->inj_flags, 0, p_in->inj_usetimingmap);
    p_params->inj_config = (p_in->inj_config << 4) | (p_in->inj_squirt_num & 0x0F);
    p_params->inj_flow_rate = MathHelpers::Round(p_in->inj_flow_rate * 64.0f);
    p_params->inj_cyl_disp = MathHelpers::Round(p_in->inj_cyl_disp * 16384.0f);
    p_params->inj_sd_igl_const = MathHelpers::Round(p_in->inj_sd_igl_const);
    p_params->inj_timing = MathHelpers::Round(p_in->inj_timing * ANGLE_MULTIPLIER);
    p_params->inj_timing_crk = MathHelpers::Round(p_in->inj_timing_crk * ANGLE_MULTIPLIER);
    p_params->inj_anglespec = p_in->inj_anglespec;
    p_params->fff_const = MathHelpers::Round((p_in->fff_const / (1000.0f*60.0f))*65536.0f);
   }
   break;
  case LAMBDA_PAR:
   {
    LambdaPar* p_in = (LambdaPar*)ip_values;
    p_params->inj_lambda_str_per_stp = p_in->lam_str_per_stp;
    p_params->inj_lambda_step_size_p = MathHelpers::Round(p_in->lam_step_size_p * 512.0f / 100.0f);
    p_params->inj_lambda_step_size_m = MathHelpers::Round(p_in->lam_step_size_m * 512.0f / 100.0f);
    p_params->inj_lambda_corr_limit_p = MathHelpers::Round(p_in->lam_corr_limit_p * 512.0f / 100.0f);
    p_params->inj_lambda_corr_limit_m = MathHelpers::Round(p_in->lam_corr_limit_m * 512.0f / 100.0f);
    p_params->inj_lambda_swt_point = MathHelpers::Round(p_in->lam_swt_point / ADC_DISCRETE);
    p_params->inj_lambda_temp_thrd = MathHelpers::Round(p_in->lam_temp_thrd * TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER);
    p_params->inj_lambda_rpm_thrd = p_in->lam_rpm_thrd;
    p_params->inj_lambda_activ_delay = p_in->lam_activ_delay;
    p_params->inj_lambda_dead_band = MathHelpers::Round(p_in->lam_dead_band / ADC_DISCRETE);
    p_params->inj_lambda_senstype = p_in->lam_senstype;
    p_params->inj_lambda_ms_per_stp = p_in->lam_ms_per_stp / 10;
    p_params->inj_lambda_htgdet = p_in->lam_htgdet;
   }
   break;
  case ACCEL_PAR:
   {
    AccelPar* p_in = (AccelPar*)ip_values;
    p_params->inj_ae_tpsdot_thrd = MathHelpers::Round(p_in->ae_tpsdot_thrd);
    p_params->inj_ae_coldacc_mult = MathHelpers::Round(((p_in->ae_coldacc_mult/100.0f) - 1.00f) * 128.0f);
    p_params->inj_ae_decay_time = p_in->ae_decay_time; //strokes
   }
   break;

  default:
   return false; //неизвестный или неподдерживаемый дескриптор
 }//switch

 return true;
}

bool ParamsIO::GetDefParamValues(BYTE i_descriptor, void* op_values)
{
 using namespace SECU3IO;

 //получаем адрес структуры параметров
 params_t* p_params = GetParamsPtr();;

 //TODO: Remove these copy/paste! (conversions should be implemented in functions, as certain class)
 switch(i_descriptor)
  {
   case TEMPER_PAR:
    {
     TemperPar* p_out = (TemperPar*)op_values;
     p_out->tmp_use  = p_params->tmp_use;
     p_out->vent_pwm = p_params->vent_pwm;
     p_out->cts_use_map = p_params->cts_use_map;
     p_out->vent_on  = ((float)p_params->vent_on) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->vent_off = ((float)p_params->vent_off) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->vent_pwmfrq = MathHelpers::Round(1.0/(((double)p_params->vent_pwmfrq) / 524288.0));
     //air conditioner related:
     p_out->cond_pvt_on = ((float)p_params->cond_pvt_on) * ADC_DISCRETE;
     p_out->cond_pvt_off = ((float)p_params->cond_pvt_off) * ADC_DISCRETE;
    }
    break;
   case CARBUR_PAR:
    {
     CarburPar* p_out = (CarburPar*)op_values;
     p_out->ephh_hit    = p_params->ephh_hit;
     p_out->ephh_lot    = p_params->ephh_lot;
     p_out->carb_invers = p_params->carb_invers;
     p_out->epm_ont =  ((float)p_params->epm_on_threshold) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->ephh_hit_g  = p_params->ephh_hit_g;
     p_out->ephh_lot_g  = p_params->ephh_lot_g;
     p_out->shutoff_delay = ((float)p_params->shutoff_delay) / 100.0f; //переводим в секунды
     p_out->tps_threshold = ((float)p_params->tps_threshold) / TPS_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->fuelcut_map_thrd = ((float)p_params->fuelcut_map_thrd) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->fuelcut_cts_thrd = ((float)p_params->fuelcut_cts_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->revlim_lot = p_params->revlim_lot;
     p_out->revlim_hit = p_params->revlim_hit;
    }
    break;
   case IDLREG_PAR:
    {
     IdlRegPar* p_out = (IdlRegPar*)op_values;
     p_out->idl_regul  = CHECKBIT8(p_params->idl_flags, 0);
     p_out->use_regongas  = CHECKBIT8(p_params->idl_flags, 1);
     p_out->closed_loop = CHECKBIT8(p_params->idl_flags, 2);
     p_out->preg_mode = CHECKBIT8(p_params->idl_flags, 3);
     p_out->idl_useiacclongas = CHECKBIT8(p_params->idl_flags, 4);
     p_out->idling_rpm = p_params->idling_rpm;
     p_out->MINEFR     = p_params->MINEFR;
     p_out->ifac1      = ((float)p_params->ifac1) / 256.0f;
     p_out->ifac2      = ((float)p_params->ifac2) / 256.0f;
     p_out->min_angle  = ((float)p_params->idlreg_min_angle) / ANGLE_MULTIPLIER;
     p_out->max_angle  = ((float)p_params->idlreg_max_angle) / ANGLE_MULTIPLIER;
     p_out->turn_on_temp = ((float)p_params->idlreg_turn_on_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     //Closed loop parameters:
     p_out->idl_to_run_add = ((float)p_params->idl_to_run_add) / 2.0f;
     p_out->rpm_on_run_add = p_params->rpm_on_run_add * 10;
     p_out->idl_reg_p = ((float)p_params->idl_reg_p) / 256.0f;
     p_out->idl_reg_i = ((float)p_params->idl_reg_i) / 256.0f;
     p_out->idl_coef_thrd1 = (((float)p_params->idl_coef_thrd1) / 128.0f) + 1.0f;
     p_out->idl_coef_thrd2 = (((float)p_params->idl_coef_thrd2) / 128.0f) + 1.0f;
     p_out->idl_intrpm_lim = p_params->idl_intrpm_lim * 10;
     p_out->idl_map_value = ((float)p_params->idl_map_value) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->idl_iacminpos = ((float)p_params->idl_iacminpos) / 2.0f;
     p_out->idl_iacmaxpos = ((float)p_params->idl_iacmaxpos) / 2.0f;
    }
    break;
   case ANGLES_PAR:
    {
     AnglesPar* p_out = (AnglesPar*)op_values;
     p_out->angle_corr = ((float)p_params->angle_corr) / ANGLE_MULTIPLIER;
     p_out->max_angle  = ((float)p_params->max_angle)  / ANGLE_MULTIPLIER;
     p_out->min_angle  = ((float)p_params->min_angle) / ANGLE_MULTIPLIER;
     p_out->dec_speed = ((float)p_params->angle_dec_speed)  / ANGLE_MULTIPLIER;
     p_out->inc_speed = ((float)p_params->angle_inc_speed)  / ANGLE_MULTIPLIER;
     p_out->zero_adv_ang = p_params->zero_adv_ang;
    }
    break;
   case FUNSET_PAR:
    {
     FunSetPar* p_out = (FunSetPar*)op_values;
     p_out->fn_benzin = p_params->fn_benzin;
     p_out->fn_gas    = p_params->fn_gas;
     p_out->map_lower_pressure = ((float)p_params->map_lower_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->map_upper_pressure = ((float)p_params->map_upper_pressure) / MAP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->map_curve_offset = ((float)p_params->map_curve_offset) * ADC_DISCRETE;
     p_out->map_curve_gradient = ((float)p_params->map_curve_gradient) / (MAP_PHYSICAL_MAGNITUDE_MULTIPLIER * ADC_DISCRETE * 128.0f);
     p_out->tps_curve_offset = ((float)p_params->tps_curve_offset) * ADC_DISCRETE;
     p_out->tps_curve_gradient = ((float)p_params->tps_curve_gradient) / ((TPS_PHYSICAL_MAGNITUDE_MULTIPLIER*64) * ADC_DISCRETE * 128.0f);
     p_out->load_src_cfg = p_params->load_src_cfg & 0x0F;
     p_out->uni_gas = p_params->mapsel_uni >> 4;
     p_out->uni_benzin = p_params->mapsel_uni & 0xF;
     if (p_out->uni_gas == 0xF)
      p_out->uni_gas = UNI_OUTPUT_NUM; //disabled
     if (p_out->uni_benzin == 0xF)
      p_out->uni_benzin = UNI_OUTPUT_NUM; //disabled
    }
    break;
   case STARTR_PAR:
    {
     StartrPar* p_out = (StartrPar*)op_values;
     p_out->starter_off = p_params->starter_off;
     p_out->smap_abandon = p_params->smap_abandon;
     p_out->inj_cranktorun_time = float(p_params->inj_cranktorun_time) / 100.0f;
     p_out->inj_aftstr_strokes = p_params->inj_aftstr_strokes * 2;
     float discrete = (GetPlatformId() == EP_ATMEGA644) ? 3.2f : 4.0f; //for ATMega644 discrete = 3.2uS, for others - 4.0uS
     p_out->inj_prime_cold = (float(p_params->inj_prime_cold) * discrete) / 1000.0f;  //convert to ms
     p_out->inj_prime_hot = (float(p_params->inj_prime_hot) * discrete) / 1000.0f;  //convert to ms
     p_out->inj_prime_delay = float(p_params->inj_prime_delay) / 10.0f; //convert to seconds
    }
    break;
   case ADCCOR_PAR:
    {
     ADCCompenPar* p_out = (ADCCompenPar*)op_values;
     p_out->map_adc_factor      = ((float)p_params->map_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     p_out->map_adc_correction  = ((((float)p_params->map_adc_correction)/16384.0f) - 0.5f) / p_out->map_adc_factor;
     p_out->map_adc_correction*=ADC_DISCRETE; //в вольты
     //-------------------------------------------------------------------------

     p_out->temp_adc_factor     = ((float)p_params->temp_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     p_out->temp_adc_correction = ((((float)p_params->temp_adc_correction)/16384.0f) - 0.5f) / p_out->temp_adc_factor;
     p_out->temp_adc_correction*=ADC_DISCRETE; //в вольты
     //-------------------------------------------------------------------------

     p_out->ubat_adc_factor     = ((float)p_params->ubat_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     p_out->ubat_adc_correction = ((((float)p_params->ubat_adc_correction)/16384.0f) - 0.5f) / p_out->ubat_adc_factor;
     p_out->ubat_adc_correction*=ADC_DISCRETE; //в вольты
     //-------------------------------------------------------------------------

     p_out->tps_adc_factor     = ((float)p_params->tps_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->tps_adc_factor > 0)
     {
      p_out->tps_adc_correction = ((((float)p_params->tps_adc_correction)/16384.0f) - 0.5f) / p_out->tps_adc_factor;
      p_out->tps_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->tps_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai1_adc_factor     = ((float)p_params->ai1_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai1_adc_factor > 0)
     {
      p_out->ai1_adc_correction = ((((float)p_params->ai1_adc_correction)/16384.0f) - 0.5f) / p_out->ai1_adc_factor;
      p_out->ai1_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai1_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai2_adc_factor     = ((float)p_params->ai2_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai2_adc_factor > 0)
     {
      p_out->ai2_adc_correction = ((((float)p_params->ai2_adc_correction)/16384.0f) - 0.5f) / p_out->ai2_adc_factor;
      p_out->ai2_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai2_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai3_adc_factor     = ((float)p_params->ai3_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai3_adc_factor > 0)
     {
      p_out->ai3_adc_correction = ((((float)p_params->ai3_adc_correction)/16384.0f) - 0.5f) / p_out->ai3_adc_factor;
      p_out->ai3_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai3_adc_correction = 0;
     //-------------------------------------------------------------------------

     p_out->ai4_adc_factor     = ((float)p_params->ai4_adc_factor) / 16384;
     //-------------------------------------------------------------------------
     if (p_out->ai4_adc_factor > 0)
     {
      p_out->ai4_adc_correction = ((((float)p_params->ai4_adc_correction)/16384.0f) - 0.5f) / p_out->ai4_adc_factor;
      p_out->ai4_adc_correction*=ADC_DISCRETE; //в вольты
     }
     else
      p_out->ai4_adc_correction = 0;
     //-------------------------------------------------------------------------
    }
    break;
   case CKPS_PAR:
    {
     CKPSPar* p_out = (CKPSPar*)op_values;
     p_out->ckps_cogs_btdc  = p_params->ckps_cogs_btdc;
     p_out->ckps_ignit_cogs = p_params->ckps_ignit_cogs;
     p_out->ckps_edge_type  = p_params->ckps_edge_type;
     p_out->ref_s_edge_type  = p_params->ref_s_edge_type;
     p_out->ckps_engine_cyl = p_params->ckps_engine_cyl;
     p_out->ckps_merge_ign_outs = p_params->ckps_merge_ign_outs;
     p_out->ckps_cogs_num = p_params->ckps_cogs_num;
     p_out->ckps_miss_num = p_params->ckps_miss_num;
     p_out->ckps_rising_spark = CHECKBIT8(p_params->hall_flags, 0);
     p_out->ckps_use_cam_ref = CHECKBIT8(p_params->hall_flags, 1);
     p_out->hall_wnd_width = ((float)p_params->hall_wnd_width) / ANGLE_MULTIPLIER;
     p_out->hall_degrees_btdc = ((float)p_params->hall_degrees_btdc) / ANGLE_MULTIPLIER;
    }
    break;
   case KNOCK_PAR:
    {
     KnockPar* p_out = (KnockPar*)op_values;
     p_out->knock_use_knock_channel = p_params->knock_use_knock_channel;
     p_out->knock_k_wnd_begin_angle = ((float)p_params->knock_k_wnd_begin_angle) / ANGLE_MULTIPLIER;
     p_out->knock_k_wnd_end_angle = ((float)p_params->knock_k_wnd_end_angle) / ANGLE_MULTIPLIER;
     p_out->knock_bpf_frequency = p_params->knock_bpf_frequency;
     p_out->knock_int_time_const = p_params->knock_int_time_const;

     p_out->knock_retard_step = ((float)p_params->knock_retard_step) / ANGLE_MULTIPLIER;
     p_out->knock_advance_step = ((float)p_params->knock_advance_step) / ANGLE_MULTIPLIER;
     p_out->knock_max_retard = ((float)p_params->knock_max_retard) / ANGLE_MULTIPLIER;
     p_out->knock_threshold = ((float)p_params->knock_threshold) * ADC_DISCRETE;
     p_out->knock_recovery_delay = p_params->knock_recovery_delay;
    }
    break;
   case MISCEL_PAR:
    {
     MiscelPar* p_out = (MiscelPar*)op_values;

     int baud_rate = 0;
     for(size_t i = 0; i < SECU3IO::SECU3_ALLOWABLE_UART_DIVISORS_COUNT; ++i)
     if (SECU3IO::secu3_allowable_uart_divisors[i].second == p_params->uart_divisor)
      baud_rate = SECU3IO::secu3_allowable_uart_divisors[i].first;
     ASSERT(baud_rate);

     p_out->baud_rate = baud_rate;
     p_out->period_ms = p_params->uart_period_t_ms * 10;
     p_out->ign_cutoff = p_params->ign_cutoff;
     p_out->ign_cutoff_thrd = p_params->ign_cutoff_thrd;
     p_out->hop_start_cogs = p_params->hop_start_cogs;
     p_out->hop_durat_cogs = p_params->hop_durat_cogs;
     p_out->flpmp_offongas = CHECKBIT8(p_params->flpmp_flags, 0);
     p_out->inj_offongas = CHECKBIT8(p_params->flpmp_flags, 1);
     p_out->evap_afbegin = p_params->evap_afbegin * 32.0f;
     p_out->evap_afslope = (p_params->evap_afslope / 1048576.0f) / 32.0f;
    }
    break;
   case CHOKE_PAR:
    {
     ChokePar* p_out = (ChokePar*)op_values;
     p_out->sm_steps = p_params->sm_steps;
     p_out->choke_rpm[0] = p_params->choke_rpm[0];
     p_out->choke_rpm[1] = p_params->choke_rpm[1];
     p_out->strt_add = p_params->choke_startup_corr / 2.0f;
     p_out->choke_rpm_if = p_params->choke_rpm_if / 1024.0f;
     p_out->choke_corr_time = p_params->choke_corr_time / 100.0f;
     p_out->choke_corr_temp = ((float)p_params->choke_corr_temp) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
     p_out->offstrtadd_ongas = CHECKBIT8(p_params->choke_flags, 0);
     p_out->offrpmreg_ongas = CHECKBIT8(p_params->choke_flags, 1);
     p_out->usethrottle_pos = CHECKBIT8(p_params->choke_flags, 2);
     p_out->sm_maxfreqinit = CHECKBIT8(p_params->choke_flags, 3);
     p_out->sm_freq = p_params->sm_freq;
    }
    break;
   case GASDOSE_PAR:
    {
     GasdosePar* p_out = (GasdosePar*)op_values;
     p_out->gd_steps = p_params->gd_steps;
     p_out->fc_closing = p_params->gd_fc_closing / 2.0f;
     p_out->lam_corr_limit_p = ((float)p_params->gd_lambda_corr_limit_p * 100.0f)/512.0f;
     p_out->lam_corr_limit_m = ((float)p_params->gd_lambda_corr_limit_m * 100.0f)/512.0f;
     p_out->lam_stoichval = ((float)p_params->gd_lambda_stoichval) / 128.0f;
     p_out->gd_freq = p_params->gd_freq;
     p_out->gd_maxfreqinit = p_params->gd_maxfreqinit;
    }
    break;
   case SECUR_PAR:
    {
     SecurPar* p_out = (SecurPar*)op_values;
     p_out->use_bt = CHECKBIT8(p_params->bt_flags, 0);
     p_out->use_imm = CHECKBIT8(p_params->bt_flags, 2);
     p_out->use_respar = CHECKBIT8(p_params->bt_flags, 3);
     for(int j = 0; j < SECU3IO::IBTN_KEYS_NUM; ++j)
      memcpy(p_out->ibtn_keys[j], p_params->ibtn_keys[j], SECU3IO::IBTN_KEY_SIZE);
    }
    break;
  case UNIOUT_PAR:
   {
    CondEncoder cen(m_quartz_frq, m_period_distance);
    UniOutPar* p_out = (UniOutPar*)op_values;
    for(int oi = 0; oi < UNI_OUTPUT_NUM; ++oi)
    {
     p_out->out[oi].logicFunc = p_params->uni_output[oi].flags >> 4;
     p_out->out[oi].invers_1 = CHECKBIT8(p_params->uni_output[oi].flags, 0);
     p_out->out[oi].invers_2 = CHECKBIT8(p_params->uni_output[oi].flags, 1);
     p_out->out[oi].condition1 = p_params->uni_output[oi].condition1;
     p_out->out[oi].condition2 = p_params->uni_output[oi].condition2;
     p_out->out[oi].on_thrd_1 = cen.UniOutDecodeCondVal(cen.isSigned(p_params->uni_output[oi].condition1) ? ((_int)p_params->uni_output[oi].on_thrd_1) : p_params->uni_output[oi].on_thrd_1, p_params->uni_output[oi].condition1);
     p_out->out[oi].off_thrd_1 = cen.UniOutDecodeCondVal(cen.isSigned(p_params->uni_output[oi].condition1) ? ((_int)p_params->uni_output[oi].off_thrd_1) : p_params->uni_output[oi].off_thrd_1, p_params->uni_output[oi].condition1);
     p_out->out[oi].on_thrd_2 = cen.UniOutDecodeCondVal(cen.isSigned(p_params->uni_output[oi].condition2) ? ((_int)p_params->uni_output[oi].on_thrd_2) : p_params->uni_output[oi].on_thrd_2, p_params->uni_output[oi].condition2);
     p_out->out[oi].off_thrd_2 = cen.UniOutDecodeCondVal(cen.isSigned(p_params->uni_output[oi].condition2) ? ((_int)p_params->uni_output[oi].off_thrd_2) : p_params->uni_output[oi].off_thrd_2, p_params->uni_output[oi].condition2);
    }
    p_out->logicFunc12 = p_params->uniout_12lf;
   }
   break;
  case INJCTR_PAR:
   {
    InjctrPar* p_out = (InjctrPar*)op_values;

    p_out->inj_usetimingmap = CHECKBIT8(p_params->inj_flags, 0);
    p_out->inj_config = p_params->inj_config >> 4;      //fuel injection configuration
    p_out->inj_squirt_num = p_params->inj_config & 0x0F; //number of squirts per cycle
    p_out->inj_flow_rate = float(p_params->inj_flow_rate) / 64.0f;
    p_out->inj_cyl_disp = float(p_params->inj_cyl_disp) / 16384.0f;
    p_out->inj_sd_igl_const = (float)p_params->inj_sd_igl_const;
    p_out->cyl_num = p_params->ckps_engine_cyl; //read-only parameter, its value required for calculations
    p_out->inj_timing = ((float)p_params->inj_timing) / ANGLE_MULTIPLIER;
    p_out->inj_timing_crk = ((float)p_params->inj_timing_crk) / ANGLE_MULTIPLIER;
    p_out->inj_anglespec = p_params->inj_anglespec;
    p_out->fff_const = ((float)p_params->fff_const/65536.0f)*(1000.0f*60.0f);
   }
   break;
  case LAMBDA_PAR:
   {
    LambdaPar* p_out = (LambdaPar*)op_values;
    p_out->lam_str_per_stp = p_params->inj_lambda_str_per_stp;
    p_out->lam_step_size_p = ((float)p_params->inj_lambda_step_size_p * 100.0f)/512.0f;
    p_out->lam_step_size_m = ((float)p_params->inj_lambda_step_size_m * 100.0f)/512.0f;
    p_out->lam_corr_limit_p = ((float)p_params->inj_lambda_corr_limit_p * 100.0f)/512.0f;
    p_out->lam_corr_limit_m = ((float)p_params->inj_lambda_corr_limit_m * 100.0f)/512.0f;
    p_out->lam_swt_point = ((float)p_params->inj_lambda_swt_point) * ADC_DISCRETE;
    p_out->lam_temp_thrd = ((float)p_params->inj_lambda_temp_thrd) / TEMP_PHYSICAL_MAGNITUDE_MULTIPLIER;
    p_out->lam_rpm_thrd = p_params->inj_lambda_rpm_thrd;
    p_out->lam_activ_delay = p_params->inj_lambda_activ_delay;
    p_out->lam_dead_band = ((float)p_params->inj_lambda_dead_band) * ADC_DISCRETE;
    p_out->lam_senstype = p_params->inj_lambda_senstype;
    p_out->lam_ms_per_stp = p_params->inj_lambda_ms_per_stp * 10;
    p_out->lam_htgdet = p_params->inj_lambda_htgdet;
   }
   break;
  case ACCEL_PAR:
   {
    AccelPar* p_out = (AccelPar*)op_values;
    p_out->ae_tpsdot_thrd = (float)p_params->inj_ae_tpsdot_thrd;
    p_out->ae_coldacc_mult = ((((float)p_params->inj_ae_coldacc_mult) + 128.0f) / 128.0f) * 100.0f; //convert to %
    p_out->ae_decay_time = p_params->inj_ae_decay_time; //strokes
   }
   break;

   default:
    return false; //неизвестный или неподдерживаемый дескриптор
  }//switch

  return true;
}

void ParamsIO::SetNumPulsesPer1Km(int pp1km)
{
 double value = MathHelpers::RestrictValue(pp1km, 1, 60000);
 m_period_distance = (float)(1000.0 / value); //distance of one period in meters
}

void ParamsIO::SetQuartzFrq(long frq)
{
 m_quartz_frq = frq;
}
