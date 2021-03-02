/*
         MEGN540 Mechatronics Lab
    Copyright (C) Andrew Petruska, 2021.
       apetruska [at] mines [dot] edu
          www.mechanical.mines.edu
*/

/*
    Copyright (c) 2021 Andrew Petruska at Colorado School of Mines

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include "MEGN540_MessageHandeling.h"


static inline void MSG_FLAG_Init(MSG_FLAG_t *p_flag) {
    p_flag->active = false;
    p_flag->duration = -1;
    p_flag->last_trigger_time.millisec = 0;
    p_flag->last_trigger_time.microsec = 0;
}


/**
 * Function MSG_FLAG_Execute indicates if the action associated with the message flag should be executed
 * in the main loop both because its active and because its time.
 * @return [bool] True for execute action, False for skip action
 */
bool MSG_FLAG_Execute(MSG_FLAG_t *p_flag) {
    // *** MEGN540  ***
    // THIS FUNCTION WILL BE MOST USEFUL FORM LAB 2 ON.
    // What is the logic to indicate an action should be executed?
    // For Lab 1, ignore the timing part.

    return p_flag->active && (p_flag->duration <= SecondsSince(&(p_flag->last_trigger_time)));

}


/**
 * Function Message_Handling_Init initializes the message handling and all associated state flags and data to their default
 * conditions.
 */
void Message_Handling_Init() {
    // *** MEGN540  ***
    // YOUR CODE HERE. This is where you'd initialize any
    // state machine flags to control your main-loop state machine

    MSG_FLAG_Init(&mf_restart); // needs to be initialized to the default values.

    return;
}

/**
 * Function Message_Handler processes USB messages as necessary and sets status flags to control the flow of the program.
 * It returns true unless the program receives a reset message.
 * @return
 */
