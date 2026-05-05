#include "UIResolve.hpp"
#include "../runtime/RuntimeState.hpp"

#include <mutex>

namespace UIResolve {
    FnScreenManagerGetInstance   gFnSMGetInstance      = nullptr;
    FnScreenManagerDraw          gFnSMDraw             = nullptr;
    FnScreenManagerUpdate        gFnSMUpdate           = nullptr;
    FnScreenManagerDeactivateAll gFnSMDeactivateAll    = nullptr;
    FnScreenManagerUnloadScreen  gFnSMUnloadScreen     = nullptr;
    FnScreenManagerFindScreen    gFnSMFindScreen       = nullptr;
    FnScreenGetRootWidget        gFnScreenGetRootWidget = nullptr;

    FnWidgetSetPosition    gFnWidgetSetPosition    = nullptr;
    FnWidgetSetSize        gFnWidgetSetSize        = nullptr;
    FnWidgetSetName        gFnWidgetSetName        = nullptr;
    FnWidgetSetParent      gFnWidgetSetParent      = nullptr;
    FnWidgetGetPresentation gFnWidgetGetPresentation = nullptr;
    FnWidgetSetPresentation gFnWidgetSetPresentation = nullptr;
    FnWidgetGetHitArea     gFnWidgetGetHitArea     = nullptr;
    FnWidgetSetHitArea     gFnWidgetSetHitArea     = nullptr;

    FnWidgetProxyBind          gFnWidgetProxyBind          = nullptr;
    FnWidgetProxySetVisible    gFnWidgetProxySetVisible    = nullptr;
    FnWidgetProxySetEnabled    gFnWidgetProxySetEnabled    = nullptr;
    FnWidgetProxySetTextHAlign gFnWidgetProxySetTextHAlign = nullptr;

    FnTextLabelCtor        gFnTextLabelCtor        = nullptr;
    FnTextLabelDtor        gFnTextLabelDtor        = nullptr;
    FnTextLabelSetText     gFnTextLabelSetText     = nullptr;
    FnTextLabelSetAutoSize gFnTextLabelSetAutoSize = nullptr;
    FnTextLabelSetMultiline gFnTextLabelSetMultiline = nullptr;

    FnButtonCtor    gFnButtonCtor    = nullptr;
    FnButtonDtor    gFnButtonDtor    = nullptr;
    FnButtonSetText gFnButtonSetText = nullptr;

    FnCheckButtonCtor       gFnCheckButtonCtor       = nullptr;
    FnCheckButtonDtor       gFnCheckButtonDtor       = nullptr;
    FnCheckButtonSetChecked gFnCheckButtonSetChecked = nullptr;
    FnCheckButtonGetChecked gFnCheckButtonGetChecked = nullptr;

    FnArtLabelCtor            gFnArtLabelCtor            = nullptr;
    FnArtLabelDtor            gFnArtLabelDtor            = nullptr;
    FnArtLabelSetAllArtVisible gFnArtLabelSetAllArtVisible = nullptr;

    FnGenericWidgetCtor gFnGenericWidgetCtor = nullptr;
    FnGenericWidgetDtor gFnGenericWidgetDtor = nullptr;

    FnLocStringCtor gFnLocStringCtor = nullptr;
    FnLocStringDtor gFnLocStringDtor = nullptr;

    FnFindWidgetExtension gFnFindWidgetExtension = nullptr;
    FnFindWidgetByName    gFnFindWidgetByName    = nullptr;
    void*                 gRawWidgetFactoryCreate = nullptr;
    void*                 gRawAddRenderChild      = nullptr;

    bool gInitOk = false;

    namespace {
        std::once_flag gInitOnce;

        template <typename T>
        void Resolve(HMODULE module, const char* name, T& out) {
            out = reinterpret_cast<T>(GetProcAddress(module, name));
        }

