#include "labs/shell.h"
#include "labs/vgatext.h"

// ------------------------ Helper function ----------------------//
static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void write_shell(int x, int y, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

char digit_to_char(int n);
int get_num_digit(int n);
void fact_fxn(shellstate_t& state);
void prime_factors_fxn(shellstate_t& state);
bool is_prime(int);
void int_to_string(shellstate_t &check, int n);
void int_to_string_write(char* arr, int n);
int get_string_length(char *string);
int char_array_to_int(char *string);
void append_char(char* left, char right);
bool equal_to(char*, char*, int);
static void insert_cursor(int x, int y, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
//
// initialize shellstate
//
void shell_init(shellstate_t& state){
	state.init_flag = -1;
	state.input_key = 0;
	state.line = 1;
	state.column = 0;
	for(int i=0; i<10; i++)	state.arguments[i][0] = '\0';
	state.command[0] = '\0';
	state.exec_command = 0x0;
	state.input_ctr=0;
	state.num_keys = 0;
	state.output[0] = '\0';
	state.enable_output = 0;
	state.mode = 0;
	state.isDone = true;
	////////////////////////// initialization of command is remaining ////////////
}

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//
void shell_update(uint8_t scankey, shellstate_t& stateinout){

    hoh_debug("Got: "<<unsigned(scankey));
    stateinout.init_flag = 1;
		stateinout.enable_output = 0; //for resetting
    if(scankey >= 2 && scankey <= 11) // 1 - 9, 0
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = 48 + ((scankey - 1)%10);
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {
				append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);
				hoh_debug("arg");hoh_debug(stateinout.arguments[0]);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 16) //q
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'q';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 17) //w
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'w';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 18) //e
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'e';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 19) //r
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'r';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 20) //t
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'t';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 21) //y
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'y';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 22) //u
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'u';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 23) //i
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'i';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 24) //o
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'o';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 25) //p
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'p';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }


    else if (scankey == 30) //a
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'a';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 31) //s
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'s';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 32) //d
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'d';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 33) //f
    {
	stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
	// -----------------------------------------------------------------------------
	stateinout.input_key = (uint8_t)'f';
	if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
	else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
	hoh_debug("left shell update condition");
    }
    else if (scankey == 34) //g
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'g';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 35) //h
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'h';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 36) //j
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'j';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 37) //k
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'k';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 38) //l
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'l';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }


    else if (scankey == 44) //z
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'z';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);
    }
    else if (scankey == 45) //x
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'x';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 46) //c
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'c';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 47) //v
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'v';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 48) //b
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'b';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 49) //n
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'n';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }
    else if (scankey == 50) //m
    {
      stateinout.num_keys++;
      if(++stateinout.column > 80 && stateinout.isDone == true) {stateinout.line++; stateinout.column = 1;}
      stateinout.input_key = (uint8_t)'m';
      if(stateinout.input_ctr != 0 && stateinout.isDone == true) {append_char(stateinout.arguments[stateinout.input_ctr-1], (char)stateinout.input_key);}
      else if(stateinout.isDone == true) append_char(stateinout.command, (char)stateinout.input_key);

    }


    if (scankey == 1) //ESC
    {
      //stateinout.input_key = 27;
    }
    else if (scankey == 28) //ENTER
    {
	stateinout.num_keys++;
	stateinout.input_key = 10;
	stateinout.exec_command = 0x01;
	stateinout.line++; //increase line number by 1 - assuming answer in only one line
	stateinout.column = 0;
    }
    else if(scankey == 75) // left arrow key
    {
			stateinout.num_keys++;
			stateinout.input_key = 8;
			if(stateinout.column > 0) stateinout.column--;
			if(stateinout.input_ctr != 0)
			{
				char* temp = stateinout.arguments[stateinout.input_ctr-1];
				int len=0;
			  for(; temp[len] != '\0'; len++){}
				if(len != 0)
				{
					temp[len-1] = '\0';
				}
				else
				{
					stateinout.input_ctr--;
				}
			}
      else
			{
				char* temp = stateinout.command;
				int len=0;
			  for(; temp[len] != '\0'; len++){}
				if(len != 0)
				{
					temp[len-1] = '\0';
				}
			}

    }

    else if(scankey == 77) // right arrow key //Using right arrow for space functionality
    {
			stateinout.input_key = (int)' ';
      stateinout.input_ctr++;
			stateinout.num_keys++;
      if(++stateinout.column > 80) {stateinout.line++; stateinout.column = 1;}
    }

    else if(scankey == 72) // top arrow key
    {
			// stateinout.num_keys++;
			// if(stateinout.line>1) stateinout.line--;
    }

    else if(scankey == 80) // bottom arrow key
    {
			// stateinout.num_keys++;
			// if(stateinout.line<22) stateinout.line++;
    }

}


