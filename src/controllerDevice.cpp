#include "controllerDevice.h"
#include "inputParser.h"
#include "openvr_driver.h"
#include "pipeHandler.h"

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

    vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "ThreeHundredController");
    vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, role);

    // Setup inputs
    char inputProfilePath[8192];
    vr::VRSettings()->GetString("driver_three_hundred_controller_settings", "input_profile_path", inputProfilePath, sizeof(inputProfilePath));
    char inputDefinitionPath[8192];
    vr::VRSettings()->GetString("driver_three_hundred_controller_settings", "input_definition_path", inputDefinitionPath, sizeof(inputDefinitionPath));

    // Parse the inputs
    inputList = parseInputList(inputDefinitionPath);

    // Set profile path for SteamVR
    vr::VRProperties()->SetStringProperty( container, vr::Prop_InputProfilePath_String, inputProfilePath);

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
	vr::DriverPose_t pose = { 0 };
	pose.qWorldFromDriverRotation.w = 1.0f;
	pose.qDriverFromHeadRotation.w = 1.0f;
	pose.qRotation.w = 1.0f;
	pose.poseIsValid = true;
	pose.deviceIsConnected = true;
	pose.result = vr::TrackingResult_Running_OK;
	return pose;
}

void ControllerDevice::UpdateThread() {
    printf("[ThreeHundred] Update Thread started for controller\n");
    while (isActive) {
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated( controllerIndex, GetPose(), sizeof( vr::DriverPose_t ) );

		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

        if ((side == Side::LEFT && PipeHandler::GetPipeHandler()->LeftMessageAvailable()) || (side == Side::RIGHT && PipeHandler::GetPipeHandler()->RightMessageAvailable())) {
            printf("[ThreeHundred] Message available for controller %s\n", side == Side::LEFT ? "LEFT" : "RIGHT");
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
                        printf("[ThreeHundred] Setting [%s] to scalar float: [%f]\n", message.path.c_str(), atof(message.param.c_str()));
                        vr::VRDriverInput()->UpdateScalarComponent(
                            inputHandles[message.path],
                            atof(message.param.c_str()),
                            0
                        );
                        break;
                    case InputType::BOOLEAN:
                        printf("[ThreeHundred] Setting [%s] to boolean: [%i]\n", message.path.c_str(), atoi(message.param.c_str()));
                        vr::VRDriverInput()->UpdateBooleanComponent(
                            inputHandles[message.path],
                            atoi(message.param.c_str()) == 1,
                            0
                        );
                        break;
                }
            } else {
                printf("[ThreeHundred] The message path could not be found in the list of input handles for this controller.\n");
            }
        }
        
    }
}

void ControllerDevice::EnterStandby()
{
	printf( "[ThreeHundred] %s hand has been put on standby", role == vr::TrackedControllerRole_LeftHand ? "Left" : "Right" );
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
