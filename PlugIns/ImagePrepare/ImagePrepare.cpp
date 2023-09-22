// ImagePrepare.cpp : Defines the initialization routines for the DLL.

#include "stdafx.h"
#include "ImagePrepare.h"
//
#include "BufStruct.h"
#include "ImageProc.h"
#include "math.h"

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
// CImagePrepareApp
BEGIN_MESSAGE_MAP(CImagePrepareApp, CWinApp)
//{{AFX_MSG_MAP(CImagePrepareApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//
/////////////////////////////////////////////////////////////////////////////
// CImagePrepareApp construction
CImagePrepareApp::CImagePrepareApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CImagePrepareApp object
CImagePrepareApp theApp;

char sInfo[] = "��������-����ͷ��Ƶ��ͼƬ��ȡ������";
bool bLastPlugin = false;

DLL_EXP void ON_PLUGIN_BELAST(bool bLast)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
	bLastPlugin = bLast;
}
// �������
DLL_EXP LPCTSTR ON_PLUGININFO(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
	return sInfo;
}
//
DLL_EXP void ON_INITPLUGIN(LPVOID lpParameter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
												 // theApp.dlg.Create(IDD_PLUGIN_SETUP);
												 // theApp.dlg.ShowWindow(SW_HIDE);
}
DLL_EXP int ON_PLUGINCTRL(int nMode, void *pParameter)
{
	// ģ��״̬�л�
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int nRet = 0;
	return nRet;
}

/****************************************************************************************/
/*                             ����ͷ��Ƶ��ͼƬ��ȡ���ز����ȴ���                       */
/****************************************************************************************/

