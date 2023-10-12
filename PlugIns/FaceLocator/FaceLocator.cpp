// FaceLocator.cpp : Defines the initialization routines for the DLL.
#include "stdafx.h"
#include "BufStruct.h"
#include "ImageProc.h"
#include "FaceLocator.h"

#ifdef _DEBUG
#    define new DEBUG_NEW
#    undef THIS_FILE
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
CFaceLocatorApp::CFaceLocatorApp() {
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CFaceLocatorApp object
CFaceLocatorApp theApp;
char            sInfo[]     = "人脸跟踪-基于彩色信息的人脸分割处理插件";
bool            bLastPlugin = false;
DLL_EXP void    ON_PLUGIN_BELAST(bool bLast) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());  // 模块状态切换
    bLastPlugin = bLast;
}
DLL_EXP LPCTSTR ON_PLUGININFO(void) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());  // 模块状态切换
    return sInfo;
}
DLL_EXP void ON_INITPLUGIN(LPVOID lpParameter) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());  // 模块状态切换
    // theApp.dlg.Create(IDD_PLUGIN_SETUP);
    // theApp.dlg.ShowWindow(SW_HIDE);
}
DLL_EXP int ON_PLUGINCTRL(int nMode, void* pParameter) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int nRet = 0;
    switch (nMode) {
        case 0: {
            // theApp.dlg.ShowWindow(SW_SHOWNORMAL);
            // theApp.dlg.SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
        } break;
    }
    return nRet;
}
/****************************************************************************************/
/*                             人脸检测与定位                                           */
/****************************************************************************************/

int getMin(int a, int b) {
    if (a > b) {
        return b;
    }
    return a;
}

int getMax(int a, int b) {
    if (a < b) {
        return b;
    }
    return a;
}

void locMaxMin(int si, int sj, int w, int h, int n, int* max, int* min, BYTE* bmp) {
    *min = 255;
    *max = 0;
    for (int j = -n / 2; j <= n / 2; j++) {
        int t = sj + j;
        if (t < 0 || t >= h) {
            continue;
        }
        for (int i = -n / 2; i <= n / 2; i++) {
            int s = si + i;
            if (s < 0 || s >= w) {
                continue;
            }
            *max = getMax(bmp[t * w + s], *max);
            *min = getMin(bmp[t * w + s], *min);
        }
    }
}

void dilate(int w, int h, int n, BYTE* bmp, BYTE* nbmp) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int max, min;
            locMaxMin(i, j, w, h, n, &max, &min, bmp);
            nbmp[j * w + i] = max;
        }
    }
}

void erode(int w, int h, int n, BYTE* bmp, BYTE* nbmp) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int max, min;
            locMaxMin(i, j, w, h, n, &max, &min, bmp);
            nbmp[j * w + i] = min;
        }
    }
}

void openOp(int w, int h, int n, BYTE* bmp, BYTE* nbmp) {
    erode(w, h, n, bmp, nbmp);
    dilate(w, h, n, nbmp, bmp);
}

void closeOp(int w, int h, int n, BYTE* bmp, BYTE* nbmp) {
    dilate(w, h, n, bmp, nbmp);
    erode(w, h, n, nbmp, bmp);
}

