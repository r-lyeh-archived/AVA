#pragma once 
#include <stdio.h>
#include <windows.h>

typedef struct {
  int scrWidth;
  int scrHeight;
  int scrBPP;
  int nWindowed;
  int nVsync;
  int nMultisample;
  int nAlwaysOnTop;

  HINSTANCE hInstance;
} fwzSettings; 

#ifdef __cplusplus
extern "C"
#endif
int OpenSetupDialog(fwzSettings * setup);