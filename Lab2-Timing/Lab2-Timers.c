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

#include "../c_lib/SerialIO.h"
#include "../c_lib/Timing.h"
#include "../Lab0-Blink/led_interface.h"
#include "../c_lib/MEGN540_MessageHandeling.h"
#include <math.h>

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
             // initialize timer zero functionality
    USB_SetupHardware();   // initialize USB

    GlobalInterruptEnable(); // Enable Global Interrupts for USB and Timer etc.
    SetupTimer0();

    bool timedLoop = false;


    while(true){
        //Need to maintain these two function calls to retain base serial functionality
        USB_Upkeep_Task();
        Message_Handling_Task();

        if(MSG_FLAG_Execute(&mf_restart)){
            USB_SetupHardware();
            GlobalInterruptEnable();
            SetupTimer0();
            mf_restart.active = false;
        }

        if (MSG_FLAG_Execute(&mf_send_time)){
            float curTime = GetTimeSec();

            if (mf_send_time.duration <= 0){
                mf_send_time.active = false;
                usb_send_msg("cf",'t', &curTime, sizeof(curTime));
            }else{
                mf_send_time.last_trigger_time = GetTime();
                struct __attribute__((__packed__)) {
                    float rate;
                    float time;
                } data;
                data.rate = mf_send_time.duration;
                data.time = curTime;

                usb_send_msg("cff",'T', &data, sizeof(data));
            }
        }

        if (MSG_FLAG_Execute(&mf_time_float_send)){
            Time_t start = GetTime();
            float testFloat = 3.1415;
            usb_send_msg("cf",'t', &testFloat, sizeof(testFloat));

            while(usb_send_msg_length() > 0){
                //waiting until message has been sent
                usb_write_next_byte();
            }
            float sendTime = SecondsSince(&start);

            if (mf_time_float_send.duration <= 0){
                usb_send_msg("cf",'t', &sendTime, sizeof(sendTime));
                mf_time_float_send.active = false;
            }else{
                struct __attribute__((__packed__)) {
                    float rate;
                    float time;
                } data;
                data.rate = mf_time_float_send.duration;
                data.time = sendTime;

                usb_send_msg("cff",'T', &data, sizeof(data));
                mf_time_float_send.last_trigger_time = GetTime();
            }
        }

        if (MSG_FLAG_Execute(&mf_loop_timer)) {
            static Time_t loopTimer;

            if (timedLoop){
                float durTime = SecondsSince(&loopTimer);
                timedLoop = !timedLoop;

                if (mf_loop_timer.duration <= 0){
                    usb_send_msg("cf",'t', &durTime, sizeof(durTime));
                    mf_loop_timer.active = false;
                }else{
                    struct __attribute__((__packed__)) {
                        float rate;
                        float time;
                    } data;
                    data.rate = mf_loop_timer.duration;
                    data.time = durTime;

                    usb_send_msg("cff",'T', &data, sizeof(data));
                    mf_loop_timer.last_trigger_time = GetTime();
                }
            }else{
                loopTimer = GetTime();
                timedLoop = !timedLoop;
            }
        }

    }
}
