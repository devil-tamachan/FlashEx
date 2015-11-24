// ropguard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include "createprocess.h"

#pragma warning(disable : 4996)


//--------------------------------------------------------------------------
// main program
int _tmain(int argc, _TCHAR* argv[])
{

  // get the full path of ropguarddll.dll
  char dllpath[1000];
  char *filename;
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

