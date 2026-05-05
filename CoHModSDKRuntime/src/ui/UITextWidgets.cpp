#include "UITextWidgets.hpp"
#include "UIResolve.hpp"

namespace UITextWidgets {
    void TextLabel_Construct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnTextLabelCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnTextLabelCtor(storage);
    }

    void TextLabel_Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnTextLabelDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnTextLabelDtor(storage);
    }

    void TextLabel_SetText(void* textLabel, const void* locString) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnTextLabelSetText == nullptr) ||
            (textLabel == nullptr) || (locString == nullptr)) {
            return;
        }
        UIResolve::gFnTextLabelSetText(textLabel, locString);
    }

    void TextLabel_SetAutoSize(void* textLabel, bool autoSize) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnTextLabelSetAutoSize == nullptr) || (textLabel == nullptr)) {
            return;
        }
        UIResolve::gFnTextLabelSetAutoSize(textLabel, autoSize);
    }

    void TextLabel_SetMultiline(void* textLabel, bool multiline) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnTextLabelSetMultiline == nullptr) || (textLabel == nullptr)) {
            return;
        }
        UIResolve::gFnTextLabelSetMultiline(textLabel, multiline);
    }

    void Button_Construct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnButtonCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnButtonCtor(storage);
    }

    void Button_Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnButtonDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnButtonDtor(storage);
    }

    void Button_SetText(void* button, const void* locString) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnButtonSetText == nullptr) ||
            (button == nullptr) || (locString == nullptr)) {
            return;
        }
        UIResolve::gFnButtonSetText(button, locString);
    }

    void CheckButton_Construct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnCheckButtonCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnCheckButtonCtor(storage);
    }

    void CheckButton_Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnCheckButtonDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnCheckButtonDtor(storage);
    }

    void CheckButton_SetChecked(void* checkButton, bool checked) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnCheckButtonSetChecked == nullptr) || (checkButton == nullptr)) {
            return;
        }
        UIResolve::gFnCheckButtonSetChecked(checkButton, checked);
    }

    bool CheckButton_GetChecked(void* checkButton) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnCheckButtonGetChecked == nullptr) || (checkButton == nullptr)) {
            return false;
        }
        return UIResolve::gFnCheckButtonGetChecked(checkButton);
    }

    void ArtLabel_Construct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnArtLabelCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnArtLabelCtor(storage);
    }

    void ArtLabel_Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnArtLabelDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnArtLabelDtor(storage);
    }

    void ArtLabel_SetAllArtVisible(void* artLabel, bool visible) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnArtLabelSetAllArtVisible == nullptr) || (artLabel == nullptr)) {
            return;
        }
        UIResolve::gFnArtLabelSetAllArtVisible(artLabel, visible);
    }

    void GenericWidget_Construct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnGenericWidgetCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnGenericWidgetCtor(storage);
    }

    void GenericWidget_Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnGenericWidgetDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnGenericWidgetDtor(storage);
    }
}
