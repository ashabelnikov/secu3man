/* SECU-3  - An open source, free engine control unit
   Copyright (C) 2007 Alexey A. Shabelnikov. Ukraine, Kiev

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

/** \file secu-3about.cpp
 * \author Alexey A. Shabelnikov
 */

#include "stdafx.h"
#include "resource.h"
#include "secu-3about.h"
#include <algorithm>
#include <stdexcept>

#include "STCAbout.h"
#include "../common/unicodesupport.h"
#include "../common/ModuleName.h"
#include "Version.h"
#include "WelcomeDlg.h"
#include "sha256.h"

extern HINSTANCE hInstance;  //DLLs hInstance


namespace {
 CString BuildSoftwareInfoStr(void)
 {
  USES_CONVERSION;
  CString string;
  CString templ = MLL::LoadString(IDS_SOFTWARE_INFO);
  int major = 0, minor = 0;
  GetVersionInfo(NULL, major, minor);
  string.Format(templ, major, minor, A2T(__DATE__)); //compiler uses only ASCII format... 
  return string;
 }

 HGLOBAL LoadResourceBits(LPCTSTR moduleName, LPCTSTR resId, LPCTSTR resType, BYTE*& buff, int& size)
 {
  HINSTANCE hInst = GetModuleHandle(moduleName);
  HRSRC rc = FindResource(hInst, resId, resType);
  if (NULL==rc)
   return NULL;

  HGLOBAL hgl = LoadResource(hInst, rc);
  if (NULL==hgl)
   return NULL;

  BYTE* bitmap = (BYTE*)LockResource(hgl);
  if (NULL==bitmap)
  {
   FreeResource(hgl);
   return NULL;
  }

  DWORD bmsize = SizeofResource(hInst, rc);
  if (0==bmsize)
  {
   FreeResource(hgl);
   return NULL;
  }

  buff = bitmap;
  size = bmsize;
  return hgl;
 }
 
 void app_fail1(void)
 {
  *((char*)(0x3416)) = 0x7F;  
 }

 int app_fail2(int val)
 {
  if (val < 100)
   return app_fail2(val / (val - 1));
  else
   return 5;
 }
}

void ABOUT_API DisplayAbout(CWnd* i_pParent)
{
 AboutStc(i_pParent, hInstance, (LPCTSTR)IDB_BITMAP0001, (LPCTSTR)IDR_RGN0001,
  BuildSoftwareInfoStr(), MLL::LoadString(IDS_AUTHOR_INFO));
}

//timeToshow - How long show splash screen, in milliseconds.
void ABOUT_API DisplaySplash(int timeToShow)
{
 AboutStc(CWnd::GetDesktopWindow(), hInstance, (LPCTSTR)IDB_BITMAP0001, (LPCTSTR)IDR_RGN0001,
  BuildSoftwareInfoStr(), MLL::LoadString(IDS_AUTHOR_INFO), true, timeToShow);
}

void ABOUT_API DisplayWelcome(void)
{
 CWelcomeDlg dialog;
 dialog.DoModal();
}

bool CheckAppTitle(CWnd* p_wnd)
{
 BYTE hash[SHA256_BLOCK_SIZE];
 Sha256 sha;

 CString str;
 p_wnd->GetWindowText(str);
 //"SECU-3 Manager v4.6"
 BYTE hash_title[SHA256_BLOCK_SIZE] = {0xc4,0x0b,0x30,0x8a,0xb1,0x60,0xf6,0x07,0x3e,0x53,0xc7,0x09,0x41,0xea,0x85,0x43,0x8a,0x0c,0xf7,0x04,0x50,0xe5,0x32,0x10,0x06,0x24,0xe3,0x2d,0xa0,0x79,0x00,0x51};
 sha.init();
 sha.update((BYTE*)str.GetBuffer(100), str.GetLength());
 str.ReleaseBuffer();
 sha.final(hash);
 if (!std::equal(hash, hash + SHA256_BLOCK_SIZE, hash_title))
 {
  app_fail1();
  return false;
 }

 TCHAR ldstr[100+1];
 LoadString(GetModuleHandle(ModuleName::secu3man), 4152/*IDS_APP_TITLE*/, ldstr, 100);
 //"SECU-3 Manager"
 BYTE hash_title1[32] = {0xc1,0x73,0xe1,0xc0,0xe8,0x01,0x45,0xec,0x04,0xd5,0x79,0xab,0xb7,0x89,0x72,0x68,0xee,0x28,0xa3,0xa6,0x5f,0xb5,0x7a,0xc6,0x61,0x46,0x0b,0x60,0xc5,0x0d,0x18,0x9e};
 sha.init();
 sha.update((BYTE*)ldstr, (_TSTRING(ldstr)).size());
 str.ReleaseBuffer();
 sha.final(hash);
 if (!std::equal(hash, hash + SHA256_BLOCK_SIZE, hash_title1))
 {
  app_fail2(5);
  return false;
 }

 return true;
}

