#pragma once

#include "openvr_driver.h"

struct DisplayComponentConfig {
	int32_t window_x;
	int32_t window_y;

	int32_t window_width;
	int32_t window_height;

	int32_t render_width;
	int32_t render_height;
};

class DisplayComponent : public vr::IVRDisplayComponent {
public:
    explicit DisplayComponent(const DisplayComponentConfig& config) : config(config) {};
    virtual ~DisplayComponent() {};

    bool IsDisplayOnDesktop() override { return true; };
    bool IsDisplayRealDisplay() override { return false; }
    void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) override;
	void GetEyeOutputViewport( vr::EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) override;
	void GetProjectionRaw( vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) override;
	vr::DistortionCoordinates_t ComputeDistortion( vr::EVREye eEye, float fU, float fV ) override;
	void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) override;
    bool ComputeInverseDistortion(vr::HmdVector2_t *pResult, vr::EVREye eEye, uint32_t unChannel, float fU, float fV ) override;
private:
    DisplayComponentConfig config;
};