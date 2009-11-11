/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "MPSZFileDataIO.h"

#include <vector>
#include "common\MathHelpers.h"

/////////////////////////////////////////////////////////////////////////////////////
//фабрика объектов дл€ типов файлов таблиц MPSZ
class MPSZIOFactory
{
 public:
  MPSZIOFactory() {};
  ~MPSZIOFactory()
  {
   for(size_t i = 0; i < garbage_list.size(); i++)
    delete garbage_list[i];						
  }

  MPSZDataBase* CreateIOObject(MPSZFileDataIO::EFileTypes i_type)
  {
   MPSZDataBase* p_object = NULL;
   switch(i_type)
   {
    case MPSZFileDataIO::FILE_TYPE_MPX:
     p_object = new MPSZDataMPX_IO();
     garbage_list.push_back(p_object);
     return p_object;	  

    case MPSZFileDataIO::FILE_TYPE_MPZ:
     p_object = new MPSZDataMPZ_IO();
     garbage_list.push_back(p_object);
     return p_object;

    default:
     ASSERT(0);
     return NULL;
   }		
  }

 private:
  std::vector<MPSZDataBase*> garbage_list;
};

/////////////////////////////////////////////////////////////////////////////////////
std::vector<_TSTRING> MPSZMapsDataHolder::GetListOfNames(void) const
{
 std::vector<_TSTRING> list;
 for (int i = 0; i < m_actual_sets_num; i++)
  if (maps[i].name!=_T(""))
   list.push_back(maps[i].name);
  else
  { //искусственное им€
   TCHAR name[32];
   _stprintf(name,MLL::GetString(IDS_MAP_NO_NAME).c_str(),i+1); 
   list.push_back(_TSTRING(name));
  }
 return list;
};

/////////////////////////////////////////////////////////////////////////////////////
bool MPSZFileDataIO::Load(const _TSTRING i_file_name, MPSZFileDataIO::EFileTypes i_file_type)
{
 CFile file;   
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(),CFile::modeRead,&ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }

 BYTE rawdata[65536]; 
 MPSZIOFactory factory;
 MPSZDataBase* p_object = factory.CreateIOObject(i_file_type);
 int size = p_object->GetRequiredRawSize();
 if (file.GetLength()!=size)
 {
  file.Close();  
  return false;  //некорректный размер файла
 }

 file.Read(rawdata,size);
 file.Close();

 p_object->operator()(rawdata,&m_data);
 return true;
}

/////////////////////////////////////////////////////////////////////////////////////
bool MPSZFileDataIO::Save(const _TSTRING i_file_name, MPSZFileDataIO::EFileTypes i_file_type)
{
 CFile file;   
 CFileException ex;
 TCHAR szError[1024];
 if(!file.Open(i_file_name.c_str(), CFile::modeWrite|CFile::modeCreate, &ex))
 {
  ex.GetErrorMessage(szError, 1024);
  AfxMessageBox(szError);
  return false;
 }

 BYTE rawdata[65536]; 
 MPSZIOFactory factory;
 MPSZDataBase* p_object = factory.CreateIOObject(i_file_type);
 int size = p_object->GetRequiredRawSize();
 //получаем необходимые бинарные данные и затем сохран€ем их
 p_object->operator()(&m_data, rawdata);
 file.Write(rawdata, size);
 file.Close();
 return true;
}

const MPSZMapsDataHolder& MPSZFileDataIO::GetData()
{
 return m_data;
}