bool ABOUT_API CheckAppLogo(void)
{
 TCHAR str[100+1];
 LoadString(GetModuleHandle(ModuleName::secu3man), 4153/*IDS_STATUS_BAR_LOGO*/, str, 100);
 //"SECU-3"
 BYTE hash[32] = {0xe8,0x40,0x9b,0x12,0x68,0x85,0xaf,0x0d,0xd5,0x64,0xfd,0x70,0x7d,0xa2,0xba,0xb3,0x42,0xd7,0x75,0xf1,0x84,0xdc,0xa9,0xba,0x8e,0x1b,0xa4,0x49,0x22,0x0b,0x29,0xbf};
 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 sha.init();
 sha.update((BYTE*)str, (_TSTRING(str)).size());
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash))
  return false;

 return true;
}

bool ABOUT_API CheckAppUrl(const _TSTRING& str)
{
 //"http://secu-3.org"
 BYTE hash[32] = {0x9b,0x1a,0x20,0x07,0xaa,0x9c,0xce,0x55,0x2a,0x3e,0xd8,0x17,0x2f,0xd8,0x48,0xe7,0xa8,0xf9,0xfb,0xf8,0xc3,0x2d,0x50,0x0d,0x99,0xd4,0x6c,0x9f,0x9b,0x82,0xd5,0x41};
 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 sha.init();
 sha.update((BYTE*)str.c_str(), str.size());
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash))
  return false;

 return true;
}

bool ABOUT_API CheckAppMenu(void)
{
 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 BYTE* bytes = NULL;
 int size = 0;
 HGLOBAL hgl = NULL;

 hgl = LoadResourceBits(ModuleName::secu3man, MAKEINTRESOURCE(4001/*IDR_MAINFRAME*/), RT_MENU, bytes, size);
 if (NULL==hgl)
  return false;

 //hash of main menu resource, size = 758 (RU), 738 (EN)
 BYTE hash1[32] = {0x27,0xf7,0xe5,0x44,0x09,0xa2,0x72,0x52,0x6d,0x9a,0xc2,0x07,0x9a,0xbb,0xb1,0xcb,0xd6,0x63,0x82,0x26,0xef,0xf9,0x2e,0x04,0xa8,0x99,0x0c,0x88,0x10,0xb5,0x8f,0x6e}; //ru
 BYTE hash2[32] = {0xb0,0x25,0xa3,0xfa,0x8e,0x46,0x5a,0x59,0x8f,0xab,0xdb,0xad,0x7e,0x57,0x8d,0xb8,0x4e,0x62,0xc6,0x09,0xfd,0xcc,0x76,0xa5,0x72,0x27,0xc8,0xac,0xa3,0x23,0xd8,0x49}; //en

 sha.init();
 sha.update(bytes, size);
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash1)) //ru
 {
  if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash2)) //en
   return false;
 }

 FreeResource(hgl);
 return true;
}

bool ABOUT_API CheckBitmaps(void)
{
 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 BYTE* bitmap = NULL;
 int size = 0;
 HGLOBAL hgl = NULL;

 hgl = LoadResourceBits(ModuleName::about, MAKEINTRESOURCE(IDB_BITMAP0001), RT_BITMAP, bitmap, size);
 if (NULL==hgl)
  return false;
 //hash of bitmap from resources (differs from the original), size = 99504
 BYTE hash1[32] = {0x86,0x8b,0xd5,0xc2,0xd4,0x4a,0x6c,0x3e,0x77,0x66,0xf8,0x77,0x77,0x1b,0xee,0x8e,0x3c,0x87,0x63,0xf9,0x96,0xea,0x5d,0xa2,0x0f,0x6d,0x7a,0x2c,0x67,0xb1,0xa2,0xfa};
 sha.init();
 sha.update(bitmap, size);
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash1))
  return false;
 FreeResource(hgl);

 hgl = LoadResourceBits(ModuleName::about, MAKEINTRESOURCE(IDB_BITMAP0002), RT_BITMAP, bitmap, size);
 if (NULL==hgl)
  return false;
 /*
 FILE* f = fopen("bbb.bbb","wb");
 fwrite(bitmap,size,1,f);
 fclose(f);
 */
 //hash of bitmap from resources (differs from the original), size = 12712
 BYTE hash2[32] = {0x7f,0x80,0xd9,0xb5,0xd6,0x29,0xf8,0x96,0x32,0x54,0xa6,0x0b,0xdb,0xf9,0x1e,0xd4,0xd4,0x83,0x0c,0x8e,0x37,0xb1,0x20,0xbd,0x3d,0x0b,0x31,0x22,0x30,0x90,0xf7,0x8e};
 sha.init();
 sha.update(bitmap, size);
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash2))
  return false;

 FreeResource(hgl);
 return true;
}

