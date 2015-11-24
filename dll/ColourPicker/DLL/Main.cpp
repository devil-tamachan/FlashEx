#include "resource.h"
#include "CPickerDll.h"
#include "stdio.h"



WNDPROC AfxWndProcOrig = (WNDPROC)0x00566f45;
DWORD *lpdwAfxWndProcAddressInAfxGetAfxWndProc = (DWORD *)0x00566f76;

CColourPicker *cpicker = NULL;

LRESULT CALLBACK HookWndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
  switch(uMsg)
  {
  case WM_CREATE:
    if(cpicker==NULL && lp!=NULL && strcmp(((CREATESTRUCT *)lp)->lpszClass, "SmartSketchMDIFrame")==0)
    {
      cpicker = new CColourPicker(NULL, 255, 0, 0, 255, true);
		  cpicker->CreatecolourPicker(CP_USE_ALPHA);
  /*    char buf[50];
      sprintf(buf, "CreatecolourPicker: hWnd=0x%08X", hWnd);
    OutputDebugString(buf);*/
    }else {
      /*char buf[50];
      sprintf(buf, "WM_CREATE: hWnd=0x%08X", hWnd);
    OutputDebugString(buf);*/
    }
    break;
  }

  //  OutputDebugString("HookWndProc");
  return AfxWndProcOrig(hWnd, uMsg, wp, lp);
}

void HookAfxWndProc()
{
  DWORD oldProtect1,oldProtect2;
  HANDLE proc = GetCurrentProcess();
  void *lpHookWndProc = HookWndProc;
  VirtualProtectEx(proc, (LPVOID)lpdwAfxWndProcAddressInAfxGetAfxWndProc, 4, PAGE_EXECUTE_READWRITE, &oldProtect1);
  WriteProcessMemory(proc, (LPVOID)(lpdwAfxWndProcAddressInAfxGetAfxWndProc), &lpHookWndProc, 4, NULL);
  VirtualProtectEx(proc, (LPVOID)lpdwAfxWndProcAddressInAfxGetAfxWndProc, 4, oldProtect1, &oldProtect2);
}

BOOL APIENTRY DllMain( HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    OutputDebugString("DLL_PROCESS_ATTACH");
    HookAfxWndProc();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}




static PixelBuffer pbuffer;

