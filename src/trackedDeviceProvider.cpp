#include "trackedDeviceProvider.h"
#include "controllerDevice.h"
#include "openvr_driver.h"

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    leftController = new ControllerDevice( vr::TrackedControllerRole_LeftHand );
    rightController = new ControllerDevice( vr::TrackedControllerRole_RightHand );

    vr::VRServerDriverHost()->TrackedDeviceAdded("ThreeHundredController_Left", vr::TrackedDeviceClass_Controller, leftController);
    vr::VRServerDriverHost()->TrackedDeviceAdded("ThreeHundredController_Right", vr::TrackedDeviceClass_Controller, rightController);

    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup() {
    delete leftController;
    leftController = NULL;

    delete rightController;
    rightController = NULL;
}

void TrackedDeviceProvider::RunFrame() {
    //Now, process events that were submitted for this frame.
	vr::VREvent_t vrevent{};
	while ( vr::VRServerDriverHost()->PollNextEvent( &vrevent, sizeof( vr::VREvent_t ) ) )
	{
        // @TODO?
    }
}
