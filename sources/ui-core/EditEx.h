
#pragma once

class AFX_EXT_CLASS CEditEx : public CEdit
{
public:
	CEditEx();
	virtual ~CEditEx();


	//поддерживаемые режимы (фильтрация вводимых символов)
    enum
	{
     MODE_STRING  = 0x00000001,
	 MODE_INT     = 0x00000002,
     MODE_FLOAT   = 0x00000004,
	 MODE_HEX     = 0x00000008,

	 MODE_SIGNED  = 0x00000100,   //несовместим с MODE_HEX
     //--------------------------------------------------------- 
	 MODE_MASK    = 0x000000FF   //для внутреннего использования
	}; 


private:
    template <class T>
    void _DDX_Value(CDataExchange *ipDX, int iIDC, T& ioValue);
public:
    inline void DDX_Value(CDataExchange *ipDX, int iIDC, float& ioValue)
	{
	 _DDX_Value(ipDX, iIDC, ioValue);
	}

    inline void DDX_Value(CDataExchange *ipDX, int iIDC, int& ioValue)
	{
	 _DDX_Value(ipDX, iIDC, ioValue);
	}

	bool GetValue(float& o_value);
    bool SetValue(float i_value);

    inline bool GetValue(int &o_value)
	{
	  float value = 0;
	  bool result = GetValue(value);
	  o_value = (int)value;
      return result;
	}

	inline bool SetValue(int i_value)
	{
	 return SetValue((float)i_value);
	}

    inline void SetMode(DWORD i_mode)
	{ 	
      //нельзя использовать SIGNED для HEX
      ASSERT(!((i_mode & MODE_HEX) && (i_mode & MODE_SIGNED)));
      m_mode = i_mode;
	}

    inline DWORD GetMode(void)
	{
      return m_mode;
	}


	//Устанавливает, для FLOAT: кол-во знаков после запятой, для целых: кол-во десятичных знаков
	inline void SetDecimalPlaces(int i_decimal_places)
	{
	  m_DecimalPlaces = i_decimal_places;
	}

	inline int GetDecimalPlaces(void)
	{
	  return m_DecimalPlaces;
	}


	float operator = (float i_value);

	// Generated message map functions
protected:
    DWORD m_mode;
	int m_DecimalPlaces;

protected:
    bool OnChar_int(UINT nChar, UINT nRepCnt, UINT nFlags);
    bool OnChar_float(UINT nChar, UINT nRepCnt, UINT nFlags);
    bool OnChar_hex(UINT nChar, UINT nRepCnt, UINT nFlags);

	//{{AFX_MSG(CEditEx)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


//-------------------------------------------------------------
template <class T>
void CEditEx::_DDX_Value(CDataExchange *ipDX, int iIDC, T& ioValue)
  {
    // Init CDataExchange members:
    HWND hwnd = ipDX->PrepareEditCtrl (iIDC);
    ASSERT (m_hWnd == hwnd);

    // Now copy to/from control
	if (ipDX->m_bSaveAndValidate)
	{
      if (!GetValue(ioValue))
	  {
        //message box!
	    ipDX->Fail();
	  }
	}
	else
	{        
	  SetValue(ioValue);
	}
  }

//-------------------------------------------------------------