LRESULT CALLBACK ColourPickerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	static CColourPicker *picker = NULL;

	switch (uMsg)
		{
		case WM_INITDIALOG:
			{
			picker = (CColourPicker*)lParam;
			pbuffer.Create(256,256);

			switch (picker->GetAlphaUsage())
				{
				case CP_NO_ALPHA:
					{
					picker->SetAlpha(100);
					}
				break;

				case CP_DISABLE_ALPHA:
					{
					picker->SetAlpha(100);
					EnableWindow(GetDlgItem(hDlg, IDC_ALPHA), false);
					EnableWindow(GetDlgItem(hDlg, IDC_ALPHATXT), false);
					EnableWindow(GetDlgItem(hDlg, IDC_ALPHATXT2), false);
					}
				break;
				}

			UpdateValues(hDlg, picker->GetCurrentColour());

			SendDlgItemMessage(hDlg, IDC_H, BM_SETCHECK, BST_CHECKED, 0);
			}
		break;

		case WM_PAINT:
			{
			HWND hWndColor, hWndColor2, hWndAlpha;
			HDC hdc, hcomp;
			HBITMAP hbmp;
			RECT rect;
			HPEN pen;
			HBRUSH brush;
			int red, green, blue, alpha;
			float rf, gf, bf;

            // Big color selector (displays non-selected features)
			// For example, if G is selected (in RGB radio box) it displays red vs. blue colors
			hWndColor = GetDlgItem(hDlg, IDC_COLOR);
			hdc = GetDC(hWndColor);
			hcomp = CreateCompatibleDC(hdc);

			GetWindowRect(hWndColor, &rect);
			hbmp = CreateCompatibleBitmap(hdc, 255, 255);
			SelectObject(hcomp, hbmp);
			
			red = GetDlgItemInt(hDlg, IDC_RED, NULL, false);
			green = GetDlgItemInt(hDlg, IDC_GREEN, NULL, false);
			blue = GetDlgItemInt(hDlg, IDC_BLUE, NULL, false);
			alpha = GetDlgItemInt(hDlg, IDC_ALPHA, NULL, false);

			// Check who is selected.
			// RED
			if (IsDlgButtonChecked(hDlg, IDC_R) == BST_CHECKED)
				{
				for (int g=0; g<255; g++)
					for (int b=0; b<255; b++)
						pbuffer.SetPixel(g, b, RGB(red, g, b));

				pbuffer.Display(hcomp);

				
				// Draws circle
				pen = CreatePen(PS_SOLID, 1, RGB(255-red, 255-green, 255-blue));
				SelectObject(hcomp, pen);
				Arc(hcomp, green-5, blue-5, green+5, blue+5, 0, 0, 0, 0);
				}
			// GREEN
			else if (IsDlgButtonChecked(hDlg, IDC_G) == BST_CHECKED)
				{
				for (int r=0; r<255; r++)
					for (int b=0; b<255; b++)
						pbuffer.SetPixel(r, b, RGB(r, green, b));
				pbuffer.Display(hcomp);

				// Draws circle
				pen = CreatePen(PS_SOLID, 1, RGB(255-red, 255-green, 255-blue));
				SelectObject(hcomp, pen);
				Arc(hcomp, red-5, blue-5, red+5, blue+5, 0, 0, 0, 0);
				}
			// BLUE
			else if (IsDlgButtonChecked(hDlg, IDC_B) == BST_CHECKED)
				{
				for (int g=0; g<255; g++)
					for (int r=0; r<255; r++)
						pbuffer.SetPixel(g, r, RGB(r, g, blue));
				pbuffer.Display(hcomp);
                
				// Draws circle
				pen = CreatePen(PS_SOLID, 1, RGB(255-red, 255-green, 255-blue));
				SelectObject(hcomp, pen);
				Arc(hcomp, green-5, red-5, green+5, red+5, 0, 0, 0, 0);
				}

			// HUE
			else if (IsDlgButtonChecked(hDlg, IDC_H) == BST_CHECKED)
				{
				double sat, val, step;
				SColour tempcol;

				sat = val = 0.0;
				step = 100.0/255.0;
				
				tempcol.h = (unsigned short) GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				tempcol.s = (unsigned short) sat;
				tempcol.v = (unsigned short) val;

				for (int y=255; y>0; y--)
				{
					for (int x=0; x<255; x++)
					{
						tempcol.UpdateRGB();
						pbuffer.SetPixel(x, y, RGB(tempcol.r, tempcol.g, tempcol.b));
						sat += step;
						tempcol.s = (unsigned short) sat;
					}
					
					val += step;
					sat = 0.0;
					tempcol.v = (unsigned short) val;
//					tempcol.UpdateRGB();
				}
				pbuffer.Display(hcomp);
                
				// Draws circle
				tempcol.s = (unsigned short) GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
				tempcol.v = (unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false);
				tempcol.UpdateRGB();

				pen = CreatePen(PS_SOLID, 1, RGB(255-tempcol.r, 255-tempcol.g, 255-tempcol.b));
				SelectObject(hcomp, pen);
				
				Arc(hcomp, (int)(tempcol.s/step)-5, 255-(int)(tempcol.v/step)+5, (int)(tempcol.s/step)+5, 
					255-(int)(tempcol.v/step)-5, 0, 0, 0, 0);
				}
            
			// SATURATION
			else if (IsDlgButtonChecked(hDlg, IDC_S) == BST_CHECKED)
				{
				double hue, val, step, step2;
				SColour tempcol;

				hue = val = 0.0;
				step2 = 359.0/255.0;
				step = 100.0/255.0;
				
				tempcol.h = (unsigned short) hue;
				tempcol.s = (unsigned short) GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
				tempcol.v = (unsigned short) val;

				for (int y=255; y>0; y--)
				{
					for (int x=0; x<255; x++)
					{
						tempcol.UpdateRGB();
						pbuffer.SetPixel(x, y, RGB(tempcol.r, tempcol.g, tempcol.b));
						hue += step2;
						tempcol.h = (unsigned short) hue;
//						tempcol.UpdateRGB();
					}

					val += step;
					hue = 0.0;
					tempcol.v = (unsigned short) val;
				}
				pbuffer.Display(hcomp);
                
				// Draws circle
				tempcol.h = (unsigned short) GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				tempcol.v = (unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false);
				tempcol.UpdateRGB();

				pen = CreatePen(PS_SOLID, 1, RGB(255-tempcol.r, 255-tempcol.g, 255-tempcol.b));
				SelectObject(hcomp, pen);
				
				Arc(hcomp, (int)(tempcol.h/step2)-5, 255-(int)(tempcol.v/step)+5, (int)(tempcol.h/step2)+5, 
					255-(int)(tempcol.v/step)-5, 0, 0, 0, 0);
				}
            
			// VALUE
			else if (IsDlgButtonChecked(hDlg, IDC_V) == BST_CHECKED)
				{
				double hue, sat, step, step2;
				SColour tempcol;

				hue = sat = 0.0;
				step2 = 359.0/255.0;
				step = 100.0/255.0;

				tempcol.h = (unsigned short) hue;
				tempcol.s = (unsigned short) sat;
				tempcol.v = (unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false);

				for (int y=255; y>0; y--)
				{
					for (int x=0; x<255; x++)
					{
						tempcol.UpdateRGB();
						pbuffer.SetPixel(x, y, RGB(tempcol.r, tempcol.g, tempcol.b));
						hue += step2;
						tempcol.h = (unsigned short) hue;
//						tempcol.UpdateRGB();
					}

					sat += step;
					hue = 0.0;
					tempcol.s = (unsigned short) sat;
				}
				pbuffer.Display(hcomp);
                
				// Draws circle
				tempcol.h = GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				tempcol.s = GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
				tempcol.UpdateRGB();

				pen = CreatePen(PS_SOLID, 1, RGB(255-tempcol.r, 255-tempcol.g, 255-tempcol.b));
				SelectObject(hcomp, pen);
				
				Arc(hcomp, (int)(tempcol.h/step2)-5, 255-(int)(tempcol.s/step)+5, (int)(tempcol.h/step2)+5, 
					255-(int)(tempcol.s/step)-5, 0, 0, 0, 0);
				}

			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hcomp, 0, 0, SRCCOPY);

			DeleteObject(hbmp);
			DeleteDC(hcomp);
			ReleaseDC(hWndColor, hdc);

			// Small color selector (displays selected feature)

			hWndColor2 = GetDlgItem(hDlg, IDC_COLOR2);
			hdc = GetDC(hWndColor2);
			hcomp = CreateCompatibleDC(hdc);

			GetClientRect(hWndColor2, &rect);
			hbmp = CreateCompatibleBitmap(hdc, rect.right, 255);
			SelectObject(hcomp, hbmp);

			brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			FillRect(hcomp, &rect, brush);
			DeleteObject(brush);
			
			// Check who is selected.
			// RED
			if (IsDlgButtonChecked(hDlg, IDC_R) == BST_CHECKED)
				{
				for (int r=255; r>0; r--)
					{
					int g = GetDlgItemInt(hDlg, IDC_GREEN, NULL, false);
					int b = GetDlgItemInt(hDlg, IDC_BLUE, NULL, false);
					
					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, r, RGB(255-r, g, b));
					}

				// Draws arrows
				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255-(red-5), NULL);
                LineTo(hcomp, 0, 255-(red+5));
				LineTo(hcomp, 5, 255-(red));
				LineTo(hcomp, 0, 255-(red-5));
				MoveToEx(hcomp, rect.right-1, 255-(red-5), NULL);
				LineTo(hcomp, rect.right-1, 255-(red+5));
				LineTo(hcomp, rect.right-6, 255-(red));
				LineTo(hcomp, rect.right-1, 255-(red-5));
				}
			// GREEN
			else if (IsDlgButtonChecked(hDlg, IDC_G) == BST_CHECKED)
				{
				for (int g=255; g>0; g--)
					{
					int r = GetDlgItemInt(hDlg, IDC_RED, NULL, false);
					int b = GetDlgItemInt(hDlg, IDC_BLUE, NULL, false);

					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, g, RGB(r, 255-g, b));
					}

				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255-(green-5), NULL);
                LineTo(hcomp, 0, 255-(green+5));
				LineTo(hcomp, 5, 255-(green));
				LineTo(hcomp, 0, 255-(green-5));
				MoveToEx(hcomp, rect.right-1, 255-(green-5), NULL);
				LineTo(hcomp, rect.right-1, 255-(green+5));
				LineTo(hcomp, rect.right-6, 255-(green));
				LineTo(hcomp, rect.right-1, 255-(green-5));
				}
            // BLUE
			else if (IsDlgButtonChecked(hDlg, IDC_B) == BST_CHECKED)
				{
				for (int b=255; b>0; b--)
					{
					int g = GetDlgItemInt(hDlg, IDC_GREEN, NULL, false);
					int r = GetDlgItemInt(hDlg, IDC_RED, NULL, false);

					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, b, RGB(r, g, 255-b));
					}

				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255-(blue-5), NULL);
                LineTo(hcomp, 0, 255-(blue+5));
				LineTo(hcomp, 5, 255-(blue));
				LineTo(hcomp, 0, 255-(blue-5));
				MoveToEx(hcomp, rect.right-1, 255-(blue-5), NULL);
				LineTo(hcomp, rect.right-1, 255-(blue+5));
				LineTo(hcomp, rect.right-6, 255-(blue));
				LineTo(hcomp, rect.right-1, 255-(blue-5));
				}
			// HUE
			else if (IsDlgButtonChecked(hDlg, IDC_H) == BST_CHECKED)
				{
				double hue, step;
				SColour tempcol;

				hue = 0.0;

				tempcol.h = 0;
				tempcol.s = 100;
				tempcol.v = 100;

				step = 359.0/255.0;

				for (int y=255; y>0; y--) // NB: 255 is the height of IDC_COLOR and IDC_COLOR1
					{
					tempcol.UpdateRGB();

					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, y, RGB(tempcol.r, tempcol.g, tempcol.b));

					hue += step;
					tempcol.h = (UINT) hue;
					}

				hue = (double) GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				hue = hue / step;

				// Draws arrows
				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255 - ((int)(hue)-5), NULL);
                LineTo(hcomp, 0, 255 - ((int)(hue)+5));
				LineTo(hcomp, 5, 255 - ((int)(hue)));
				LineTo(hcomp, 0, 255 - ((int)(hue)-5));
				MoveToEx(hcomp, rect.right-1, 255 - ((int)(hue)-5), NULL);
				LineTo(hcomp, rect.right-1, 255 - ((int)(hue)+5));
				LineTo(hcomp, rect.right-6, 255 - ((int)(hue)));
				LineTo(hcomp, rect.right-1, 255 - ((int)(hue)-5));
				}
            // SATURATION
			else if (IsDlgButtonChecked(hDlg, IDC_S) == BST_CHECKED)
				{
				double sat, step;
				SColour tempcol;

				sat = 0.0;

				tempcol.h = (unsigned short) GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				tempcol.s = 0;
				tempcol.v = (unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false);
                                    
				step = 100.0/255.0;

				for (int y=255; y>0; y--) // NB: 255 is the height of IDC_COLOR and IDC_COLOR1
					{
					tempcol.UpdateRGB();
					
					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, y, RGB(tempcol.r, tempcol.g, tempcol.b));

					sat += step;
					tempcol.s = (UINT) sat;
					}

				sat = (double) GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
				sat = sat / step;

				// Draws arrows
				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255 - ((int)(sat)-5), NULL);
                LineTo(hcomp, 0, 255 - ((int)(sat)+5));
				LineTo(hcomp, 5, 255 - ((int)(sat)));
				LineTo(hcomp, 0, 255 - ((int)(sat)-5));
				MoveToEx(hcomp, rect.right-1, 255 - ((int)(sat)-5), NULL);
				LineTo(hcomp, rect.right-1, 255 - ((int)(sat)+5));
				LineTo(hcomp, rect.right-6, 255 - ((int)(sat)));
				LineTo(hcomp, rect.right-1, 255 - ((int)(sat)-5));
				}
            // VALUE
			else if (IsDlgButtonChecked(hDlg, IDC_V) == BST_CHECKED)
				{
				double val, step;
				SColour tempcol;

				val = 0.0;

				tempcol.h = GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
				tempcol.s = GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
				tempcol.v = 0;
                                    
				step = 100.0/255.0;

				for (int y=255; y>0; y--) // NB: 255 is the height of IDC_COLOR and IDC_COLOR1
					{
					tempcol.UpdateRGB();
					
					for (int x=6; x<rect.right-6; x++)
						SetPixel(hcomp, x, y, RGB(tempcol.r, tempcol.g, tempcol.b));

					val += step;
					tempcol.v = (unsigned short) val;
					}

				val = (double) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false);
				val  = val / step;

				// Draws arrows
				pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
				SelectObject(hcomp, pen);
				MoveToEx(hcomp, 0, 255 - ((int)(val)-5), NULL);
                LineTo(hcomp, 0, 255 - ((int)(val)+5));
				LineTo(hcomp, 5, 255 - ((int)(val)));
				LineTo(hcomp, 0, 255 - ((int)(val)-5));
				MoveToEx(hcomp, rect.right-1, 255 - ((int)(val)-5), NULL);
				LineTo(hcomp, rect.right-1, 255 - ((int)(val)+5));
				LineTo(hcomp, rect.right-6, 255 - ((int)(val)));
				LineTo(hcomp, rect.right-1, 255 - ((int)(val)-5));
				}
			
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hcomp, 0, 0, SRCCOPY);

			DeleteObject(hbmp);
			DeleteDC(hcomp);
			ReleaseDC(hWndColor, hdc);
  
			// Alpha slider
			if (picker->GetAlphaUsage() != CP_NO_ALPHA)
				{
				hWndAlpha = GetDlgItem(hDlg, IDC_ALPHASLIDE);
				hdc = GetDC(hWndAlpha);
				hcomp = CreateCompatibleDC(hdc);

				GetClientRect(hWndAlpha, &rect);
				hbmp = CreateCompatibleBitmap(hdc, rect.right, 255);
				SelectObject(hcomp, hbmp);

				brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
				FillRect(hcomp, &rect, brush);
				DeleteObject(brush);

				rf = (float)red/255.0f;
				gf = (float)green/255.0f;
				bf = (float)blue/255.0f;
				bool flag = false;

				for (int y=255; y>0; y--)
					{
					COLORREF cb, cw;

					if (!(y%(WIDTH(rect)/2-6)))
						flag = !flag;

					float af = 1.0f - (float)y/255.0f;

					cb = RGB((rf*af)*255, (gf*af)*255, (bf*af)*255);
					cw = RGB((rf*af+1-af)*255, (gf*af+1-af)*255, (bf*af+1-af)*255);

					if (flag)
						{
						for (int x=6; x<(WIDTH(rect)/2); x++)
							SetPixel(hcomp, x, y, cw);
						for (int x=(WIDTH(rect)/2); x<rect.right-6; x++)
							SetPixel(hcomp, x, y, cb);
						}
					else
						{
						for (int x=6; x<(WIDTH(rect)/2); x++)
							SetPixel(hcomp, x, y, cb);
						for (int x=(WIDTH(rect)/2); x<rect.right-6; x++)
							SetPixel(hcomp, x, y, cw);
						}
					}

				if (picker->GetAlphaUsage() == CP_USE_ALPHA)
					{
					// Draws arrows
					int a;

					a = (int)((float)alpha/100.0f*255.0f);
					pen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
					SelectObject(hcomp, pen);
					MoveToEx(hcomp, 0, 255-(a-5), NULL);
					LineTo(hcomp, 0, 255-(a+5));
					LineTo(hcomp, 5, 255-(a));
					LineTo(hcomp, 0, 255-(a-5));
					MoveToEx(hcomp, rect.right-1, 255-(a-5), NULL);
					LineTo(hcomp, rect.right-1, 255-(a+5));
					LineTo(hcomp, rect.right-6, 255-(a));
					LineTo(hcomp, rect.right-1, 255-(a-5));
					}

				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hcomp, 0, 0, SRCCOPY);

				DeleteObject(hbmp);
				DeleteDC(hcomp);
				ReleaseDC(hWndColor, hdc);
				}

			// Current color & old color
			hWndColor = GetDlgItem(hDlg, IDC_CURRCOLOR);

			DrawCheckedRect(hWndColor, picker->GetCurrentColour().r, picker->GetCurrentColour().g, 
				picker->GetCurrentColour().b, picker->GetCurrentColour().a, 10, 10);
			
