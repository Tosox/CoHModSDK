#include "UIScreenHooks.hpp"
#include "UIResolve.hpp"

#include "../runtime/RuntimeState.hpp"

#include <mutex>
#include <vector>

namespace {
    struct DrawCallbacks {
        CoHModSDKSMDrawPreFn  pre  = nullptr;
        CoHModSDKSMDrawPostFn post = nullptr;
    };

    struct UpdateCallbacks {
        CoHModSDKSMUpdatePreFn  pre  = nullptr;
        CoHModSDKSMUpdatePostFn post = nullptr;
    };

    struct DeactivateAllCallbacks {
        CoHModSDKSMDeactivateAllPreFn  pre  = nullptr;
        CoHModSDKSMDeactivateAllPostFn post = nullptr;
    };

    struct UnloadScreenCallbacks {
        CoHModSDKSMUnloadScreenPreFn  pre  = nullptr;
        CoHModSDKSMUnloadScreenPostFn post = nullptr;
    };

    std::mutex gMutex;
    std::vector<DrawCallbacks>         gDrawCallbacks;
    std::vector<UpdateCallbacks>       gUpdateCallbacks;
    std::vector<DeactivateAllCallbacks> gDeactivateAllCallbacks;
    std::vector<UnloadScreenCallbacks> gUnloadScreenCallbacks;

    UIResolve::FnScreenManagerDraw          oFnSMDraw          = nullptr;
    UIResolve::FnScreenManagerUpdate        oFnSMUpdate        = nullptr;
    UIResolve::FnScreenManagerDeactivateAll oFnSMDeactivateAll = nullptr;
    UIResolve::FnScreenManagerUnloadScreen  oFnSMUnloadScreen  = nullptr;

    std::once_flag gDrawHookOnce;
    std::once_flag gUpdateHookOnce;
    std::once_flag gDeactivateAllHookOnce;
    std::once_flag gUnloadScreenHookOnce;

    // Hooked implementations use __fastcall with a dummy EDX to intercept __thiscall.
    // This is the standard MSVC technique for hooking __thiscall methods.

    void __fastcall HookedSMDraw(void* sm, void* /*edx*/, float dt) {
        {
            std::scoped_lock lock(gMutex);
            for (const DrawCallbacks& cb : gDrawCallbacks) {
                if ((cb.pre != nullptr) && !cb.pre(sm, &dt)) {
                    return;
                }
            }
        }

        if (oFnSMDraw != nullptr) {
            oFnSMDraw(sm, dt);
        }

        {
            std::scoped_lock lock(gMutex);
            for (const DrawCallbacks& cb : gDrawCallbacks) {
                if (cb.post != nullptr) {
                    cb.post(sm, dt);
                }
            }
        }
    }

    void __fastcall HookedSMUpdate(void* sm, void* /*edx*/, float dt) {
        {
            std::scoped_lock lock(gMutex);
            for (const UpdateCallbacks& cb : gUpdateCallbacks) {
                if ((cb.pre != nullptr) && !cb.pre(sm, &dt)) {
                    return;
                }
            }
        }

        if (oFnSMUpdate != nullptr) {
            oFnSMUpdate(sm, dt);
        }

        {
            std::scoped_lock lock(gMutex);
            for (const UpdateCallbacks& cb : gUpdateCallbacks) {
                if (cb.post != nullptr) {
                    cb.post(sm, dt);
                }
            }
        }
    }

    void __fastcall HookedSMDeactivateAll(void* sm, void* /*edx*/) {
        {
            std::scoped_lock lock(gMutex);
            for (const DeactivateAllCallbacks& cb : gDeactivateAllCallbacks) {
                if ((cb.pre != nullptr) && !cb.pre(sm)) {
                    return;
                }
            }
        }

        if (oFnSMDeactivateAll != nullptr) {
            oFnSMDeactivateAll(sm);
        }

        {
            std::scoped_lock lock(gMutex);
            for (const DeactivateAllCallbacks& cb : gDeactivateAllCallbacks) {
                if (cb.post != nullptr) {
                    cb.post(sm);
                }
            }
        }
    }