DLL_EXP void ON_PLUGINRUN(int w, int h, BYTE* pYBits, BYTE* pUBits, BYTE* pVBits, BYTE* pBuffer) {
    // pYBits 大小为w*h
    // pUBits 和 pVBits 的大小为 w*h/2
    // pBuffer 的大小为 w*h*4
    // 下面算法都基于一个假设，即w是16的倍数

    AFX_MANAGE_STATE(AfxGetStaticModuleState());  // 模块状态切换

    // 请编写相应处理程序
    long int wh = w * h;

    BUF_STRUCT* pImgProcBuf = (BUF_STRUCT*)pBuffer;

    // 2. 肤色建模
    {
        BYTE* pB = pImgProcBuf->TempImage1d8;

        BYTE* pOY = pImgProcBuf->clrBmp_1d8;
        BYTE* pOU = pOY + wh / 8;
        BYTE* pOV = pOU + wh / 16;

        for (int j = 0; j < h / 4; j++) {
            int i;
            for (i = 0; i < w / 4; i++) {
                pB[i] = pOU[i] >= 85 && pOU[i] <= 126 && pOV[i] >= 130 && pOV[i] <= 165 ? 255 : 0;
            }
            pB += w / 4;
            pOU += w / 4;
            pOV += w / 4;
        }
    }
    // 3. 形态学处理
    {
        aBYTE* tempImg;
        tempImg = myHeapAlloc(w * h / 16);
        openOp(w / 4, h / 4, 3, pImgProcBuf->TempImage1d8, tempImg);
        closeOp(w / 4, h / 4, 7, pImgProcBuf->TempImage1d8, tempImg);
        myHeapFree(tempImg);
    }

    // 4. 连通域标记
    {
        // 4.1 连通域临时标号
        BYTE* pT = pImgProcBuf->TempImage1d8;

        aBYTE* tempImg;
        tempImg = myHeapAlloc(w * h / 16);
        // {
        //     for (int j = 0; j < w / 4; j++) {
        //         for (int i = 0; i < h / 4; i++) {
        //             tempImg[j * w / 4 + i] = 0;
        //         }
        //     }
        // }

        int LK[1024];
        int LK1[1024];
        {
            for (int i = 0; i < 1024; i++) {
                LK[i] = i;
            }
        }

        {
            int num = 1;
            for (int j = 0; j < h / 4; j++) {
                for (int i = 0; i < w / 4; i++) {
                    if (pT[j * w / 4 + i] != 255) {
                        tempImg[j * w / 4 + i] = 0;
                        continue;
                    }
                    bool l = false, r = false;
                    if (i >= 1 && tempImg[j * w / 4 + i - 1] != 0) {
                        l = true;
                    }
                    if (j >= 1 && tempImg[(j - 1) * w / 4 + i] != 0) {
                        r = true;
                    }

                    if (!l && !r) {
                        tempImg[j * w / 4 + i] = num++;
                    } else if (l && !r) {
                        tempImg[j * w / 4 + i] = tempImg[j * w / 4 + i - 1];
                    } else if (!l && r) {
                        tempImg[j * w / 4 + i] = tempImg[(j - 1) * w / 4 + i];
                    } else {
                        tempImg[j * w / 4 + i] = tempImg[j * w / 4 + i - 1];

                        if (tempImg[j * w / 4 + i - 1] != tempImg[(j - 1) * w / 4 + i]) {
                            LK[getMax(tempImg[j * w / 4 + i - 1], tempImg[(j - 1) * w / 4 + i])] = getMin(tempImg[j * w / 4 + i - 1], tempImg[(j - 1) * w / 4 + i]);
                        }
                    }
                }
            }
        }

        // 4.2 整理等价表
        {
            for (int i = 1; i < 1024; i++) {
                LK[i] = LK[LK[i]];
            }
        }

        // 4.3 连通域重新编号
        int cNum = 1;
        {
            LK1[0] = 0;
            for (int i = 1; i < 1024; i++) {
                if (LK[i] == i) {
                    LK1[i] = cNum++;
                } else {
                    LK1[i] = LK1[LK[i]];
                }
            }
        }

        // 4.4 图片代换
        {
            int maxNums[1024];
            for (int i = 0; i < 1024; i++) {
                maxNums[i] = 0;
            }
            int j;
            for (j = 0; j < h / 4; j++) {
                for (int i = 0; i < w / 4; i++) {
                    pT[j * w / 4 + i] = LK1[tempImg[j * w / 4 + i]];
                    maxNums[pT[j * w / 4 + i]]++;
                }
            }

            int maxNum = 0;
            int maxIdx = 0;
            for (int k = 1; k <= cNum; k++) {
                if (maxNum < maxNums[k]) {
                    maxIdx = k;
                    maxNum = maxNums[k];
                }
            }

            for (j = 0; j < h / 4; j++) {
                for (int i = 0; i < w / 4; i++) {
                    if (pT[j * w / 4 + i] == maxIdx) {
                        pT[j * w / 4 + i] = 255;
                    } else {
                        pT[j * w / 4 + i] = 0;
                    }
                }
            }
        }
        myHeapFree(tempImg);
    }

    // 6. 替换
    {
        BYTE* pB = pImgProcBuf->TempImage1d8;

        BYTE* pNY = pImgProcBuf->clrBmp_1d8;

        for (int j = 0; j < h / 4; j++) {
            int i;
            for (i = 0; i < w / 4; i++) {
                pNY[2 * i]     = pB[i];
                pNY[2 * i + 1] = pB[i];
            }
            pB += w / 4;
            pNY += w / 2;
        }
    }

    // ?????????????????????????????????
    if (bLastPlugin) {
        // ????grayBmp_1d16:??grayBmp_1d16?????????????????
        // CopyToRect(pImgProcBuf->grayBmp_1d16, pYBits, w / 4, h / 4, w, h, 0, 0, true);
        // ????colorBmp:??clrBmp_1d8?????????????????
        CopyToRect(pImgProcBuf->clrBmp_1d8, pYBits, w / 2, h / 4, w, h, w / 2, 0, false);
    }
}

DLL_EXP void ON_PLUGINEXIT() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());  // 模块状态切换
    // theApp.dlg.DestroyWindow();
}