/*			hWndColor = GetDlgItem(hDlg, IDC_OLDCOLOR);
			
			DrawCheckedRect(hWndColor, picker->GetOldColour().r, picker->GetOldColour().g, picker->GetOldColour().b, 
				picker->GetOldColour().a, 10, 10);*/
			}
		break; // WM_PAINT

		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
			{
			POINT p;
			RECT rectC1, rectC2, rectA;
			HWND Color1, Color2, Alpha;

			if (uMsg == WM_MOUSEMOVE && wParam != MK_LBUTTON)
				break;
			
			// Determine where the mouse is
			Color1 = GetDlgItem(hDlg, IDC_COLOR);
			Color2 = GetDlgItem(hDlg, IDC_COLOR2);
			Alpha = GetDlgItem(hDlg, IDC_ALPHASLIDE);

			GetWindowRect(Color1, &rectC1);
			GetWindowRect(Color2, &rectC2);
			GetWindowRect(Alpha, &rectA);

			GetCursorPos(&p);
			
			// IDC_COLOR1 picked
			if (_IS_IN(rectC1.left-10, rectC1.right+10, p.x) && 
				_IS_IN(rectC1.top-10, rectC1.bottom+10, p.y))
				{
					int x = (p.x-rectC1.left);
					int y = (p.y-rectC1.top);
					CLAMP(x,0,255);
					CLAMP(y,0,255);

				if (IsDlgButtonChecked(hDlg, IDC_R) == BST_CHECKED)
				{
					picker->SetRGB((unsigned short)GetDlgItemInt(hDlg, IDC_RED, NULL, false), x, y);
					
					UpdateValues(hDlg, picker->GetCurrentColour());
				}

				else if (IsDlgButtonChecked(hDlg, IDC_G) == BST_CHECKED)
					{
					picker->SetRGB((unsigned short)(p.x-rectC1.left), 
						(unsigned short)GetDlgItemInt(hDlg, IDC_GREEN, NULL, false),
						(unsigned short)(p.y-rectC1.top));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}

				else if (IsDlgButtonChecked(hDlg, IDC_B) == BST_CHECKED)
					{
					picker->SetRGB(y, x,
						(unsigned short) GetDlgItemInt(hDlg, IDC_BLUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());
					} 

				else if (IsDlgButtonChecked(hDlg, IDC_H) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short)GetDlgItemInt(hDlg, IDC_HUE, NULL, false),
						(unsigned short)(x/255.0*100.0), 
						(unsigned short)((255-y)/255.0*100.0));

					UpdateValues(hDlg, picker->GetCurrentColour());					
					}

				else if (IsDlgButtonChecked(hDlg, IDC_S) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short)(x/255.0*359.0),
						(unsigned short)GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false), 
						(unsigned short)((255-y)/255.0*100.0));

					UpdateValues(hDlg, picker->GetCurrentColour());					
					} 
				
				else if (IsDlgButtonChecked(hDlg, IDC_V) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short)(x/255.0*359.0),
						(unsigned short)((255-y)/255.0*100.0), 
						(unsigned short)GetDlgItemInt(hDlg, IDC_VALUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());					
					}
				}
			// IDC_COLOR2 picked
			else if (_IS_IN(rectC2.left-10, rectC2.right+10, p.x) && 
				_IS_IN(rectC2.top-10, rectC2.bottom+10, p.y))
				{
					int y = (p.y-rectC2.top);
					CLAMP(y,0,255);

				if (IsDlgButtonChecked(hDlg, IDC_R) == BST_CHECKED)
					{
					picker->SetRGB((unsigned short)(255 - y),
						(unsigned short) GetDlgItemInt(hDlg, IDC_GREEN, NULL, false),
						(unsigned short) GetDlgItemInt(hDlg, IDC_BLUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}

				else if (IsDlgButtonChecked(hDlg, IDC_G) == BST_CHECKED)
					{
					picker->SetRGB((unsigned short) GetDlgItemInt(hDlg, IDC_RED, NULL, false),
						(unsigned short) (255 - y),
						(unsigned short) GetDlgItemInt(hDlg, IDC_BLUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}
				
				else if (IsDlgButtonChecked(hDlg, IDC_B) == BST_CHECKED)
					{
					picker->SetRGB((unsigned short) GetDlgItemInt(hDlg, IDC_RED, NULL, false),
						(unsigned short) GetDlgItemInt(hDlg, IDC_GREEN, NULL, false),
						(unsigned short) (255 - y));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}

				else if (IsDlgButtonChecked(hDlg, IDC_H) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short) ((255 - y)/255.0*359.0),
						(unsigned short) GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false),
						(unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}

				else if (IsDlgButtonChecked(hDlg, IDC_S) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short) GetDlgItemInt(hDlg, IDC_HUE, NULL, false),
						(unsigned short) ((255 - y)/255.0*100.0),
						(unsigned short) GetDlgItemInt(hDlg, IDC_VALUE, NULL, false));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}

				else if (IsDlgButtonChecked(hDlg, IDC_V) == BST_CHECKED)
					{
					picker->SetHSV((unsigned short) GetDlgItemInt(hDlg, IDC_HUE, NULL, false),
						(unsigned short) GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false),
						(unsigned short) ((255 - y)/255.0*100.0));

					UpdateValues(hDlg, picker->GetCurrentColour());
					}
				}
			// IDC_ALPHASLIDE picked
			else if (_IS_IN(rectA.left-10, rectA.right+10, p.x) &&
				_IS_IN(rectA.top-10, rectA.bottom+10, p.y) && (picker->GetAlphaUsage()==CP_USE_ALPHA))
				{
				int y = (p.y-rectA.top);
				CLAMP(y,0,255);

				picker->SetAlpha((unsigned short)((float)(255 - y)/255.0f*100.0f));

				UpdateValues(hDlg, picker->GetCurrentColour());
				}

			SendMessage(hDlg, WM_PAINT, 0, 0);
			}
		break; // WM_LBUTTONDOWN, WM_MOUSEMOVE		

		case WM_COMMAND:
			switch(HIWORD(wParam))
				{
				case EN_SETFOCUS:
					{
					SendDlgItemMessage(hDlg, LOWORD(wParam), EM_SETSEL, 0, -1);
					}
				break;

				case EN_KILLFOCUS:
					{
					switch(LOWORD(wParam))
						{
						int tempcolor;

						case IDC_RED:
						case IDC_GREEN:
						case IDC_BLUE:
							{
							tempcolor = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, false);
							tempcolor = min(255, tempcolor);
							tempcolor = max(0, tempcolor);
							SetDlgItemInt(hDlg, LOWORD(wParam), tempcolor, false);

							picker->SetRGB(GetDlgItemInt(hDlg, IDC_RED, NULL, false),
								GetDlgItemInt(hDlg, IDC_GREEN, NULL, false), 
								GetDlgItemInt(hDlg, IDC_BLUE, NULL, false));
							}
						break;

						case IDC_HUE:
							{
							tempcolor = GetDlgItemInt(hDlg, IDC_HUE, NULL, false);
							tempcolor = min(359, tempcolor);
							tempcolor = max(0, tempcolor);
							SetDlgItemInt(hDlg, IDC_HUE, tempcolor, false);

							picker->SetHSV(GetDlgItemInt(hDlg, IDC_HUE, NULL, false),
								GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false), 
								GetDlgItemInt(hDlg, IDC_VALUE, NULL, false));
							}
						break;

						case IDC_SATURATION:
						case IDC_VALUE:
							{
							tempcolor = GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false);
							tempcolor = min(100, tempcolor);
							tempcolor = max(0, tempcolor);
							SetDlgItemInt(hDlg, IDC_SATURATION, tempcolor, false);							

							picker->SetHSV(GetDlgItemInt(hDlg, IDC_HUE, NULL, false),
								GetDlgItemInt(hDlg, IDC_SATURATION, NULL, false), 
								GetDlgItemInt(hDlg, IDC_VALUE, NULL, false));
							}
						break;
						} 

					// Update color
					UpdateValues(hDlg, picker->GetCurrentColour());
					SendMessage(hDlg, WM_PAINT, 0, 0);
					}
				break;
				
				case BN_CLICKED: // Equivalent to STN_CLICKED
					{
					switch(LOWORD(wParam))
						{
						case IDC_R:
						case IDC_B:
						case IDC_G:
						case IDC_H:
						case IDC_S:
						case IDC_V:
							{
							SendMessage(hDlg, WM_PAINT, 0, 0);
							}
						break;

						case IDC_OLDCOLOR:
							{
							picker->Revert();
							UpdateValues(hDlg, picker->GetCurrentColour());
							SendMessage(hDlg, WM_PAINT, 0, 0);
							}
						break;

						case IDB_OK:
							{
							picker->UpdateOldColour();
							pbuffer.Destroy();
							EndDialog(hDlg, IDB_OK);
							}
						break;
 
						case IDB_CANCEL:
							{
							SColour old = picker->GetOldColour();
 
							picker->SetRGB(old.r, old.g, old.b);
							picker->SetAlpha(old.a);
							pbuffer.Destroy();
							EndDialog(hDlg, IDB_CANCEL);
							}
						break;
						}
					}
				break;
				}
		break; // WM_COMMAND
		}
	return 0L;
	}

