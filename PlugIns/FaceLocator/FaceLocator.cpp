// FaceLocator.cpp : Defines the initialization routines for the DLL.
#include "stdafx.h"
#include "FaceLocator.h"
#include "BufStruct.h"
#include "ImageProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//
/////////////////////////////////////////////////////////////////////////////
// CFaceLocatorApp
BEGIN_MESSAGE_MAP(CFaceLocatorApp, CWinApp)
//{{AFX_MSG_MAP(CFaceLocatorApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFaceLocatorApp construction
CFaceLocatorApp::CFaceLocatorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CFaceLocatorApp object
CFaceLocatorApp theApp;
char sInfo[] = "��������-���ڲ�ɫ��Ϣ�������ָ����";
bool bLastPlugin = false;
DLL_EXP void ON_PLUGIN_BELAST(bool bLast)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
	bLastPlugin = bLast;
}
DLL_EXP LPCTSTR ON_PLUGININFO(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
	return sInfo;
}
DLL_EXP void ON_INITPLUGIN(LPVOID lpParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
												 // theApp.dlg.Create(IDD_PLUGIN_SETUP);
												 // theApp.dlg.ShowWindow(SW_HIDE);
}
DLL_EXP int ON_PLUGINCTRL(int nMode, void *pParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int nRet = 0;
	switch (nMode)
	{
	case 0:
	{
		// theApp.dlg.ShowWindow(SW_SHOWNORMAL);
		// theApp.dlg.SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
	}
	break;
	}
	return nRet;
}
/****************************************************************************************/
/*                             ��������붨λ                                           */
/****************************************************************************************/
void dilate(int w, int h, BYTE *bmp, int n)
{
	int len = n / 2;
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			if (bmp[j * w + i])
			{
				int s = i - n / 2;
				int t = j - n / 2;
				int sf = i + n / 2;
				int tf = j + n / 2;

				for (; t <= tf; t++)
				{
					if (t >= 0 && t < h)
					{
						for (; s <= sf; s++)
						{
							if (s >= 0 && s < w)
							{
								bmp[j * w + i] = 255;
							}
						}
					}
				}
			}
		}
	}
}

DLL_EXP void ON_PLUGINRUN(int w, int h, BYTE *pYBits, BYTE *pUBits, BYTE *pVBits, BYTE *pBuffer)
{
	// pYBits ��СΪw*h
	// pUBits �� pVBits �Ĵ�СΪ w*h/2
	// pBuffer �Ĵ�СΪ w*h*4
	// �����㷨������һ�����裬��w��16�ı���

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�

	// ���д��Ӧ�������
	long int wh = w * h;

	BUF_STRUCT *pImgProcBuf = (BUF_STRUCT *)pBuffer;

	// 2. ��ɫ��ģ
	{
		BYTE *pB = pImgProcBuf->TempImage1d8;

		BYTE *pOY = pImgProcBuf->clrBmp_1d8;
		BYTE *pOU = pOY + wh / 8;
		BYTE *pOV = pOU + wh / 16;

		for (int j = 0; j < h / 4; j++)
		{
			int i;
			for (i = 0; i < w / 4; i++)
			{
				pB[i] = pOU[i] >= 85 && pOU[i] <= 126 && pOV[i] >= 130 && pOV[i] <= 165 ? 255 : 0;
			}
			pB += w / 4;
			pOU += w / 4;
			pOV += w / 4;
		}
	}
	// 3. ��̬ѧ����
	{
		ShowDebugMessage("%d", pImgProcBuf->TempImage1d8[100]);
		dilate(w / 4, h / 4, pImgProcBuf->TempImage1d8, 3);
		ShowDebugMessage("%d", pImgProcBuf->TempImage1d8[100]);
	}

	// 6. ��չ
	{
		BYTE *pB = pImgProcBuf->TempImage1d8;

		BYTE *pNY = pImgProcBuf->clrBmp_1d8;

		for (int j = 0; j < h / 4; j++)
		{
			int i;
			for (i = 0; i < w / 4; i++)
			{
				pNY[2 * i] = pB[i];
				pNY[2 * i + 1] = pB[i];
			}
			pB += w / 4;
			pNY += w / 2;
		}
	}

	// ����Ĳ������ڲ���ͼ������Ľ���Ƿ���ȷ��
	if (bLastPlugin)
	{
		// ����grayBmp_1d16:��grayBmp_1d16���Ƶ���ʾͼƬ�����Ͻ�
		// CopyToRect(pImgProcBuf->grayBmp_1d16, pYBits, w / 4, h / 4, w, h, 0, 0, true);
		// ����colorBmp:��clrBmp_1d8���Ƶ���ʾͼƬ�����Ͻ�
		CopyToRect(pImgProcBuf->clrBmp_1d8, pYBits, w / 2, h / 4, w, h, w / 2, 0, false);
	}
}

void erode(int w, int h, BYTE *bmp, int n)
{
	int len = n / 2;
	{
		BYTE *bw = bmp;
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				for (int k = 0; k < len; k++)
				{
					if (bw[i + k])
					{
					}
				}
			}
		}
	}
}


DLL_EXP void ON_PLUGINEXIT()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
												 // theApp.dlg.DestroyWindow();
}
