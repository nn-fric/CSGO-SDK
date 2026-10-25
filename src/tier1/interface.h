// This header defines the interface convention used in the valve engine.
// To make an interface and expose it:
//    1. The interface must be ALL pure virtual, and have no data members.
//    2. Define a name for it.
//    3. In its implementation file, use EXPOSE_INTERFACE or EXPOSE_SINGLE_INTERFACE.

// Versioning
// There are two versioning cases that are handled by this:
// 1. You add functions to the end of an interface, so it is binary compatible with the previous interface. In this case,
//    you need two EXPOSE_INTERFACEs: one to expose your class as the old interface and one to expose it as the new interface.
// 2. You update an interface so it's not compatible anymore (but you still want to be able to expose the old interface
//    for legacy code). In this case, you need to make a new version name for your new interface, and make a wrapper interface and
//    expose it for the old interface.

#pragma once

#define DLL_EXPORT
#include <SDL3/SDL_platform.h>

#define CREATEINTERFACE_PROCNAME "CreateInterface"

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

// Used internally to register classes.
class InterfaceReg
{
public:
	InterfaceReg( InstantiateInterfaceFn fn, const char *pName );

	InstantiateInterfaceFn	m_CreateFn;
	const char				*m_pName;

	InterfaceReg			*m_pNext; // For the global list.
};

// Use this to expose an interface that can have multiple instances.
// e.g.:
// EXPOSE_INTERFACE( CInterfaceImp, IInterface, "MyInterface001" )
// This will expose a class called CInterfaceImp that implements IInterface (a pure class)
// clients can receive a pointer to this class by calling CreateInterface( "MyInterface001" )
//
// In practice, the shared header file defines the interface (IInterface) and version name ("MyInterface001")
// so that each component can use these names/vtables to communicate
//
// A single class can support multiple interfaces through multiple inheritance
#define EXPOSE_INTERFACE( className, interfaceName, versionName ) \
	static void *__Create##className##_interface() { return static_cast<interfaceName *>( new className ); } \
	static InterfaceReg __g_Create##className##_reg( __Create##className##_interface, versionName );

// Use this to expose a singleton interface with a global variable you've created.
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR( className, interfaceName, versionName, globalVarName ) \
	static void *__Create##className##interfaceName##_interface() { return static_cast<interfaceName *>( &globalVarName ); } \
	static InterfaceReg __g_Create##className##interfaceName##_reg( __Create##className##interfaceName##_interface, versionName );

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE( className, interfaceName, versionName ) \
	static className __g_##className##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR( className, interfaceName, versionName, __g_##className##_singleton )

// interface return status
enum
{
	IFACE_OK,
	IFACE_FAILED
};

// This function is automatically exported and allows you to access any interfaces exposed with the above macros.
// if pReturnCode is set, it will return one of the following values (IFACE_OK, IFACE_FAILED)
// extend this for other error conditions/code
extern "C" SDL_DECLSPEC void *CreateInterface( const char *pName, int *pReturnCode );
