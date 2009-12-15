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
  //Округление в int
  template <class FPN>
  inline int Round(FPN i_value) 
  {
   return (int)((i_value) + 0.5f - (FPN)((i_value) < 0)); 
  }
  
  //Округление в указанный тип
  template <class ITN, class FPN>
  inline ITN Round(FPN i_value) 
  {
   return (ITN)((i_value) + 0.5f - (FPN)((i_value) < 0)); 
  }

  //Свертка для одномерной функции.
  //i_kernel_size - любое число
  template <class T>
  bool Convolve1D(T* ip_in, T* op_out, size_t i_data_size, const T* ip_kernel, size_t i_kernel_size)
  {
   size_t i, j, k;

   //Проверяем правильность входных параметров
   if(!ip_in || !op_out || !ip_kernel)
    return false;
   if(i_data_size <=0 || i_kernel_size <= 0)
    return false;

   // начинаем сворачивание от out[i_kernel_size-1] до out[i_data_size-1] (последний)
   for(i = i_kernel_size-1; i < i_data_size; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; k < i_kernel_size; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }

   // продолжаем сворачивание от out[0] до out[i_kernel_size-2]
   for(i = 0; i < i_kernel_size - 1; ++i)
   {
    op_out[i] = 0;
    for(j = i, k = 0; ((int)j) >= 0; --j, ++k)
     op_out[i] += ip_in[j] * ip_kernel[k];
   }   
   return true;
  }

  //Сглаживание для одномерной функции методом скользящего среднего с симметричным наложением ядра 
  //i_kernel_size - нечетное число! Например 1,3,5,7...
  template <class T>
  bool Smooth1D(T* ip_in, T* op_out, size_t i_data_size, size_t i_kernel_size)
  {
   //Проверяем правильность входных параметров
   if (!(i_kernel_size & 1) || i_kernel_size <= 0)
    return false;
   if(!ip_in || !op_out || i_data_size <=0)
    return false;

   int j, k, k2 = i_kernel_size / 2;
   for(size_t i = 0; i < i_data_size; ++i)
   {
    op_out[i] = 0;
    T divisor = 0;
    for(k = -k2; k <= k2; ++k)
    {
     j = (i + k);
     if (j >= 0 && j < i_data_size)
     {
      op_out[i]+= ip_in[j];
      divisor+= 1;
     }
    }
    op_out[i]/= divisor;
   }
   return true;
  }

}
