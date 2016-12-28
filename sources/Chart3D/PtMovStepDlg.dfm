object PtMovStepDlg: TPtMovStepDlg
  Left = 816
  Top = 170
  BorderStyle = bsDialog
  Caption = 'Specify points'#39' moving step'
  ClientHeight = 114
  ClientWidth = 207
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonOk: TButton
    Left = 16
    Top = 72
    Width = 73
    Height = 33
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 0
    OnClick = ButtonOkClick
  end
  object ButtonCancel: TButton
    Left = 120
    Top = 72
    Width = 73
    Height = 33
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object EditStep: TEdit
    Left = 72
    Top = 16
    Width = 65
    Height = 33
    AutoSize = False
    TabOrder = 2
    Text = '0.5'
    OnChange = EditStepChange
  end
  object SpinStep: TFloatUpDown
    Left = 138
    Top = 16
    Width = 16
    Height = 33
    FloatAssociate = EditStep
    FloatMin = 0
    FloatMax = 10
    FloatPosition = 0
    FloatIncrement = 0.01
    TabOrder = 3
    TabStop = False
    Visible = True
    Wrap = False
    ArrowKeys = True
    ShowHint = True    
  end
end