//
// do computation
//
void shell_step(shellstate_t& stateinout)
{
	if(stateinout.exec_command == 0x01 && stateinout.mode == 0x0)
	{
		hoh_debug("in shell_step");
		// calling corresponding fuctions
		// hoh_debug(stateinout.command);
		if(equal_to(stateinout.command, "fact", 5)) // INPUT FORM -"fact $"
		{
			hoh_debug("fact is true");
			fact_fxn(stateinout);
			stateinout.enable_output = 1;
			//Changing state to post-render-output stage
			stateinout.line ++;
			stateinout.column = 0;
			stateinout.exec_command = 0x0;
		}

		else if(equal_to(stateinout.command, "pfactors", 9)) //INPUT FORM-"primefactors $"
		{
			hoh_debug("Primes");
			prime_factors_fxn(stateinout);
			stateinout.enable_output = 1;
			//Changing state to post-render-output stage
			stateinout.line  ++ ; //VERIFY
			stateinout.column = 0;
			stateinout.exec_command = 0x0;
		}

		else if(equal_to(stateinout.command, "",1))
		{
			hoh_debug("Empty command");
			stateinout.exec_command = 0x0;
		}
		//COROUTINE COMMAND
		else if(equal_to(stateinout.command, "pfactorsc", 10))
		{
			hoh_debug("pfactorsc");
			stateinout.mode = 1;
			// hoh_debug("pfactors for coroutine in shell step");
		}
		//FIBER COMMAND
		else if(equal_to(stateinout.command, "pfactorsf", 10))
		{
			hoh_debug("pfactorsf");
			stateinout.mode = 2;
		}
		else
		{
			hoh_debug("OTHER STRINGS");
			stateinout.output[0] = 'E';
			stateinout.output[1] = 'R';
			stateinout.output[2] = 'R';
			stateinout.output[3] = 'O';
			stateinout.output[4] = 'R';
			stateinout.output[5] = '\0';
			stateinout.enable_output = 1;
			//Changing state to post-render-output stage
			stateinout.line ++;
			stateinout.column = 0;
			stateinout.exec_command = 0x0;
		}
		// clearing command and arguments of stateinout
		//don't clear if argument is pfactorsc
		if(stateinout.mode == 0x0)
		{
			for(int i=0; i<10; i++)
			{
				for(int j=0; j<10; j++)
					stateinout.arguments[i][j] = '\0';
				stateinout.command[i] = '\0';
			}
			stateinout.input_ctr = 0;
		}
	}
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render)
{

	// coping corresponding data structure
	render.enable_output = shell.enable_output;
	if(shell.exec_command == 0x01)
	{
		render.line = shell.line - 1;
	}
	else
	{
		render.line = shell.line;
	}
	render.column = shell.column;
	render.num_keys = shell.num_keys;
	if(shell.init_flag == -1)
	{
		render.init_flag = 0;
	}
	else
	{
		render.init_flag = 1;
	}
	render.input_key = shell.input_key;
	if(shell.enable_output == 1)
	{
		int i=0;
		for(i=0; i<80 && shell.output[i] != '\0'; i++)
			render.output[i] = shell.output[i];
		render.output[i] = '\0';
	}

	//copying mode
	render.mode = shell.mode;
	// status: whether the computation is running or not
	render.isDone = shell.isDone;
}


//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
	bool res =( (a.num_keys != b.num_keys) || (a.line != b.line) || (a.column != b.column) || (a.input_key != b.input_key));
	return !res;
}