        void ResolveAll() {
            // LoadLibraryA on an already-loaded DLL increments refcount and returns the
            // existing handle. FreeLibrary is intentionally omitted since game DLLs must
            // stay loaded for the entire process lifetime.
            HMODULE hUI  = LoadLibraryA("UserInterface.dll");
            HMODULE hLoc = LoadLibraryA("Localizer.dll");

            if ((hUI == nullptr) || (hLoc == nullptr)) {
                return;
            }

            Resolve(hUI, "?i@ScreenManager@UI@@SGPAV12@XZ",               gFnSMGetInstance);
            Resolve(hUI, "?Draw@ScreenManager@UI@@QAEXM@Z",               gFnSMDraw);
            Resolve(hUI, "?Update@ScreenManager@UI@@QAEXM@Z",             gFnSMUpdate);
            Resolve(hUI, "?DeactivateAllScreens@ScreenManager@UI@@QAEXXZ", gFnSMDeactivateAll);
            Resolve(hUI, "?UnloadScreen@ScreenManager@UI@@QAEXPAVScreen@2@@Z", gFnSMUnloadScreen);
            Resolve(hUI, "?FindScreen@ScreenManager@UI@@QAEPAVScreen@2@PBD@Z", gFnSMFindScreen);
            Resolve(hUI, "?GetRootWidget@Screen@UI@@QAEPAVWidget@2@XZ",    gFnScreenGetRootWidget);

            Resolve(hUI, "?SetPosition@Widget@UI@@QAEXMM@Z",              gFnWidgetSetPosition);
            Resolve(hUI, "?SetSize@Widget@UI@@QAEXMM@Z",                  gFnWidgetSetSize);
            Resolve(hUI, "?SetName@Widget@UI@@QAEXPBD@Z",                 gFnWidgetSetName);
            Resolve(hUI, "?SetParent@Widget@UI@@QAEXPAV12@@Z",            gFnWidgetSetParent);
            Resolve(hUI, "?GetPresentation@Widget@UI@@QAEPAVPresentation@2@XZ", gFnWidgetGetPresentation);
            Resolve(hUI, "?SetPresentation@Widget@UI@@QAEXPAVPresentation@2@@Z", gFnWidgetSetPresentation);
            Resolve(hUI, "?GetHitArea@Widget@UI@@QAEPAVHitArea@2@XZ",     gFnWidgetGetHitArea);
            Resolve(hUI, "?SetHitArea@Widget@UI@@QAEXPAVHitArea@2@@Z",    gFnWidgetSetHitArea);

            Resolve(hUI, "?Bind@WidgetProxy@UI@@IAEXPAVWidget@2@@Z",                      gFnWidgetProxyBind);
            Resolve(hUI, "?SetVisible@WidgetProxy@UI@@UAEX_N@Z",                          gFnWidgetProxySetVisible);
            Resolve(hUI, "?SetEnabled@WidgetProxy@UI@@UAEX_N@Z",                          gFnWidgetProxySetEnabled);
            Resolve(hUI, "?SetTextHAlign@WidgetProxy@UI@@QAEXW4HAlign@RenderProxy@@PBD@Z", gFnWidgetProxySetTextHAlign);

            Resolve(hUI, "??0TextLabel@UI@@QAE@XZ",                       gFnTextLabelCtor);
            Resolve(hUI, "??1TextLabel@UI@@UAE@XZ",                       gFnTextLabelDtor);
            Resolve(hUI, "?SetText@TextLabel@UI@@QAEXABVLocString@@@Z",   gFnTextLabelSetText);
            Resolve(hUI, "?SetAutoSize@TextLabel@UI@@QAEX_N@Z",           gFnTextLabelSetAutoSize);
            Resolve(hUI, "?SetMultiline@TextLabel@UI@@QAEX_N@Z",          gFnTextLabelSetMultiline);

            Resolve(hUI, "??0Button@UI@@QAE@XZ",                          gFnButtonCtor);
            Resolve(hUI, "??1Button@UI@@UAE@XZ",                          gFnButtonDtor);
            Resolve(hUI, "?SetText@Button@UI@@QAEXABVLocString@@@Z",      gFnButtonSetText);

            Resolve(hUI, "??0CheckButton@UI@@QAE@XZ",                     gFnCheckButtonCtor);
            Resolve(hUI, "??1CheckButton@UI@@UAE@XZ",                     gFnCheckButtonDtor);
            Resolve(hUI, "?SetChecked@CheckButton@UI@@QAEX_N@Z",          gFnCheckButtonSetChecked);
            Resolve(hUI, "?GetChecked@CheckButton@UI@@QBE_NXZ",           gFnCheckButtonGetChecked);

            Resolve(hUI, "??0ArtLabel@UI@@QAE@XZ",                        gFnArtLabelCtor);
            Resolve(hUI, "??1ArtLabel@UI@@UAE@XZ",                        gFnArtLabelDtor);
            Resolve(hUI, "?SetAllArtVisible@ArtLabel@UI@@QAEX_N@Z",       gFnArtLabelSetAllArtVisible);

            Resolve(hUI, "??0GenericWidget@UI@@QAE@XZ",                   gFnGenericWidgetCtor);
            Resolve(hUI, "??1GenericWidget@UI@@UAE@XZ",                   gFnGenericWidgetDtor);

            Resolve(hLoc, "??0LocString@@QAE@PB_W@Z",                     gFnLocStringCtor);
            Resolve(hLoc, "??1LocString@@QAE@XZ",                         gFnLocStringDtor);

            // Pattern-scanned functions: not exported by name.
            if (const auto addr = Runtime::FindPattern("UserInterface.dll",
                    "8B 81 B0 00 00 00 8B 91 AC 00 00 00 56 8D 04 40 8D 34 C2 57 3B D6 74 ??")) {
                gFnFindWidgetExtension = reinterpret_cast<FnFindWidgetExtension>(*addr);
            }
            if (const auto addr = Runtime::FindPattern("UserInterface.dll",
                    "64 A1 00 00 00 00 6A FF 68 ?? ?? ?? ?? 50 64 89 25 00 00 00 00 83 EC 54 80 7C 24 6C 00")) {
                gFnFindWidgetByName = reinterpret_cast<FnFindWidgetByName>(*addr);
            }
            if (const auto addr = Runtime::FindPattern("UserInterface.dll",
                    "56 BE ?? ?? ?? ?? 8B 06 FF D0 8B CF 8D 64 24 00 8A 11 3A 10")) {
                gRawWidgetFactoryCreate = reinterpret_cast<void*>(*addr);
            }
            if (const auto addr = Runtime::FindPattern("UserInterface.dll",
                    "55 8B EC 83 E4 F8 83 EC 10 83 7F 3C 00 53 8B 5D 0C 56 8B 77 04 7E ??")) {
                gRawAddRenderChild = reinterpret_cast<void*>(*addr);
            }

            // Init is considered successful as long as the core widget and screen functions resolved.
            gInitOk = (gFnSMDeactivateAll != nullptr) &&
                      (gFnSMUnloadScreen  != nullptr) &&
                      (gFnWidgetSetPosition != nullptr) &&
                      (gFnWidgetSetSize    != nullptr) &&
                      (gFnWidgetSetParent  != nullptr) &&
                      (gFnWidgetProxyBind  != nullptr) &&
                      (gFnTextLabelCtor    != nullptr) &&
                      (gFnLocStringCtor    != nullptr) &&
                      (gRawWidgetFactoryCreate != nullptr) &&
                      (gRawAddRenderChild      != nullptr);
        }
    }

    void EnsureInit() {
        std::call_once(gInitOnce, ResolveAll);
    }
}