    void __fastcall HookedSMUnloadScreen(void* sm, void* /*edx*/, void* screen) {
        {
            std::scoped_lock lock(gMutex);
            for (const UnloadScreenCallbacks& cb : gUnloadScreenCallbacks) {
                if ((cb.pre != nullptr) && !cb.pre(sm, screen)) {
                    return;
                }
            }
        }

        if (oFnSMUnloadScreen != nullptr) {
            oFnSMUnloadScreen(sm, screen);
        }

        {
            std::scoped_lock lock(gMutex);
            for (const UnloadScreenCallbacks& cb : gUnloadScreenCallbacks) {
                if (cb.post != nullptr) {
                    cb.post(sm, screen);
                }
            }
        }
    }

    void InstallDrawHook() {
        UIResolve::EnsureInit();
        if (UIResolve::gFnSMDraw == nullptr) {
            return;
        }
        Runtime::GetState().hookEngine.CreateHook(
            reinterpret_cast<void*>(UIResolve::gFnSMDraw),
            reinterpret_cast<void*>(&HookedSMDraw),
            reinterpret_cast<void**>(&oFnSMDraw));
    }

    void InstallUpdateHook() {
        UIResolve::EnsureInit();
        if (UIResolve::gFnSMUpdate == nullptr) {
            return;
        }
        Runtime::GetState().hookEngine.CreateHook(
            reinterpret_cast<void*>(UIResolve::gFnSMUpdate),
            reinterpret_cast<void*>(&HookedSMUpdate),
            reinterpret_cast<void**>(&oFnSMUpdate));
    }

    void InstallDeactivateAllHook() {
        UIResolve::EnsureInit();
        if (UIResolve::gFnSMDeactivateAll == nullptr) {
            return;
        }
        Runtime::GetState().hookEngine.CreateHook(
            reinterpret_cast<void*>(UIResolve::gFnSMDeactivateAll),
            reinterpret_cast<void*>(&HookedSMDeactivateAll),
            reinterpret_cast<void**>(&oFnSMDeactivateAll));
    }

    void InstallUnloadScreenHook() {
        UIResolve::EnsureInit();
        if (UIResolve::gFnSMUnloadScreen == nullptr) {
            return;
        }
        Runtime::GetState().hookEngine.CreateHook(
            reinterpret_cast<void*>(UIResolve::gFnSMUnloadScreen),
            reinterpret_cast<void*>(&HookedSMUnloadScreen),
            reinterpret_cast<void**>(&oFnSMUnloadScreen));
    }
}

namespace UIScreenHooks {
    void Shutdown() {
        std::scoped_lock lock(gMutex);
        gDrawCallbacks.clear();
        gUpdateCallbacks.clear();
        gDeactivateAllCallbacks.clear();
        gUnloadScreenCallbacks.clear();
    }

    bool RegisterDraw(CoHModSDKSMDrawPreFn pre, CoHModSDKSMDrawPostFn post) {
        if ((pre == nullptr) && (post == nullptr)) {
            return false;
        }
        std::call_once(gDrawHookOnce, InstallDrawHook);
        std::scoped_lock lock(gMutex);
        gDrawCallbacks.push_back({pre, post});
        return true;
    }

    bool RegisterUpdate(CoHModSDKSMUpdatePreFn pre, CoHModSDKSMUpdatePostFn post) {
        if ((pre == nullptr) && (post == nullptr)) {
            return false;
        }
        std::call_once(gUpdateHookOnce, InstallUpdateHook);
        std::scoped_lock lock(gMutex);
        gUpdateCallbacks.push_back({pre, post});
        return true;
    }

    bool RegisterDeactivateAll(CoHModSDKSMDeactivateAllPreFn pre, CoHModSDKSMDeactivateAllPostFn post) {
        if ((pre == nullptr) && (post == nullptr)) {
            return false;
        }
        std::call_once(gDeactivateAllHookOnce, InstallDeactivateAllHook);
        std::scoped_lock lock(gMutex);
        gDeactivateAllCallbacks.push_back({pre, post});
        return true;
    }

    bool RegisterUnloadScreen(CoHModSDKSMUnloadScreenPreFn pre, CoHModSDKSMUnloadScreenPostFn post) {
        if ((pre == nullptr) && (post == nullptr)) {
            return false;
        }
        std::call_once(gUnloadScreenHookOnce, InstallUnloadScreenHook);
        std::scoped_lock lock(gMutex);
        gUnloadScreenCallbacks.push_back({pre, post});
        return true;
    }
}
