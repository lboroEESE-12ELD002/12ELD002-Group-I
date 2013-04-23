// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// SimpliciTI functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "display.h"
#include "vti_as.h"
#include "ports.h"
#include "timer.h"
#include "radio.h"

// logic
#include "acceleration.h"
#include "rfsimpliciti.h"
#include "bluerobin.h"
#include "simpliciti.h"
#include "clock.h"
#include "date.h"
#include "alarm.h"
#include "temperature.h"
#include "vti_ps.h"
#include "altitude.h"


// *************************************************************************************************
// Prototypes section
void simpliciti_get_data_callback(void);
void start_simpliciti_tx_only(simpliciti_mode_t mode);
void start_simpliciti_sync(void);


// *************************************************************************************************
// Defines section

// Each packet index requires 2 bytes, so we can have 9 packet indizes in 18 bytes usable payload
#define BM_SYNC_BURST_PACKETS_IN_DATA		(9u)


// *************************************************************************************************
// Global Variable section
struct RFsmpl sRFsmpl;

// flag contains status information, trigger to send data and trigger to exit SimpliciTI
unsigned char simpliciti_flag;

// 4 data bytes to send 
unsigned char simpliciti_data[SIMPLICITI_MAX_PAYLOAD_LENGTH];

// 4 byte device address overrides SimpliciTI end device address set in "smpl_config.dat"
unsigned char simpliciti_ed_address[4];

// Length of data 
unsigned char simpliciti_payload_length;

// 1 = send one or more reply packets, 0 = no need to reply
//unsigned char simpliciti_reply;
unsigned char simpliciti_reply_count;

// 1 = send packets sequentially from burst_start to burst_end, 2 = send packets addressed by their index
u8 		burst_mode;

// Start and end index of packets to send out
u16		burst_start, burst_end;

// Array containing requested packets
u16		burst_packet[BM_SYNC_BURST_PACKETS_IN_DATA];

// Current packet index
u8		burst_packet_index;


// *************************************************************************************************
// Extern section
extern void (*fptr_lcd_function_line1)(u8 line, u8 update);


// *************************************************************************************************
// @fn          reset_rf
// @brief       Reset SimpliciTI data. 
// @param       none
// @return      none
// *************************************************************************************************
void reset_rf(void)
{
	// No connection
	sRFsmpl.mode = SIMPLICITI_OFF;

	// Standard packets are 4 bytes long	
	simpliciti_payload_length = 4;
}


// *************************************************************************************************
// @fn          sx_rf
// @brief       Start SimpliciTI. Button DOWN connects/disconnects to access point.
// @param       u8 line		LINE2
// @return      none
// *************************************************************************************************
void sx_rf(u8 line)
{
	// Exit if battery voltage is too low for radio operation
	if (sys.flag.low_battery) return;

	// Exit if BlueRobin stack is active
	if (is_bluerobin()) return;
  	
  	// Start SimpliciTI in tx only mode
	start_simpliciti_tx_only(SIMPLICITI_ACCELERATION);
}


// *************************************************************************************************
// @fn          sx_ppt
// @brief       Start SimpliciTI. Button DOWN connects/disconnects to access point.
// @param       u8 line		LINE2
// @return      none
// *************************************************************************************************
void sx_ppt(u8 line)
{
	// Exit if battery voltage is too low for radio operation
	if (sys.flag.low_battery) return;

	// Exit if BlueRobin stack is active
	if (is_bluerobin()) return;
  	
  	// Start SimpliciTI in tx only mode
	start_simpliciti_tx_only(SIMPLICITI_BUTTONS);
}


// *************************************************************************************************
// @fn          sx_sync
// @brief       Start SimpliciTI. Button DOWN connects/disconnects to access point.
// @param       u8 line		LINE2
// @return      none
// *************************************************************************************************
void sx_sync(u8 line)
{
	// Exit if battery voltage is too low for radio operation
	if (sys.flag.low_battery) return;

	// Exit if BlueRobin stack is active
	if (is_bluerobin()) return;
  	
  	// Start SimpliciTI in sync mode
	start_simpliciti_sync();
}