// Draw a b/w checked rectangle, "covered" with the rgba color provided.
// cx and cy are the size of the checks
EXPORT void DrawCheckedRect(HWND hWnd, int r, int g, int b, int a, int cx, int cy)
	{
	float rf = (float)r/255.0f,
		gf = (float)g/255.0f,
		bf = (float)b/255.0f,
		af = (float)a/100.0f;
	HDC hdc = GetDC(hWnd);
	HBRUSH brush, brush2;
	RECT rect, r2;
	bool flag;

	brush = CreateSolidBrush(RGB((rf*af)*255, (gf*af)*255, (bf*af)*255));
	brush2 = CreateSolidBrush(RGB((rf*af+1-af)*255, (gf*af+1-af)*255, (bf*af+1-af)*255));

	GetWindowRect(hWnd, &rect);

	for (int x=0; (x*cx)<WIDTH(rect); x++)
		{
		if (x%2)
			flag = false;
		else
			flag = true;
		for (int y=0; (y*cy)<HEIGHT(rect); y++)
			{
			r2.left = x*cx;
			r2.right = min((x+1)*cx, WIDTH(rect)-2);
			r2.top = y*cy;
			r2.bottom = min((y+1)*cy, HEIGHT(rect)-2);
			
			if (flag)
				FillRect(hdc, &r2, brush);
			else
				FillRect(hdc, &r2, brush2);

			flag = !flag;
			}
		}

	DeleteObject(brush);
	DeleteObject(brush2);
	ReleaseDC(hWnd, hdc);
	}

