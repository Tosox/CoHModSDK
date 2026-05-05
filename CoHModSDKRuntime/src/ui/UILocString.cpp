#include "UILocString.hpp"
#include "UIResolve.hpp"

namespace UILocString {
    void Construct(void* storage, const wchar_t* text) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnLocStringCtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnLocStringCtor(storage, text);
    }

    void Destruct(void* storage) {
        UIResolve::EnsureInit();
        if (!UIResolve::gInitOk || (UIResolve::gFnLocStringDtor == nullptr) || (storage == nullptr)) {
            return;
        }
        UIResolve::gFnLocStringDtor(storage);
    }
}