// *************************************************************************************************
// @fn          start_simpliciti_tx_only
// @brief       Start SimpliciTI (tx only). 
// @param       simpliciti_state_t		SIMPLICITI_ACCELERATION, SIMPLICITI_BUTTONS
// @return      none
// *************************************************************************************************
void start_simpliciti_tx_only(simpliciti_mode_t mode)
{
  	

	
	
	// Turn on beeper icon to show activity
	display_symbol(LCD_ICON_BEEPER1, SEG_ON_BLINK_ON);
	

	
	// Prepare radio for RF communication
	open_radio();

	// Set SimpliciTI mode
	sRFsmpl.mode = mode;
	
	// Set SimpliciTI timeout to save battery power
	sRFsmpl.timeout = SIMPLICITI_TIMEOUT; 
		
	// Start SimpliciTI stack. Try to link to access point.
	// Exit with timeout or by a button DOWN press.
	if (simpliciti_link())
	{
		

		// Enter TX only routine. This will transfer button events and/or acceleration data to access point.
		simpliciti_main_tx_only();
	}

	// Set SimpliciTI state to OFF
	sRFsmpl.mode = SIMPLICITI_OFF;

	// Stop acceleration sensor
	as_stop();

	// Powerdown radio
	close_radio();
	
	// Clear last button events

	BUTTONS_IFG = 0x00;  
	button.all_flags = 0;
	
	// Clear icons
	display_symbol(LCD_ICON_BEEPER1, SEG_OFF_BLINK_OFF);

	
 	
	
}


// *************************************************************************************************
// @fn          display_rf
// @brief       SimpliciTI display routine. 
// @param       u8 line			LINE2
//				u8 update		DISPLAY_LINE_UPDATE_FULL
// @return      none
// *************************************************************************************************
void display_rf(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_5_0, (u8 *)"   ACC", SEG_ON);
	}
}


// *************************************************************************************************
// @fn          display_ppt
// @brief       SimpliciTI display routine. 
// @param       u8 line			LINE2
//				u8 update		DISPLAY_LINE_UPDATE_FULL
// @return      none
// *************************************************************************************************
void display_ppt(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_5_0, (u8 *)"   PPT", SEG_ON);
	}
}


// *************************************************************************************************
// @fn          display_sync
// @brief       SimpliciTI display routine. 
// @param       u8 line			LINE2
//				u8 update		DISPLAY_LINE_UPDATE_FULL
// @return      none
// *************************************************************************************************
void display_sync(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_5_0, (u8 *)"  SYNC", SEG_ON);
	}
}


// *************************************************************************************************
// @fn          is_rf
// @brief       Returns TRUE if SimpliciTI receiver is connected. 
// @param       none
// @return      u8
// *************************************************************************************************
u8 is_rf(void)
{
	return (sRFsmpl.mode != SIMPLICITI_OFF);
}


