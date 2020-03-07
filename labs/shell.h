#pragma once
#include "util/config.h"
#include "util/debug.h"

struct shellstate_t
{
	// ------ set of fields that are common accross renderstate and shellstate ------ //
	int input_key;
	char output[80];
	uint8_t enable_output;
	int line, column, num_keys;
	// +------------------------------------------------------------------------+
	// | NOTE: user_input is of the following format: command arg0 arg1 ... argn|
	// +------------------------------------------------------------------------+
	// shell_state specific variables
	uint8_t exec_command;
	char command[10];
	char arguments[10][10];
	int input_ctr, init_flag;
	//0 -> normal mode
	//1 -> coroutine mode
	//2 -> fiber mode
	short mode;
};

struct renderstate_t
{
	// 2 === user_input, output
	int input_key;
	char output[80];
	int line, column, num_keys;
	uint8_t enable_output;//if(enable[i]==true) qualified for output on vga screen
	int init_flag;
	//0 -> normal mode
	//1 -> coroutine mode
	//2 -> fiber mode
	short mode;
	// uint32_t presses_in_c_mode; //number of presses in c_mode
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);
