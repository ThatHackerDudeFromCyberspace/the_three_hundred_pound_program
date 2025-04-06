#pragma once

#include "openvr_driver.h"

class ControllerDevice : public vr::ITrackedDeviceServerDriver {
public:
    ControllerDevice( vr::ETrackedControllerRole role );

    void EnterStandby() override;
	void *GetComponent( const char *pchComponentNameAndVersion ) override;
	void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) override;
	vr::DriverPose_t GetPose() override;
	void Deactivate() override;

    void RunFrame();
    void PostUpdateThread();

private:
    
};