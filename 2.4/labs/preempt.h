#pragma once

#include "util/config.h"
//
// preempt_t : State for your timer/preemption handler
//
// Note:
//  We've one instance of core_t per each core.
//  To access this instance, you need to use %gs:0
//  (The entire kernel doesn't have any global/static variables)
//
// %gs:core_offset_preempt will point to start of preempt_t instance
//
// for example:
// %gs:0 will return pointer to core_t
// %gs:core_offset_mainstack will return core_t::main_stack
// %gs:core_offset_preempt+0 will return core_t::saved_stack
//
// etc.
//
// See Definition of core_t in x86/main.h
//

struct preempt_t
{
	/*	scheduler fxn	:	path through
	 *	---------------------------------------
	 *	flag	0	: 	f_stack
	 *	flag	1	:	main_stack and timer is set
	 *	flag	2	:	main_stack and timer not set
	*/
	int flag = 2;
	addr_t f_stack;
};


//
//
// This macro is being called from x86/except.cc
//
//
// _name: label name
// _f   : C function to be called
//        ex: we may have to do send EOI to LAPIC or PIC etc.
//

#  define  _ring0_preempt(_name,_f)            \
	__asm(                                       \
		"  .text                            \n\t"\
		" " STR(_name) ":                   \n\t"\
		"  pushl %edx                       \n\t"\
		"  pushl %ecx                       \n\t"\
		"  pushl %eax                       \n\t"\
		"  call " STR(_f) "                 \n\t"\
		"  popl  %eax                       \n\t"\
		"  popl  %ecx                       \n\t"\
		"  popl  %edx                       \n\t"\
		"                                   \n\t"\
		"  # insert your code here          \n\t"\
		"                                   \n\t"\
		"  #if thread is already inside yield\n\t"\
		"  cmp $0, %gs:"STR(core_offset_preempt+0)"  \n\t"\
		"  je "STR(Inter_handler)"          \n\t"\
		"  movl $2, %gs:"STR(core_offset_preempt+0)"  \n\t"\
		"  jmp iret_toring0                 \n\t"\
		"                                   \n\t"\
		"  "STR(Inter_handler)":            \n\t"\
		"  movl $2, %gs:"STR(core_offset_preempt+0)"  \n\t"\
		"  pushl %eax                       \n\t"\
		"  pushl %ecx                       \n\t"\
		"  pushl %ebp                       \n\t"\
		"                                   \n\t"\
		"                                   \n\t"\
		"  movl %esp, %ebp                  \n\t"\
		"  subl $512, %esp                  \n\t"\
		"  andl $0xfffffff0, %esp           \n\t"\
		"  FXSAVE (%esp)                    \n\t"\
		"  PUSHF               		    \n\t"\
		"  #set interrupt enable flag       \n\t"\
		"  STI                              \n\t"\
		"  pushl $1f  #restore handler      \n\t"\
		"                                   \n\t"\
		"  #store esp to core.preempt_t     \n\t"\
		"  #calling stackrestore            \n\t"\
		"  movl %esp, %gs:"STR(core_offset_preempt+4)"      \n\t"\
		"  movl %gs:"STR(core_offset_mainstack)", %esp      \n\t"\
		"                                   \n\t"\
		"  ret #switch stack                \n\t"\
		"                                   \n\t"\
		"  1:                               \n\t"\
		"  POPF				                      \n\t"\
		"  FXRSTOR (%esp)                      \n\t"\
		"  movl %ebp, %esp                  \n\t"\
		"  popl %ebp                        \n\t"\
		"  popl %ecx                        \n\t"\
		"  popl %eax                        \n\t"\
		"  jmp iret_toring0                 \n\t"\
	)                                        \

