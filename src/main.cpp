#include "openvr_driver.h"
#include "trackedDeviceProvider.h"
#include <cstring>

#ifdef WIN32
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#else
#define HMD_DLL_EXPORT extern "C" __attribute__((__visibility__("default")))
#endif

TrackedDeviceProvider trackedDeviceProvider;

HMD_DLL_EXPORT 
void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( vr::IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &trackedDeviceProvider;
	}
	if( 0 == strcmp( vr::IVRWatchdogProvider_Version, pInterfaceName ) )
	{
		return 0; // global for watchdog
	}


	if( pReturnCode )
		*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;

	return NULL;
}