// *************************************************************************************************
// @fn          simpliciti_get_ed_data_callback
// @brief       Callback function to read end device data from acceleration sensor (if available) 
//				and trigger sending. Can be also be used to transmit other data at different packet rates.
//				Please observe the applicable duty limit in the chosen ISM band.
// @param       none
// @return      none
// *************************************************************************************************
void simpliciti_get_ed_data_callback(void)
{
	static u8 toggle = 0;
	static u8 aux1 = 0;			
			simpliciti_flag |= SIMPLICITI_TRIGGER_SEND_DATA;
			
			
			if(toggle == 0)
			{
				simpliciti_data[0] = ++aux1;
				simpliciti_data[1] = 1;
				simpliciti_data[2] = 2;
				simpliciti_data[3] = 3;
				simpliciti_flag |= SIMPLICITI_TRIGGER_SEND_DATA;
				toggle = 1;
			}
			else
			{
				simpliciti_flag |= SIMPLICITI_TRIGGER_STOP;
				toggle = 0;
			}
			
			if (aux1 == 4 ) aux1 = 0;
			
//				// Trigger packet sending
//				if(toggle == 0)
//				{
//					simpliciti_data[0] = 1;
//					
//					simpliciti_flag |= SIMPLICITI_TRIGGER_SEND_DATA;
//					toggle++;
//				}
//				else if (toggle ==1)
//				{
//					simpliciti_data[0] = 2;
//					simpliciti_data[1] = simpliciti_data[4] ;
//					simpliciti_data[2] = simpliciti_data[5];
//					simpliciti_data[3] = simpliciti_data[6];
//					simpliciti_flag |= SIMPLICITI_TRIGGER_SEND_DATA;
//					
//					toggle++;
//				}
//				else if (toggle ==2)
//				{
//					simpliciti_data[0] = 3;
//					simpliciti_data[1] = simpliciti_data[7] ;
//					simpliciti_data[2] =  simpliciti_data[8];
//					simpliciti_data[3] = simpliciti_data[9];
//					simpliciti_flag |= SIMPLICITI_TRIGGER_SEND_DATA;
//					toggle++;
//				}
//				else if (toggle ==3)
//				{
//					simpliciti_data[0] = 4;
//					simpliciti_data[1] = simpliciti_data[10];
//					toggle++;
//					
//				}
//				else
//				{
//					simpliciti_flag = SIMPLICITI_TRIGGER_STOP;
//					toggle = 0;
//				}
//					
		
	
	
	
	
}


// *************************************************************************************************
// @fn          start_simpliciti_sync
// @brief       Start SimpliciTI (sync mode). 
// @param       none
// @return      none
// *************************************************************************************************
void start_simpliciti_sync(void)
{
  	

	display_symbol(LCD_ICON_BEEPER1, SEG_ON_BLINK_ON);
	
	// Debounce button event
	//Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_OUT));

	// Prepare radio for RF communication
	open_radio();

	// Set SimpliciTI mode
	sRFsmpl.mode = SIMPLICITI_SYNC;
	
	// Set SimpliciTI timeout to save battery power
	sRFsmpl.timeout = SIMPLICITI_TIMEOUT; 
		
	// Start SimpliciTI stack. Try to link to access point.
	// Exit with timeout or by a button DOWN press.
	if (simpliciti_link())
	{
		// Enter sync routine. This will send ready-to-receive packets at regular intervals to the access point.
		// The access point replies with a command (NOP if no other command is set)
		simpliciti_main_sync();
	}

	// Set SimpliciTI state to OFF
	sRFsmpl.mode = SIMPLICITI_OFF;

	// Powerdown radio
	close_radio();
	
	
	BUTTONS_IFG = 0x00;  
	button.all_flags = 0;
	
	// Clear icons
	display_symbol(LCD_ICON_BEEPER1, SEG_OFF_BLINK_OFF);
	
	
}



// *************************************************************************************************
// @fn          simpliciti_sync_decode_ap_cmd_callback
// @brief       For SYNC mode only: Decode command from access point and trigger actions. 
// @param       none
// @return      none
// *************************************************************************************************
	
	void simpliciti_sync_decode_ap_cmd_callback(void)
{
	u8 i;
	s16 t1, offset;
	
	// Default behaviour is to send no reply packets
	simpliciti_reply_count = 1;
	
	display_value1(LCD_SEG_L2_3_2, simpliciti_data[0]	, 2, 0);								
	simpliciti_reply_count = 1;
	simpliciti_flag =  SIMPLICITI_TRIGGER_STOP;
	
}


// *************************************************************************************************
// @fn          simpliciti_sync_get_data_callback
// @brief       For SYNC mode only: Access point has requested data. Copy this data into the TX buffer now. 
// @param       u16 index		Index used for memory requests
// @return      none
// *************************************************************************************************
void simpliciti_sync_get_data_callback(unsigned int index)
{
	u8  x;
	for ( x= 1; x<14;x++)
		simpliciti_data[x] = x;
	simpliciti_reply_count = 1;
}