MPSZMapsDataHolder& MPSZFileDataIO::GetDataLeft()
{
 return m_data;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void MPSZDataMPX_IO::operator()(const BYTE* ip_rawdata, MPSZMapsDataHolder* op_data)
{
 ASSERT(op_data);

 #pragma pack(1) //¬нимание! выравнивание 1 байт.

 struct MPXStructure
 {
  WORD names[MPSZ_NUMBER_OF_MAPS][MPSZ_MAPS_NAME_SIZE];
  WORD idle_and_start[MPSZ_NUMBER_OF_MAPS][MPSZ_START_MAP_SIZE];
  WORD work[MPSZ_NUMBER_OF_MAPS][MPSZ_WORK_MAP_SIZE_L][MPSZ_WORK_MAP_SIZE_F];
 }; 

 MPXStructure* p_raws = (MPXStructure*)ip_rawdata;
 int i, j;

 USES_CONVERSION;
 //выт€гиваем имена наборов характеристик
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {  
  char raw_str[32];
  memset(raw_str,0,32);
  for(j = 0; j < MPSZ_MAPS_NAME_SIZE; j++)
   raw_str[j] = HIBYTE(p_raws->names[i][j]);
  op_data->maps[i].name = A2T(raw_str); 
 }
  
 //104 - дибильное магическое число 
 //выт€гиваем пусковую карту 
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {
  for(j = 0; j < MPSZ_START_MAP_SIZE; j++)
   op_data->maps[i].f_str[j] = (104 - HIBYTE(p_raws->idle_and_start[i][j])) / 2.0f;
 } 

 //выт€гиваем карту ’’
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {
  for(j = 0; j < MPSZ_IDLE_MAP_SIZE; j++)
   op_data->maps[i].f_idl[j] = (56 - LOBYTE(p_raws->idle_and_start[i][j])) / 2.0f;
 } 

 //выт€гиваем рабочую карту
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 { 
  int k = 0;
  for(j = MPSZ_WORK_MAP_SIZE_L-1; j >= 0; j--, k++)
  {
   for(int f = 0; f < MPSZ_WORK_MAP_SIZE_F; f++)
    op_data->maps[i].f_wrk[(k*MPSZ_WORK_MAP_SIZE_F)+f] = (104 - HIBYTE(p_raws->work[i][j][f])) / 2.0f;
  }
 } 

 op_data->m_actual_sets_num = GetActualSetsNumber();
}

void MPSZDataMPX_IO::operator()(const MPSZMapsDataHolder* ip_data, BYTE* op_rawdata)
{
 ASSERT(ip_data);

 #pragma pack(1)

 struct MPXStructure
 {
  WORD names[MPSZ_NUMBER_OF_MAPS][MPSZ_MAPS_NAME_SIZE];
  WORD idle_and_start[MPSZ_NUMBER_OF_MAPS][MPSZ_START_MAP_SIZE];
  WORD work[MPSZ_NUMBER_OF_MAPS][MPSZ_WORK_MAP_SIZE_L][MPSZ_WORK_MAP_SIZE_F];
 }; 

 MPXStructure* p_raws = (MPXStructure*)op_rawdata;
 int i, j;

 USES_CONVERSION;
 //засовываем имена наборов характеристик
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {  
  char raw_str[32];
  memset(raw_str,0,32);
  strcpy(raw_str,T2A(const_cast<TCHAR*>(ip_data->maps[i].name.c_str())));
  for(j = 0; j < MPSZ_MAPS_NAME_SIZE; j++)
   p_raws->names[i][j] = MAKEWORD(0,raw_str[j]);
 }
  
 //104 - дибильное магическое число 
 //засовываем пусковую карту 
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {
  for(j = 0; j < MPSZ_START_MAP_SIZE; j++)
   p_raws->idle_and_start[i][j] = MAKEWORD(0,(104 - MathHelpers::Round(ip_data->maps[i].f_str[j] * 2.0f)));
 } 

 //засовываем карту ’’ (при этом не затираем пусковую карту)
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 {
  for(j = 0; j < MPSZ_IDLE_MAP_SIZE; j++)
   p_raws->idle_and_start[i][j] = MAKEWORD((56 - MathHelpers::Round(ip_data->maps[i].f_idl[j] * 2.0f)), HIBYTE(p_raws->idle_and_start[i][j]));    
 } 

 //засовываем рабочую карту
 for (i = 0; i < MPSZ_NUMBER_OF_MAPS; i++)
 { 
  int k = 0;
  for(j = MPSZ_WORK_MAP_SIZE_L-1; j >= 0; j--, k++)
  {
   for(int f = 0; f < MPSZ_WORK_MAP_SIZE_F; f++)
    p_raws->work[i][j][f] = MAKEWORD(0,(104 - MathHelpers::Round(ip_data->maps[i].f_wrk[(k*MPSZ_WORK_MAP_SIZE_F)+f] * 2.0f)));
  }
 } 
}

////////////////////////////////////////////////////////////////////////////////
void MPSZDataMPZ_IO::operator()(const BYTE* ip_rawdata, MPSZMapsDataHolder* op_data)
{
 ASSERT(op_data);

 #pragma pack(1)

 struct MPZStructure
 {
  BYTE work[MPSZ_WORK_MAP_SIZE_L][MPSZ_WORK_MAP_SIZE_F];
  BYTE str[MPSZ_START_MAP_SIZE];
  BYTE name[MPSZ_MAPS_NAME_SIZE];
  BYTE idl[MPSZ_IDLE_MAP_SIZE];
 }; 
  
 MPZStructure* p_raws = (MPZStructure*)ip_rawdata;
 int i = 0, j;

 //выт€гиваем рабочую карту  
 int k = 0;
 for(j = MPSZ_WORK_MAP_SIZE_L-1; j >= 0; j--, k++)
 {
  for(int f = 0; f < MPSZ_WORK_MAP_SIZE_F; f++)
   op_data->maps[i].f_wrk[(k*MPSZ_WORK_MAP_SIZE_F)+f] = (p_raws->work[j][f]) / 2.0f;
 }
  
 //выт€гиваем пусковую карту 
 for(j = 0; j < MPSZ_START_MAP_SIZE; j++)
  op_data->maps[i].f_str[j] = (p_raws->str[j]) / 2.0f;

 USES_CONVERSION;
 char raw_str[32];
 memset(raw_str,0,32);
 for(j = 0; j < MPSZ_MAPS_NAME_SIZE; j++)
  raw_str[j] = p_raws->name[j];
 op_data->maps[i].name = A2T(raw_str);

 //выт€гиваем карту ’’
 for(j = 0; j < MPSZ_IDLE_MAP_SIZE; j++)
  op_data->maps[i].f_idl[j] = (p_raws->idl[j]) / 2.0f;

 op_data->m_actual_sets_num = GetActualSetsNumber();
}

void MPSZDataMPZ_IO::operator()(const MPSZMapsDataHolder* ip_data, BYTE* op_rawdata)
{
 ASSERT(ip_data);

 #pragma pack(1)

 struct MPZStructure
 {
  BYTE work[MPSZ_WORK_MAP_SIZE_L][MPSZ_WORK_MAP_SIZE_F];
  BYTE str[MPSZ_START_MAP_SIZE];
  BYTE name[MPSZ_MAPS_NAME_SIZE];
  BYTE idl[MPSZ_IDLE_MAP_SIZE];
 };

 MPZStructure* p_raws = (MPZStructure*)op_rawdata;
 int i = 0, j;

 //засовываем рабочую карту
 int k = 0;
 for(j = MPSZ_WORK_MAP_SIZE_L-1; j >= 0; j--, k++)
 {
  for(int f = 0; f < MPSZ_WORK_MAP_SIZE_F; f++)
   p_raws->work[j][f] = MathHelpers::Round((ip_data->maps[i].f_wrk[(k*MPSZ_WORK_MAP_SIZE_F)+f]) * 2.0f);
 }

 //засовываем пусковую карту
 for(j = 0; j < MPSZ_START_MAP_SIZE; j++)
  p_raws->str[j] = MathHelpers::Round((ip_data->maps[i].f_str[j]) * 2.0f);

 //засовываем fucking им€
 USES_CONVERSION;
 char raw_str[32];
 memset(raw_str,0,32);
 strcpy(raw_str,T2A(const_cast<TCHAR*>(ip_data->maps[i].name.c_str())));
 for(j = 0; j < MPSZ_MAPS_NAME_SIZE; j++) //копируем только определенное кол-во символов
   p_raws->name[j] = raw_str[j];

 //засовываем карту ’’
 for(j = 0; j < MPSZ_IDLE_MAP_SIZE; j++)
  p_raws->idl[j] = MathHelpers::Round((ip_data->maps[i].f_idl[j]) * 2.0f);
}
