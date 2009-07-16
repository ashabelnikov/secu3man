 /****************************************************************
 *       SECU-3  - An open source, free engine control unit
 *    Designed by Alexey A. Shabelnikov. Ukraine, Gorlovka 2007.
 *       Microprocessors systems - design & programming.
 *    contacts:
 *              http://secu-3.narod.ru
 *              ICQ: 405-791-931
 ****************************************************************/

#include "stdafx.h"
#include "AppSettingsController.h"
#include "AppSettingsModel.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using namespace fastdelegate;

//Presenter implementation

CAppSettingsController::CAppSettingsController(IAppSettingsDlg* i_pView, CAppSettingsModel* i_pModel)
: m_pView(i_pView)
, m_pModel(i_pModel)
{
  ASSERT(m_pView);
  m_pView->setFunctionOnOk(MakeDelegate(this,&CAppSettingsController::OnOk));
  m_pView->setFunctionOnCancel(MakeDelegate(this,&CAppSettingsController::OnCancel));
  m_pView->setFunctionOnActivate(MakeDelegate(this,&CAppSettingsController::OnActivate));

}

CAppSettingsController::~CAppSettingsController()
{

}

void CAppSettingsController::OnOk(void)
{
  //сохраняем данные из представления в модель
  ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));
  m_pModel->m_optPortName = m_pView->GetPortName();
  m_pModel->m_optBaudRateApplication = m_pView->GetBaudRateApplication();
  m_pModel->m_optBaudRateBootloader = m_pView->GetBaudRateBootloader(); 
  m_pModel->m_optLogFilesFolder = m_pView->GetLogFilesFolder();
  m_pModel->m_optUseAppFolder = m_pView->GetUseAppFolder();

  size_t index = m_pView->GetCSVSepSymbol();
  m_pModel->m_optCSVSepSymbol = m_pModel->m_AllowaleCSVSepSymbols[index].second;
}

void CAppSettingsController::OnCancel(void)
{
  //При нажатии на Cancel ничего не делаем (не трогаем модель!)
}

void CAppSettingsController::OnActivate(void)
{
  //перекачиваем данные из модели в представление
  ASSERT((m_pModel!=NULL)&&(m_pView!=NULL));  
  m_pView->FillCtrlsWithAllowableBaudRates(m_pModel->m_AllowableBaudRates);
  m_pView->FillCtrlsWithAllowablePorts(m_pModel->m_AllowablePorts);
  m_pView->FillCtrlsWithAllowableCSVSepSymbols(m_pModel->m_AllowaleCSVSepSymbols);
  m_pView->SetPortName(m_pModel->m_optPortName);
  m_pView->SetBaudRateApplication(m_pModel->m_optBaudRateApplication);
  m_pView->SetBaudRateBootloader(m_pModel->m_optBaudRateBootloader); 
  m_pView->SetLogFilesFolder(m_pModel->m_optLogFilesFolder);
  m_pView->SetUseAppFolder(m_pModel->m_optUseAppFolder);

  size_t count = m_pModel->m_AllowaleCSVSepSymbols.size(); 
  for(size_t i = 0; i < count; i++)
	if (m_pModel->m_optCSVSepSymbol == m_pModel->m_AllowaleCSVSepSymbols[i].second)
      m_pView->SetCSVSepSymbol(i);
}
