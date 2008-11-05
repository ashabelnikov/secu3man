/****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2008.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "MPSZImpExpController.h"
#include "MapImpExpDlg.h"
#include "common/FastDelegate.h"
#include "MPSZFileDataIO.h"

using namespace fastdelegate;


MPSZImportController::MPSZImportController(FWMapsDataHolder* ip_fwd)
: mp_view(NULL)
, mp_fwd(ip_fwd)
, m_mpsz_file_name(_T(""))
{
 mp_view = new CMapImpExpDlg();
 ASSERT(mp_fwd);
 
 //��������� ����������� ������� �� �������������
 mp_view->setOnOkButton(MakeDelegate(this,&MPSZImportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&MPSZImportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&MPSZImportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&MPSZImportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&MPSZImportController::IsExchangeButtonAllowed));

}

int MPSZImportController::DoImport(void)
{   
   static TCHAR BASED_CODE szFilter[] = _T("MPSZ Files (*.mpx)|*.mpx|MPSZ Files (*.mpz)|*.mpz|All Files (*.*)|*.*||");
   CFileDialog open(TRUE,NULL,NULL,NULL,szFilter,NULL);
 
  if (open.DoModal()==IDOK)
  {    
	MPSZFileDataIO::EFileTypes type;
    if (open.GetFileExt()==_T("mpx"))
	  type = MPSZFileDataIO::FILE_TYPE_MPX;
	if (open.GetFileExt()==_T("mpz"))
	  type = MPSZFileDataIO::FILE_TYPE_MPZ;
	else
      type = MPSZFileDataIO::FILE_TYPE_MPX; //���� � ����� ��� ���������� ��� ��� ������ �� �� ��������� mpx 

    m_mpsz_file_name = _TSTRING(open.GetFileName());
	bool result = m_mpsz_io.Load(m_mpsz_file_name,type);  
	if (!result)
	{
	  AfxMessageBox(_T("�� ���� ��� ��������� ���� ����, ��������!"),MB_OK|MB_ICONWARNING);
      return IDCANCEL; 
	}
  }
  else
	return IDCANCEL; //����� ������������

 return mp_view->DoModal();
}


bool MPSZImportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}


MPSZImportController::~MPSZImportController()
{
}

void MPSZImportController::OnOkPressed(void)
{
}

void MPSZImportController::OnCancelPressed(void)
{
}

//������ ������ ����������� ������
void MPSZImportController::OnExchangePressed(void)
{
  int current_sel = mp_view->GetFWDCurrentListSelection();
  int other_sel   = mp_view->GetFWDOtherListSelection();

  if (m_mpsz_io.GetData().maps[other_sel].name!=_T(""))
    mp_fwd->maps[current_sel].name = m_mpsz_io.GetData().maps[other_sel].name;
  else
  { //���� ������ ������, �� ���������� "�����c�������" ���
    TCHAR name[32];
    _stprintf(name,_T("<��� ����� %d>"),other_sel+1); 	
	mp_fwd->maps[current_sel].name = name; 
  }

  mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
  mp_view->SetFWDCurrentListSelection(current_sel);

  if (mp_view->GetFWDFlag(FLAG_START_MAP))
    memcpy(mp_fwd->maps[current_sel].f_str, m_mpsz_io.GetData().maps[other_sel].f_str,sizeof(float) * F_STR_POINTS); 

  if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
    memcpy(mp_fwd->maps[current_sel].f_idl, m_mpsz_io.GetData().maps[other_sel].f_idl,sizeof(float) * F_IDL_POINTS);

  if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
    memcpy(mp_fwd->maps[current_sel].f_wrk, m_mpsz_io.GetData().maps[other_sel].f_wrk,sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

}

//��������� ���� �������������� - �������� ��� �������������
void MPSZImportController::OnViewActivate(void)
{
  mp_view->SetFWDCurrentListTitle(_T("SECU-3 ������� ��������"));
  CString title;
  title.Format(_T("MPSZ %s"),m_mpsz_file_name.c_str());
  mp_view->SetFWDOtherListTitle(_TSTRING(title));

  mp_view->SetFWDFlag(FLAG_TEMP_MAP,false);
  mp_view->EnableFWDFlag(FLAG_TEMP_MAP,false);
  mp_view->SetExchangeButtonCaption(_T("<"));
  mp_view->SetWindowText(_T("������ ������ �� MPSZ"));

  mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
  std::vector<_TSTRING> strings = m_mpsz_io.GetData().GetListOfNames();
  mp_view->FillFWDOtherList(strings);

  mp_view->SetFWDFlag(FLAG_START_MAP,true);
  mp_view->SetFWDFlag(FLAG_IDLE_MAP,true);
  mp_view->SetFWDFlag(FLAG_WORK_MAP,true);
}



//////////////////////////////////////////////////////////////////////////////////////////
MPSZExportController::MPSZExportController(FWMapsDataHolder* ip_fwd)
: mp_view(NULL)
, mp_fwd(ip_fwd)
, m_mpsz_file_name(_T(""))
{
 mp_view = new CMapImpExpDlg();
 ASSERT(mp_fwd);
 
 //��������� ����������� ������� �� �������������
 mp_view->setOnOkButton(MakeDelegate(this,&MPSZExportController::OnOkPressed));
 mp_view->setOnCancelButton(MakeDelegate(this,&MPSZExportController::OnCancelPressed));
 mp_view->setOnExchangeButton(MakeDelegate(this,&MPSZExportController::OnExchangePressed));
 mp_view->setOnActivate(MakeDelegate(this,&MPSZExportController::OnViewActivate));
 mp_view->setIsExchengeButtonAllowed(MakeDelegate(this,&MPSZExportController::IsExchangeButtonAllowed));

}

int MPSZExportController::DoExport(void)
{	
  //���� ����� ��������� ����� �������� ���������� ���������� � ����������� �� ���������� �������
  class CFileDialogEx : public CFileDialog
  {
   public:
    CFileDialogEx() : CFileDialog(FALSE,_T("mpx"),NULL,NULL,
	    _T("MPSZ Files (*.mpx)|*.mpx|MPSZ Files (*.mpz)|*.mpz|All Files (*.*)|*.*||"),NULL) {};
    virtual ~CFileDialogEx() {};

   protected:
    virtual void OnTypeChange( )
    {
     if (m_ofn.nFilterIndex==1)
      m_ofn.lpstrDefExt = _T("mpx");
 
     if (m_ofn.nFilterIndex==2)
 	  m_ofn.lpstrDefExt = _T("mpz");
    }
  };

  CFileDialogEx save;
  if (save.DoModal()==IDOK)
  {    
	MPSZFileDataIO::EFileTypes type;
    if (save.GetFileExt()==_T("mpx"))
	{
	  type = MPSZFileDataIO::FILE_TYPE_MPX;
	  m_mpsz_io.SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS);
	}
	else if (save.GetFileExt()==_T("mpz"))
	{
	  type = MPSZFileDataIO::FILE_TYPE_MPZ;
	  m_mpsz_io.SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS_IN_MPZ_FILE);
	}
	else
	{
      type = MPSZFileDataIO::FILE_TYPE_MPX; //���� � ����� ��� ���������� ��� ��� ������ �� �� ��������� mpx 
	  m_mpsz_io.SetActualSetsNumber(MPSZ_NUMBER_OF_MAPS);
	}
    
	//��������� ��� ����� ���������� ������������� ��� ����������
    m_mpsz_file_name = _TSTRING(save.GetFileName());

    int id = mp_view->DoModal();
    if (id == IDOK)
	{
     bool result = m_mpsz_io.Save(m_mpsz_file_name,type);  
     if (!result)
	 {
       AfxMessageBox(_T("�� ���� ��� ��������� ���� ����, ��������!"),MB_OK|MB_ICONWARNING);
       return IDCANCEL; 
	 }
	}
    return id;
  }
  else
	return IDCANCEL; //����� ������������
}


bool MPSZExportController::IsExchangeButtonAllowed(void)
{
 int current_sel = mp_view->GetFWDCurrentListSelection();
 int other_sel   = mp_view->GetFWDOtherListSelection();
 return (current_sel != -1) && (other_sel != -1);
}


MPSZExportController::~MPSZExportController()
{
}

void MPSZExportController::OnOkPressed(void)
{
}

void MPSZExportController::OnCancelPressed(void)
{
}

//������ ������ ����������� ������
void MPSZExportController::OnExchangePressed(void)
{
  int current_sel = mp_view->GetFWDCurrentListSelection();
  int other_sel   = mp_view->GetFWDOtherListSelection();

  m_mpsz_io.GetDataLeft().maps[other_sel].name = mp_fwd->maps[current_sel].name;
  mp_view->FillFWDOtherList(m_mpsz_io.GetData().GetListOfNames());
  mp_view->SetFWDOtherListSelection(other_sel);
  
  if (mp_view->GetFWDFlag(FLAG_START_MAP))
    memcpy(m_mpsz_io.GetDataLeft().maps[other_sel].f_str, mp_fwd->maps[current_sel].f_str, sizeof(float) * F_STR_POINTS); 

  if (mp_view->GetFWDFlag(FLAG_IDLE_MAP))
    memcpy(m_mpsz_io.GetDataLeft().maps[other_sel].f_idl, mp_fwd->maps[current_sel].f_idl, sizeof(float) * F_IDL_POINTS);

  if (mp_view->GetFWDFlag(FLAG_WORK_MAP))
    memcpy(m_mpsz_io.GetDataLeft().maps[other_sel].f_wrk, mp_fwd->maps[current_sel].f_wrk, sizeof(float) * F_WRK_POINTS_L * F_WRK_POINTS_F);

}

//��������� ���� �������������� - �������� ��� �������������
void MPSZExportController::OnViewActivate(void)
{
  mp_view->SetFWDCurrentListTitle(_T("SECU-3 ������� ��������"));
  CString title;
  title.Format(_T("MPSZ %s"),m_mpsz_file_name.c_str());
  mp_view->SetFWDOtherListTitle(_TSTRING(title));

  mp_view->SetFWDFlag(FLAG_TEMP_MAP,false);
  mp_view->EnableFWDFlag(FLAG_TEMP_MAP,false);
  mp_view->SetExchangeButtonCaption(_T(">"));
  mp_view->SetWindowText(_T("������� ������ � MPSZ"));

  mp_view->FillFWDCurrentList(mp_fwd->GetListOfNames());
  std::vector<_TSTRING> strings = m_mpsz_io.GetData().GetListOfNames();
  mp_view->FillFWDOtherList(strings);

  mp_view->SetFWDFlag(FLAG_START_MAP,true);
  mp_view->SetFWDFlag(FLAG_IDLE_MAP,true);
  mp_view->SetFWDFlag(FLAG_WORK_MAP,true);
}