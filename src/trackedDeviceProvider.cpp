#include "trackedDeviceProvider.h"
#include "globalState.h"
#include "hmdDevice.h"
#include "openvr_driver.h"

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    hmdDevice = new HMDDevice();

    // Add HMD
    vr::VRServerDriverHost()->TrackedDeviceAdded("300HMD", vr::TrackedDeviceClass_HMD, hmdDevice);

    // Add two controllers @TODO

    GlobalState::GetGlobalState()->frameNumber = 0;
    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup() {
    delete hmdDevice;
    hmdDevice = NULL;
}

void TrackedDeviceProvider::RunFrame() {
    GlobalState::GetGlobalState()->frameNumber++;

    //Now, process events that were submitted for this frame.
	vr::VREvent_t vrevent{};
	while ( vr::VRServerDriverHost()->PollNextEvent( &vrevent, sizeof( vr::VREvent_t ) ) )
	{
        // @TODO?
    }
}