BYTE **g_ppClassBRUSH = (BYTE **)0x006F9698;
DWORD ofsBrushColor   = 0x00000014;
BYTE *g_pBrushColorUI = (BYTE *)0x00D91834;//0x03930280;

class CSketchApp //0x0067CB04
{
public:													//  Index Offset
  virtual 	void sub_587EB1(void);								// 0000  0000
  virtual 	void sub_603F80(void);								// 0001  0004
  virtual 	void nullsub_221(void);								// 0002  0008
  virtual 	void nullsub_222(void);								// 0003  000C
  virtual 	void nullsub_223(void);								// 0004  0010
  virtual 	 int _stdcall OnCmdMsg(unsigned int,int,void near *,void* /*struct AFX_CMDHANDLERINFO near **/);								// 0005  0014
  virtual 	 void _stdcall OnFinalRelease(void);								// 0006  0018
  virtual 	void sub_56A5F6(void);								// 0007  001C
  virtual 	void sub_56A5FE(void);								// 0008  0020
  virtual 	void sub_56A603(void);								// 0009  0024
  virtual 	void sub_56A606(void);								// 0010  0028
  virtual 	 long _stdcall GetTypeLib(unsigned long, void*/*struct ITypeLib near * near **/);								// 0011  002C
  virtual 	void sub_59FA30(void);								// 0012  0030
  virtual 	void sub_56A63D(void);								// 0013  0034
  virtual 	void sub_56A67B(void);								// 0014  0038
  virtual 	void sub_56A649(void);								// 0015  003C
  virtual 	void sub_56A643(void);								// 0016  0040
  virtual 	void sub_56A670(void);								// 0017  0044
  virtual 	void sub_56A676(void);								// 0018  0048
  virtual 	void sub_56A686(void);								// 0019  004C
  virtual 	void sub_56A681(void);								// 0020  0050
  virtual 	void sub_604A60(void);								// 0021  0054
  virtual 	 int _stdcall Run(void);								// 0022  0058
  virtual 	 int _stdcall PreTranslateMessage(void */*struct tagMSG near **/);								// 0023  005C
  virtual 	 int _stdcall PumpMessage(void);								// 0024  0060
  virtual 	void sub_59FAA0(void);								// 0025  0064
  virtual 	void sub_59FB30(void);								// 0026  0068
  virtual 	void sub_6068B0(void);								// 0027  006C
  virtual 	void sub_59FB60(void);								// 0028  0070
  virtual 	 int _stdcall ProcessMessageFilter(int,void* /*struct tagMSG near **/);								// 0029  0074
  virtual 	 class CWnd near * _stdcall GetMainWnd(void);								// 0030  0078
  virtual 	 void _stdcall Delete(void);								// 0031  007C
  virtual 	void unknown_libname_916(void);								// 0032  0080
  virtual 	void sub_59FD20(void);								// 0033  0084
  virtual 	void sub_6046D0(void);								// 0034  0088
  virtual 	void sub_606520(void);								// 0035  008C
  virtual 	void sub_660AC0(void);								// 0036  0090
  virtual 	void sub_577182(void);								// 0037  0094
  virtual 	void sub_577445(void);								// 0038  0098
  virtual 	void sub_5BBA30(void);								// 0039  009C
};
// Auto reconstructed from vtable block @ 0x00683154
// from "Flash4.exe", by VTableRec.idc
class CColorDef
{
public:													//  Index Offset
	virtual void CColorDef_5FC1A0(void);								// 0000  0000
	virtual _stdcall ~CColorDef();								// 0001  0004
	virtual void CColorDef_606E10(void);								// 0002  0008
	virtual void nullsub_222(void);								// 0003  000C
	virtual void nullsub_223(void);								// 0004  0010
public:
  DWORD m_color;
};

