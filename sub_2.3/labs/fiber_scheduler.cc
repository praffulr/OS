// FIXME: check few fix set of assignments are done inside error block

#include "labs/fiber_scheduler.h"

//--------------HELPER FUNCTIONS---------------

static void copy_fiber_output(shellstate_t& shellstate, int index)
{
	/*	copy fiber output to general output	*/

	if(index >= 0 && index <3)
	{
		shellstate.output[0] = 'g';
		shellstate.output[1] = ' ';
		shellstate.output[2] = (char)(48+shellstate.head+1);
		shellstate.output[3] = ':';

	}
	else if(index >= 3 && index < 6)
	{
		shellstate.output[0] = 'h';
		shellstate.output[1] = ' ';
		shellstate.output[2] = (char)(48+shellstate.head-2);
		shellstate.output[3] = ':';
	}
	int i=0;
	for(i=0; shellstate.fiber_output[index][i] != '\0'; i++)
		shellstate.output[4+i] = shellstate.fiber_output[index][i];
	shellstate.output[4+i] = '\0';
}

static void clean_fiber_args_output(shellstate_t& shellstate, int index)
{
	/*	cleans shellstate fiber command and arguments at given index	*/
	for(int i=0; i<10; i++)	shellstate.fiber_arguments[index][i][0] = '\0';
	int i=0;
	for(i=0; shellstate.fiber_output[index][i] != '\0'; i++)
		shellstate.fiber_output[index][i] = '\0';
}

// check if first string is subset of second or not.
bool equal_to3(char* str1, char* str2, int len)
{
	for(int i=0; i<len; i++)
	{
		if(str1[i] != str2[i]) return false;
		else if(str1[i] == '\0') return true;
	}
	return true;
}

int get_string_length3(char *string)
{
	int length=0;
	for(; string[length] != '\0'; length++){}
	return length;
}

