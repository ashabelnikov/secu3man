object Form1: TForm1
  Left = 368
  Top = 147
  Cursor = crHSplit
  HorzScrollBar.Color = clInfoBk
  HorzScrollBar.ParentColor = False
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Редактирование карт УОЗ'
  ClientHeight = 392
  ClientWidth = 573
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010002002020100000000000E80200002600000010101000000000002801
    00000E0300002800000020000000400000000100040000000000800200000000
    0000000000000000000000000000000000000000800000800000008080008000
    00008000800080800000C0C0C000808080000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    00FFFFF000000000000000000000000000FFF000000000000000000000000000
    00FFF00000000000000000000000000000F00F00000000000000000000000000
    000000F00000000000000000000000000000000F000000000000000000000000
    00000000F000000000000F0000000000000000000F000000000000FF00000000
    0000000000FFFFFFFFFFFFFFF00000000000000000FBB000000000FF00000000
    0000000000F0BB0000000F00000000000000000000F00BB00000000000000000
    0000000000F000BBB0000000000000000000000000F0000BBBBBBB0000000000
    0000000000F000000BBBBB00000000000000000000F000000000000000000000
    0000000000F0000000000000000000000000000000F000000000000000000000
    00000000F0F0F000000000000000000000000000F0F0F0000000000000000000
    000000000FFF00000000000000000000000000000FFF00000000000000000000
    0000000000F00000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    FFFFFC1FFFFFFC7FFFFFFC7FFFFFFDBFFFFFFFDFFFFFFFEFFFFFFFF7FFBFFFFB
    FFCFFFFC0007FFFC7FCFFFFD3FBFFFFD9FFFFFFDC7FFFFFDE03FFFFDF83FFFFD
    FFFFFFFDFFFFFFFDFFFFFFF57FFFFFF57FFFFFF8FFFFFFF8FFFFFFFDFFFFFFFF
    FFFFFFFFFFFFFFFFFFFFFFFFFFFF280000001000000020000000010004000000
    0000C00000000000000000000000000000000000000000000000000080000080
    000000808000800000008000800080800000C0C0C000808080000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0000000000000000000000
    0000000000000000000000000000000EE00000000000000EE000000000000000
    0E0000000000000000E000000E000000000EEEEEEEE00000000EB0000E000000
    000E0B0000000000000E00BBBB000000000E00000000000000EEE00000000000
    000E0000000000000000000000000000000000000000FFFF0000FFFF0000FFFF
    0000E7FF0000E7FF0000FBFF0000FDFB0000FE010000FE7B0000FEBF0000FEC3
    0000FEFF0000FC7F0000FEFF0000FFFF0000FFFF0000}
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = OnCloseForm
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 361
    Top = 365
    Width = 17
    Height = 16
    Alignment = taRightJustify
    Caption = '16'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 32
    Top = 365
    Width = 106
    Height = 16
    Caption = 'Расход воздуха:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Chart1: TChart
    Left = 0
    Top = 0
    Width = 569
    Height = 353
    AllowZoom = False
    AnimatedZoomSteps = 4
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    Title.Text.Strings = (
      'График рабочего УОЗ')
    OnClickSeries = Chart1ClickSeries
    BottomAxis.Title.Caption = 'Обороты (мин-1)'
    LeftAxis.Automatic = False
    LeftAxis.AutomaticMaximum = False
    LeftAxis.AutomaticMinimum = False
    LeftAxis.Maximum = 60
    LeftAxis.Minimum = -17.5
    LeftAxis.Title.Caption = 'УОЗ (град.)'
    Legend.ResizeChart = False
    Legend.Visible = False
    View3D = False
    View3DOptions.Orthogonal = False
    View3DOptions.Zoom = 72
    TabOrder = 0
    OnMouseMove = Chart1MouseMove
    OnMouseUp = Chart1MouseUp
    object Series1: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = 16744703
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series2: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series3: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series4: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series5: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series6: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series7: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series8: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series9: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series10: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series11: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series12: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series13: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series14: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series15: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series16: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clFuchsia
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series17: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clRed
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series18: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = True
      SeriesColor = clGreen
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series19: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clYellow
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series20: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clBlue
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series21: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clWhite
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series22: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clGray
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series23: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clTeal
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series24: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clNavy
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series25: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clMaroon
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series26: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clLime
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series27: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clOlive
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series28: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clPurple
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series29: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clSilver
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series30: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clAqua
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series31: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clBlack
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Series32: TLineSeries
      Marks.ArrowLength = 8
      Marks.Frame.Visible = False
      Marks.Style = smsValue
      Marks.Transparent = True
      Marks.Visible = False
      SeriesColor = clRed
      ValueFormat = '#,##0.#'
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = True
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
  end
  object TrackBar1: TTrackBar
    Left = 160
    Top = 357
    Width = 193
    Height = 33
    Max = 15
    Orientation = trHorizontal
    Frequency = 1
    Position = 1
    SelEnd = 0
    SelStart = 0
    TabOrder = 1
    ThumbLength = 15
    TickMarks = tmTopLeft
    TickStyle = tsAuto
    OnChange = TrackBar1Change
  end
  object CheckBox1: TCheckBox
    Left = 402
    Top = 365
    Width = 41
    Height = 17
    Caption = '3D'
    TabOrder = 2
    OnClick = CheckBox1Click
  end
  object CheckBox2: TCheckBox
    Left = 464
    Top = 361
    Width = 81
    Height = 25
    Caption = 'вид сзади'
    TabOrder = 3
    OnClick = CheckBox2Click
  end
end
