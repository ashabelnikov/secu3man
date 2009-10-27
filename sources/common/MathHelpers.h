 /****************************************************************
 *
 *  Created by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008. 
 *   ICQ: 405-791-931. e-mail: shabelnikov-stc@mail.ru
 *  Microprocessors systems - design & programming.
 *
 *****************************************************************/

#pragma once

namespace MathHelpers
{
  //для правильного преобразования в целые числа (округление)
  inline int Round(float i_value) 
  {
   return (int)((i_value) + 0.5f - (float)((i_value) < 0)); 
  }
  
  inline int Round(double i_value) 
  { 
   return (int)((i_value) + 0.5 - (double)((i_value) < 0)); 
  }
}
