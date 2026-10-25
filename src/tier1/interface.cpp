#include <cstring>
#include <tier1/interface.h>

static InterfaceReg *s_pInterfaceRegs = nullptr;

InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const char *pName ) :
	m_pName( pName )
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

// This is the primary exported function by a dll, referenced by name via dynamic binding
// that exposes an opqaue function pointer to the interface.
//
// We have the Internal variant so Sys_GetFactoryThis() returns the correct internal
// symbol under GCC/Linux/Mac as CreateInterface is DLL_EXPORT so its global so the loaders
// on those OS's pick exactly 1 of the CreateInterface symbols to be the one that is process wide and
// all Sys_GetFactoryThis() calls find that one, which doesn't work. Using the internal walkthrough here
// makes sure Sys_GetFactoryThis() has the dll specific symbol and GetProcAddress() returns the module specific
// function for CreateInterface again getting the dll specific symbol we need.
static void *CreateInterfaceInternal( const char *pName, int *pReturnCode )
{
	for( const InterfaceReg *pCur = s_pInterfaceRegs; pCur; pCur=pCur->m_pNext )
	{
		if( !strcmp(pCur->m_pName, pName) )
		{
			if( pReturnCode )
				*pReturnCode = IFACE_OK;

			return pCur->m_CreateFn();
		}
	}

	if( pReturnCode )
		*pReturnCode = IFACE_FAILED;

	return nullptr;
}

void *CreateInterface( const char *pName, int *pReturnCode )
{
	return CreateInterfaceInternal( pName, pReturnCode );
}