// convert string number to int number
int char_array_to_int3(char *string)
{
	int index = get_string_length3(string)-1, factor=1, number=0;
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
int get_num_digit3(int n)
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


char digit_to_char3(int n)
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

void int_to_string_write3(char* arr, int n)
{
	if(n==0)
	{
		arr[0] = '0';
		arr[1] = '\0';
		return;
	}
	int digit_length = get_num_digit3(n);
	int index = digit_length;
	//char int_string[digit_length+1];
	arr[index--] = '\0';
	while(n != 0)
	{
		int lsb = n%10;
		arr[index--] = digit_to_char3(lsb);

		n = n/10;
	}
}
//-------------------------------------------------------


//
// stackptrs:      Type: addr_t[stackptrs_size].  array of stack pointers (generalizing: main_stack and f_stack)
// stackptrs_size: number of elements in 'stacks'.
//
// arrays:      Type: uint8_t [arrays_size]. array of memory region for stacks (generalizing: f_array)
// arrays_size: size of 'arrays'. equal to stackptrs_size*STACK_SIZE.
//
// Tip: divide arrays into stackptrs_size parts.
// Tip: you may implement a circular buffer using arrays inside shellstate_t
//      if you choose linked lists, create linked linked using arrays in
//      shellstate_t. (use array indexes as next pointers)
// Note: malloc/new is not available at this point.
//

void pfactorsf3(shellstate_t* pshellstate, int* pn, addr_t* pmain_stack, addr_t* pf_stack)
{
	/*
		finds all prime factors of number n
		implemented using fibers and stores
		all the answer to shellstate's
		output vatiable
	*/

	//restoring the pointer values
//	shellstate_t& shellstate = *pshellstate;
//	addr_t& main_stack = *pmain_stack;
//	addr_t& f_stack = *pf_stack;
//	int& n = *pn;
//	hoh_debug("____________1");
	
	int output_iterator = 0;
	pshellstate->fiber_isFirst[pshellstate->head] = false;
	int i=2, j=2, num_digits;
	for(i=2; i<= *pn; i++)
	{
		// check if i is prime
		for(j=2; j<i; j++)	if(i%j == 0)	{ break;}
		// if prime and a factor of n
		if(j==i && (*pn%i == 0))
		{
			// store the output
			num_digits = get_num_digit3(i);
			
			if((output_iterator + num_digits+2 == 80))	break;
			int_to_string_write3(pshellstate->fiber_output[pshellstate->head] + output_iterator, i);
			output_iterator += 1 + num_digits;
			pshellstate->fiber_output[pshellstate->head][output_iterator-1] = ' ';
		}
		// return to main
		stack_saverestore(*pf_stack, *pmain_stack);
//		hoh_debug("__________3");
	}
	pshellstate->fiber_output[pshellstate->head][output_iterator] = '\0';
	pshellstate->fiber_isFirst[pshellstate->head] = true;
	pshellstate->fiber_isDone[pshellstate->head] = true;
	stack_saverestore(*pf_stack, *pmain_stack);
}


void removeThis(int head, char* info0, char* info1)
{
	if(head == 0)	hoh_debug(info0);
	else		hoh_debug(info1);
}

void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size)
{
	// FIXME: point mainstack to proper location

	//insert your code here
	if (shellstate.mode == 3)
	{
      //Local variables as per head
      //stack_init4(f_stack, &f_array, f_arraysize, &pfactorsf3, &shellstate, &n, &main_stack, &f_stack);
      //1 -> main_stack
      //2-7 -> 6 f_stacks
      addr_t& f_stack = stackptrs[1+shellstate.head];
      addr_t& main_stack = stackptrs[0];
//      size_t f_arraysize = arrays_size/stackptrs_size;
      size_t f_arraysize = arrays_size/shellstate.FIBER_SIZE;
//      addr_t f_array = arrays + (shellstate.head+1)*(f_arraysize);
      addr_t f_array = arrays + (shellstate.head)*(f_arraysize);

			if(shellstate.head>=0 && shellstate.head <6)
			{
				// get the number of arguments
				int num_arguments = 0;
				for(num_arguments=0x0; !equal_to3(shellstate.fiber_arguments[shellstate.head][num_arguments], "", 1) && num_arguments <= 0x9; num_arguments++){}

				// FIXME: check error all time vs only at first
				// if number of arguments == 0 then show error
				if(num_arguments != 1)
				{
					shellstate.output[0] = 'E';
					shellstate.output[1] = 'R';
					shellstate.output[2] = 'R';
					shellstate.output[3] = 'O';
					shellstate.output[4] = 'R';
					shellstate.output[5] = '\0';
					shellstate.enable_output = 1;
					shellstate.line++;
					shellstate.column = 0;
					shellstate.fiber_isDone[shellstate.head] = true;
					clean_fiber_args_output(shellstate, shellstate.head);
				}
				else
				{
					int n = char_array_to_int3(shellstate.fiber_arguments[shellstate.head][0]);
					// if parameter is negative then error
					if(n == -1)
					{
						shellstate.output[0] = 'E';
						shellstate.output[1] = 'R';
						shellstate.output[2] = 'R';
						shellstate.output[3] = 'O';
						shellstate.output[4] = 'R';
						shellstate.output[5] = '\0';
						shellstate.enable_output = 1;
						shellstate.line++;
						shellstate.column = 0;
						shellstate.fiber_isDone[shellstate.head] = true;
						// clearing command and arguments of shellstate
						clean_fiber_args_output(shellstate, shellstate.head);
					}
					else
					{
						// for the first time initialize the fstack
						if(shellstate.fiber_isFirst[shellstate.head])
						{
							if(shellstate.head >= 0 && shellstate.head < 3)
							{
								shellstate.g_count++;
								stack_init4(f_stack, f_array, f_arraysize, &pfactorsf3, &shellstate, &n, &main_stack, &f_stack);
							}
							else if(shellstate.head >= 3 && shellstate.head<6)
							{
								shellstate.h_count++;
								stack_init4(f_stack, f_array, f_arraysize, &pfactorsf3, &shellstate, &n, &main_stack, &f_stack);
							}
						}
						// if num_arg == 1 and arg >= 0 then
						// change stack and call fxn
						stack_saverestore(main_stack, f_stack);
						// FIXME: can have int {0, -1, 1} for isdone: isFirst, not-init, isDone
						if(shellstate.fiber_isDone[shellstate.head])
						{
							// calculation done
							// enable the output
							shellstate.enable_output = 0x1;
							copy_fiber_output(shellstate, shellstate.head);

							shellstate.line++;
							shellstate.column = 0;
							clean_fiber_args_output(shellstate, shellstate.head);
							if(shellstate.head >= 0 && shellstate.head < 3)
								shellstate.g_count--;
							else if(shellstate.head >= 3 && shellstate.head<6)
								shellstate.h_count--;
						}
					}
				}
				//allot to next available head
				int head = (shellstate.head+1)%shellstate.FIBER_SIZE;
				while(head != (shellstate.head))
				{
					if(!shellstate.fiber_isDone[head]) break;
					head = (head+1)%shellstate.FIBER_SIZE;
				}
				shellstate.head = head;
			}
			//CHANGING MODE TO ZERO
		    bool to_zero = true;
		      for(int i=0; i<shellstate.FIBER_SIZE; i++)
		      {
		        if(!shellstate.fiber_isDone[i]) { to_zero = false; break;}
		      }
		      if(to_zero)
		      {
		        shellstate.mode = 0x0;
		        return;
		      }
	}
}
