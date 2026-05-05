#pragma once

#include <Windows.h>

namespace UIResolve {
    // Function pointer types — all game UI functions use __thiscall (first arg = this in ECX),
    // except ScreenManager::i() (__stdcall static) and FindWidgetByName (__stdcall free function).
    using FnScreenManagerGetInstance = void*(__stdcall*)();
    using FnScreenManagerDraw        = void(__thiscall*)(void*, float);
    using FnScreenManagerUpdate      = void(__thiscall*)(void*, float);
    using FnScreenManagerDeactivateAll = void(__thiscall*)(void*);
    using FnScreenManagerUnloadScreen  = void(__thiscall*)(void*, void*);
    using FnScreenManagerFindScreen    = void*(__thiscall*)(void*, const char*);
    using FnScreenGetRootWidget        = void*(__thiscall*)(void*);

    using FnWidgetSetPosition    = void(__thiscall*)(void*, float, float);
    using FnWidgetSetSize        = void(__thiscall*)(void*, float, float);
    using FnWidgetSetName        = void(__thiscall*)(void*, const char*);
    using FnWidgetSetParent      = void(__thiscall*)(void*, void*);
    using FnWidgetGetPresentation = void*(__thiscall*)(void*);
    using FnWidgetSetPresentation = void(__thiscall*)(void*, void*);
    using FnWidgetGetHitArea     = void*(__thiscall*)(void*);
    using FnWidgetSetHitArea     = void(__thiscall*)(void*, void*);

    using FnWidgetProxyBind           = void(__thiscall*)(void*, void*);
    using FnWidgetProxySetVisible     = void(__thiscall*)(void*, bool);
    using FnWidgetProxySetEnabled     = void(__thiscall*)(void*, bool);
    using FnWidgetProxySetTextHAlign  = void(__thiscall*)(void*, int, const char*);

    using FnTextLabelCtor        = void(__thiscall*)(void*);
    using FnTextLabelDtor        = void(__thiscall*)(void*);
    using FnTextLabelSetText     = void(__thiscall*)(void*, const void*);
    using FnTextLabelSetAutoSize = void(__thiscall*)(void*, bool);
    using FnTextLabelSetMultiline = void(__thiscall*)(void*, bool);

    using FnButtonCtor    = void(__thiscall*)(void*);
    using FnButtonDtor    = void(__thiscall*)(void*);
    using FnButtonSetText = void(__thiscall*)(void*, const void*);

    using FnCheckButtonCtor       = void(__thiscall*)(void*);
    using FnCheckButtonDtor       = void(__thiscall*)(void*);
    using FnCheckButtonSetChecked = void(__thiscall*)(void*, bool);
    using FnCheckButtonGetChecked = bool(__thiscall*)(void*);

    using FnArtLabelCtor            = void(__thiscall*)(void*);
    using FnArtLabelDtor            = void(__thiscall*)(void*);
    using FnArtLabelSetAllArtVisible = void(__thiscall*)(void*, bool);

    using FnGenericWidgetCtor = void(__thiscall*)(void*);
    using FnGenericWidgetDtor = void(__thiscall*)(void*);

    using FnLocStringCtor = void(__thiscall*)(void*, const wchar_t*);
    using FnLocStringDtor = void(__thiscall*)(void*);

    // RVA-resolved: non-standard calling conventions, called via naked ASM stubs.
    // Raw address stored as void* — do not call directly.
    using FnFindWidgetExtension = void*(__thiscall*)(void*, int);
    using FnFindWidgetByName    = void*(__stdcall*)(void*, const char*, int);
    // WidgetFactory::Create uses EDI for the type name arg — stored as raw void*.
    // AddRenderChild uses EDI for the parent render object — stored as raw void*.

    extern FnScreenManagerGetInstance  gFnSMGetInstance;
    extern FnScreenManagerDraw         gFnSMDraw;
    extern FnScreenManagerUpdate       gFnSMUpdate;
    extern FnScreenManagerDeactivateAll gFnSMDeactivateAll;
    extern FnScreenManagerUnloadScreen gFnSMUnloadScreen;
    extern FnScreenManagerFindScreen   gFnSMFindScreen;
    extern FnScreenGetRootWidget       gFnScreenGetRootWidget;

    extern FnWidgetSetPosition    gFnWidgetSetPosition;
    extern FnWidgetSetSize        gFnWidgetSetSize;
    extern FnWidgetSetName        gFnWidgetSetName;
    extern FnWidgetSetParent      gFnWidgetSetParent;
    extern FnWidgetGetPresentation gFnWidgetGetPresentation;
    extern FnWidgetSetPresentation gFnWidgetSetPresentation;
    extern FnWidgetGetHitArea     gFnWidgetGetHitArea;
    extern FnWidgetSetHitArea     gFnWidgetSetHitArea;

    extern FnWidgetProxyBind          gFnWidgetProxyBind;
    extern FnWidgetProxySetVisible    gFnWidgetProxySetVisible;
    extern FnWidgetProxySetEnabled    gFnWidgetProxySetEnabled;
    extern FnWidgetProxySetTextHAlign gFnWidgetProxySetTextHAlign;

    extern FnTextLabelCtor        gFnTextLabelCtor;
    extern FnTextLabelDtor        gFnTextLabelDtor;
    extern FnTextLabelSetText     gFnTextLabelSetText;
    extern FnTextLabelSetAutoSize gFnTextLabelSetAutoSize;
    extern FnTextLabelSetMultiline gFnTextLabelSetMultiline;

    extern FnButtonCtor    gFnButtonCtor;
    extern FnButtonDtor    gFnButtonDtor;
    extern FnButtonSetText gFnButtonSetText;

    extern FnCheckButtonCtor       gFnCheckButtonCtor;
    extern FnCheckButtonDtor       gFnCheckButtonDtor;
    extern FnCheckButtonSetChecked gFnCheckButtonSetChecked;
    extern FnCheckButtonGetChecked gFnCheckButtonGetChecked;

    extern FnArtLabelCtor            gFnArtLabelCtor;
    extern FnArtLabelDtor            gFnArtLabelDtor;
    extern FnArtLabelSetAllArtVisible gFnArtLabelSetAllArtVisible;

    extern FnGenericWidgetCtor gFnGenericWidgetCtor;
    extern FnGenericWidgetDtor gFnGenericWidgetDtor;

    extern FnLocStringCtor gFnLocStringCtor;
    extern FnLocStringDtor gFnLocStringDtor;

    extern FnFindWidgetExtension gFnFindWidgetExtension;
    extern FnFindWidgetByName    gFnFindWidgetByName;
    extern void*                 gRawWidgetFactoryCreate;
    extern void*                 gRawAddRenderChild;

    extern bool gInitOk;

    void EnsureInit();
}