bool ABOUT_API CheckAbout(void)
{
 _TSTRING str = MLL::GetString(IDS_AUTHOR_INFO);
 //author info (IDS_AUTHOR_INFO)
 BYTE hash1[32] = {0xAC,0x31,0x10,0x9B,0x13,0xDD,0x5C,0xBC,0xF2,0xAF,0x2F,0x92,0x43,0xD5,0x9D,0xB3,0x75,0x2F,0x82,0x68,0x87,0x52,0x70,0x3E,0x86,0x92,0xDC,0x74,0x39,0xD7,0x3C,0x9C};
 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 sha.init();
 sha.update((BYTE*)str.c_str(), str.size());
 sha.final(digest);
/*
 FILE* f = fopen("ccc.ccc","wb");
 for (int i = 0; i < 32; ++i)
  fprintf(f,"0x%02X,", digest[i]); 
 fclose(f);
*/
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash1))
  return false;

 str = MLL::GetString(IDS_SOFTWARE_INFO);
 //author info (IDS_AUTHOR_INFO)
 BYTE hash2[32] = {0xca,0x3b,0xee,0x41,0x72,0xe4,0xf2,0xbc,0x27,0xf7,0xe3,0x0d,0x2b,0xaa,0x02,0xb3,0xbb,0x85,0xc3,0x69,0xa4,0xda,0x76,0x52,0x95,0x9b,0x8a,0xae,0x5a,0xcb,0xc5,0x4d};
 sha.init();
 sha.update((BYTE*)str.c_str(), str.size());
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash2))
  return false;

 return true;
}

bool ABOUT_API CheckVersion(void)
{//hashes for 4.6 version info
 BYTE hash1[SHA256_BLOCK_SIZE] = {0x36,0xFD,0xE2,0x4B,0xE9,0x17,0x2B,0x73,0x20,0x9F,0xA5,0x7A,0x38,0x65,0x4C,0x86,0x2F,0xC4,0xCB,0x70,0xD0,0x24,0x57,0x81,0x75,0x77,0x58,0xA4,0x45,0xD4,0xA5,0x99};
 BYTE hash2[SHA256_BLOCK_SIZE] = {0xC9,0x00,0x39,0xC8,0x73,0x6A,0x97,0xE5,0x2C,0x81,0x17,0x73,0x51,0x2B,0x49,0x0A,0xD6,0xF2,0x8C,0x06,0x73,0xB1,0x74,0xE8,0xC5,0xF2,0x89,0xDF,0x70,0xF8,0x3A,0x95};

 std::vector<std::pair<_TSTRING, BYTE*> > mods;
 mods.push_back(std::make_pair(ModuleName::about, hash1));
 mods.push_back(std::make_pair(ModuleName::chart2d, hash2)); //different hash
 mods.push_back(std::make_pair(ModuleName::chart3d, hash2)); //different hash
 mods.push_back(std::make_pair(ModuleName::fwimpexp, hash1));
 mods.push_back(std::make_pair(ModuleName::hexutils, hash1));
 mods.push_back(std::make_pair(ModuleName::iocore, hash1));
 mods.push_back(std::make_pair(ModuleName::midesk, hash1));
 mods.push_back(std::make_pair(ModuleName::paramdesk, hash1));
 mods.push_back(std::make_pair(ModuleName::tabldesk, hash1));
 mods.push_back(std::make_pair(ModuleName::uicore, hash1));
 mods.push_back(std::make_pair(ModuleName::secu3man, hash1));

 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 std::vector<BYTE> data;
 for (size_t i = 0; i < mods.size(); ++i)
 {
  if (!GetVersionData(mods[i].first.c_str(), data))
   return false;
  sha.init();
  sha.update(&data[0], data.size());
  sha.final(digest);  
/*
  FILE* f = fopen("ccc.ccc","wb");
  for (int j = 0; j < 32; ++j)
   fprintf(f,"0x%02X,", digest[j]); 
  fclose(f);  
*/
  /*
  char fname[64];
  sprintf(fname,"verdata%d.bin", i);
  FILE* f = fopen(fname,"wb");
  fwrite(&data[0],data.size(),1,f);
  fclose(f);
  */

  if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, mods[i].second))
  {
   data.push_back(0);
   data.push_back(0);
   data.push_back(0);
   data.push_back(0);
   sha.init();
   sha.update(&data[0], data.size());
   sha.final(digest);  
   if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, mods[i].second))
    return false;
  }
 }

 return true; //ok
}


bool ABOUT_API CalcFileDigest(LPCTSTR filePath, BYTE hash[])
{
 Sha256 sha;
 size_t read = 0;
 sha.init();

 FILE* fin = _tfopen(filePath, _T("rb"));
 if (!fin)
  return false;
 BYTE buff[2048+1];
 int blk = 2048;
 do {
  read = fread(buff, 1, blk, fin);
  sha.update(buff, read);
 }while(read > 0);

 fclose(fin);
 sha.final(hash);
 return true;
}

bool ABOUT_API CompBuffDigest(BYTE* buff, int size, BYTE hash[])
{
 BYTE digest[SHA256_BLOCK_SIZE];
 if (NULL==buff)
  return false;
 Sha256 sha;
 sha.init();
 sha.update(buff, size);
 sha.final(digest);
 if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, hash))
  return false;
 return true;
}

