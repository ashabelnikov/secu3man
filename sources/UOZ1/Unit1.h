//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
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
        void __fastcall Chart1ClickSeries(TCustomChart *Sender,
          TChartSeries *Series, int ValueIndex, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall Chart1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall Chart1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
public:
       void SetOnChange(EventHandler i_pOnChange,void* i_param);
       void SetOnClose(EventHandler i_pOnClose,void* i_param);
private:	// User declarations
        bool setval;
        int  val_n;

        //адрес функции которая будет вызываться после изменения данных
        EventHandler m_pOnChange;
        void* m_param_on_change;

        EventHandler m_pOnClose;
        void* m_param_on_close;
        
public:		// User declarations
        float aai_min;
        float aai_max;
        int         count_of_function_points;
        int         horizontal_axis_grid_values[1024];
        AnsiString  chart_title_text;
        AnsiString  x_axis_title;
        float       *original_function;
        float       *modified_function;

        __fastcall TForm1(TComponent* Owner);
        void DataPrepare();
};
#endif
