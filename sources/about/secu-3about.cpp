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
#include <map>

#include "STCAbout.h"
#include "../common/unicodesupport.h"
#include "../common/ModuleName.h"
#include "Version.h"
#include "WelcomeDlg.h"
#include "sha256.h"

extern HINSTANCE hInstance;  //DLLs hInstance

#ifdef _DEBUG //in debug mode only
#define SECU3NOPROTECTION
#endif

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
#ifdef SECU3NOPROTECTION
 return true;
#else

 BYTE hash[SHA256_BLOCK_SIZE];
 Sha256 sha;

 CString str;
 p_wnd->GetWindowText(str);
 //"SECU-3 Manager v4.6"
 BYTE hash_title[SHA256_BLOCK_SIZE] = {0x86,0xa8,0x59,0x85,0x66,0xc7,0xcd,0xed,0xbb,0x61,0xb4,0x6e,0xdf,0x0f,0x93,0x70,0xbc,0x02,0x2c,0xb4,0x26,0x65,0x6c,0xd8,0x8a,0xd7,0xed,0x58,0x8a,0x60,0xbf,0xc5};
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

#endif
}

bool ABOUT_API CheckAppLogo(void)
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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
#endif
}

bool ABOUT_API CheckAppUrl(const _TSTRING& str)
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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
#endif
}

bool ABOUT_API CheckAppMenu(void)
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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
#endif
}

bool ABOUT_API CheckBitmaps(void)
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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
#endif
}

bool ABOUT_API CheckAbout(void)
{
#ifdef SECU3NOPROTECTION
 return true;
#else

 _TSTRING str = MLL::GetString(IDS_AUTHOR_INFO);
 //author info (IDS_AUTHOR_INFO)
 BYTE hash1[32] = {0x5D,0xDC,0x9F,0x00,0x5F,0x83,0x5B,0xFB,0x72,0xEE,0xB7,0xDF,0xE5,0x37,0x04,0xB8,0xD6,0x3C,0x61,0x52,0x13,0x0B,0xC8,0xCB,0xFB,0x14,0xE7,0xAC,0x6D,0x63,0x15,0x5B};
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
#endif
}

bool ABOUT_API CheckVersion(void)
{//hashes for 4.6 version info
#ifdef SECU3NOPROTECTION
 return true;
#else

 BYTE hash1[SHA256_BLOCK_SIZE] = {0x52,0xDF,0xB6,0xED,0xD4,0xAB,0xE3,0x1D,0x41,0xAA,0x60,0xF7,0x7E,0x00,0xCA,0xB0,0xB3,0xAE,0xEC,0x7D,0x16,0xA8,0x6B,0x80,0x7F,0x38,0x42,0xE3,0x25,0xFB,0x4D,0xBF};
 BYTE hash2[SHA256_BLOCK_SIZE] = {0x54,0x1D,0x09,0x5E,0x67,0x43,0x4C,0x59,0xF8,0x96,0x23,0xAD,0x4F,0x64,0x3D,0x99,0x17,0x8A,0xF6,0x33,0xF5,0xE1,0x58,0x8B,0x16,0xEF,0xB1,0x28,0x4D,0x43,0x1D,0xB5};
 BYTE hash3[SHA256_BLOCK_SIZE] = {0xF2,0xBA,0xC7,0x78,0x81,0x78,0x83,0x3E,0xDD,0x19,0xDE,0x27,0xC4,0x72,0xD2,0x2D,0x1B,0x6D,0xEB,0x14,0x2D,0xF9,0x80,0xD4,0x2C,0x33,0x2B,0xE0,0x27,0x84,0xA0,0xF7};
 BYTE hash4[SHA256_BLOCK_SIZE] = {0xE5,0x62,0x5E,0x22,0xA9,0x22,0xFA,0x54,0xC1,0x04,0xE2,0xAA,0xA2,0x31,0xC6,0xB0,0x13,0x21,0x04,0x3C,0x4F,0x1E,0x9D,0x19,0x7C,0xAE,0x4E,0x0E,0x24,0xA6,0xF8,0x6F};

 std::map<size_t, BYTE*> hashdb;
 //note: size include first two bytes
 hashdb.insert(std::make_pair(0x106, hash1)); //win98
 hashdb.insert(std::make_pair(0x116, hash2)); //win98, chart2d & chart3d
 hashdb.insert(std::make_pair(0x1CB, hash3)); //XP, chart2d & chart3d
 hashdb.insert(std::make_pair(0x1D3, hash4)); //XP

 std::vector<_TSTRING> mods;
 mods.push_back(ModuleName::about);
 mods.push_back(ModuleName::chart2d);
 mods.push_back(ModuleName::chart3d);
 mods.push_back(ModuleName::fwimpexp);
 mods.push_back(ModuleName::hexutils);
 mods.push_back(ModuleName::iocore);
 mods.push_back(ModuleName::midesk);
 mods.push_back(ModuleName::paramdesk);
 mods.push_back(ModuleName::tabldesk);
 mods.push_back(ModuleName::uicore);
 mods.push_back(ModuleName::secu3man);

 BYTE digest[SHA256_BLOCK_SIZE];
 Sha256 sha;
 std::vector<BYTE> data;
 for (size_t i = 0; i < mods.size(); ++i)
 {
  if (!GetVersionData(mods[i].c_str(), data))
   return false;
  int dataSize = *((WORD*)(&data[0])); //from first two bytes (wLength)
  if (dataSize > (int)data.size())
   return false;
  //remove zeros at the end
  while(dataSize > 0)
  {
   if (data[dataSize-1]==0)
    --dataSize;
   else
    break;
  }
  sha.init();
  sha.update(&data[2], dataSize-2);  //skip first two bytes of size
  sha.final(digest);  
/*
  char fname1[64];
  sprintf(fname1,"hasdata%d.bin", i);
  FILE* ff = fopen(fname1,"wb");
  for (int j = 0; j < 32; ++j)
   fprintf(ff,"0x%02X,", digest[j]); 
  fclose(ff);  

  char fname[64];
  sprintf(fname,"verdata%d.bin", i);
  FILE* f = fopen(fname,"wb");
  fwrite(&data[0],data.size(),1,f);
  fclose(f);
*/
  std::map<size_t, BYTE*>::iterator it = hashdb.find(dataSize);
  if (it == hashdb.end())
   return false;
  if (!std::equal(digest, digest + SHA256_BLOCK_SIZE, it->second))
   return false;
 }

 return true; //ok
#endif
}

bool ABOUT_API CalcFileDigest(LPCTSTR filePath, BYTE hash[])
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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

#endif
}

bool ABOUT_API CompBuffDigest(BYTE* buff, int size, BYTE hash[])
{
#ifdef SECU3NOPROTECTION
 return true;
#else

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

#endif
}
