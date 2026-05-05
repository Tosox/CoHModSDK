#pragma once

namespace UITextWidgets {
    void TextLabel_Construct(void* storage);
    void TextLabel_Destruct(void* storage);
    void TextLabel_SetText(void* textLabel, const void* locString);
    void TextLabel_SetAutoSize(void* textLabel, bool autoSize);
    void TextLabel_SetMultiline(void* textLabel, bool multiline);

    void Button_Construct(void* storage);
    void Button_Destruct(void* storage);
    void Button_SetText(void* button, const void* locString);

    void CheckButton_Construct(void* storage);
    void CheckButton_Destruct(void* storage);
    void CheckButton_SetChecked(void* checkButton, bool checked);
    bool CheckButton_GetChecked(void* checkButton);

    void ArtLabel_Construct(void* storage);
    void ArtLabel_Destruct(void* storage);
    void ArtLabel_SetAllArtVisible(void* artLabel, bool visible);

    void GenericWidget_Construct(void* storage);
    void GenericWidget_Destruct(void* storage);
}
