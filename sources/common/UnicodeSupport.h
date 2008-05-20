
//IF You use _UNICODE:
//In the Output category of the Link tab in the Project Settings dialog box, 
//set the Entry Point Symbol to wWinMainCRTStartup. 


#ifdef _UNICODE
	typedef std::wstring _TSTRING;
#else
	typedef std::string  _TSTRING;
#endif
