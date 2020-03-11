#pragma once
#include "util/config.h"
#include "util/debug.h"

// size of callable fibers// global variables

struct shellstate_t
{
	// +------------------------------------------------------------------------+
	// | NOTE: user_input is of the following format: command arg0 arg1 ... argn|
	// +------------------------------------------------------------------------+
	//0 -> normal mode
	//1 -> coroutine mode
	//2 -> fiber mode
	//3 -> non-preemptive scheduling
	// current, head = {0, 1, 2, 3, 4, 5}
	
	uint8_t exec_command;
	int input_ctr, init_flag;
	int input_key;
	uint8_t enable_output;
	int line, column, num_keys;
	short mode;
	char command[10];
	char arguments[10][10];
	char output[80];
	// status of the fiber fxn
	bool isDone;

	// io for 6 callable fibers
	int FIBER_SIZE = 6;
	// first 3 entries: g() and later entries: h()
	char fiber_arguments[6][10][10];
	char fiber_output[6][80];

	// variables to incorporate 2.3
	int g_count, h_count, head;

	bool fiber_isDone[6];
	bool fiber_isFirst[6];
	uint8_t fiber_enable_output[6];
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
	// status of the fiber fxn
	bool isDone;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);
