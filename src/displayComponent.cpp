#include "displayComponent.h"

// Code taken from
// https://github.com/ValveSoftware/openvr/blob/master/samples/drivers/drivers/simplehmd/src/hmd_device_driver.cpp

//-----------------------------------------------------------------------------
// Purpose: To inform the rest of the vr system what the recommended target size should be
//-----------------------------------------------------------------------------
void DisplayComponent::GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight )
{
	*pnWidth = config.render_width;
	*pnHeight = config.render_height;
}

//-----------------------------------------------------------------------------
// Purpose: To inform vrcompositor how the screens should be organized.
//-----------------------------------------------------------------------------
void DisplayComponent::GetEyeOutputViewport( vr::EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight )
{
	*pnY = 0;

	// Each eye will have half the window
	*pnWidth = config.window_width / 2;

	// Each eye will have the full height
	*pnHeight = config.window_height;

	if ( eEye == vr::Eye_Left )
	{
		// Left eye viewport on the left half of the window
		*pnX = 0;
	}
	else
	{
		// Right eye viewport on the right half of the window
		*pnX = config.window_width / 2;
	}
}

//-----------------------------------------------------------------------------
// Purpose: To inform the compositor what the projection parameters are for this HMD.
//-----------------------------------------------------------------------------
void DisplayComponent::GetProjectionRaw( vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom )
{
	*pfLeft = -1.0;
	*pfRight = 1.0;
	*pfTop = -1.0;
	*pfBottom = 1.0;
}

//-----------------------------------------------------------------------------
// Purpose: To compute the distortion properties for a given uv in an image.
//-----------------------------------------------------------------------------
vr::DistortionCoordinates_t DisplayComponent::ComputeDistortion( vr::EVREye eEye, float fU, float fV )
{
	vr::DistortionCoordinates_t coordinates{};
	coordinates.rfBlue[ 0 ] = fU;
	coordinates.rfBlue[ 1 ] = fV;
	coordinates.rfGreen[ 0 ] = fU;
	coordinates.rfGreen[ 1 ] = fV;
	coordinates.rfRed[ 0 ] = fU;
	coordinates.rfRed[ 1 ] = fV;
	return coordinates;
}

bool DisplayComponent::ComputeInverseDistortion(vr::HmdVector2_t *pResult, vr::EVREye eEye, uint32_t unChannel, float fU, float fV ) {
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: To inform vrcompositor what the window bounds for this virtual HMD are.
//-----------------------------------------------------------------------------
void DisplayComponent::GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight )
{
	*pnX = config.window_x;
	*pnY = config.window_y;
	*pnWidth = config.window_width;
	*pnHeight = config.window_height;
}