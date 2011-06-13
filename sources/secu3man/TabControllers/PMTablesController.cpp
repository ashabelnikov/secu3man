/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Gorlovka

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   contacts:
              http://secu-3.org
              email: shabelnikov@secu-3.org
*/

#include "stdafx.h"
#include "Resources/resource.h"
#include "PMTablesController.h"

#include "common/fastdelegate.h"
#include "ParamDesk/Tables/TablesDeskDlg.h"

using namespace fastdelegate;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPMTablesController::CPMTablesController(CTablesDeskDlg* ip_view)
: mp_view(ip_view)
{
 mp_view->setOnMapChanged(MakeDelegate(this, &CPMTablesController::OnMapChanged));
 mp_view->setOnCloseMapWnd(MakeDelegate(this, &CPMTablesController::OnCloseMapWnd));
 mp_view->setOnOpenMapWnd(MakeDelegate(this, &CPMTablesController::OnOpenMapWnd));
 mp_view->setOnTabActivate(MakeDelegate(this, &CPMTablesController::OnTabActivate));
 mp_view->setOnSaveButton(MakeDelegate(this, &CPMTablesController::OnSaveButton));
 mp_view->setOnChangeTablesSetName(MakeDelegate(this, &CPMTablesController::OnChangeTablesSetName));
}

CPMTablesController::~CPMTablesController()
{
 //empty
}

void CPMTablesController::OnActivate(void)
{
 mp_view->Enable(true); //test
}

void CPMTablesController::OnDeactivate(void)
{

}

//----------------------------------------------------------------
void CPMTablesController::OnMapChanged(int fuel_type, int i_mapType)
{
 //todo
}

void CPMTablesController::OnCloseMapWnd(HWND i_hwnd, int i_mapType)
{
 //todo
}

void CPMTablesController::OnOpenMapWnd(HWND i_hwnd, int i_mapType)
{
 //todo
}

void CPMTablesController::OnTabActivate(void)
{
 //todo
}

void CPMTablesController::OnSaveButton(void)
{
 //todo
}

void CPMTablesController::OnChangeTablesSetName(int fuel_type)
{
 //todo
}
