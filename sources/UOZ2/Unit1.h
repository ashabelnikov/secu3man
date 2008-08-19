//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>

typedef void (__cdecl *EventHandler)(void* i_param);

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TChart *Chart1;
        TLineSeries *Series1;
        TLineSeries *Series2;
        TLineSeries *Series3;
        TLineSeries *Series4;
        TLineSeries *Series5;
        TLineSeries *Series6;
        TLineSeries *Series7;
        TLineSeries *Series8;
        TLineSeries *Series9;
        TLineSeries *Series10;
        TLineSeries *Series11;
        TLineSeries *Series12;
        TLineSeries *Series13;
        TLineSeries *Series14;
        TLineSeries *Series15;
        TLineSeries *Series16;
        TTrackBar *TrackBar1;
        TLabel *Label1;
        TLabel *Label2;
        TCheckBox *CheckBox1;
        TLineSeries *Series17;
        TLineSeries *Series18;
        TLineSeries *Series19;
        TLineSeries *Series20;
        TLineSeries *Series21;
        TLineSeries *Series22;
        TLineSeries *Series23;
        TLineSeries *Series24;
        TLineSeries *Series25;
        TLineSeries *Series26;
        TLineSeries *Series27;
        TLineSeries *Series28;
        TLineSeries *Series29;
        TLineSeries *Series30;
        TLineSeries *Series31;
        TLineSeries *Series32;
        TCheckBox *CheckBox2;
        void __fastcall TrackBar1Change(TObject *Sender);
        void __fastcall Chart1ClickSeries(TCustomChart *Sender,
          TChartSeries *Series, int ValueIndex, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall CheckBox1Click(TObject *Sender);
        void __fastcall CheckBox2Click(TObject *Sender);
        void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
public:
        void SetOnChange(EventHandler i_pOnChange,void* i_param);
        void SetOnClose(EventHandler i_pOnClose,void* i_param);
private:	// User declarations
        //адрес функции которая будет вызываться после изменения данных
        EventHandler m_pOnChange;
        void* m_param_on_change;

        EventHandler m_pOnClose;
        void* m_param_on_close;
public:		// User declarations
        int         count_x;
        int         count_z;
        int         u_slots[1024];
        AnsiString  u_title;
        AnsiString  x_title;
        float       *modified_function;
        float       *original_function;
        int         airflow;
        int         setval;
        int         val_n;
        float       aai_min;
        float       aai_max;

        __fastcall TForm1(TComponent* Owner);
        void DataPrepare();
        float GetItem_m(int c, int r);
        float GetItem_o(int c, int r);

        int SetItem(int c, int r, float value);
        void SetAirFlow(int flow);
        void MakeOneVisible(int flow);
        void ShowPoints(bool show);
        void MakeAllVisible(void);
        void FillChart(bool dir,int cm);
        void HideAllSeries(void);
};
//---------------------------------------------------------------------------
#endif
