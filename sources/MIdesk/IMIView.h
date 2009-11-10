 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#pragma once

namespace SECU3IO
{
 struct SensorDat;
}

//интерфейс для управления панелью измерительных инструментов (представлением)
//TODO: Если в будущем появится необходимость обрабатывать UI события от представления (оно станет активным)
//то надо засунуть в интерфейс соответствующие функции назначения делегатов. Для этого можно использовать
//boost::bind или FastDelegate.

class IMIView  
{
 public:
  virtual void Show(bool show) = 0;                                //показать/спрятать контент представления
  virtual void Enable(bool enable) = 0;                            //разрешение/запрещение представления
  virtual void SetValues(const SECU3IO::SensorDat* i_values) = 0;  //загоняет данные в представление
  virtual void GetValues(SECU3IO::SensorDat* o_values) = 0;        //извлекает данные из представления	
};
