
#include "ui-core/TabDialog.h"
#include "resource.h"
#include "common\fastdelegate.h"
#include "HiSCCtrl/sources/ChartCtrl.h"
#include "ParamDesk/KnockPageDlg.h"
#include "ui-core\OScopeCtrl.h"

#pragma once

class CChartPointsSerie;

class CKnockChannelTabDlg : public CTabDialog
{
	typedef CTabDialog Super;
public:
	typedef fastdelegate::FastDelegate0<> EventHandler;

	CKnockChannelTabDlg(CWnd* pParent = NULL);   // standard constructor  
	
	void setOnSaveParameters(EventHandler OnFunction);

	void EnableAll(bool i_enable);

	//Добавляет новую выборку в осциллограф
	void AppendPoint(float value); 

	//Установка значений функции для графика который показывает зависимость сигнала от оборотов
	void SetRPMKnockSignal(const std::vector<float> &i_values);

	enum { RPM_KNOCK_SIGNAL_POINTS = 128 };
	enum { IDD = IDD_KNOCK_CHANNEL };

   virtual LPCTSTR GetDialogID(void) const;

   CKnockPageDlg m_knock_parameters_dlg;
protected:	
   virtual BOOL OnInitDialog();		
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnDestroy();
   afx_msg void OnSaveParameters();
   afx_msg void OnUpdateControls(CCmdUI* pCmdUI);
   afx_msg void OnTimer(UINT nIDEvent);
   DECLARE_MESSAGE_MAP()

        
   void _InitializeOscilloscopeControl(void);
   void _InitializeRPMKnockSignalControl(void);

   CButton  m_param_save_button;
   CChartCtrl* mp_RTChart;
   CChartPointsSerie* m_pPointSerie;
   CChartLineSerie* m_pLineSerie;
   COScopeCtrl m_OScopeCtrl;

private:
    EventHandler  m_OnSaveParameters;  
	bool m_all_enabled;
};