//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){
	int x0 = 0,y0 = 1;
	int st_x = 15, st_y = 23;
	if(state.init_flag == 0)
	{
		drawrect(x0, y0, 80, 25, (uint8_t)0x0, (uint8_t)0x7, w, h, vgatext_base);
		write_shell(x0+1, y0+1, 0x0, 0x2, w, h, vgatext_base);
		//draw status bar
		char status_line[43] = "row:1  column:7  number_of_key_presses:0  ";
		drawtext(15, 23, status_line, 43, (uint8_t)0xe, (uint8_t)0x0, w, h, vgatext_base);
		//insert_cursor(1, 2, 0x8, 0x7 ,w , h, vgatext_base);
	}
	else
	{
		int y = (state.line) + y0;
		int x = x0 + (state.column);
		bool enable = state.enable_output;
		if(enable)
		{
			drawtext(x+1, y-1, state.output, 100, 0x0, 0x7, w, h, vgatext_base);
			write_shell(1,y, 0x0, 0x2, w, h, vgatext_base);
		}
		else if(state.isDone == true)
		{
			if(state.mode == 0x1 || state.mode == 0x2)
			{
				if(state.input_key != 10)
				{
					if(state.input_key != 8) //Backspace key (ASCII)
						vgatext::writechar((y+1)*w+x, state.input_key, (uint8_t)0x0, (uint8_t)0x7, vgatext_base);
					else
						vgatext::writechar((y+1)*w+x+1, (int)'\0', (uint8_t)0x0, (uint8_t)0x7, vgatext_base);
				}
			}
			else
			{
				if(state.input_key != 10) //Not enter key (ASCII)
				{
					if(state.input_key != 8) //Backspace key (ASCII)
					{
						vgatext::writechar(y*w+x+6, state.input_key, (uint8_t)0x0, (uint8_t)0x7, vgatext_base);
						hoh_debug("five left render condition");
					}
					else
						vgatext::writechar(y*w+x+7, (int)'\0', (uint8_t)0x0, (uint8_t)0x7, vgatext_base);
				}
				else	write_shell(x+1,y, 0x0, 0x2, w, h, vgatext_base);
			}
		}
		char line_num[4], col_num[4], num_keys[4];
		if(state.isDone == true)
		{
			int_to_string_write(line_num, state.line);
			drawtext(19, 23, line_num, 3, (uint8_t)0xe, (uint8_t)0x0, w, h, vgatext_base);
			int_to_string_write(col_num, (state.column+7));
			drawtext(29, 23, col_num, 3, (uint8_t)0xe, (uint8_t)0x0, w, h, vgatext_base);
		}		
		int_to_string_write(num_keys, state.num_keys);
		drawtext(54, 23, num_keys, 3, (uint8_t)0xe, (uint8_t)0x0, w, h, vgatext_base);
		hoh_debug("left hoh render");
	}
}


//
//
// helper functions
//
//

static void insert_cursor(int x, int y, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
  vgatext::writechar(y*w+x, 124, bg, fg, vgatext_base);
}

static void write_shell(int x, int y, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
	char temp[7] = "$hell:";
	drawtext(x, y, temp, 6, bg, fg, w, h, vgatext_base);
}

static void write_Res(int x, int y, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base)
{
	char temp[6] = "RES: ";
	drawtext(x, y, temp, 5, bg, fg, w, h, vgatext_base);
}

void append_char(char* left, char right)
{
  int len=0;
  for(; left[len] != '\0'; len++){}
  left[len] = right;
  left[len+1] = '\0';
}

bool equal_to(char* str1, char* str2, int len)
{
	for(int i=0; i<len; i++)
	{
		if(str1[i] != str2[i]) return false;
		else if(str1[i] == '\0') return true;
	}
	return true;
}

int get_string_length(char *string)
{
	int length=0;
	for(; string[length] != '\0'; length++){}
	return length;
}

int char_array_to_int(char *string)
{
	int index = get_string_length(string)-1, factor=1, number=0;
	for(; index >= 0 ; index--)
	{
		char c = string[index];
		int digit = ((int)c-'0');
		if(digit <0 || digit > 9)
			return -1;
		number += factor*((int)c-'0');
		factor *= 10;
	}
	return number;
}

