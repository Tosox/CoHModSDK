#pragma once

namespace UIWidgetInterop {
    void* ScreenManager_GetInstance();
    void* ScreenManager_FindScreen(void* screenManager, const char* name);
    void* Screen_GetRootWidget(void* screen);

    void  Widget_SetPosition(void* widget, float x, float y);
    void  Widget_SetSize(void* widget, float w, float h);
    void  Widget_SetName(void* widget, const char* name);
    void  Widget_SetParent(void* widget, void* parent);
    void* Widget_GetPresentation(void* widget);
    void  Widget_SetPresentation(void* widget, void* presentation);
    void* Widget_GetHitArea(void* widget);
    void  Widget_SetHitArea(void* widget, void* hitArea);

    void WidgetProxy_Bind(void* proxy, void* widget);
    void WidgetProxy_SetVisible(void* proxy, bool visible);
    void WidgetProxy_SetEnabled(void* proxy, bool enabled);
    void WidgetProxy_SetTextHAlign(void* proxy, int hAlign, const char* fontTag);

    void* WidgetFactory_Create(const char* typeName, int flag);
    void* FindWidgetExtension(void* widget, int extensionId);
    void* FindWidgetByName(void* root, const char* name, int index);
    bool  AddRenderChild(void* parentRenderObject, void* childWidget, int insertIndex);

    void ConfigureWidget(void* widget, void* parent, const char* name,
                         float x, float y, float w, float h);
    bool AttachRenderChild(void* parent, void* child);
    bool DetachRenderChild(void* parent, void* child);
    void CopyPresentation(void* dstWidget, void* srcWidget);
}
