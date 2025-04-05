#include "openvr_driver.h"
#include <cstring>

#ifdef WIN32
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#else
#define HMD_DLL_EXPORT extern "C" __attribute__((__visibility__("default")))
#endif

using namespace vr;

HMD_DLL_EXPORT 
void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return 0;
	}
	if( 0 == strcmp( IVRWatchdogProvider_Version, pInterfaceName ) )
	{
		return 0;
	}


	if( pReturnCode )
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}