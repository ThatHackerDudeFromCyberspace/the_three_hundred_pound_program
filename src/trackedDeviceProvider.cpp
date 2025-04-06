#include "trackedDeviceProvider.h"
#include "controllerDevice.h"
#include "globalState.h"
#include "hmdDevice.h"
#include "openvr_driver.h"

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    hmdDevice = new HMDDevice();
    leftController = new ControllerDevice( vr::TrackedControllerRole_LeftHand );
    rightController = new ControllerDevice( vr::TrackedControllerRole_RightHand );

    // Add HMD
    vr::VRServerDriverHost()->TrackedDeviceAdded("300HMD", vr::TrackedDeviceClass_HMD, hmdDevice);
    vr::VRServerDriverHost()->TrackedDeviceAdded("300Controller_Left", vr::TrackedDeviceClass_Controller, leftController);
    vr::VRServerDriverHost()->TrackedDeviceAdded("300Controller_Right", vr::TrackedDeviceClass_Controller, rightController);

    // Add two controllers @TODO

    GlobalState::GetGlobalState()->frameNumber = 0;
    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup() {
    delete hmdDevice;
    hmdDevice = NULL;

    delete leftController;
    leftController = NULL;

    delete rightController;
    rightController = NULL;
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