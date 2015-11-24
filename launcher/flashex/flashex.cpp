// ropguard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include "createprocess.h"
#include "crc.h"

#pragma warning(disable : 4996)


//--------------------------------------------------------------------------
// main program
int _tmain(int argc, _TCHAR* argv[])
{

  // get the full path of ropguarddll.dll
  char dllpath[1000];
  char *filename;
  long s;
  crc crcExe;

  if (!GetModuleFileName(NULL, dllpath, 980))
  {
    printf("Error: could not obtain current executable path\n");
    return 0;
  }
  filename = strrchr(dllpath,'\\');
  if(!filename) {
    printf("Error: could not obtain current executable path\n");
    return 0;
  }
  filename++;
  strcpy(filename, "cpicker.dll");
  
  FILE *fp = fopen("Flash4.exe", "rb");
  if(fp==NULL)return 0;
  fseek(fp, 0L, SEEK_END);
  s = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  unsigned char *buf = (unsigned char*)malloc(s);
  if(buf==NULL)
  {
    fclose(fp);
    return 0;
  }
  fread(buf, 1, s, fp);
  fclose(fp);

	crcInit();
  crcExe = crcFast(buf, s);
  free(buf);

  if(crcExe!=0x0B4A/*Flash4J*/ && crcExe!=0xAA51/*Flash4J 2GB+*/)
  {
    char dbg[50];
    sprintf(dbg, "Unsupport Flash4.exe (CRC: 0x%04X)", crcExe);
    MessageBox(NULL, dbg, "Error", MB_OK);
    return 0;
  }

  char *pCmdline = (char *)malloc(5000);
  pCmdline[0] = NULL;
  for(int i=0; i<argc; i++)
  {
    strcat(pCmdline, i==0?"Flash4.exe":argv[i]);
    strcat(pCmdline, " ");
  }
  OutputDebugString(pCmdline);

  CreateProcessWithDll(pCmdline, dllpath, true);

  free(pCmdline);

  return 0;
}

