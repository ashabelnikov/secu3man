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

  template <class T>
  bool Convolve1D(T* ip_in, T* op_out, size_t i_dataSize, const T* ip_kernel, size_t i_kernelSize)
  {
   size_t i, j, k;

   //Проверяем правильность входных параметров
   if(!ip_in || !op_out || !ip_kernel)
    return false;
   if(i_dataSize <=0 || i_kernelSize <= 0)
    return false;

   // начинаем сворачивание от out[i_kernelSize-1] до out[i_dataSize-1] (последний)
   for(i = i_kernelSize-1; i < i_dataSize; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; k < i_kernelSize; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }

   // продолжаем сворачивание от out[0] до out[i_kernelSize-2]
   for(i = 0; i < i_kernelSize - 1; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; ((int)j) >= 0; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }
   return true;
  }

}
