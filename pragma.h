#ifndef __pragma_h__
#define __pragma_h__

enum {

	// integer-based
	p_KIND = 1,
	p_RETURN,
	p_PARAMETERS,
	p_LOCALS,


	// string-based
	p_SEGMENT,

	// no argument.

	p_NORETURN,
	p_VOLATILE,
	p_DEBUG,
	p_DATABANK,
	p_DYNAMIC,
	p_VOID,
	p_PRIVATE,


	p_RTI,
	p_RTS,
	p_RTL,

	p_NAKED,
	p_STDCALL,
	p_PASCAL,
	p_CDECL,


};

#endif