DLL_EXP void ON_PLUGINRUN(int w, int h, BYTE *pYBits, BYTE *pUBits, BYTE *pVBits, BYTE *pBuffer)
{
	// pYBits ��СΪw*h
	// pUBits �� pVBits �Ĵ�СΪ w*h/2
	// pBuffer �Ĵ�СΪ w*h*6
	// �����㷨������һ�����裬��w��16�ı���
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�

	// ShowDebugMessage("��printf�����÷�����,X=%d,Y=%d\n",10,5);

	// ���д��Ӧ�������

	BUF_STRUCT *pImgProcBuf = (BUF_STRUCT *)pBuffer;
	long int wh = w * h;
	if (pImgProcBuf->bNotInited)
	{
		// (1) init pointer vars
		pImgProcBuf->colorBmp = pBuffer + sizeof(BUF_STRUCT);
		pImgProcBuf->grayBmp = pImgProcBuf->colorBmp;
		pImgProcBuf->clrBmp_1d8 = pImgProcBuf->grayBmp + wh * 2;
		pImgProcBuf->grayBmp_1d16 = pImgProcBuf->clrBmp_1d8 + wh / 4;
		pImgProcBuf->TempImage1d8 = pImgProcBuf->grayBmp_1d16 + wh / 16;
		pImgProcBuf->lastImageQueue1d16m8 = pImgProcBuf->TempImage1d8 + wh / 8;
		pImgProcBuf->pOtherVars = (OTHER_VARS *)(pImgProcBuf->lastImageQueue1d16m8 + wh / 2);
		pImgProcBuf->pOtherData = (aBYTE *)(pImgProcBuf->pOtherVars + sizeof(pImgProcBuf->pOtherVars));
		int i;
		for (i = 0; i < 8; i++)
		{
			pImgProcBuf->pImageQueue[i] = pImgProcBuf->lastImageQueue1d16m8 + i * (wh / 16);
		}
		ShowDebugMessage("(1) ���");
		// (2) init signals
		pImgProcBuf->W = w;
		pImgProcBuf->H = h;
		pImgProcBuf->cur_allocSize = 0;
		pImgProcBuf->allocTimes = 0;
		pImgProcBuf->cur_maxallocsize = 0;
		pImgProcBuf->bLastEyeChecked = 0;
		pImgProcBuf->EyeBallConfirm = 0;
		pImgProcBuf->EyePosConfirm = 0;
		pImgProcBuf->nImageQueueIndex = -1;
		pImgProcBuf->nLastImageIndex = -1;
		ShowDebugMessage("(2) ���");
		// (3) init byHistMap
		for (i = 0; i < sizeof(pImgProcBuf->pOtherVars->byHistMap_U); i++)
		{
			pImgProcBuf->pOtherVars->byHistMap_U[i] = (i >= 85 && i <= 126);
			pImgProcBuf->pOtherVars->byHistMap_V[i] = (i >= 130 && i <= 165);
		}
		ShowDebugMessage("(3) ���");
		// (4) init OTHER_VARS
		// aRect rc;
		// rc.height = 0;
		// rc.width = 0;
		// rc.left = 0;
		// rc.top = 0;
		// pImgProcBuf->pOtherVars->objLefteye.rcObject = rc;
		// pImgProcBuf->pOtherVars->objLefteye.spdxObj = 0;
		// pImgProcBuf->pOtherVars->objLefteye.spdyObj = 0;
		// pImgProcBuf->pOtherVars->objLefteye.nMinDist = 0x7fffffff;
		// pImgProcBuf->pOtherVars->objLefteye.bBrokenTrace = false;
		// pImgProcBuf->pOtherVars->objLefteye.bSaveit = false;
		// pImgProcBuf->pOtherVars->objLefteye.nBrokenTimes = 0;
		// pImgProcBuf->pOtherVars->objLefteye.fvObject.size = sizeof(FeatureVector4P) * 5;
		// ((FeatureVector4P *)pImgProcBuf->pOtherVars->objLefteye.fvObject.Vector)->nLevels = 2;
		// ((FeatureVector4P *)pImgProcBuf->pOtherVars->objLefteye.fvObject.Vector)->Vector->x = 0;
		// ((FeatureVector4P *)pImgProcBuf->pOtherVars->objLefteye.fvObject.Vector)->Vector->y = 0;
		// ((FeatureVector4P *)pImgProcBuf->pOtherVars->objLefteye.fvObject.Vector)->pNL_LeftTop->nLevels = 1;
		// ShowDebugMessage("(4) ���");
		// (5) init FeaProcBuf
		// (6) init max_allocSize
		pImgProcBuf->max_allocSize = wh * 17 / 16 - sizeof(BUF_STRUCT) - sizeof(OTHER_VARS);
		ShowDebugMessage("(6) ���");
		// (7) init pBS
		myHeapAllocInit(pImgProcBuf);
		aBYTE *tempImg;
		tempImg = myHeapAlloc(wh / 8);
		ShowDebugMessage("(7) ���");

		pImgProcBuf->bNotInited = false;
	}

	ShowDebugMessage("�ǳ�ʼ���׶�");

	// 2. set img pointer
	// ((BUF_STRUCT *)pBuffer)->displayImage = pYBits;
	// 3. copy colorBmp
	// ((BUF_STRUCT *)pBuffer)->colorBmp = pYBits;

	// 4. 1/8 sample
	{
		ShowDebugMessage("�������׶�");
		BYTE *pY = pImgProcBuf->clrBmp_1d8;
		BYTE *pU = pY + wh / 8;
		BYTE *pV = pU + wh / 2 / 8;

		BYTE *pNY = pY;
		BYTE *pNU = pU;
		BYTE *pNV = pV;
		BYTE *pOY = pYBits;
		BYTE *pOU = pUBits;
		BYTE *pOV = pVBits;

		for (int j = 0; j < h / 4; j++)
		{
			int i;
			for (i = 0; i < w / 2; i++) // һ�д���
			{
				pNY[i] = pOY[i * 2];
			}
			for (i = 0; i < w / 4; i++)
			{
				pNU[i] = pOU[i * 2];
				pNV[i] = pOV[i * 2];
			}
			pNY += w / 2;
			pNU += w / 4;
			pNV += w / 4;
			pOY += w * 4;
			pOU += w / 2 * 4;
			pOV += w / 2 * 4;
		}
	}
	// 4. 1/16 sample
	{
		ShowDebugMessage("�ҶȲ����׶�");
		BYTE *pY = pImgProcBuf->grayBmp_1d16;
		BYTE *pU = pY + wh / 16;
		BYTE *pV = pU + wh / 2 / 16;

		BYTE *pNY = pY;
		BYTE *pNU = pU;
		BYTE *pNV = pV;
		BYTE *pOY = pYBits;
		BYTE *pOU = pUBits;
		BYTE *pOV = pVBits;

		for (int j = 0; j < h / 4; j++)
		{
			int i;
			for (i = 0; i < w / 4; i++) // һ�д���
			{
				pNY[i] = pOY[i * 4];
			}
			for (i = 0; i < w / 8; i++)
			{
				pNU[i] = 0;
				pNV[i] = 0;
			}
			pNY += w / 4;
			pNU += w / 8;
			pNV += w / 8;
			pOY += w * 4;
		}
	}

	// ����Ĳ������ڲ���ͼ������Ľ���Ƿ���ȷ��
	if (bLastPlugin)
	{
		// ����grayBmp_1d16:��grayBmp_1d16���Ƶ���ʾͼƬ�����Ͻ�
		CopyToRect(pImgProcBuf->grayBmp_1d16, pYBits, w / 4, h / 4, w, h, 0, 0, true);
		// ����colorBmp:��clrBmp_1d8���Ƶ���ʾͼƬ�����Ͻ�
		CopyToRect(pImgProcBuf->clrBmp_1d8, pYBits, w / 2, h / 4, w, h, w / 2, 0, false);
	}
}

DLL_EXP void ON_PLUGINEXIT()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); // ģ��״̬�л�
												 // theApp.dlg.DestroyWindow();
}
