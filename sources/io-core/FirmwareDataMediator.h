

#pragma once

class AFX_EXT_CLASS CFirmwareDataMediator
{
public:
     CFirmwareDataMediator();
	~CFirmwareDataMediator();

static void CalculateAndPlaceFirmwareCRC(BYTE* io_data);

};