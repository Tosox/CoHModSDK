#include "UIWidgetInterop.hpp"
#include "UIResolve.hpp"

#include "../../include/CoHModSDKUI.hpp"

#include <cstdint>

namespace UIWidgetInterop {
    void* ScreenManager_GetInstance() {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnSMGetInstance == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnSMGetInstance();
    }

    void* ScreenManager_FindScreen(void* screenManager, const char* name) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnSMFindScreen == nullptr) || (screenManager == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnSMFindScreen(screenManager, name);
    }

    void* Screen_GetRootWidget(void* screen) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnScreenGetRootWidget == nullptr) || (screen == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnScreenGetRootWidget(screen);
    }

    void Widget_SetPosition(void* widget, float x, float y) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetPosition == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetPosition(widget, x, y);
    }

    void Widget_SetSize(void* widget, float w, float h) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetSize == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetSize(widget, w, h);
    }

    void Widget_SetName(void* widget, const char* name) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetName == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetName(widget, name);
    }

    void Widget_SetParent(void* widget, void* parent) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetParent == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetParent(widget, parent);
    }

    void* Widget_GetPresentation(void* widget) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetGetPresentation == nullptr) || (widget == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnWidgetGetPresentation(widget);
    }

    void Widget_SetPresentation(void* widget, void* presentation) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetPresentation == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetPresentation(widget, presentation);
    }

    void* Widget_GetHitArea(void* widget) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetGetHitArea == nullptr) || (widget == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnWidgetGetHitArea(widget);
    }

    void Widget_SetHitArea(void* widget, void* hitArea) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetSetHitArea == nullptr) || (widget == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetSetHitArea(widget, hitArea);
    }

    void WidgetProxy_Bind(void* proxy, void* widget) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetProxyBind == nullptr) || (proxy == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetProxyBind(proxy, widget);
    }

    void WidgetProxy_SetVisible(void* proxy, bool visible) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetProxySetVisible == nullptr) || (proxy == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetProxySetVisible(proxy, visible);
    }

    void WidgetProxy_SetEnabled(void* proxy, bool enabled) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetProxySetEnabled == nullptr) || (proxy == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetProxySetEnabled(proxy, enabled);
    }

    void WidgetProxy_SetTextHAlign(void* proxy, int hAlign, const char* fontTag) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnWidgetProxySetTextHAlign == nullptr) || (proxy == nullptr)) {
            return;
        }
        UIResolve::gFnWidgetProxySetTextHAlign(proxy, hAlign, fontTag);
    }

    // WidgetFactory::Create passes the type name in EDI rather than on the stack.
    // Uses the same non-naked inline ASM pattern as the original mods.
    // The callee cleans up the flag argument (callee-clean convention for that push).
    void* WidgetFactory_Create(const char* typeName, int flag) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gRawWidgetFactoryCreate == nullptr) || (typeName == nullptr)) {
            return nullptr;
        }

        void* widget = nullptr;
        void* createFn = UIResolve::gRawWidgetFactoryCreate;
        __asm {
            mov edi, typeName
            mov eax, createFn
            push flag
            call eax
            mov widget, eax
        }
        return widget;
    }

    void* FindWidgetExtension(void* widget, int extensionId) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnFindWidgetExtension == nullptr) || (widget == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnFindWidgetExtension(widget, extensionId);
    }

    void* FindWidgetByName(void* root, const char* name, int index) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnFindWidgetByName == nullptr) || (root == nullptr)) {
            return nullptr;
        }
        return UIResolve::gFnFindWidgetByName(root, name, index);
    }

    // AddRenderChild passes the parent render object in EDI rather than on the stack.
    // Push order matches the original mods: insertIndex first, childWidget second.
    // The callee cleans up both stack arguments.
    bool AddRenderChild(void* parentRenderObject, void* childWidget, int insertIndex) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gRawAddRenderChild == nullptr) ||
            (parentRenderObject == nullptr) || (childWidget == nullptr)) {
            return false;
        }

        void* addFn = UIResolve::gRawAddRenderChild;
        __asm {
            mov edi, parentRenderObject
            mov eax, addFn
            push insertIndex
            push childWidget
            call eax
        }
        return true;
    }

    void ConfigureWidget(void* widget, void* parent, const char* name,
                         float x, float y, float w, float h) {
        if (widget == nullptr) {
            return;
        }
        if (parent != nullptr) {
            Widget_SetParent(widget, parent);
        }
        if ((name != nullptr) && (name[0] != '\0')) {
            Widget_SetName(widget, name);
        }
        Widget_SetPosition(widget, x, y);
        Widget_SetSize(widget, w, h);
    }

    bool AttachRenderChild(void* parent, void* child) {
        if ((parent == nullptr) || (child == nullptr)) {
            return false;
        }
        void* const renderObject = FindWidgetExtension(parent, COHMODSDK_UI_DRAW_CHILDREN_EXTENSION_ID);
        if (renderObject == nullptr) {
            return false;
        }
        return AddRenderChild(renderObject, child, -1);
    }

    bool DetachRenderChild(void* parent, void* child) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (parent == nullptr) || (child == nullptr) ||
            (UIResolve::gFnFindWidgetExtension == nullptr)) {
            return false;
        }

        void* const renderObject = UIResolve::gFnFindWidgetExtension(
            parent, COHMODSDK_UI_DRAW_CHILDREN_EXTENSION_ID);
        if (renderObject == nullptr) {
            return false;
        }

        // Direct memory access: render child list pointer is at offset 0x1C,
        // child count is at offset 0x20 (both verified from mod source).
        const auto renderAddress = reinterpret_cast<std::uintptr_t>(renderObject);
        void** children = *reinterpret_cast<void***>(renderAddress + 0x1Cu);
        unsigned int& count = *reinterpret_cast<unsigned int*>(renderAddress + 0x20u);

        if ((children == nullptr) || (count == 0u)) {
            return false;
        }

        for (unsigned int i = 0u; i < count; ++i) {
            if (children[i] != child) {
                continue;
            }
            for (unsigned int j = i + 1u; j < count; ++j) {
                children[j - 1u] = children[j];
            }
            children[count - 1u] = nullptr;
            --count;
            return true;
        }

        return false;
    }

    void CopyPresentation(void* dstWidget, void* srcWidget) {
        if ((dstWidget == nullptr) || (srcWidget == nullptr)) {
            return;
        }
        void* const presentation = Widget_GetPresentation(srcWidget);
        if (presentation == nullptr) {
            return;
        }
        Widget_SetPresentation(dstWidget, presentation);
    }
}
