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


// FIXME: last two args are dedundant, remove it later and make changes accordingly
void pfactorsf3_preemptive(shellstate_t* pshellstate, int* pn, addr_t* pmain_stack, addr_t* pf_stack)
{
	/*
		finds the nth prime,implemented
		using fibers and stores the answer
		to shellstate's output variable
	*/
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
	}
	pshellstate->fiber_output[pshellstate->head][output_iterator] = '\0';
	pshellstate->fiber_isFirst[pshellstate->head] = true;
	pshellstate->fiber_isDone[pshellstate->head] = true;
}

void nth_prime3(shellstate_t* pshellstate, int* pn, addr_t* pmain_stack, addr_t* pf_stack, preempt_t* ppreempt)
{
	/*
		finds the nth prime,implemented
		using fibers and stores the answer
		to shellstate's output variable
	*/

	pshellstate->fiber_isFirst[pshellstate->head] = false;

	shellstate_t& shellstate = *pshellstate;
	int n = *pn;
	addr_t& main_stack = *pmain_stack;
	addr_t& f_stack = *pf_stack;
	preempt_t& preempt = *ppreempt;

	int prime_count = 0;
	int i=1;
	while(prime_count != n)
	{
		i++;
		// check if i is not prime
		int j;
		for(j=2; j<i; j++)	if(i%j == 0)	break;
		if(j<i)			continue;
		// i is prime
		prime_count++;
		// hoh_debug("i: "<<i);
	}

	// store the output
	int output_iterator = 0;
	int num_digits = get_num_digit3(i);

	int_to_string_write3(pshellstate->fiber_output[pshellstate->head] + output_iterator, i);
	output_iterator += 1 + num_digits;
	pshellstate->fiber_output[pshellstate->head][output_iterator-1] = ' ';
	pshellstate->fiber_output[pshellstate->head][output_iterator] = '\0';

	pshellstate->fiber_isFirst[pshellstate->head] = true;
	pshellstate->fiber_isDone[pshellstate->head] = true;

	// hoh_debug("calculation done...");
	// hoh_debug("ans: " << i);
	while(1)
	{
		hoh_debug("fag enddddd--------------");
		preempt.flag = 1;
		stack_saverestore(f_stack, main_stack);
	}
}

void pfactorsf3(shellstate_t* pshellstate, int* pn, addr_t* pmain_stack, addr_t* pf_stack, preempt_t* ppreempt)
{
	/*
		finds all prime factors of number n
		implemented using fibers and stores
		all the answer to shellstate's
		output vatiable
	*/
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
		ppreempt->flag = 1;
		stack_saverestore(*pf_stack, *pmain_stack);
	}
	pshellstate->fiber_output[pshellstate->head][output_iterator] = '\0';
	pshellstate->fiber_isFirst[pshellstate->head] = true;
	pshellstate->fiber_isDone[pshellstate->head] = true;
	ppreempt->flag = 1;
	stack_saverestore(*pf_stack, *pmain_stack);
}

void removeThis(int flag, char* string)
{
	if(flag == 0)	hoh_debug(string << "flag is 0");
	if(flag == 1)	hoh_debug(string << "flag is 1");
	if(flag == 2)	hoh_debug(string << "flag is 2");
}

void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t& lapic)
{
	/*
	// convention	flag	semantics
	//		0	f_stack
	//		1	main_stack and timer set
	//		2	main_stack and timer not set
	*/


	if (shellstate.mode == 3)
	{
		//Local variables as per head
		//stack_init4(f_stack, &f_array, f_arraysize, &pfactorsf3, &shellstate, &n, &main_stack, &f_stack);
		//1 -> main_stack
		//2-7 -> 6 f_stacks
		addr_t& f_stack = stackptrs[1+shellstate.head];
		size_t f_arraysize = arrays_size/shellstate.FIBER_SIZE;
		addr_t f_array = arrays + (shellstate.head)*(f_arraysize);

		if(shellstate.head>=0 && shellstate.head <6)
		{
			// get the number of arguments
			int num_arguments = 0;

			for(	num_arguments=0x0;
				!equal_to3(shellstate.fiber_arguments[shellstate.head][num_arguments], "", 1) &&
				num_arguments <= 0x9;
				num_arguments++)
			{}

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
							stack_init5(	f_stack, f_array, f_arraysize, &pfactorsf3,
									&shellstate, &n, &main_stack, &f_stack, &preempt);
						}
						else if(shellstate.head >= 3 && shellstate.head<6)
						{
							shellstate.h_count++;
							stack_init5(	f_stack, f_array, f_arraysize,
									&nth_prime3, &shellstate,
									&n, &main_stack, &f_stack, &preempt);
						}
					}

					if(preempt.flag == 2)
					{
						hoh_debug("setting the timer...");
						lapic.reset_timer_count(shellstate.cut_off_time);
						preempt.flag = 1;
					}

					preempt.flag = 0;
					stack_saverestore(main_stack, f_stack);

					if(preempt.flag == 2)
					{
						hoh_debug("f_stack to main_stack via interrupt handler...");
						f_stack = preempt.f_stack;
					}
					if(shellstate.fiber_isDone[shellstate.head])
					{
						hoh_debug("in main_stack calculation done...");
						// calculation done, enable the output
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
			if(!shellstate.fiber_isDone[i])
			{
				to_zero = false;
				break;
			}
		if(to_zero)
		{
			hoh_debug("mode changed and timer to 0");
			lapic.reset_timer_count(0);
			preempt.flag = 2;
			shellstate.mode = 0x0;
			return;
		}
	}
}