// Auto reconstructed from vtable block @ 0x0068149C
// from "Flash4.exe", by VTableRec.idc
class CColorPopupButton
{
public:													//  Index Offset
	virtual void sub_5FBF50(void);								// 0000  0000
	virtual _stdcall ~CColorPopupButton();								// 0001  0004
	virtual void nullsub_221(void);								// 0002  0008
	virtual void nullsub_222(void);								// 0003  000C
	virtual void nullsub_223(void);								// 0004  0010
	virtual int _stdcall OnCmdMsg(unsigned int,int,void near *,struct AFX_CMDHANDLERINFO near *);								// 0005  0014
	virtual void _stdcall OnFinalRelease(void);								// 0006  0018
	virtual void CCmdTarget_56A5F6(void);								// 0007  001C
	virtual void CCmdTarget_56A5FE(void);								// 0008  0020
	virtual void CCmdTarget_56A603(void);								// 0009  0024
	virtual void CCmdTarget_56A606(void);								// 0010  0028
	virtual long _stdcall GetTypeLib(unsigned long,struct ITypeLib near * near *);								// 0011  002C
	virtual void sub_5FBFB0(void);								// 0012  0030
	virtual void CCmdTarget_56A63D(void);								// 0013  0034
	virtual void CCmdTarget_56A67B(void);								// 0014  0038
	virtual void CCmdTarget_56A649(void);								// 0015  003C
	virtual void CCmdTarget_56A643(void);								// 0016  0040
	virtual void CCmdTarget_56A670(void);								// 0017  0044
	virtual void CCmdTarget_56A676(void);								// 0018  0048
	virtual void CCmdTarget_56A686(void);								// 0019  004C
	virtual void CCmdTarget_56A681(void);								// 0020  0050
	virtual void nullsub_224(void);								// 0021  0054
	virtual int _stdcall Create(char const near *,char const near *,unsigned long,struct tagRECT const near &,class CWnd near *,unsigned int,struct CCreateContext near *);								// 0022  0058
	virtual int _stdcall DestroyWindow(void);								// 0023  005C
	virtual void CWnd_5675BB(void);								// 0024  0060
	virtual void _stdcall CalcWindowRect(struct tagRECT near *,unsigned int);								// 0025  0064
	virtual int _stdcall OnToolHitTest(class CPoint,struct tagTOOLINFOA near *)const;								// 0026  0068
	virtual void CWnd_568AFF(void);								// 0027  006C
	virtual void CWnd_567E2F(void);								// 0028  0070
	virtual int _stdcall ContinueModal(void);								// 0029  0074
	virtual void _stdcall EndModalLoop(int);								// 0030  0078
	virtual int _stdcall OnCommand(unsigned int,long);								// 0031  007C
	virtual int _stdcall OnNotify(unsigned int,long,long near *);								// 0032  0080
	virtual void CWnd_5678B2(void);								// 0033  0084
	virtual void nullsub_225(void);								// 0034  0088
	virtual void CWnd_401680(void);								// 0035  008C
	virtual void CWnd_401690(void);								// 0036  0090
	virtual int _stdcall PreTranslateMessage(struct tagMSG near *);								// 0037  0094
	virtual int _stdcall OnAmbientProperty(class COleControlSite near *,long,struct tagVARIANT near *);								// 0038  0098
	virtual long _stdcall WindowProc(unsigned int,unsigned int,long);								// 0039  009C
	virtual void Func0040(void);								// 0040  00A0
	virtual long _stdcall DefWindowProcA(unsigned int,unsigned int,long);								// 0041  00A4
	virtual void nullsub_218(void);								// 0042  00A8
	virtual void Func0043(void);								// 0043  00AC
	virtual void CWnd_5699B6(void);								// 0044  00B0
	virtual void CWnd_569EF0(void);								// 0045  00B4
	virtual void CWnd_566496(void);								// 0046  00B8
	virtual void sub_60E130(void);								// 0047  00BC
	virtual void sub_60D680(void);								// 0048  00C0
	virtual void CColorPopupButton_60D6C0(void);								// 0049  00C4
	virtual void sub_478100(void);								// 0050  00C8
public:
  DWORD unknown[29];
  CColorDef m_colordef;
};

