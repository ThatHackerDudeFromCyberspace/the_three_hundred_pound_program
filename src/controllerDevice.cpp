#include "controllerDevice.h"
#include "inputParser.h"
#include "openvr_driver.h"
#include "pipeHandler.h"
#include "vrmath.h"

ControllerDevice::ControllerDevice( vr::ETrackedControllerRole role ) {
    isActive = false;
    this->role = role;
    if (role == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand) {
        side = Side::LEFT;
    } else if (role == vr::ETrackedControllerRole::TrackedControllerRole_RightHand) {
        side = Side::RIGHT;
    }
}

vr::EVRInitError ControllerDevice::Activate(uint32_t unObjectId) {
    isActive = true;
    controllerIndex = unObjectId;

    vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(controllerIndex);

    vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "Controller300");
    vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, role);

    // Setup inputs
    char inputProfilePath[8192];
    vr::VRSettings()->GetString("driver_three_hundred_controller_settings", "input_profile_path", inputProfilePath, sizeof(inputProfilePath));

    // Parse the inputs
    inputList = parseInputList(inputProfilePath);

    // Load the profile
    vr::VRProperties()->SetStringProperty( container, vr::Prop_InputProfilePath_String, inputProfilePath); //"{three_hundred_fuzzer}/input/controller_profile.json" );

    // Register the inputs in the list
    for (auto& input : inputList) {
        if (input.second.side != Side::BOTH && !(input.second.side == side)) {
            continue; // Skip
        }

        switch (input.second.inputType) {
            case InputType::SCALAR_ONE_SIDED:
            case InputType::SCALAR_TWO_SIDED:
                vr::VRDriverInput()->CreateScalarComponent(
                    container,
                    input.first.c_str(),
                    &inputHandles[input.first],
                    vr::VRScalarType_Absolute,
                    input.second.inputType == InputType::SCALAR_ONE_SIDED ? vr::VRScalarUnits_NormalizedOneSided : vr::VRScalarUnits_NormalizedTwoSided
                );
                break;
            case InputType::BOOLEAN:
                vr::VRDriverInput()->CreateBooleanComponent(
                    container,
                    input.first.c_str(),
                    &inputHandles[input.first]
                );
                break;
        }
    }

    controllerUpdateThread = std::thread(&ControllerDevice::UpdateThread, this);

    return vr::VRInitError_None;
}

void ControllerDevice::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t ControllerDevice::GetPose()
{
    // Let's retrieve the Hmd pose to base our controller pose off.

	// First, initialize the struct that we'll be submitting to the runtime to tell it we've updated our pose.
	vr::DriverPose_t pose = { 0 };

	// These need to be set to be valid quaternions. The device won't appear otherwise.
	pose.qWorldFromDriverRotation.w = 1.f;
	pose.qDriverFromHeadRotation.w = 1.f;

	vr::TrackedDevicePose_t hmd_pose{};

	// GetRawTrackedDevicePoses expects an array.
	// We only want the hmd pose, which is at index 0 of the array so we can just pass the struct in directly, instead of in an array
	vr::VRServerDriverHost()->GetRawTrackedDevicePoses( 0.f, &hmd_pose, 1 );

	// Get the position of the hmd from the 3x4 matrix GetRawTrackedDevicePoses returns
	const vr::HmdVector3_t hmd_position = HmdVector3_From34Matrix( hmd_pose.mDeviceToAbsoluteTracking );
	// Get the orientation of the hmd from the 3x4 matrix GetRawTrackedDevicePoses returns
	const vr::HmdQuaternion_t hmd_orientation = HmdQuaternion_FromMatrix( hmd_pose.mDeviceToAbsoluteTracking );

	// pitch the controller 90 degrees so the face of the controller is facing towards us
	const vr::HmdQuaternion_t offset_orientation = HmdQuaternion_FromEulerAngles( 0.f, DEG_TO_RAD(90.f), 0.f );

	// Set the pose orientation to the hmd orientation with the offset applied.
	pose.qRotation = hmd_orientation * offset_orientation;

	const vr::HmdVector3_t offset_position = {
		role == vr::TrackedControllerRole_LeftHand ? -0.15f : 0.15f, // translate the controller left/right 0.15m depending on its role
		0.1f,																		// shift it up a little to make it more in view
		-0.5f,																		// put each controller 0.5m forward in front of the hmd so we can see it.
	};

	// Rotate our offset by the hmd quaternion (so the controllers are always facing towards us), and add then add the position of the hmd to put it into position.
	const vr::HmdVector3_t position = hmd_position + ( offset_position * hmd_orientation );

	// copy our position to our pose
	pose.vecPosition[ 0 ] = position.v[ 0 ];
	pose.vecPosition[ 1 ] = position.v[ 1 ];
	pose.vecPosition[ 2 ] = position.v[ 2 ];

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

	return pose;
}

void ControllerDevice::UpdateThread() {
    printf("[Fuzzer300] Update Thread started for controller\n");
    while (isActive) {
        // Inform the vrserver that our tracked device's pose has updated, giving it the pose returned by our GetPose().
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated( controllerIndex, GetPose(), sizeof( vr::DriverPose_t ) );

		// Update our pose every five milliseconds.
		// In reality, you should update the pose whenever you have new data from your device.
		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

        if ((side == Side::LEFT && PipeHandler::GetPipeHandler()->LeftMessageAvailable()) || (side == Side::RIGHT && PipeHandler::GetPipeHandler()->RightMessageAvailable())) {
            printf("[Fuzzer300] Message available for controller %s\n", side == Side::LEFT ? "LEFT" : "RIGHT");
            Message message;
            if (side == Side::LEFT) {
                message = PipeHandler::GetPipeHandler()->PopLeftMessage();
            } else {
                message = PipeHandler::GetPipeHandler()->PopRightMessage();   
            }

            if (inputHandles.find(message.path) != inputHandles.end()) {
                switch (inputList[message.path].inputType) {
                    case InputType::SCALAR_ONE_SIDED:
                    case InputType::SCALAR_TWO_SIDED:
                        vr::VRDriverInput()->UpdateScalarComponent(
                            inputHandles[message.path],
                            atof(message.param.c_str()),
                            0
                        );
                        break;
                    case InputType::BOOLEAN:
                        vr::VRDriverInput()->UpdateBooleanComponent(
                            inputHandles[message.path],
                            atoi(message.param.c_str()) == 1,
                            0
                        );
                        break;
                }
            } else {
                printf("[Fuzzer300] The message path could not be found in the list of input handles for this controller.\n");
            }
        }
        
    }
}

void ControllerDevice::EnterStandby()
{
	printf( "[Fuzzer300] %s hand has been put on standby", role == vr::TrackedControllerRole_LeftHand ? "Left" : "Right" );
}

void ControllerDevice::Deactivate()
{
	// Let's join our pose thread that's running
	// by first checking then setting is_active_ to false to break out
	// of the while loop, if it's running, then call .join() on the thread
	if ( isActive.exchange( false ) )
	{
		controllerUpdateThread.join();
	}

	// unassign our controller index (we don't want to be calling vrserver anymore after Deactivate() has been called
	controllerIndex = vr::k_unTrackedDeviceIndexInvalid;
}