void Message_Handling_Task() {
    // *** MEGN540  ***
    // YOUR CODE HERE. I suggest you use your peak function and a switch interface
    // Either do the simple stuff strait up, set flags to have it done later.
    // If it just is a USB thing, do it here, if it requires other hardware, do it in the main and
    // set a flag to have it done here.

    // Check to see if their is data in waiting
    if (!usb_msg_length())
        return; // nothing to process...

    // Get Your command designator without removal so if their are not enough bytes yet, the command persists
    char command = usb_msg_peek();

    // process command
    switch (command) {
        case '*':
            if (usb_msg_length() >= MEGN540_Message_Len('*')) {

                //then process your times...

                // remove the command from the usb recieved buffer using the usb_msg_get() function
                usb_msg_get(); // removes the first character from the received buffer, we already know it was a * so no need to save it as a variable

                // Build a meaningful structure to put your data in. Here we want two floats.
                struct __attribute__((__packed__)) {
                    float v1;
                    float v2;
                } data;

                // Copy the bytes from the usb receive buffer into our structure so we can use the information
                usb_msg_read_into(&data, sizeof(data));

                // Do the thing you need to do. Here we want to multiply
                float ret_val = (data.v1 * data.v2);

                // send response right here if appropriate.
                usb_send_msg("cf", command, &ret_val, sizeof(ret_val));
            }
            break;
        case '/':
            if (usb_msg_length() >= MEGN540_Message_Len('/')) {
                //then process your divide...

                // remove the command from the usb recieved buffer using the usb_msg_get() function
                usb_msg_get(); // removes the first character from the received buffer, we already know it was a / so no need to save it as a variable

                // Build a meaningful structure to put your data in. Here we want two floats.
                struct __attribute__((__packed__)) {
                    float v1;
                    float v2;
                } data;

                // Copy the bytes from the usb receive buffer into our structure so we can use the information
                usb_msg_read_into(&data, sizeof(data));

                // Do the thing you need to do. Here we want to divide
                float ret_val = data.v1 / data.v2;

                // send response right here if appropriate.
                usb_send_msg("cf", command, &ret_val, sizeof(ret_val));
            }
            break;
        case '+':
            if (usb_msg_length() >= MEGN540_Message_Len('+')) {
                //then process your plus...

                // remove the command from the usb recieved buffer using the usb_msg_get() function
                usb_msg_get(); // removes the first character from the received buffer, we already know it was a + so no need to save it as a variable

                // Build a meaningful structure to put your data in. Here we want two floats.
                struct __attribute__((__packed__)) {
                    float v1;
                    float v2;
                } data;

                // Copy the bytes from the usb receive buffer into our structure so we can use the information
                usb_msg_read_into(&data, sizeof(data));

                // Do the thing you need to do. Here we want to add
                float ret_val = data.v1 + data.v2;

                // send response right here if appropriate.
                usb_send_msg("cf", command, &ret_val, sizeof(ret_val));
            }
            break;
        case '-':
            if (usb_msg_length() >= MEGN540_Message_Len('-')) {
                //then process your minus...

                // remove the command from the usb recieved buffer using the usb_msg_get() function
                usb_msg_get(); // removes the first character from the received buffer, we already know it was a - so no need to save it as a variable

                // Build a meaningful structure to put your data in. Here we want two floats.
                struct __attribute__((__packed__)) {
                    float v1;
                    float v2;
                } data;

                // Copy the bytes from the usb receive buffer into our structure so we can use the information
                usb_msg_read_into(&data, sizeof(data));

                // Do the thing you need to do. Here we want to subtract
                float ret_val = data.v1 - data.v2;

                // send response right here if appropriate.
                usb_send_msg("cf", command, &ret_val, sizeof(ret_val));
            }
            break;

        case 't':
            if (usb_msg_length() >= MEGN540_Message_Len('t')) {
                usb_msg_get();

                uint8_t choice = usb_msg_get();

                if (choice == 0) {
                    //set flags so that Current time is sent during next flag execution
                    mf_send_time.active = true;
                    mf_send_time.last_trigger_time = GetTime();
                    mf_send_time.duration = -1;
                }
                if (choice == 1) {
                    //set flags so that time for a float is sent during next flag execution
                    mf_time_float_send.active = true;
                    mf_time_float_send.last_trigger_time = GetTime();
                    mf_time_float_send.duration = -1;
                }
                if (choice == 2) {
                    //set flag so that time to complete a full loop iteration is sent during next flag execution
                    mf_loop_timer.active = true;
                    mf_loop_timer.last_trigger_time = GetTime();
                    mf_loop_timer.duration = -1;

                }
            }
            break;

        case 'T':
            if (usb_msg_length() >= MEGN540_Message_Len('T')) {
                usb_msg_get();

                struct __attribute__((__packed__)) {
                    uint8_t choice;
                    float rate;
                } data;

                // Copy the bytes from the usb receive buffer into our structure so we can use the information
                usb_msg_read_into(&data, sizeof(data));

                if (data.choice == 0) {
                    //set flags so that Current time is sent during next flag execution
                    mf_send_time.active = true;
                    mf_send_time.last_trigger_time = GetTime();
                    mf_send_time.duration = data.rate;
                }
                if (data.choice == 1) {
                    //set flags so that time for a float is sent during next flag execution
                    mf_time_float_send.active = true;
                    mf_time_float_send.last_trigger_time = GetTime();
                    mf_time_float_send.duration = data.rate;
                }
                if (data.choice == 2) {
                    //set flag so that time to complete a full loop iteration is sent during next flag execution
                    mf_loop_timer.active = true;
                    mf_loop_timer.last_trigger_time = GetTime();
                    mf_loop_timer.duration = data.rate;
                }
                if (data.choice == 3) {
                    //deactivate all of the timers
                    mf_send_time.active = false;
                    mf_time_float_send.active = false;
                    mf_loop_timer.active = false;
                }
            }
            break;

        case '~':
            //then process your reset by setting the mf_restart flag
            usb_msg_get();
            mf_restart.active = true;
            break;
<<<<<<< HEAD
        //Lab 2 code
        case 't':
        	{
        	//If the length allotted in the buffer is greater than or equal to the necessary length, go forth
        	if(usb_msg_length()>=MEGN540_Message_Len('t'))
        	{
        		//Get rid of that first usb character we received
        		usb_msg_get();
        		
        		//Create a structure to hold the character
        		struct __attribute__((__packed__)) {char choice;} data;
                
                //Initialize ret_val as a float
                float ret_val;
                
                //If the character is 0,
                if(data.choice==0)
                	{
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set the mf_send_time flag
                	mf_send_time.active=true;
                	
                	//Set ret_val as the current time
                	ret_val=GetTimeSec();
                	usb_send_msg("cc",command,&ret_val,sizeof(ret_val));
                	}
                
                //If the character is 1,
                if(data.choice==1)
                	{
                	//Set the float flag
                	mf_time_float_send.active=true;
                	
                	//Set a second float for comparison
                	float ret_val2;
                	
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set ret_val as the current time
                	ret_val=GetTimeSec();
                	
                	//Send the time, a float (This seems weird, need to change)
                	usb_send_msg("cc",command,&ret_val,sizeof(ret_val));
                	
                	//Set ret_val2 equal to the current time now
                	ret_val2=GetTimeSec();
                	
                	//Set the difference as a float
                	float diff=ret_val2-ret_val;
                	
                	//Send it
                	usb_send_msg("cc",command,&diff,sizeof(diff));
                	}
                
                //If the character is 2,
                if(data.choice==2)
                	{
                	//Set the loop flag
                	mf_loop_timer.active=true;
                	
                	//Set a second float for comparison
                	float ret_val2;
                	
                	//Set ret_val as the current time
                	ret_val=GetTimeSec();
                	
                	//Need to time how long it takes for the send buffer to clear out
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set ret_val2 equal to the current time now
                	ret_val2=GetTimeSec();
                	
                	//Set the difference as a float
                	float diff=ret_val2-ret_val;
                	
                	//Send it
                	usb_send_msg("cc",command,&diff,sizeof(diff));
                	}
                	}
        	}
        	}
        	break;
        case 'T':
        	{
        	//If the length allotted in the buffer is greater than or equal to the necessary length, go forth
        	if(usb_msg_length()>=MEGN540_Message_Len('T'))
        	{
        		//Get rid of that first USB character we received
        		usb_msg_get();
        		
        		//Create structure data to hold the character and float
        		struct __attribute__((__packed__)) {char choice; float ms;} data;
        		
        		//If the time ms is negative or zero,
        		if(data.ms<=0)
        		{
        			//Get out of the loop
        			break;
        		}
                
                //Initialize ret_val as a float
                float ret_val;
                
                //If the character is 0,
                if(data.choice==0)
                {
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set the mf_send_time flag
                	mf_send_time.active=true;
                	
                	//Set ret_val as the current time
                	ret_val=GetTimeSec();
                	usb_send_msg("ccf",command,&ret_val,sizeof(ret_val));
                }
                
                //If the character is 1,
                if(data.choice==1)
                {
                	//Set the float flag
                	mf_time_float_send.active=true;
                	
                	//Set a second float for comparison
                	float ret_val2;
                	
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set float ct as the current time
                	float ct=GetTimeSec();
                	
                	//Send the time, a float (This seems weird, need to change)
                	usb_send_msg("ccf",command,&ct,sizeof(ct));
                	
                	//Set float ct2 equal to the current time now
                	float ct2=GetTimeSec();
                	
                	//Set the difference as ret_val
                	ret_val=ct2-ct1;
                	
                	//Send it
                	usb_send_msg("ccf",command,&ret_val,sizeof(ret_val));
                }
                
                //If the character is 2,
                if(data.choice==2)
                {
                	//Set the loop flag
                	mf_loop_timer.active=true;
                	
                	//Set a second float for comparison
                	float ct2;
                	
                	//Set float ct as the current time
                	float ct=GetTimeSec();
                	
                	//Need to time how long it takes for the send buffer to clear out
                	// Copy the bytes from the usb receive buffer into our structure so we can use the information
                	usb_msg_read_into( &data, sizeof(data) );
                	
                	//Set ct2 equal to the current time now
                	ct2=GetTimeSec();
                	
                	//Set the difference as a float
                	ret_val=ct2-ct1;
                	
                	//Send it
                	usb_send_msg("ccf",command,&ret_val,sizeof(ret_val));
                }
            	/*Repeat the time ret_val every data.ms milliseconds
            	Get the current time and set it as a benchmark*/
            	Time_t benchmark_time=GetTime();
            	
            	/*Create a loop to continually check if data.ms milliseconds have passed
            	since it last checked*/
            	//I hate making an infinite loop like this, but I'm not sure how else to
            	//do it
            	while(0<1)
            	{
            		//Create a Time_t structure current_time to get the current time
            		Time_t current_time=GetTime();
            		
            		//If the time elapsed since the benchmark time is cleanly divisible
            		//by the millisecond time in data,
            		if((current_time-benchmark_time)%data.ms==0)
            		{
            			//Send the time again
            			usb_send_msg("ccf",command,&ret_val,sizeof(ret_val));
            		}
            	}
            }
            
        	}
}
        	break;