void fact_fxn(shellstate_t &state)
{
	// identify number or arguments
	int num_arguments = 0;
	for(num_arguments=0; !equal_to(state.arguments[num_arguments], "", 1) and num_arguments <= 9; num_arguments++){}
	// user didn't provided any input
	if(num_arguments != 1)
	{
		state.output[0] = 'E';
		state.output[1] = 'R';
		state.output[2] = 'R';
		state.output[3] = 'O';
		state.output[4] = 'R';
		state.output[5] = '\0';
	}
	else
	{
//		int n = atoi(state.arguments[0]), factorial = 1;
		int n = char_array_to_int(state.arguments[0]), factorial=1;
		if(n==-1)
		{
			state.output[0] = 'E';
			state.output[1] = 'R';
			state.output[2] = 'R';
			state.output[3] = 'O';
			state.output[4] = 'R';
			state.output[5] = '\0';
			return;
		}
		for(; n>= 1; n--)	factorial *= n;
		// updates output field in state variable
		int_to_string(state, factorial);
	}
}

void prime_factors_fxn(shellstate_t& state)
{
	int i=0;
	int j=0;
	// identify number or arguments
	int num_arguments = 0;
	for(num_arguments=0; !equal_to(state.arguments[num_arguments], "", 1) and num_arguments <= 9; num_arguments++){}
	// user didn't provided any input
	if(num_arguments != 1)
	{
		hoh_debug("Primes arguments");
		state.output[0] = 'E';
		state.output[1] = 'R';
		state.output[2] = 'R';
		state.output[3] = 'O';
		state.output[4] = 'R';
		state.output[5] = '\0';
	}
	else
	{
		int n = char_array_to_int(state.arguments[0]);
		if(n==-1)
		{
			state.output[0] = 'E';
			state.output[1] = 'R';
			state.output[2] = 'R';
			state.output[3] = 'O';
			state.output[4] = 'R';
			state.output[5] = '\0';
			return;
		}
		int output_iterator = 0;
		for(i=2; i<=n; i++)
		{
			bool is_prime = true;
			for(j=2; j<i; j++)
			{
				if(i%j==0)
				{
					is_prime = false;
					// break;
				}
			}
			if(is_prime && (n%i==0))
			{
				int num_digit = get_num_digit(i);
				if((output_iterator + num_digit+2) >= 80)
				{
					break;
				}
				int_to_string_write(state.output+output_iterator, i);
				output_iterator += (1+get_num_digit(i));
				state.output[output_iterator-1] = ' ';
			}
		}
		state.output[output_iterator] = '\0';
	}
}



bool is_prime(int x)
{
	for(int i=2; i<x; i++)
	{
		if(x%i==0) return false;
	}
	return true;
}

char digit_to_char(int n)
{
	char ans;
	if(n == 0) ans = '0';
	if(n == 1) ans = '1';
	if(n == 2) ans = '2';
	if(n == 3) ans = '3';
	if(n == 4) ans = '4';
	if(n == 5) ans = '5';
	if(n == 6) ans = '6';
	if(n == 7) ans = '7';
	if(n == 8) ans = '8';
	if(n == 9) ans = '9';
	return ans;
}

int get_num_digit(int n)
{
	if(n==0) return 1;
	int length=0;
	while(n != 0)
	{
		n = n/10;
		length++;
	}
	return length;
}

void int_to_string(shellstate_t &check, int n)
{
	if(n==0)
	{
		check.output[0] = '0';
		check.output[1] = '\0';
		return;
	}
	int digit_length = get_num_digit(n);
	int index = digit_length;
	char int_string[digit_length+1];
	int_string[index--] = '\0';
	while(n != 0)
	{
		int lsb = n%10;
		int_string[index--] = digit_to_char(lsb);

		n = n/10;
	}
	for(int i=0; i<80 && i<digit_length+1; i++)	check.output[i] = int_string[i];
}

void int_to_string_write(char* arr, int n)
{
	if(n==0)
	{
		arr[0] = '0';
		arr[1] = '\0';
		return;
	}
	int digit_length = get_num_digit(n);
	int index = digit_length;
	//char int_string[digit_length+1];
	arr[index--] = '\0';
	while(n != 0)
	{
		int lsb = n%10;
		arr[index--] = digit_to_char(lsb);

		n = n/10;
	}
}

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
			// for(int j=i+1; j<maxw;j++) writecharxy(x+j,y,str[i],bg,fg,w,h,vgatext_base);
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}