// Auto reconstructed from vtable block @ 0x0067F16C
// from "Flash4.exe", by VTableRec.idc
class CSketchToolBar
{
public:													//  Index Offset
	virtual void sub_5FB800(void);								// 0000  0000
	virtual _stdcall ~CSketchToolBar();								// 0001  0004
	virtual void nullsub_221(void);								// 0002  0008
	virtual void nullsub_222(void);								// 0003  000C
	virtual void nullsub_223(void);								// 0004  0010
	virtual int _stdcall OnCmdMsg(unsigned int,int,void near *,struct AFX_CMDHANDLERINFO near *);								// 0005  0014
	virtual void _stdcall OnFinalRelease(void);								// 0006  0018
	virtual void CCmdTarget_56A5F6(void);								// 0007  001C
	virtual void CCmdTarget_56A5FE(void);								// 0008  0020
	virtual void CCmdTarget_56A603(void);								// 0009  0024
	virtual void CCmdTarget_56A606(void);								// 0010  0028
	virtual long _stdcall GetTypeLib(unsigned long,struct ITypeLib near * near *);								// 0011  002C
	virtual void Func0012(void);								// 0012  0030
	virtual void CCmdTarget_56A63D(void);								// 0013  0034
	virtual void CCmdTarget_56A67B(void);								// 0014  0038
	virtual void CCmdTarget_56A649(void);								// 0015  003C
	virtual void CCmdTarget_56A643(void);								// 0016  0040
	virtual void CCmdTarget_56A670(void);								// 0017  0044
	virtual void CCmdTarget_56A676(void);								// 0018  0048
	virtual void CCmdTarget_56A686(void);								// 0019  004C
	virtual void CCmdTarget_56A681(void);								// 0020  0050
	virtual void nullsub_224(void);								// 0021  0054
	virtual int _stdcall Create(char const near *,char const near *,unsigned long,struct tagRECT const near &,class CWnd near *,unsigned int,struct CCreateContext near *);								// 0022  0058
	virtual int _stdcall DestroyWindow(void);								// 0023  005C
	virtual void sub_5876B2(void);								// 0024  0060
	virtual void _stdcall CalcWindowRect(struct tagRECT near *,unsigned int);								// 0025  0064
	virtual void sub_60B0E0(void);								// 0026  0068
	virtual void CWnd_568AFF(void);								// 0027  006C
	virtual void CWnd_567E2F(void);								// 0028  0070
	virtual int _stdcall ContinueModal(void);								// 0029  0074
	virtual void _stdcall EndModalLoop(int);								// 0030  0078
	virtual int _stdcall OnCommand(unsigned int,long);								// 0031  007C
	virtual int _stdcall OnNotify(unsigned int,long,long near *);								// 0032  0080
	virtual void CWnd_5678B2(void);								// 0033  0084
	virtual void nullsub_225(void);								// 0034  0088
	virtual void CWnd_401680(void);								// 0035  008C
	virtual void CWnd_401690(void);								// 0036  0090
	virtual int _stdcall PreTranslateMessage(struct tagMSG near *);								// 0037  0094
	virtual int _stdcall OnAmbientProperty(class COleControlSite near *,long,struct tagVARIANT near *);								// 0038  0098
	virtual long _stdcall WindowProc(unsigned int,unsigned int,long);								// 0039  009C
	virtual void Func0040(void);								// 0040  00A0
	virtual long _stdcall DefWindowProcA(unsigned int,unsigned int,long);								// 0041  00A4
	virtual void _stdcall PostNcDestroy(void);								// 0042  00A8
	virtual int _stdcall OnChildNotify(unsigned int,unsigned int,long,long near *);								// 0043  00AC
	virtual void CWnd_5699B6(void);								// 0044  00B0
	virtual void CWnd_569EF0(void);								// 0045  00B4
	virtual void CWnd_566496(void);								// 0046  00B8
	virtual void CSketchToolBar_6628F0(void);								// 0047  00BC
	virtual class CSize _stdcall CalcDynamicLayout(int,unsigned long);								// 0048  00C0
	virtual void CSketchToolBar_60C7A0(void);								// 0049  00C4
	virtual void _stdcall DelayShow(int);								// 0050  00C8
	virtual int _stdcall IsVisible(void)const;								// 0051  00CC
	virtual void sub_579925(void);								// 0052  00D0
	virtual void sub_578F0F(void);								// 0053  00D4
	virtual void nullsub_279(void);								// 0054  00D8
	virtual void sub_60B5A0(void);								// 0055  00DC
	virtual int _stdcall SetStatusText(int);								// 0056  00E0
	virtual void sub_60B410(void);								// 0057  00E4
	virtual void sub_60B2B0(void);								// 0058  00E8
	virtual void CSketchToolBar_6629E0(void);								// 0059  00EC
	virtual void sub_60B1B0(void);								// 0060  00F0
	virtual void sub_46A1B0(void);								// 0061  00F4
	virtual void CSketchToolBar_60BFD0(void);								// 0062  00F8
	virtual CColorPopupButton* GetCColorPopupButtonBRUSH(void);								// 0063  00FC
};

