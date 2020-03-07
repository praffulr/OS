#include "labs/coroutine.h"

bool equal_to1(char* str1, char* str2, int len)
{
	for(int i=0; i<len; i++)
	{
		if(str1[i] != str2[i]) return false;
		else if(str1[i] == '\0') return true;
	}
	return true;
}

int get_string_length1(char *string)
{
	int length=0;
	for(; string[length] != '\0'; length++){}
	return length;
}

int char_array_to_int1(char *string)
{
	int index = get_string_length1(string)-1, factor=1, number=0;
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

int get_num_digit1(int n)
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

char digit_to_char1(int n)
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

void int_to_string_write1(char* arr, int n)
{
	if(n==0)
	{
		arr[0] = '0';
		arr[1] = '\0';
		return;
	}
	int digit_length = get_num_digit1(n);
	int index = digit_length;
	//char int_string[digit_length+1];
	arr[index--] = '\0';
	while(n != 0)
	{
		int lsb = n%10;
		arr[index--] = digit_to_char1(lsb);

		n = n/10;
	}
}


void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){

    //insert your code here

    //copying local data
    int& i = f_locals.i;
    int& n = f_locals.n;
    int& output_iterator = f_locals.output_iterator;
    if(shellstate.exec_command == 0x01 && shellstate.mode == 0x01)
    {
      if(equal_to1(shellstate.command, "pfactorsc", 10))
      {
        h_begin(f_coro);
        //function implementation

        // identify number or arguments
        int num_arguments=0;
        for(num_arguments=0x0; !equal_to1(shellstate.arguments[num_arguments], "", 1) and num_arguments <= 0x9; num_arguments++){}
        // user didn't provided any input


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
        }
        else
        {
          n = char_array_to_int1(shellstate.arguments[0]);
          if(n==-1)
          {
            shellstate.output[0] = 'E';
            shellstate.output[1] = 'R';
            shellstate.output[2] = 'R';
            shellstate.output[3] = 'O';
            shellstate.output[4] = 'R';
            shellstate.output[5] = '\0';
          }
          output_iterator = 0;

          i = 2;
          while(i<=n)
          {
            bool is_prime = true;
            for(int j=2; j<i; j++)
            {
              if(i%j==0)
              {
                is_prime = false;
                break;
              }
            }
            if(is_prime && (n%i==0))
            {
              int num_digit = get_num_digit1(i);
              if((output_iterator + num_digit+2) >= 80)
              {
                hoh_debug("break");
                break;
              }
              int_to_string_write1(shellstate.output+output_iterator, i);
              output_iterator += (1+get_num_digit1(i));
              shellstate.output[output_iterator-1] = ' ';
            }

            h_yield(f_coro);

            i = i+1;

          }
          shellstate.output[output_iterator] = '\0';
        }

        //function implementation ends

        shellstate.enable_output = 0x01;


        // clearing command and arguments of shellstate
        for(int i=0; i<10; i++)
        {
          for(int j=0; j<10; j++)
            shellstate.arguments[i][j] = '\0';
          shellstate.command[i] = '\0';
        }
        shellstate.input_ctr = 0;
        //for displaying keypresses while a long task is running
        shellstate.line ++;
        shellstate.column=0;
        shellstate.exec_command = 0x0;
        shellstate.mode = 0x0;
        hoh_debug("end_co routine");
      }
      // h_end(f_coro);
      coroutine_reset(f_coro);
    }
}
