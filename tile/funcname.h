//
// FileName :
//	funcname.h
//
// Description :
//	Broken workaround of "function name" macro.
//	_G_FUNCNAME* are replaced function name or function signature.
//
//
//	_G_FUNCTIONNAME is very simple function name
//		such as	"funcname"		(GCC)
//		,		"classC<int>::funcname"		(VC)
//
//	_G_FUNCTIONNAMEEX is detailed function name
//		such as	"int classC<T1>::funcname(T1) [with T1 = int]"	(GCC)
//		, 		"int __thiscall classC<int>::funcname(int)"				(VC)
//
//	_G_FUNCTIONNAMEDEC is decoded(internal) function name
//		such as	"?funcname@?$classC@H@@QAEHH@Z"	(VC)
//
// History : 
//	2003/05/29 : first release. [Nazy]
//
// Copyright (C) Nazy 2003. All rights reserved.
//
// Sample :
//	
//	#define dbg_print(fmt, ...) \
//					{printf("DEBUG_OUTPUT(\"%s\",%d,\"%s\")::"), __FILE__, __LINE__, _G_FUNCTIONNAMEEX); \
//					  printf((fmt), __VA_ARGS__);}
//	
//	int main(int argc, char* argv[])
//	{
//		dbg_print("%s:argc=%d\n", argv[0], argc);
//		return 0;
//	}
//
//	// Output :
//	//	DEBUG_OUTPUT("sample.cpp",7,"main")::/home/user/sample:argc=1
//

#if !defined(__FUNCNAME_H_INCLUDED__)
#define __FUNCNAME_H_INCLUDED__		1

#if defined(__GNUC__)
// GNU C

#	define _G_FUNCTIONNAME		__FUNCTION__
#	define _G_FUNCTIONNAMEEX	__PRETTY_FUNCTION__
#	define _G_FUNCTIONNAMEDEC	""

#elif defined(_MSC_VER)
// Microsoft Visual C++

#	define _G_FUNCTIONNAME		__FUNCTION__
#	define _G_FUNCTIONNAMEEX	__FUNCSIG__
#	define _G_FUNCTIONNAMEDEC	__FUNCDNAME__

#elif defined  __MWERKS__
// Metrowerks CodeWarrior

#	define _G_FUNCTIONNAME		__FUNCTION__
#	define _G_FUNCTIONNAMEEX	""
#	define _G_FUNCTIONNAMEDEC	""


#elif defined(__BORLANDC__)
// Borland C++ Compiler

#	warning _G_FUNCTIONNAME Not Supported

#	define _G_FUNCTIONNAME		""
#	define _G_FUNCTIONNAMEEX	""
#	define _G_FUNCTIONNAMEDEC	""

#elif (__STDC_VERSION__ > 199901L)
// C99 or later

#	define _G_FUNCTIONNAME		__func__
#	define _G_FUNCTIONNAMEEX	""
#	define _G_FUNCTIONNAMEDEC	""

#else 

#	warning Unknown Compiler

#	if defined(__FUNCTION__)
#		define _G_FUNCTIONNAME		__FUNCTION__
#	elif defined(__func__)
#		define _G_FUNCTIONNAME		__func__
#	else
#		define _G_FUNCTIONNAME		""
#	endif
#	define _G_FUNCTIONNAMEEX	""
#	define _G_FUNCTIONNAMEDEC	""

#endif // #if defined(*Compiler Identify*) 

#endif	// !defined(__FUNCNAME_H_INCLUDED__)

// **** Emacs ****
// Local Variables:
//  buffer-file-coding-system: shift_jis-unix
//  tab-width: 4
//  c-basic-offset: 4
//  indent-tabs-mode: t
// End:
