#pragma once

#include "../../include/CoHModSDKUI.hpp"

namespace UIScreenHooks {
    void Shutdown();

    bool RegisterDraw(CoHModSDKSMDrawPreFn pre, CoHModSDKSMDrawPostFn post);
    bool RegisterUpdate(CoHModSDKSMUpdatePreFn pre, CoHModSDKSMUpdatePostFn post);
    bool RegisterDeactivateAll(CoHModSDKSMDeactivateAllPreFn pre, CoHModSDKSMDeactivateAllPostFn post);
    bool RegisterUnloadScreen(CoHModSDKSMUnloadScreenPreFn pre, CoHModSDKSMUnloadScreenPostFn post);
}