CSketchToolBar* CSketchApp__GetCSketchToolBar(CSketchApp *sketchapp)
{
  __asm
  {
    push 0;
    mov ecx, sketchapp;
    mov eax, 0x0059FBB0;
    call eax;
  }
}


typedef CSketchApp* /*pWinThread*/ (__stdcall *flAfxGetThread_t)();
flAfxGetThread_t flAfxGetThread = (flAfxGetThread_t)0x0056B396;

#include <math.h>

void UpdateValues(HWND hDlg, SColour col)
	{
	char buff[10];
  BYTE *pClassBRUSH = *g_ppClassBRUSH;
  if(pClassBRUSH)
  {
    BYTE *pBrushColor = pClassBRUSH+ofsBrushColor;

    pBrushColor[0] = pBrushColor[0] = col.r;
    pBrushColor[1] = pBrushColor[1] = col.g;
    pBrushColor[2] = pBrushColor[2] = col.b;
    BYTE a255 = floor(((float)col.a) * 2.55 + 0.5);
    pBrushColor[3] = pBrushColor[3] = a255;
  }
  {
    //char b[50];
    CSketchApp* sketchapp = flAfxGetThread();
    //sprintf(b, "AfxGetThread: ret==0x%08X", CSketchApp__GetCSketchToolBar(sketchapp)->GetCColorPopupButtonBRUSH()->m_colordef.m_color);
    //OutputDebugString(b);
    memcpy(&(CSketchApp__GetCSketchToolBar(sketchapp)->GetCColorPopupButtonBRUSH()->m_colordef.m_color), pClassBRUSH+ofsBrushColor, 4);
  }

	SetDlgItemInt(hDlg, IDC_RED, col.r, false);
	SetDlgItemInt(hDlg, IDC_GREEN, col.g, false);
	SetDlgItemInt(hDlg, IDC_BLUE, col.b, false);
	SetDlgItemInt(hDlg, IDC_ALPHA, col.a, false);
	SetDlgItemInt(hDlg, IDC_HUE, col.h, false);
	SetDlgItemInt(hDlg, IDC_SATURATION, col.s, false);
	SetDlgItemInt(hDlg, IDC_VALUE, col.v, false);
    
	sprintf(buff, "%02X%02X%02X", col.r, col.g, col.b);
	SetDlgItemText(hDlg, IDC_HEXCOL, buff);
	}