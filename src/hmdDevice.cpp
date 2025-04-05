#include "hmdDevice.h"
#include "displayComponent.h"
#include "globalState.h"
#include "openvr_driver.h"
#include <chrono>
#include <cmath>
#include <cstring>
#include <thread>

HMDDevice::HMDDevice() {
    vr::VRSettings()->GetString("driver_300hmd", "model_number", modelNumber, sizeof(modelNumber));
    vr::VRSettings()->GetString("driver_300hmd", "serial_number", serialNumber, sizeof(serialNumber));

    DisplayComponentConfig config = {
        .window_x = vr::VRSettings()->GetInt32( "driver_300hmd", "window_x" ),
        .window_y = vr::VRSettings()->GetInt32( "driver_300hmd", "window_y" ),
        .window_width = vr::VRSettings()->GetInt32( "driver_300hmd", "window_width" ),
        .window_height = vr::VRSettings()->GetInt32( "driver_300hmd", "window_height" ),
        .render_width = vr::VRSettings()->GetInt32( "driver_300hmd", "render_width" ),
        .render_height = vr::VRSettings()->GetInt32( "driver_300hmd", "render_height" )
    };

    displayComponent = new DisplayComponent(config);
}

vr::EVRInitError HMDDevice::Activate(uint32_t unObjectId) {
    deviceIndex = unObjectId;
    isActive = true;

    // Following code taken from https://github.com/ValveSoftware/openvr/blob/master/samples/drivers/drivers/simplehmd/src/hmd_device_driver.cpp

    // Properties are stored in containers, usually one container per device index. We need to get this container to set
	// The properties we want, so we call this to retrieve a handle to it.
	vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer( deviceIndex );

	// Let's begin setting up the properties now we've got our container.
	// A list of properties available is contained in vr::ETrackedDeviceProperty.

	// First, let's set the model number.
	vr::VRProperties()->SetStringProperty( container, vr::Prop_ModelNumber_String, modelNumber );

	// Next, display settings

	// Get the ipd of the user from SteamVR settings
	const float ipd = vr::VRSettings()->GetFloat( vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float );
	vr::VRProperties()->SetFloatProperty( container, vr::Prop_UserIpdMeters_Float, ipd );

	// For HMDs, it's required that a refresh rate is set otherwise VRCompositor will fail to start.
	vr::VRProperties()->SetFloatProperty( container, vr::Prop_DisplayFrequency_Float, 0.f );

	// The distance from the user's eyes to the display in meters. This is used for reprojection.
	vr::VRProperties()->SetFloatProperty( container, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.f );

	// How long from the compositor to submit a frame to the time it takes to display it on the screen.
	vr::VRProperties()->SetFloatProperty( container, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.11f );

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty( container, vr::Prop_IsOnDesktop_Bool, false );

	vr::VRProperties()->SetBoolProperty(container, vr::Prop_DisplayDebugMode_Bool, true);

    updateThread = std::thread(&HMDDevice::updateThread, this);

	// We've activated everything successfully!
	// Let's tell SteamVR that by saying we don't have any errors.
	return vr::VRInitError_None;
}

void* HMDDevice::GetComponent( const char *pchComponentNameAndVersion )
{
	if ( strcmp( pchComponentNameAndVersion, vr::IVRDisplayComponent_Version ) == 0 )
	{
		return displayComponent;
	}

	return nullptr;
}

void HMDDevice::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t HMDDevice::GetPose()
{
	// Let's retrieve the Hmd pose to base our controller pose off.

	// First, initialize the struct that we'll be submitting to the runtime to tell it we've updated our pose.
	vr::DriverPose_t pose = { 0 };

	// These need to be set to be valid quaternions. The device won't appear otherwise.
	pose.qWorldFromDriverRotation.w = 1.f;
	pose.qDriverFromHeadRotation.w = 1.f;

	pose.qRotation.w = 1.f;

	pose.vecPosition[ 0 ] = 0.0f;
	pose.vecPosition[ 1 ] = sin( GlobalState::GetGlobalState()->frameNumber * 0.01 ) * 0.1f + 1.0f; // slowly move the hmd up and down.
	pose.vecPosition[ 2 ] = 0.0f;

	// The pose we provided is valid.
	// This should be set is
	pose.poseIsValid = true;

	// Our device is always connected.
	// In reality with physical devices, when they get disconnected,
	// set this to false and icons in SteamVR will be updated to show the device is disconnected
	pose.deviceIsConnected = true;

	// The state of our tracking. For our virtual device, it's always going to be ok,
	// but this can get set differently to inform the runtime about the state of the device's tracking
	// and update the icons to inform the user accordingly.
	pose.result = vr::TrackingResult_Running_OK;

	// For HMDs we want to apply rotation/motion prediction
	pose.shouldApplyHeadModel = true;

	return pose;
}

void HMDDevice::UpdateThread() {
    while (isActive) {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(deviceIndex, GetPose(), sizeof (vr::DriverPose_t));
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // eh
    }
}