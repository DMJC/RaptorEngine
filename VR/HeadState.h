/*
 *  HeadState.h
 */

#pragma once
class HeadState;

#include "PlatformSpecific.h"

#include "Pos.h"
#include "Framebuffer.h"
#include <cstddef>

#ifndef NO_VR
	#if ! defined(HAVE_NULLPTR) && (! defined(__GNUC__) || (__GNUC__ < 12))
		#if defined(__GNUC__) && (__GNUC__ >= 5)
			#define nullptr __null
		#else
			#define nullptr NULL
		#endif
	#endif
	
	#ifdef DEBIAN
		#include <openvr/openvr.h>
	#else
		#include <openvr.h>
	#endif

	#ifdef nullptr
		#undef nullptr
	#endif
#endif


class HeadState
{
public:
	bool Initialized;
	bool VR;
	Pos3D Basis, Current;
	Framebuffer *EyeL, *EyeR;
#ifndef NO_VR
	vr::IVRSystem *m_pHMD;
	vr::IVRRenderModels *m_pRenderModels;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
#endif
	
	HeadState( void );
	virtual ~HeadState();
	
	void Initialize( void );
	
	void StartVR( void );
	void StopVR( void );
	void RestartVR( void );
	
	void Recenter( void );
	void Draw( void );

#ifndef NO_VR
	void SetEyeProjection( Framebuffer *eye, vr::EVREye which_eye );
	void OffsetEye( vr::EVREye which_eye );
#endif
};
