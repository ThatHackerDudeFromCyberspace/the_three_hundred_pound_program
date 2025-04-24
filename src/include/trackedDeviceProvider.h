#pragma once

#include "controllerDevice.h"
#include "openvr_driver.h"

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider {
public:
    vr::EVRInitError Init( vr::IVRDriverContext* pDriverContext ) override;
    void Cleanup() override;
    const char* const* GetInterfaceVersions() override {
        return vr::k_InterfaceVersions;
    };
    void RunFrame() override; // Poll events in here

    void EnterStandby() override {};
    void LeaveStandby() override {};
    bool ShouldBlockStandbyMode() override { return false; } // ShouldBlockStandbyMode is deprecated, and will never be called in newer versions of SteamVR. Drivers must implement this function (because it is pure virtual), but the return value has no effect on behavior.
private:
    ControllerDevice* leftController;
    ControllerDevice* rightController;
};
