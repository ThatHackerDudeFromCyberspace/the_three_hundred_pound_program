#pragma once

#include "inputParser.h"
#include "openvr_driver.h"
#include <atomic>
#include <map>
#include <thread>

class ControllerDevice : public vr::ITrackedDeviceServerDriver {
public:
    ControllerDevice( vr::ETrackedControllerRole role);
    virtual ~ControllerDevice() {};

    vr::EVRInitError Activate(uint32_t unObjectId) override;
    void EnterStandby() override;
	void *GetComponent( const char *pchComponentNameAndVersion ) override { return nullptr; };
	void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) override;
	vr::DriverPose_t GetPose() override;
	void Deactivate() override;

    void RunFrame();
    void UpdateThread();

private:
    std::atomic<vr::TrackedDeviceIndex_t> controllerIndex;
    vr::ETrackedControllerRole role;
    Side side;
    std::string modelNumber;
    std::string serialNumber;

    std::map<std::string, InputInfo> inputList;
    std::map<std::string, vr::VRInputComponentHandle_t> inputHandles;

    std::atomic<bool> isActive;
    std::thread controllerUpdateThread;
};