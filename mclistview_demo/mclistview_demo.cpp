﻿// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "StudentSmsDlg.h"

//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 1
//#define RES_TYPE 0   //从文件中加载资源
// #define RES_TYPE 1  //从PE资源中加载UI资源

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif
    
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));

    int nRet = 0;
    
    SComMgr *pComMgr = new SComMgr;

    //将程序的运行路径修改到项目所在目录所在的目录
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
    LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
    _tcscpy(lpInsertPos + 1, _T("..\\mclistview_demo"));
    SetCurrentDirectory(szCurrentDir);
    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
        bLoaded = pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        SApplication *theApp = new SApplication(pRenderFactory, hInstance);
        theApp->RegisterWindowClass<SSearchDropdownList>();
        //从DLL加载系统资源
        HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
        if (hModSysResource)
        {
            CAutoRefPtr<IResProvider> sysResProvider;
            CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
            sysResProvider->Init((WPARAM)hModSysResource, 0);
            theApp->LoadSystemNamedResource(sysResProvider);
            FreeLibrary(hModSysResource);
        }else
        {
            SASSERT(0);
        }

        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
        if (!pResProvider->Init((LPARAM)_T("uires"), 0))
        {
            SASSERT(0);
            return 1;
        }
#else 
        CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance, 0);
#endif

        theApp->AddResProvider(pResProvider);
        theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);

        
        // BLOCK: Run application
        {
            CStudentSmsDlg dlgMain;
            nRet = dlgMain.DoModal();
        }

        delete theApp;
    }
    
    delete pComMgr;
    
    OleUninitialize();
    return nRet;
}
