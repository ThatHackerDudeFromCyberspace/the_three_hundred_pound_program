#pragma once

#include "displayComponent.h"
#include "openvr_driver.h"
#include <thread>

class HMDDevice : public vr::ITrackedDeviceServerDriver {
public:
    HMDDevice();
    virtual ~HMDDevice() { delete displayComponent; };
    vr::EVRInitError Activate( uint32_t unObjectId ) override;
    void Deactivate() override { isActive = false; };
    void EnterStandby() override {};
    void* GetComponent( const char* pchComponentNameAndVersion ) override;
    vr::DriverPose_t GetPose() override;
    void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override;

    void UpdateThread();
private:
    bool isActive=false; // Track when activate is called
    uint32_t deviceIndex; // Apparently this is useful
    char modelNumber[1024]; // I don't get this
    char serialNumber[1024]; // Or this
    DisplayComponent* displayComponent;
    std::thread updateThread;
};