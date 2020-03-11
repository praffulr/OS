// FIXME: check few fix set of assignments are done inside error block

#include "labs/fiber.h"

// check if first string is subset of second or not.
bool equal_to2(char* str1, char* str2, int len)
{
	for(int i=0; i<len; i++)
	{
		if(str1[i] != str2[i]) return false;
		else if(str1[i] == '\0') return true;
	}
	return true;
}

int get_string_length2(char *string)
{
	int length=0;
	for(; string[length] != '\0'; length++){}
	return length;
}

// convert string number to int number
int char_array_to_int2(char *string)
{
	int index = get_string_length2(string)-1, factor=1, number=0;
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

// returns number of digits in n
int get_num_digit2(int n)
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


char digit_to_char2(int n)
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

void int_to_string_write2(char* arr, int n)
{
	if(n==0)
	{
		arr[0] = '0';
		arr[1] = '\0';
		return;
	}
	int digit_length = get_num_digit2(n);
	int index = digit_length;
	//char int_string[digit_length+1];
	arr[index--] = '\0';
	while(n != 0)
	{
		int lsb = n%10;
		arr[index--] = digit_to_char2(lsb);

		n = n/10;
	}
}

void pfactorsf(shellstate_t* pshellstate, int* pn, addr_t* pmain_stack, addr_t* pf_stack)
{
	/*
		finds all prime factors of number n
		implemented using fibers and stores
		all the answer to shellstate's
		output vatiable
	*/

	//restoring the pointer values
	shellstate_t& shellstate = *pshellstate;
	int& n = *pn;
	addr_t& main_stack = *pmain_stack;
	addr_t& f_stack = *pf_stack;

	int output_iterator = 0;
	shellstate.isDone = false;
	for(int i=2; i<= n; i++)
	{
		bool is_prime = true;
		// check if i is prime
		for(int j=2; j<i; j++)	if(i%j == 0)	{ is_prime = false; break;}
		// if prime and a factor of n
		if(is_prime && (n%i == 0))
		{
			// store the output
			int num_digits = get_num_digit2(i);
			if((output_iterator + num_digits+2 == 80))	break;
			int_to_string_write2(shellstate.output + output_iterator, i);
			output_iterator += 1 + num_digits;
			shellstate.output[output_iterator-1] = ' ';
		}
		// return to main
		stack_saverestore(f_stack, main_stack);
	}
	shellstate.output[output_iterator] = '\0';
	shellstate.isDone = true;
	stack_saverestore(f_stack, main_stack);
}

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize)
{
	// currently active stack: main_stack
	if((shellstate.exec_command == 0x01 && shellstate.mode == 0x02))
	{
		if(equal_to2(shellstate.command, "pfactorsf", 10))
		{
			// get the number of arguments
			int num_arguments = 0;
			for(num_arguments=0x0; !equal_to2(shellstate.arguments[num_arguments], "", 1) && num_arguments <= 0x9; num_arguments++){}

			// if number of arguments == 0 then show error
			if(num_arguments != 1)
			{
				hoh_debug(num_arguments);
				hoh_debug("Primes arguments");
				shellstate.output[0] = 'E';
				shellstate.output[1] = 'R';
				shellstate.output[2] = 'R';
				shellstate.output[3] = 'O';
				shellstate.output[4] = 'R';
				shellstate.output[5] = '\0';
				// clearing command and arguments of shellstate
				for(int i=0; i<10; i++)
				{
					for(int j=0; j<10; j++)	shellstate.arguments[i][j] = '\0';
					shellstate.command[i] = '\0';
				}
				shellstate.input_ctr = 0;
				shellstate.line++;
				shellstate.column = 0;
				shellstate.exec_command = 0x0;
				shellstate.mode = 0x0;
			}
			else
			{
				int n = char_array_to_int2(shellstate.arguments[0]);
				// if parameter is negative then error
				// TODO: negative not -1
				if(n == -1)
				{
					shellstate.output[0] = 'E';
					shellstate.output[1] = 'R';
					shellstate.output[2] = 'R';
					shellstate.output[3] = 'O';
					shellstate.output[4] = 'R';
					shellstate.output[5] = '\0';
					// clearing command and arguments of shellstate
					for(int i=0; i<10; i++)
					{
						for(int j=0; j<10; j++)	shellstate.arguments[i][j] = '\0';
						shellstate.command[i] = '\0';
					}
					shellstate.input_ctr = 0;
					shellstate.line++;
					shellstate.column = 0;
					shellstate.exec_command = 0x0;
					shellstate.mode = 0x0;
				}
				else
				{
					// for the first time initialize the fstack
					if(shellstate.isDone)
						stack_init4(f_stack, &f_array, f_arraysize, &pfactorsf, &shellstate, &n, &main_stack, &f_stack);
					// if num_arg == 1 and arg >= 0 then
					// change stack and call fxn
					stack_saverestore(main_stack, f_stack);
					if(shellstate.isDone)
					{
						// calculation done
						// enable the output
						shellstate.enable_output = 0x1;

						// clearing command and arguments of shellstate
						for(int i=0; i<10; i++)
						{
							for(int j=0; j<10; j++)	shellstate.arguments[i][j] = '\0';
							shellstate.command[i] = '\0';
						}
						shellstate.input_ctr = 0;
						shellstate.line++;
						shellstate.column = 0;
						shellstate.exec_command = 0x0;
						shellstate.mode = 0x0;
					}
				}
			}
		}
	}
}