=======
>>>>>>> a8a5e8e23625205bd09ff45b2dd633a94716f1dc
        default:
            // What to do if you dont recognize the command character
            usb_msg_get();
            usb_flush_input_buffer();
            usb_send_msg("cc", '?', &command, sizeof(command));
            break;
    }
}



/**
 * Function MEGN540_Message_Len returns the number of bytes associated with a command string per the
 * class documentation;
 * @param cmd
 * @return Size of expected string. Returns 0 if unreconized.
 */
uint8_t MEGN540_Message_Len(char cmd) {
    switch (cmd) {
        case '~':
            return 1;
            break;
        case '*':
            return 9;
            break;
        case '/':
            return 9;
            break;
        case '+':
            return 9;
            break;
        case '-':
            return 9;
            break;
        case 't':
            return 2;
            break;
        case 'T':
            return 6;
            break;
        case 'e':
            return 1;
            break;
        case 'E':
            return 5;
            break;
//        case 'a': return	1; break;
//        case 'A': return 	5; break;
//        case 'w': return	1; break;
//        case 'W': return 	5; break;
//        case 'm': return	1; break;
//        case 'M': return	5; break;
        case 'p':
            return 5;
            break;
        case 'P':
            return 9;
            break;
        case 's':
            return 1;
            break;
        case 'S':
            return 1;
            break;
        case 'q':
            return 1;
            break;
        case 'Q':
            return 5;
            break;
        case 'd':
            return 9;
            break;
        case 'D':
            return 13;
            break;
        case 'v':
            return 1;
            break;
        case 'V':
            return 5;
            break;
        default:
            return 0;
            break;
    }
}
