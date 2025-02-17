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

/** \file FirmwareConstants.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "FirmwareTabController.h"
#include "Settings/ISettingsData.h"
#include "TabDialogs/FirmwareTabDlg.h"
#include "io-core/FirmwareDataMediator.h"
#include "ui-core/DynFieldsDialog.h"

void CFirmwareTabController::OnEditFwConsts(void)
{
 CDynFieldsContainer dfd(mp_view, (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN) ? _T("Редактирование констант прошивки") : _T("Editing constants of firmware"), 408, true);

 SECU3IO::FwConstsData d;
 mp_fwdm->GetFwConstsData(d);

 //Sizes of averaging circular buffers
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размеры буферов усреднения входов:"));
 else
  dfd.AppendItem(_T("Sizes of inputs' averaging buffers:"));

 int avnmax = 32;
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения оборотов"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[0], _T("Размер буфера усреднения оборотов. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for RPM"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[0], _T("Size of averaging buffer for RPM. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДАД"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[1], _T("Размер буфера усреднения ДАД. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for MAP"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[1], _T("Size of averaging buffer for MAP. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения напряжения бортовой сети"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[2], _T("Размер буфера усреднения напряжения бортовой сети. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("Size of averaging buffer for board voltage"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[2], _T("Size of averaging buffer for board voltage. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДТОЖ"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[3], _T("Размер буфера усреднения ДТОЖ. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for CLT"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[3], _T("Size of averaging buffer for CLT. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ДПДЗ"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[4], _T("Размер буфера усреднения ДПДЗ. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for TPS"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[4], _T("Size of averaging buffer for TPS. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I1"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[5], _T("Размер буфера усреднения ADD_I1. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I1"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[5], _T("Size of averaging buffer for ADD_I1. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I2"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[6], _T("Размер буфера усреднения ADD_I2. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I2"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[6], _T("Size of averaging buffer for ADD_I2. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I3"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[8], _T("Размер буфера усреднения ADD_I3. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I3"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[8], _T("Size of averaging buffer for ADD_I3. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I4"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[9], _T("Размер буфера усреднения ADD_I4. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I4"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[9], _T("Size of averaging buffer for ADD_I4. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I5"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[10], _T("Размер буфера усреднения ADD_I5. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I5"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[10], _T("Size of averaging buffer for ADD_I5. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I6"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[11], _T("Размер буфера усреднения ADD_I6. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I6"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[11], _T("Size of averaging buffer for ADD_I6. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I7"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[12], _T("Размер буфера усреднения ADD_I7. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I7"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[12], _T("Size of averaging buffer for ADD_I7. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения ADD_I8"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[13], _T("Размер буфера усреднения ADD_I8. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for ADD_I8"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[13], _T("Size of averaging buffer for ADD_I8. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Размер буфера усреднения скорости"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[7], _T("Размер буфера усреднения скорости автомобиля. Чем больше значение усреднения, тем больше сглаживание, но при этом увеличивается запаздывание."));
 else
  dfd.AppendItem(_T("size of averaging buffer for VSS"), _T(""), 1, avnmax, 1, 0, &d.inpavnum[7], _T("Size of averaging buffer for vehicle speed. The more averaging value, the greater smoothing, but at the same time the delay increases."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Опрашивать ДАД синхронно с положением коленвала"), &d.map_samp_mode, _T("Если галочка установлена, то опрос ДАД будет привязан к определенным углам коленвала (1 раз за такт). Если галочка не установлена, то опрос ДАД будет осуществляться через равные промежутки времени (каждые 3.28мс)."));
 else
  dfd.AppendItem(_T("Sample MAP synchronously with crankshaft"), &d.map_samp_mode, _T("If the check is set, then MAP will be sampled at certain crankshaft degrees (in each stroke). If the check is not set, then MAP will be sampled regularly (each 3.28ms)."));

 //idling regulator
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Регулятор холостого хода:"));
 else
  dfd.AppendItem(_T("Idling regulator:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Зона оборотов подхвата УОЗ РХХ"), _T("мин-1"), 0, 1000, 1, 0, &d.idlreg_captrange, _T("УОЗ РХХ будет активирован когда обороты опустятся ниже значения равного сумме целевых оборотов ХХ и зоны подхвата. Например целевые обороты ХХ = 900, зона подхвата = 200, значит регулятор начнет работать когда обороты двигателя опустятся ниже 1100."));
 else
  dfd.AppendItem(_T("Capture range of the ign. tim. idling regulator"), _T("min-1"), 0, 1000, 1, 0, &d.idlreg_captrange, _T("Ignition-based idling regulator will be activated when the RPM falls below a value equal to the sum of the target idling RPM and the pickup zone. For example, target RPM = 900, pick-up zone = 200, which means the regulator will start working when the RPM drops below 1100."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка включения УОЗ РХХ"), _T("сек"), 0.01f, 2.5f, 0.01f, 2, &d.idlent_timval, _T("После наступления всех необходимых условий УОЗ РХХ включится с задержкой определяемой этим параметром"));
 else
  dfd.AppendItem(_T("Ign. tim. idling regulator's turn on delay"), _T("sec"), 0.01f, 2.5f, 0.01f, 2, &d.idlent_timval, _T("Ign. tim. idling regulator will be turned on with delay specified by this parameter"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Смещение РХХ при включении вентилятора"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.vent_iacoff, _T("Это значение будет прибавлено к положению РХХ при поступлении запроса на включение вентилятора. В режиме closed loop смещение один раз прибавляется к положению РХХ. В режиме open loop смещение прибавляется при включении вентилятора и вычитается при выключении вентилятора"));
 else
  dfd.AppendItem(_T("Displacement of IAC on turn on of cooling fan"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.vent_iacoff, _T("This value will be added to the IAC position when a request to turn on the fan is received. In closed loop mode, the offset is added once to the IAC position. In open loop mode, the displacement value is added when the fan is turned on and subtracted when the fan is turned off"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Смещение РХХ при включении усилителя руля"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.epas_iacoff, _T("Это значение прибавляется к положению РХХ при поступлении запроса включения усилителя руля (вход EPAS_I). В режиме closed loop смещение один раз прибавляется к положению РХХ. В режиме open loop смещение прибавляется при включении услилителя руля и вычитается при его выключении"));
 else
  dfd.AppendItem(_T("Displacement of IAC on turn on of EPAS"), _T("%"), 0.00f, 50.0f, 0.5f, 1, &d.epas_iacoff, _T("This value will be added to the IAC position when a request to turn on the EPAS is received (EPAS_I input). In closed loop mode, the offset is added once to the IAC position. In open loop mode, the displacement value is added when the EPAS is turned on and subtracted when the EPAS is turned off"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Добавка к РДВ при вкл. кондиционера"), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add, _T("'Мин.положение РДВ' увеличивается на это значение при включении кондиционера"));
 else
  dfd.AppendItem(_T("Add. to IAC pos on turn on of air cond. "), _T("%"), 0.0f, 100.0f, 0.5, 1, &d.iac_cond_add, _T("'Minimum IAC position' increases by this value when the air conditioner is turned on"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Шаг уменьшения добавки после выхода РХХ"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_en, _T("Значение добавки после выхода уменьшается на это значение каждые 100мс если дроссель закрыт и обороты двигателя находятся между 1-м и 2-м порогами переходных режимов (пороги пер. режимов задаются соответствующими коэффициентами на вкладке параметров '3:ХХ', там же задается значение добавки после выхода)"));
 else
  dfd.AppendItem(_T("Decrease step of the IAC idle to run additive"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_en, _T("Value of the IAC add after exit decreases by this value every 100ms if the throttle is closed and the engine speed is between the 1st and 2nd transient thresholds (transient thresholds are set by the corresponding coefficients on the '3:Idling' parameters' tab, value of the 'IAC add after exit' is also set there)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Шаг увеличения добавки после выхода РХХ"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_le, _T("Значение добавки после выхода увеличивается на это значение каждые 100мс если дроссель открыт и обороты двигателя находятся выше 2-го порога переходных режимов (значение добавки после выхода и пороги пер. режимов задаются соответствующими коэффициентами на вкладке параметров '3:ХХ', там же задается значение добавки после выхода)"));
 else
  dfd.AppendItem(_T("Increase step of the IAC idle to run additive"), _T("%"), 0.025f, 7.5f, 0.025f, 3, &d.idltorun_stp_le, _T("Value of the IAC add after exit increases by this value every 100ms if the throttle is open and the engine speed is above the 2nd transient threshold (transient thresholds are set by the corresponding coefficients on the '3:Idling' parameters' tab, value of the 'IAC add after exit' is also set there)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать специальный алгоритм для холодного двигателя"), _T(""), 0, 1, 1, 1, &d.cold_eng_int, _T("Использовать отдельный алгоритм для регулятора ХХ при холодном двигателе (1). Установите в 0, чтобы использовать одинаковый алгоритм для холодного и горячего двигателя. В алгоритме для холодного двигателя регулятор будет включаться только когда текущие обороты ниже целевых и падают, при этом будет использоваться только интегральная часть регулятора."));
 else
  dfd.AppendItem(_T("Use special algorithm for cold engine"), _T(""), 0, 1, 1, 1, &d.cold_eng_int, _T("Use separate closed loop algorithm when engine is cold (1). Set to 0 for using same algorithm for cold and hot engine. In the algorithm for a cold engine, regulator will be used only when current RPM is below the target and falling, while only the integral part of regulator will be used."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Выбор алгоритма входа в режим ХХ"), _T(""), 0, 1, 1, 1, &d.thrass_algo,_T("Если установлен 0: положение РДВ плавно уменьшается каждые 100мс на 'Шаг уменьшения добавки после выхода РХХ' eсли дроссель закрыт и обороты ниже 2-го порога оборотов\nЕсли установлена 1: условием является только закрытый дроссель (обороты не принимаются во внимание)."));
 else
  dfd.AppendItem(_T("Selection of idling entering's algorithm"), _T(""), 0, 1, 1, 1, &d.thrass_algo, _T("If 0 is set: the IAC position smoothly decreases every 100ms by 'Decrease step of the IAC idle to run additive' if the throttle is closed and the rpm is below the 2nd rpm threshold\nIf 1 is set: the condition is only a closed throttle (rpm is not taken into account)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Период вызова кода closed loop РДВ/ВЗ"), _T("сек"), 0.00f, 1.0f, 0.01f, 2, &d.iacreg_period, _T("Период вызова кода closed loop РДВ/ВЗ. Определяет частоту вызова регулятора. Используется в коде управления РДВ и в коде управления ВЗ"));
 else
  dfd.AppendItem(_T("Period of calling IAC's/Choke closed loop code"), _T("sec"), 0.00f, 1.0f, 0.01f, 2, &d.iacreg_period, _T("Period of calling IAC's/Choke closed loop code. Used in the IAC's regulator code and in the choke regulator code."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. РДВ closed loop"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.iacreg_turn_on_temp, _T("Режим closed loop для РДВ включится только если температура двигателя выше этого порога или обороты выше целевых оборотов ХХ. Данный параметр актуален только если используется специальный алгоритм для холодного двигателя."));
 else
  dfd.AppendItem(_T("CLT threshold for turn on of the IAC closed loop"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.iacreg_turn_on_temp, _T("IAC closed loop will turn on only if the engine temperature is above this threshold or RPM is above target value. This parameter is useful only if special algorithm for cold engine is used."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Вычислять пороги переход. режимов только по целевым оборотам ХХ"), &d.tmrpmtc_mode, _T("Если галочка установлена, то пороги оборотов переходных режимов будут вычисляться только на основе целевых оборотов."));
 else
  dfd.AppendItem(_T("Calculate transient mode thretholds using only idl. target RPM"), &d.tmrpmtc_mode, _T("If the checkbox is checked, then transient's mode RPM thresholds will be calculated using idling target RPM only."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог начала движения для РХХ"), _T("км/ч"), 1.0f, 100.0f, 0.1f, 1, &d.iac_addonrun_vss_thrd, _T("Если скорость авто превысит этот порог, то РХХ повысит обороты двигателя на значение 'Добавка к обор. на ходу'."));
 else
  dfd.AppendItem(_T("Start of running threshold for IAC"), _T("km/h"), 1.0f, 100.0f, 0.1f, 1, &d.iac_addonrun_vss_thrd, _T("If the speed of vehicle is exceed this threshold, then IAC regulator will increase RPM by 'RPM add on run' value."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Минимальные обороты на ходу"), _T("мин-1"), 0, 3000, 1, 0, &d.iac_min_rpm_on_run, _T("Если скорость авто превысит этот порог, то РХХ будет поддерживать обороты двигателя на уровне не меньше этого значения."));
 else
  dfd.AppendItem(_T("Minimum RPM on running"), _T("min-1"), 0, 3000, 1, 0, &d.iac_min_rpm_on_run, _T("If the speed of vehicle is exceed this threshold, then IAC regulator will keep RPM not less that this value."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Отключать УОЗ РХХ при начале движения "), &d.use_vss_thrd_for_igntim_reg, _T("Если галочка установлена, УОЗ РХХ будет отключен если скорость автомобиля выше значения 'Порог начала движения для РХХ'."));
 else
  dfd.AppendItem(_T("Turn off Ign.tim.idling regul. on running"), &d.use_vss_thrd_for_igntim_reg, _T("If the checkbox is checked, Ign.timing idling regulator will be turned off when vehicle speed is above 'Start of running threshold for IAC' value."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Коэфф.1 порога запуска регулятора оборотов"), _T(""), 0.5f, 10.0f, 0.01f, 2, &d.iac_clen_coeff, _T("Если после пуска двигателя обороты превысят порог = целевые обороты * коэффициент1 и затем упадут ниже порога = целевые обороты * коэффициент2, тогда запустится регулятор оборотов ХХ (если он разрешен)."));
 else
  dfd.AppendItem(_T("Coeff.1 of threshold for starting CL"), _T(""), 0.5f, 10.0f, 0.01f, 2, &d.iac_clen_coeff, _T("If after start of engine RPM exceed threshold = target RPM * coeff.1 and after that RPM fall below threshold = target RPM * coeff.2, then closed loop idling regulator will be activated."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Коэфф.2 порога запуска регулятора оборотов"), _T(""), 0.5f, 10.0f, 0.01f, 2, &d.iac_clon_coeff, _T("Если после пуска двигателя обороты превысят порог = целевые обороты * коэффициент1 и затем упадут ниже порога = целевые обороты * коэффициент2, тогда запустится регулятор оборотов ХХ (если он разрешен)."));
 else
  dfd.AppendItem(_T("Coeff.2 of threshold for starting CL"), _T(""), 0.5f, 10.0f, 0.01f, 2, &d.iac_clon_coeff, _T("If after start of engine RPM exceed threshold = target RPM * coeff.1 and after that RPM fall below threshold = target RPM * coeff.2, then closed loop idling regulator will be activated."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Коэфф. увеличения положения РДВ после пуска"), _T(""), 0.5f, 1.5f, 0.005f, 3, &d.iac_wrkadd_coeff, _T("После запуска двигателя положение РДВ будет умножаться на этот коэффициент на протяжении времени 'Длительность увеличенного положения РДВ после пуска'."));
 else
  dfd.AppendItem(_T("Coeff. of IAC position growth after engine start"), _T(""), 0.5f, 1.5f, 0.005f, 3, &d.iac_wrkadd_coeff, _T("After engine start, the IAC position will be multiplied by this factor for the duration of 'Duration of grown IAC position after start'."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длительность увеличенного положения РДВ после пуска"), _T(""), 0.0f, 30.0f, 0.01f, 2, &d.iac_wrkadd_time, _T("После запуска двигателя положение РДВ будет умножаться на коэффициент 'Коэфф. увеличения положения РДВ после пуска' на протяжении этого времени."));
 else
  dfd.AppendItem(_T("Duration of grown IAC position after start"), _T(""), 0.0f, 30.0f, 0.01f, 2, &d.iac_wrkadd_time, _T("After engine start, the IAC position will be multiplied by 'Coeff. of IAC position growth after engine start' factor during this time."));

 //injection
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Впрыск топлива:"));
 else
  dfd.AppendItem(_T("Fuel injection:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного входа в ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_enter_strokes, _T("Длительность плавного входа в ПХХ в тактах двигателя. Впрыск топлива отключится не сразу, а будет постепенно убывать на протяжении указанного кол-ва тактов."));
 else
  dfd.AppendItem(_T("Smoothing of forced idle entering"), _T("str"), 0, 255, 1, 1, &d.fi_enter_strokes, _T("Smoothing of forced idle entering in engine strokes. Fuel injection will not turn off immediately, but will gradually decrease over the specified number of strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длит. плавного выхода из ПХХ"), _T("такт"), 0, 255, 1, 1, &d.fi_leave_strokes, _T("Длительность плавного выхода из ПХХ в тактах двигателя. Впрыск топлива включится не сразу, а будет постепенно возрастать на протяжении указанного кол-ва тактов."));
 else
  dfd.AppendItem(_T("Smoothing of forced idle leaving"), _T("str"), 0, 255, 1, 1, &d.fi_leave_strokes, _T("Smoothing of forced idle entering in engine strokes. Fuel injection will not turn on immediately, but will gradually increase over the specified number of strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДПДЗ резкого выхода из режима ПХХ"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.sfc_tps_thrd, _T("Выход из режима ПХХ будет резким, а не плавным, если ДПДЗ выше установленного порога"));
 else
  dfd.AppendItem(_T("TPS threshold for immediate exit from fuel cut mode"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.sfc_tps_thrd, _T("Exiting from fuel cut mode will be immediate (not smooth) if TPS greater than this threshold"));

 std::vector<_TSTRING> use_idl_ve;
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
 {
  use_idl_ve.push_back(_T("Основная"));
  use_idl_ve.push_back(_T("Константа"));
  use_idl_ve.push_back(_T("Отдельная"));
 }
 else
 {
  use_idl_ve.push_back(_T("Main map"));
  use_idl_ve.push_back(_T("Constant"));
  use_idl_ve.push_back(_T("Sep. map"));
 }
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Таблица VE на ХХ для бензина"), use_idl_ve, &d.use_idl_ve[0], _T("Выберите какую таблицу наполения нужно использовать для режима ХХ (бензин). Варианты: основная таблица, константа, отдельная таблица."));
 else
  dfd.AppendItem(_T("VE map for idling on petrol"), use_idl_ve, &d.use_idl_ve[0], _T("Select with VE map must be used on idling (petrol). Variants: main map, constant, separate map."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Таблица VE на ХХ для газа"), use_idl_ve, &d.use_idl_ve[1], _T("Выберите какую таблицу наполения нужно использовать для режима ХХ (газ). Варианты: основная таблица, константа, отдельная таблица."));
 else
  dfd.AppendItem(_T("VE map for idling on gas"), use_idl_ve, &d.use_idl_ve[1], _T("Select with VE map must be used on idling (gas). Variants: main map, constant, separate map."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Наполнение на ХХ для бензина"), _T("коэф"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve[0], _T("Наполнение на ХХ. Выберите в выпадающем списке пункт 'Константа', если вам нужно чтобы вместо таблицы наполнения в режиме ХХ использовалась эта константа. Данный параметр используется для бензина (GAS_V = 0)."));
 else
  dfd.AppendItem(_T("VE on idling for petrol"), _T("coef"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve[0], _T("VE on idling. Select in the drop down list 'Constant' item if you want this constant to be used instead of the VE table on idling. This parameter is used for petrol (GAS_V = 0)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Наполнение на ХХ для газа"), _T("коэф"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve[1], _T("Наполнение на ХХ. Выберите в выпадающем списке пункт 'Константа', если вам нужно чтобы вместо таблицы наполнения в режиме ХХ использовалась эта константа. Данный параметр используется для газа (GAS_V = 1)."));
 else
  dfd.AppendItem(_T("VE on idling for gas"), _T("coef"), 0.0f, 1.99f, 0.01f, 2, &d.idl_ve[1], _T("VE on idling. Select in the drop down list 'Constant' item if you want this constant to be used instead of the VE table on idling. This parameter is used for LPG (GAS_V = 1)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Alpha-N: умножать время впрыска на ДПДЗ"), _T(""), 0, 2, 1, 0, &d.an_tps_mul, _T("Выбор на что умножать базовое время впрыска при работе в режиме Alpha-N: на ДАД или ДПДЗ. Если установить 0, то прошивка производит умножение на ДАД, если установить 1, то прошивка производит умножение на ДПДЗ. Если установить 2, то прошивка производит умножение на 101.3"));
 else
  dfd.AppendItem(_T("Alpha-N: multiply inj.PW by TPS"), _T(""), 0, 2, 1, 0, &d.an_tps_mul, _T("The choice of what to multiply the base injection PW when working in Alpha-N mode: by MAP or TPS. If you set 0, then the firmware multiplies by MAP, if you set 1, then the firmware multiplies by TPS. If you set it to 2, then firmware multiplies by 101.3 constant"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Манометрическое давление в топливной рампе"), _T("кПа"), 0.0f, 900.0f, 0.1f, 1, &d.frgp, _T("Манометрическое давление в топливной рампе. Установите значение больше 0, если хотите чтобы время впрыска корректировалось по разности давлений во впускном коллекторе и топливной рампе. Смотрите также параметр 'Манометрическое давление производительности форсунок'."));
 else
  dfd.AppendItem(_T("Gauge pressure in the fuel rail"), _T("kPa"), 0.0f, 900.0f, 0.1f, 1, &d.frgp, _T("Gauge pressure in the fuel rail. Set value greater than 0 if you want the injection PW to be corrected for the pressure difference in the intake manifold and the fuel rail. See also parameter 'Gauge pressure of the injector flow rate'."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Манометрическое давление производительности форсунок"), _T("кПа"), 0.0f, 1000.0f, 0.1f, 1, &d.ifr_gp, _T("Относительное давление (манометрическое) при котором форсунки имеют номинальную производительность (например 300 кПа или 3 бара."));
 else
  dfd.AppendItem(_T("Gauge pressure of the injector flow rate"), _T("kPa"), 0.0f, 1000.0f, 0.1f, 1, &d.ifr_gp, _T("Gauge pressure at which injectors have its nominal performance (for example 300 kPa or 3 bars)"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать ручную корр. впрыска на ХХ"), _T(""), 0, 1, 1, 1, &d.maninjpw_idl, _T("Установите в 0, если не хотите чтобы ручная коррекция длительности впрыска работала на ХХ. Установите в 1 и коррекция будет работать не только в режиме под нагрузкой, но и в режиме ХХ."));
 else
  dfd.AppendItem(_T("Use manual inj. PW correction on idling"), _T(""), 0, 1, 1, 1, &d.maninjpw_idl, _T("Set to 0 if you don't want the manual inj.PW correction to work at idling. Set to 1 and correction will work not only in load mode, but also in idling mode."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать таблицы коррекции нелинейности форсунок"), &d.use_injnonlin_corr, _T("При малых длительностях впрыска форсунки имеют заметную нелинейность, для компенсации которой предусмотрена специальная таблица."));
 else
  dfd.AppendItem(_T("Use injector's non-linearity correction tables"), &d.use_injnonlin_corr, _T("At small injection PWs injectors have significant non-linearity, for compensation of which a special table is provided."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка лямбда коррекции после отсечки топлива"), _T("т"), 0, 255, 1, 1, &d.ego_fc_delay, _T("После возобновления подачи топлива лямбда коррекция начнет работать не сразу, а спустя заданное время в тактах"));
 else
  dfd.AppendItem(_T("EGO correction delay after fuel cut"), _T("str"), 0, 255, 1, 1, &d.ego_fc_delay, _T("After the fuel supply is resumed, the lambda correction will not start working immediately, but after a specified time in strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка лямбда коррекции после обогащения при ускорении"), _T("т"), 0, 255, 1, 1, &d.ego_ac_delay, _T("После завершения работы обогащения при ускорении лямбда коррекция начнет работать не сразу, а спустя заданное время в тактах"));
 else
  dfd.AppendItem(_T("EGO correction delay after acceleration enrichment"), _T("str"), 0, 255, 1, 1, &d.ego_ac_delay, _T("After finishing acceleration enrichment the lambda correction will not start working immediately, but after a specified time in strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длительность ОПП перед его убыванием"), _T("такт"), 0, 1000, 1, 1, &d.aftstr_flat_strokes, _T("После запуска двигателя ОПП будет держаться на одном уровне в течение данного времени перед тем, как начнет плавно убывать."));
 else
  dfd.AppendItem(_T("Duration of ASE before it will start to decrease"), _T("str"), 0, 1000, 1, 1, &d.aftstr_flat_strokes, _T("After starting the engine, the ASE will remain at the same level for a given time before it begins to gradually decrease."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Множитель длительности предпускового впрыска"), _T("x"), 0, 10, 1, 1, &d.inj_prime_times, _T("Длительность предпускового впрыска будет увеличена в указанное количество раз"));
 else
  dfd.AppendItem(_T("Multiplier of the prime pulse duration"), _T("x"), 0, 10, 1, 1, &d.inj_prime_times, _T("Duration of the prime pulse will be increased by specified number of times"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог перехода на основную таблицу воздух/топливо"), _T("°C"), -60.0f, 180.0f, 0.25f, 2, &d.wuafr_clt_thrd, _T("Пока температура двигателя ниже этого порога (двигатель холодный), прошивка использует прогревочные таблицы 'воздух/топливо'. Когда температура превысит этот порог, то прошивка будет использовать основную таблицу 'воздух/топливо'."));
 else
  dfd.AppendItem(_T("Threshold for switching to the main AFR map"), _T("°C"), -60.0f, 180.0f, 0.25f, 2, &d.wuafr_clt_thrd, _T("While engine temperature is less this threshold (cold engine), firmware will use warm up AFR maps. Once temperature exceed this threshold firmware will switch to the main AFR map."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать ДДГ для коррекции впрыска"), &d.ifrvmc_use_fps, _T("Если галочка установлена, то датчик давления топлива будет использоваться для коррекции впрыска вместо константы 'Манометрическое давление в топливной рампе'."));
 else
  dfd.AppendItem(_T("Use FPS for correction of injection PW"), &d.ifrvmc_use_fps, _T("If the check is set, then fuel pressure sensor will be used for correction of injection PW. instead of the 'Gauge pressure in the fuel rail' constant"));

 //air conditioner
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Кондиционер:"));
 else
  dfd.AppendItem(_T("Air conditioner:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. кондиционера"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt, _T("Кондиционер включится только если температура двигателя выше этого порога, иначе запросы на включение будут игнорироваться."));
 else
  dfd.AppendItem(_T("CLT threshold for turn on of air cond."), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.aircond_clt, _T("Air conditioner will turn on only if the engine temperature is above this threshold, otherwise requests for switching on will be ignored."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДПДЗ выкл. кондиционера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.aircond_tps, _T("Компрессор кондиционера не будет включаться (или будет выключаться) если положение дроссельной заслогки превышает указанное значение."));
 else
  dfd.AppendItem(_T("TPS threshold for turning off air cond."), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.aircond_tps, _T("The compressor of air conditioner will not turn on (or it will turn off) if throttle position exceeds specified value."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка сброса оборотов до оборотов ХХ"), _T("сек"), 0.0f, 200.0f, 0.01f, 2, &d.aircond_idlrpm_delay, _T("Задержка начала перехода с оборотов кондиционирования до оборотов ХХ."));
 else
  dfd.AppendItem(_T("Delay of RPM decreasing to idling RPM"), _T("sec"), 0.0f, 200.0f, 0.01f, 2, &d.aircond_idlrpm_delay, _T("Delay of the start of transition from air conditioning RPM to idling RPM."));

 //canister purge valve
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление адсорбером:"));
 else
  dfd.AppendItem(_T("Canister purge valve:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог ДТОЖ вкл. продувки адсорбера"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.evap_clt, _T("Продувка адсорбера будет включаться только если температура двигателя выше этого порога."));
 else
  dfd.AppendItem(_T("CLT threshold for turning on of the canister purge valve"), _T("°C"), 0.0f, 120.0f, 0.25f, 2, &d.evap_clt, _T("The canister purge valve will work only if engine's temperature is above this threshold."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Нижн. порог ДПДЗ продувки адсорбера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_lo, _T("Продувка адсорбера не будет включаться если положение дроссельной заслонки ниже или равно этому порогу. Продувка не должна работать на ХХ."));
 else
  dfd.AppendItem(_T("Low TPS threshold for the canister purge valve"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_lo, _T("The canister purge valve will not turn on if throttle position is lower or equal to this threshold. Purge should not work on idling."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Верх. порог ДПДЗ продувки адсорбера"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_hi, _T("Продувка адсорбера не будет включаться если положение дроссельной заслонки выше этого порога. Продувка не должна работать в режимах полных нагрузок."));
 else
  dfd.AppendItem(_T("Hi TPS threshold for the canister purge valve"), _T("%"), 0.0f, 100.0f, 0.5f, 1, &d.evap_tps_hi, _T("The canister purge valve will not turn on if throttle position is above this threshold. Purge should not work in full load conditions."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Абсол. давление выключения продувки адсорбера"), _T("кПа"), 0.0f, 500.0f, 1.0f, 1, &d.evap_map_thrd, _T("Порог абсолютного давления на впуске, при превышении которого клапан продувки будет выключен."));
 else
  dfd.AppendItem(_T("Absolute pressure threshold for valve's turn off"), _T("kPa"), 0.0f, 500.0f, 1.0f, 1, &d.evap_map_thrd, _T("Manifold absolute pressure threshold, above which canister purge valve will be turned off."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог скорости разрешения включения продувки адсорбера"), _T("км/ч"), 1.0f, 100.0f, 0.1f, 1, &d.evap_on_vss_thrd, _T("Продувка адсорбера будет разрешена только когда скорость авто будет выше этого порога"));
 else
  dfd.AppendItem(_T("VSS threshold for enabling purge of absorber"), _T("km/h"), 1.0f, 100.0f, 0.1f, 1, &d.evap_on_vss_thrd, _T("Canister purge valve will be enabled to purge only if vehicle speed is above this threshold"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог скорости запрещения включения продувки адсорбера"), _T("км/ч"), 1.0f, 50.0f, 0.1f, 1, &d.evap_off_vss_thrd, _T("Продувка адсорбера будет запрещена когда скорость авто будет ниже этого порога"));
 else
  dfd.AppendItem(_T("VSS threshold for disabling purge of absorber"), _T("km/h"), 1.0f, 50.0f, 0.1f, 1, &d.evap_off_vss_thrd, _T("Canister purge valve will be disabled to purge if vehicle speed is below this threshold"));

 //heating control
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление подогревом:"));
 else
  dfd.AppendItem(_T("Control of heaters:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог выключения подогрева коллектора"), _T("°C"), -40.0f, 120.0f, 0.25f, 2, &d.heating_t_off, _T("Подогрев впускного коллектора будет выключен после того как температура двигателя достигнет указанного значения. Управление подогревом осуществляется посредством выхода переназначенного как INTK_HEAT."));
 else
  dfd.AppendItem(_T("Turn off threshold of intake manifold heating"), _T("°C"), -40.0f, 120.0f, 0.25f, 2, &d.heating_t_off, _T("Intake manifold heating will be turned off after the temperature of engine reaches the specified value. The heating control is carried out by means of the output reassigned as INTK_HEAT."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Макс. время работы подогрева коллектора"), _T("мин"), 0.1f, 10.9f, 0.1f, 1, &d.heating_time, _T("Подогрев впускного коллектора будет выключен по истечении этого времени даже если температурный порог еще не достигнут. Это должно предотвратить чрезмерный разряд аккумулятора."));
 else
  dfd.AppendItem(_T("Turn off threshold of intake manifold heating"), _T("min"), 0.1f, 10.9f, 0.1f, 1, &d.heating_time, _T("Intake manifold heating will be turned off after this time, even if the temperature threshold has not yet been reached. This should prevent over-discharge of the battery."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Макс. время работы подогрева газового редуктора"), _T("мин"), 0.01f, 10.0f, 0.01f, 2, &d.grheat_time, _T("Подогрев газового редуктора будет выключен по истечении этого времени когда двигатель остановлен. Это должно предотвратить чрезмерный разряд аккумулятора."));
 else
  dfd.AppendItem(_T("Timeout threshold of gas reducer's heater"), _T("min"), 0.01f, 10.0f, 0.01f, 2, &d.grheat_time, _T("Gas reducer's heating will be turned off after this time when engine is stopped. This should prevent over-discharge of the battery."));

 //cooling fan
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Управление вентилятором:"));
 else
  dfd.AppendItem(_T("Cooling fan's control:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка включения вентилятора"), _T("сек"), 0.00f, 2.5f, 0.01f, 2, &d.vent_delay, _T("Используется для включения вентилятора с задержкой чтобы РХХ успел за это время поднять обороты двигателя (актуально только для прошивок с впрыском топлива)."));
 else
  dfd.AppendItem(_T("Cooling fan's turn on delay"), _T("sec"), 0.00f, 2.5f, 0.01f, 2, &d.vent_delay, _T("It is used to turn on the fan with delay so that the IAC has time to increase the engine speed during this time (relevant only for firmware with fuel injection)."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число дискрет (шагов) ШИМ вентилятора"), _T("N"), 4, 64, 1, 1, &d.vent_pwmsteps, _T("Число шагов ШИМ вентилятора. Чем больше число шагов, тем шире зона температуры и тем более плавно вентилятор меняет скорость и наоборот, чем меньше число шагов, тем уже диапазон температуры и тем менее плавно вентилятор меняет скорость. Обратите внимание. Если вы используете низкочастотный ШИМ (ECF назначен на другой выход), то число шагов должно быть равно 16."));
 else
  dfd.AppendItem(_T("Number of cooling fan's PWM discretes"), _T("N"), 4, 64, 1, 1, &d.vent_pwmsteps, _T("Number of cooling fan's PWM steps. The larger number of steps, the wider the temperature band and fan changes speed more smoothly and vice versa, the smaller number of steps, the narrower the temperature band and fan changes speed less smoothly. Warning! If you use a low frequency PWM (ECF is assigned to other output), then number of PWM steps must be 16."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Нижняя мертвая зона ШИМ вентилятора"), _T("N"), 0, 60, 1, 1, &d.vent_minband, _T("Если текущий номер шага меньше этого значения, то вентилятор полностью выключается (или не включается). Это нужно для того, чтобы ШИМ зря не 'мучил' щетки вентилятора когда скважности не хватает для создания достаточного крутящего момента чтобы вал вращался."));
 else
  dfd.AppendItem(_T("Low dead band for cooling fan's PWM"), _T("N"), 0, 60, 1, 1, &d.vent_minband, _T("If the current step number (duty) is less than this value, the fan turns off completely (or does not turn on). This is necessary so that the PWM in vain does not 'torment' the fan's brushes when duty cycle is not enough to create torque to rotate the shaft."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число шагов ШИМ используемое при работе кондиционера"), _T("N"), 0, 60, 1, 1, &d.vent_maxband, _T("При работе кондиционера скважность ШИМ вентилятора будет определяться этим числом шагов."));
 else
  dfd.AppendItem(_T("Number of PWM steps used with air conditioner"), _T("N"), 0, 60, 1, 1, &d.vent_maxband, _T("When the air conditioner is on, the duty cycle of the fan's PWM will be determined by this number of steps."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Гистерезис при выключении ШИМ вентилятора"), _T("°C"), 0.0f, 5.0f, 0.25f, 2, &d.vent_pwm_turnoff_hyst, _T("Полное выключение ШИМ вентилятора произойдет только после после того как температура опустится ниже точки выключения минус значение гистерезиса."));
 else
  dfd.AppendItem(_T("Hysteresis for turn off of cooling fan"), _T("°C"), 0.0f, 5.0f, 0.25f, 2, &d.vent_pwm_turnoff_hyst, _T("Cooling fan's PWM will be fully turned off only after engine temperature drop below the switch off point minus value of hysteresis."));

 //Crankshaft position settings
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Положение коленвала (нов.алгор.)"));
 else
  dfd.AppendItem(_T("Crankshaft position (new algor.):"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 1"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[0], _T("Положение ВМТ 1-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 1 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[0], _T("TDC position of 1 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 2"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[1], _T("Положение ВМТ 2-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 2 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[1], _T("TDC position of 2 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 3"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[2], _T("Положение ВМТ 3-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 3 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[2], _T("TDC position of 3 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 4"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[3], _T("Положение ВМТ 4-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 4 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[3], _T("TDC position of 4 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 5"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[4], _T("Положение ВМТ 5-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 5 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[4], _T("TDC position of 5 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 6"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[5], _T("Положение ВМТ 6-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 6 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[5], _T("TDC position of 6 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 7"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[6], _T("Положение ВМТ 7-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 7 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[6], _T("TDC position of 7 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("ВМТ цилиндра 8"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[7], _T("Положение ВМТ 8-го цилиндра относительно первого зуба диска синхронизации"));
 else
  dfd.AppendItem(_T("TDC of the 8 cylinder"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.tdc_angle[7], _T("TDC position of 8 cylinder relatively to the first tooth of trigger wheel"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Угол опроса ДАД (до ВМТ)"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.smp_angle, _T("Угол начала опроса ДАД (запуск АЦП) относительно ВМТ (угол до ВМТ). Смотрите также параметр 'Опрашивать ДАД синхронно с положением коленвала'"));
 else
  dfd.AppendItem(_T("Angle of MAP's sampling (BTDC)"), _T("°"), 0.0f, 720.0f, 0.1f, 2, &d.smp_angle, _T("Angle of MAP's sampling (start of ADC) relatively to TDC (angle before TDC). See also parameter 'Sample MAP synchronously with crankshaft'."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время между искрой и началом накопления"), _T("мс"), 0.10f, 5.00f, 0.01f, 2, &d.dwl_dead_time, _T("Данный параметр ограничивает длительность импульса нокопления чтобы последнее не начиналось пока горит искра."));
 else
  dfd.AppendItem(_T("Time between spark and start of dwell"), _T("ms"), 0.10f, 5.00f, 0.01f, 2, &d.dwl_dead_time, _T("This parameter limits pulse width of dwell so that the latter does not start while the spark is on"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число зубьев пропускаемое перед пуском"), _T("N"), 0, 500, 1, 1, &d.ckps_skip_trig, _T("Число зубьев, которое система проигнорирует перед поиском синхрометки в начале пуска двигателя."));
 else
  dfd.AppendItem(_T("Number of teeth to skip at the beginning of cranking"), _T("N"), 0, 500, 1, 1, &d.ckps_skip_trig, _T("Number of teeth, system will ignore before looking for missing tooth at the beginning of cranking"));

 //LTFT
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Долговременная лямбда-коррекция (LTFT)"));
 else
  dfd.AppendItem(_T("Long Term Fuel Trim"));

 std::vector<_TSTRING> ltft_algos;
 ltft_algos.push_back(_T("Standard"));
 ltft_algos.push_back(_T("Kosh"));
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Выбор алгоритма LTFT"), ltft_algos, &d.ltft_algo, _T("Выбор алгоритма LTFT. Параметры, которые не используются алгоритмом 'Kosh' отмечены *"));
 else
  dfd.AppendItem(_T("Select LTFT algorithm"), ltft_algos, &d.ltft_algo, _T("Selection of LTFT algorithm. Parameters, which are not used by 'Kosh' algorithm are marked with *"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Допуск на стационарность для адаптации*"), _T("%"), 0.0f, 50.0f, 0.5f, 1, &d.ltft_cell_band, _T("Определяет зону вокруг обучаемой ячейки таблицы, в которую должна попасть режимная точка. Используется по осям оборотов и нагрузки."));
 else
  dfd.AppendItem(_T("Stationarity tolerance for learning*"), _T("%"), 0.0f, 50.0f, 0.5f, 1, &d.ltft_cell_band, _T("Defines zone around the learning cell in table into which the regime point should fall. It is used for rpm and load axises."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время стационарности рабочей точки в секундах*"), _T("сек"), 0.01f, 2.5f, 0.01f, 2, &d.ltft_stab_time, _T("Адаптация ячейки происходит только когда рабочая точка находится в ее зоне в течение этого времени. Этот параметр используется только если соответствующий параметр в тактах установлено в 0."));
 else
  dfd.AppendItem(_T("Stationarity time for regime point in second*"), _T("sec"), 0.01f, 2.5f, 0.01f, 2, &d.ltft_stab_time, _T("Learning of a cell will be performed only when regime point resides in its zone during this period of time. This parameter is used only if corresponding parameter in strokes is set to 0."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время стационарности рабочей точки в тактах*"), _T("т"), 0, 255, 1, 0, &d.ltft_stab_str, _T("Адаптация ячейки происходит только когда рабочая точка находится в ее зоне в течение этого числа тактов. Если это значение установлено в 0, то вместо тактов используется время."));
 else
  dfd.AppendItem(_T("Stationarity time for regime point in strokes*"), _T("str"), 0, 255, 1, 0, &d.ltft_stab_str, _T("Learning of a cell will be performed only when regime point resides in its zone during this number of strokes. If this parameter is set to 0, then time is used instead of strokes."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Радиус обучения соседей*"), _T(""), 0, 16, 1, 1, &d.ltft_neigh_rad, _T("Определяет удаление (от текущей ячейки в таблице) в пределах которого обучаются соседние ячейки. Чем выше значение, тем на более дальние окружающие ячейки оказывается влияние."));
 else
  dfd.AppendItem(_T("Neighbours' learning radius*"), _T(""), 0, 16, 1, 1, &d.ltft_neigh_rad, _T("Determines the distance (from the current cell in the table) within which adjacent cells are trained(altered). The higher the value, the more distant surrounding cells are affected."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Число устойчивых переключений уровня сигнала ДК*"), _T(""), 0, 16, 1, 1, &d.ltft_sigswt_num, _T("Требуемое число устойчивых переключений уровня сигнала ДК во время нахождения рабочей точки в зоне ячейки. Чем выше значение, тем медленнее будет происходть адаптация."));
 else
  dfd.AppendItem(_T("Number of successive switches of EGO sensor's signal*"), _T(""), 0, 16, 1, 1, &d.ltft_sigswt_num, _T("The required number of successive switchings of the EGO sensor's signal level while the operating point is in the cell area. The higher the value, the slower the learning will be."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Обучать таблицу LTFT на ХХ*"), &d.ltft_on_idling, _T("Если галочка установлена, то обновление значений в таблице LTFT будет происходить при закрытом дросселе. В противном случае обновление значений LTFT будет происходить только в рабочих режимах."));
 else
  dfd.AppendItem(_T("Learn LTFT map on idling*"), &d.ltft_on_idling, _T("If the checkbox is checked, then LTFT map will be updated on idling, otherwise LTFT map will be updated only on working modes."));

 //Bluetooth
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Параметры Bluetooth:"));
 else
  dfd.AppendItem(_T("Bluetooth parameters:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать АТ-команды в Bluetooth на скорости 9600"), &d.btbaud_use[0], _T("Слать АТ-команды в Bluetooth на скорости 9600 бод. Команды дублируются на разных скоростях. Чем больше скоростей разрешено, тем дольше происходит конфигурирование Bluetooth."));
 else
  dfd.AppendItem(_T("Send AT-commands to Bluetooth using 9600 baud"), &d.btbaud_use[0], _T("Send AT-commands to Bluetooth using 9600 baud rate. Commands are duplicated with different baud rates. The more baud rates are allowed, the longer it takes to configure Bluetooth."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать АТ-команды в Bluetooth на скорости 19200"), &d.btbaud_use[1], _T("Слать АТ-команды в Bluetooth на скорости 19200 бод. Команды дублируются на разных скоростях. Чем больше скоростей разрешено, тем дольше происходит конфигурирование Bluetooth."));
 else
  dfd.AppendItem(_T("Send AT-commands to Bluetooth using 19200 baud"), &d.btbaud_use[1], _T("Send AT-commands to Bluetooth using 19200 baud rate. Commands are duplicated with different baud rates. The more baud rates are allowed, the longer it takes to configure Bluetooth."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать АТ-команды в Bluetooth на скорости 38400"), &d.btbaud_use[2], _T("Слать АТ-команды в Bluetooth на скорости 38400 бод. Команды дублируются на разных скоростях. Чем больше скоростей разрешено, тем дольше происходит конфигурирование Bluetooth."));
 else
  dfd.AppendItem(_T("Send AT-commands to Bluetooth using 38400 baud"), &d.btbaud_use[2], _T("Send AT-commands to Bluetooth using 38400 baud rate. Commands are duplicated with different baud rates. The more baud rates are allowed, the longer it takes to configure Bluetooth."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать АТ-команды в Bluetooth на скорости 57600"), &d.btbaud_use[3], _T("Слать АТ-команды в Bluetooth на скорости 57600 бод. Команды дублируются на разных скоростях. Чем больше скоростей разрешено, тем дольше происходит конфигурирование Bluetooth."));
 else
  dfd.AppendItem(_T("Send AT-commands to Bluetooth using 57600 baud"), &d.btbaud_use[3], _T("Send AT-commands to Bluetooth using 57600 baud rate. Commands are duplicated with different baud rates. The more baud rates are allowed, the longer it takes to configure Bluetooth."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать АТ-команды в Bluetooth на скорости 115200"), &d.btbaud_use[4], _T("Слать АТ-команды в Bluetooth на скорости 115200 бод. Команды дублируются на разных скоростях. Чем больше скоростей разрешено, тем дольше происходит конфигурирование Bluetooth."));
 else
  dfd.AppendItem(_T("Send AT-commands to Bluetooth using 115200 baud"), &d.btbaud_use[4], _T("Send AT-commands to Bluetooth using 115200 baud rate. Commands are duplicated with different baud rates. The more baud rates are allowed, the longer it takes to configure Bluetooth."));

 //miscellaneous
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Прочие константы:"));
 else
  dfd.AppendItem(_T("Other constants:"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Алгоритм прогнозирования положения коленвала для ДХ"), _T(""), 0, 1, 1, 0, &d.hall_predict, _T("Выбор алгоритма прогнозирования положения коленвала в прошивках с синхронизацией от ДХ (Nзуб=Nцил). 0 - последний интервал. 1 - Первая производная"));
 else
  dfd.AppendItem(_T("Algorithm of crankshaft position prediction for hall"), _T(""), 0, 1, 1, 0, &d.hall_predict, _T("Selection of algorithm of crankshaft position prediction for firmwares with synchronization from a Hall sensor (Nteeth=Ncyl). 0 - Last interval. 1 - First derivative."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Множитель частоты импульсов на выходе для тахометра (VTACHOM)"), _T("*"), 0.125f, 16.0f, 0.001f, 3, &d.vtachom_mult, _T("Данный множитель определяет соотношение оборотов и частоты импульсов для тахометра выдаваемых на выход VTACHOM. Например, 1.000 означает, что за один оборот коленвала выдается 1 импульс. Чем выше значение множителя, тем выше частота."));
 else
  dfd.AppendItem(_T("Frequency multiplier for tachometer output (VTACHOM)"), _T("*"), 0.125f, 16.0f, 0.001f, 3, &d.vtachom_mult, _T("This multiplier specify ratio of RPM and frequency of pulses produced at VTACHOM output. For example, 1.000 means that for 1 revolution of crankshaft 1 pulse is produced. The higher the value of multiplier, the higher the frequency."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Передавать вместо ADD_I1 значение другого входа"), _T(""), 1, 8, 1, 0, &d.add_i1_sub, _T("Выбор что прошивка передает вместо значения входа ADD_I1. 1 - вход ADD_I1 (по умолчанию), 2 - вход ADD_I2, 3 - вход ADD_I3, 4 - вход ADD_I4 и т.д."));
 else
  dfd.AppendItem(_T("Send value of another input instead of ADD_I1"), _T(""), 1, 8, 1, 0, &d.add_i1_sub, _T("Select what firmware should send instead of ADD_I1 value. 1 - ADD_I1 input (default), 2 - ADD_I2 input, 3 - ADD_I3 input, 4 - ADD_I4 input etc"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Передавать вместо ADD_I2 значение другого входа"), _T(""), 1, 8, 1, 0, &d.add_i2_sub, _T("Выбор что прошивка передает вместо значения входа ADD_I2. 1 - вход ADD_I1, 2 - вход ADD_I2 (по умолчанию), 3 - вход ADD_I3, 4 - вход ADD_I4 и т.д."));
 else
  dfd.AppendItem(_T("Send value of another input instead of ADD_I2"), _T(""), 1, 8, 1, 0, &d.add_i2_sub, _T("Select what firmware should send instead of ADD_I2 value. 1 - ADD_I1 input, 2 - ADD_I2 input (default), 3 - ADD_I3 input, 4 - ADD_I4 input etc"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Время открытого газового клапана на не запущенном двигателе"), _T("сек"), 1.0f, 100.0f, 0.1f, 1, &d.gasval_ontime, _T("Время на протяжении которого выход управляющий газовым клапаном (GASVAL_O) будет включен на не запущеном двигателе."));
 else
  dfd.AppendItem(_T("Time of opened gas valve on stopped engine"), _T("sec"), 1.0f, 100.0f, 0.1f, 1, &d.gasval_ontime, _T("During this time output which controls gas valve (GASVAL_O) will be turned on when engine is stopped."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Отсечка зажигания и впрыска при низком давлении масла"), _T(""), 0, 1, 1, 0, &d.oilpress_cut, _T("Защита двигателя от работы при низком или отсутствующем давлении масла. 1 - включить защиту, 0 - выключить."));
 else
  dfd.AppendItem(_T("Allow ignition and fuel injection cut off when oil pressure is low"), _T(""), 0, 1, 1, 0, &d.oilpress_cut, _T("Protection of engine against deterioration when pressure of oil is too low"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Мин. значение dt используемое для вычисления d%/dt ДЗ"), _T("мс"), 5.0f, 200.0f, 1.0f, 1, &d.tpsdot_mindt, _T("Укажите значение дифференциала времени используемого для вычисления скорости движения дроссельной заслонки"));
 else
  dfd.AppendItem(_T("Minimum value of dt used for calculation of the d%/dt for TPS"), _T("ms"), 5.0f, 200.0f, 1.0f, 1, &d.tpsdot_mindt, _T("Specify value of the time differencial used by calculation of the speed of moving throttle"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Мин. значение dt используемое для вычисления dP/dt ДАД"), _T("мс"), 5.0f, 200.0f, 1.0f, 1, &d.mapdot_mindt, _T("Укажите значение дифференциала времени используемого для вычисления скорости изменения давления на впуске"));
 else
  dfd.AppendItem(_T("Minimum value of dt used for calculation of the dP/dt for MAP"), _T("ms"), 5.0f, 200.0f, 1.0f, 1, &d.mapdot_mindt, _T("Specify value of the time differencial used by calculation of the MAP dot"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Длительность работы блока после выключения зажигания."), _T("сек"), 0.01f, 100.0f, 0.1f, 2, &d.pwron_time, _T("Блок продолжит работу указанное время после выключения зажигания (завершения работы 'задержки реакции на пропадание уровня на входе IGN (IGN_I)'). Подразумевается использование управления питанием блока."));
 else
  dfd.AppendItem(_T("The duration of the unit's operation after the ignition is turned off. "), _T("sec"), 0.01f, 100.0f, 0.1f, 2, &d.pwron_time, _T("Unit will continue to operate specified time after switching ignition off."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Задержка реакции на пропадание уровня на входе IGN (IGN_I)."), _T("сек"), 0.0f, 10.0f, 0.1f, 2, &d.pwron_time1, _T("Блок продолжит работу указанное время после выключения зажигания, все функции (импульсы зажигания, впрыск и т.д.) в это время продолжают работать. Подразумевается использование управления питанием блока."));
 else
  dfd.AppendItem(_T("Delay in response to loss of level at the IGN input (IGN_I)"), _T("sec"), 0.0f, 10.0f, 0.1f, 2, &d.pwron_time1, _T("Unit will continue to operate specified time after switching ignition off, all functions (ignition pulses, injection etc) will continue to work during this time."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Номер универсального выхода для управления питанием"), _T(""), 0, 6, 1, 0, &d.pwrrelay_uni, _T("Выбор универсального выхода, условия которого будут использованы при управлении реле питания блока. Результат вычисления условий универ.выхода объединяется с результатом вычисления других условий через функцию 'И'. Установите значение 0 для отключения этой функции."));
 else
  dfd.AppendItem(_T("Number of univ. output for controlling power relay"), _T(""), 0, 6, 1, 0, &d.pwrrelay_uni, _T("Use conditions of the selected universal output for control of power management. The result calculated from universal output's conditions is combined with the result calculated from other conditions through the logic 'AND' function. Set this value to 0 if you want to disable this function."));
  
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Разрешить режим молчания для основной прошивки"), &d.uart_silent, _T("Если галочка установлена, то после включения зажигания прошивка не будет отправлять пакеты с данными до получения команды от программы. Режим молчания позволяет экономить вычислительные ресурсы процессора."));
 else
  dfd.AppendItem(_T("Allow silent mode for main firmware"), &d.uart_silent, _T("If the checkbox is checked, then after the ignition is turned on, the firmware will not send data packets until it receives a command from the program. Silent mode allows to spare computational resources of processor."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать таблицу коррекции плотности топлива"), _T(""), 0, 3, 1, 0, &d.fueldens_corr_use, _T("0 - использовать только для бензина (GAS_V=0); 1 - использовать только для газа (GAS_V=1); 2 - использовать и для бензина и для газа; 3 - не использовать вообще."));
 else
  dfd.AppendItem(_T("Use fuel density correction map"), _T(""), 0, 3, 1, 0, &d.fueldens_corr_use, _T("0 - use for petrol only (GAS_V=0); 1 - use for gas only (GAS_V=1); 2 - use for both (petrol and gas); 3 - don't use at all."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать датчик температуры топлива"), &d.fts_source, _T("Если галочка установлена, то температура топлива определяется датчиком. В противном случае температура топлива определяется по модели на основе ДТОЖ и ДТВ."));
 else
  dfd.AppendItem(_T("Use fuel temperature sensor"), &d.fts_source, _T("If the checkbox is checked, then temperature of fuel is defined by sensor. Otherwise, temperature is defined by model using CLT and IAT."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Таймаут автосохранения параметров"), _T("сек"), .0f, 60.0f, 0.1f, 2, &d.save_param_timeout, _T("Блок SECU-3 следит за состоянием параметров и если после последнего редактирования прошло указанное время, то параметры будут автоматически сохранены. Установите 0, если вам нужно запретить автосохранение."));
 else
  dfd.AppendItem(_T("Timeout for parameters' automatic saving"), _T("sec"), .0f, 60.0f, 0.1f, 2, &d.save_param_timeout, _T("SECU-3 unit monitors the state of the parameters and if the specified time has passed since the last editing, the parameters will be automatically saved. Set to 0 if you need to disable autosaving."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог отсечки топлива по скорости авто"), _T("км/ч"), 0.0f, 100.0f, 0.1f, 1, &d.fuelcut_vss_thrd, _T("Отсечка топлива на ходу (ЭПХХ) не будет применяться если скорость авто ниже этого порога"));
 else
  dfd.AppendItem(_T("Vehicle speed threshold for fuel cut"), _T("km/h"), 0.0f, 100.0f, 0.1f, 1, &d.fuelcut_vss_thrd, _T("Fuel cut off on run will not be applied if vehicle's speed is less than this threshold"));

 std::vector<_TSTRING> dashboards;
 dashboards.push_back(_T("OFF"));
 dashboards.push_back(_T("Lada Granta"));
 dashboards.push_back(_T("Nissan Almera Classic"));
 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Выбор CAN приборной панели"), dashboards, &d.can_dashboard, _T("Выбор модели приборной панели подключаемой через CAN-шину."));
 else
  dfd.AppendItem(_T("Select CAN dashboard"), dashboards, &d.can_dashboard, _T("Select model of dashboard connected via CAN-bus"));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Порог работы лямбда коррекции по ДТВГ"), _T("°C"), 0.0f, 1000.0f, 0.25f, 2, &d.lambda_egts_thrd, _T("Коррекция смеси по ДК начнет работу только если температура выхлопных газов превысит этот порог."));
 else
  dfd.AppendItem(_T("EGTS threshold for lambda correction"), _T("°C"), 0.0f, 1000.0f, 0.25f, 2, &d.lambda_egts_thrd, _T("EGO correction will start to work if exhaust gas temperature exceeds this threshold."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Использовать порог работы лямбда коррекции по ДТВГ"), &d.lambda_use_egts, _T("Если галочка установлена, то будет использоваться порог работы лямбда коррекции по ДТВГ."));
 else
  dfd.AppendItem(_T("Use EGTS threshold for lambda correction"), &d.lambda_use_egts, _T("If the check is set, then EGTS threshold for lambda correction will be used."));

 if (mp_settings->GetInterfaceLanguage() == IL_RUSSIAN)
  dfd.AppendItem(_T("Слать пакеты с отладочными переменными"), &d.use_dbgvar, _T("Если галочка установлена, то прошивка будет отправлять пакеты с отладночными переменными. Прошивка должна быть скомпилирована с опцией DEBUG_VARIABLES"));
 else
  dfd.AppendItem(_T("Send packets with debug variables"), &d.use_dbgvar, _T("If the check is set, then firmware will send packets with debug variables. Firmware must be compiled with DEBUG_VARIABLES option."));

 if (dfd.DoModal()==IDOK)
 {
  mp_fwdm->SetFwConstsData(d);
 }
}
