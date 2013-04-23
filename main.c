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
// Initialization and control of application.
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"
#include <string.h>

// driver
#include "clock.h"
#include "display.h"
#include "vti_as.h"
#include "vti_ps.h"
#include "radio.h"
#include "buzzer.h"
#include "ports.h"
#include "timer.h"
#include "pmm.h"
#include "rf1a.h"


// logic
#include "menu.h"
#include "date.h"
#include "alarm.h"
#include "stopwatch.h"
#include "battery.h"
#include "temperature.h"
#include "altitude.h"
#include "battery.h"
#include "acceleration.h"
#include "bluerobin.h"
#include "rfsimpliciti.h"
#include "simpliciti.h"



// *************************************************************************************************
// Prototypes section
void init_application(void);
void init_global_variables(void);
void wakeup_event(void);
void process_requests(void);
void display_update(void);
void idle_loop(void);
void configure_ports(void);
void read_calibration_values(void);


void timer_function(void);
void sw1pressed (void);
void clear_variables(void);
void oneminutetone(void);
void accelerationtest(void);
void countremaining(void);
void timeelapsed(void);
u8 measure_acceleration(void);
void display_timeRafael(void); 
void start_timer(u16 tick);


// *************************************************************************************************
// Defines section

// Number of calibration data bytes in INFOA memory
#define CALIBRATION_DATA_LENGTH		(13u)


// *************************************************************************************************
// Global Variable section

// Variable holding system internal flags
volatile s_system_flags sys;

// Variable holding flags set by logic modules 
volatile s_request_flags request;

// Variable holding message flags
volatile s_message_flags message;

// Global radio frequency offset taken from calibration memory
// Compensates crystal deviation from 26MHz nominal value
u8 rf_frequoffset;
u16 auxaccel;

// Function pointers for LINE1 and LINE2 display function 
void (*fptr_lcd_function_line1)(u8 line, u8 update);
void (*fptr_lcd_function_line2)(u8 line, u8 update);


// *************************************************************************************************
// Extern section

extern void start_simpliciti_sync(void);

extern u16 ps_read_register(u8 address, u8 mode);
extern u8 ps_write_register(u8 address, u8 data);


//**************************************************************************************************
// Variables created by me (Rafael)
struct Timecounting
{
	u8 TelapsedSec; // Seconds of time elapsed
	u8 TelapsedMin; // Minutes of time elapsed
	u8 CremainSec;  // Seconds of count remaining
	u8 CremainMin;  // Minutes of count remaining
	u8 Tickcount;  // Counts the ticks of the count remaining
	u8 TickElapsed; //Counts the ticks of time elapsed
	u16 tick;
	u16 Oneminutetick; //Tick of the one minute tone
	u8 Oneminutesec; //How many seconds are being counted during the one minute tone
} t1;

struct Control
{
	u16 sw1tick; // To count the time after the button is pressed
	u16 sw1count; // Count the number of times that SW1 is pressed
	u16 period; // Define the period of counting according with some conditions
	u16 hbp; // Define to help in the heart beep
	u8 sbp; // Help with the sound beep
	u8 stg; // toggle the buzzer
	u8 play; // Counting time
	u8 state; // State
	u16 acceltick;
	u16 maxaccel;
	u8 Display;
	
}c1;
#define ACCELTHRESHOLD 1000  // Threshold of acceleration, the countdown will be accelerated if the
//The best value is 1000							// acceleration axes reads above ths value.
#define TOACCELPERIOD  400    //The countdown will accelerate further if the acceleration axes
							// remain above the AccelThreshold for more than it (in 10m seconds)						
#define TOZEROVALUE 500	// The count will reach zero after this period
#define TONORMALPERIOD 500 // Return to normal count rate after this period with acceleariont axes below than the accethreshold

// Periods (unit: 10 ms)
#define NORMALPERIOD 100 
#define ACCELPERIOD 50
#define ACCELPERIOD2 20						
#define SW1PERIOD  200


// *************************************************************************************************
// @fn          main
// @brief       Main routine
// @param       none
// @return      none
// *************************************************************************************************
int main(void)
{
	volatile u8 ps;
	
	u16 value;
	u16 aux1;
	// Init MCU 
	init_application();
	init_global_variables();
	clear_variables();
	value = TA0R + t1.tick;
	c1.state = 1;
	c1.Display = 0;
	as_start();
	aux1 = 0;
	while(1)
	{
			
		if (c1.play == 1)
			if (value <= TA0R)
			{
				if( 65535 - TA0R < t1.tick)
				{
					value = t1.tick;
					TA0R = 0;
				}
				else		
					value = TA0R + t1.tick;
				timer_function();
			}
		
		// Reseting the Watchdog counter
		idle_loop();
		
		// Detecting if any button is pressed
    	if (button.all_flags || sys.all_flags) wakeup_event();
		
		display_timeRafael();
			
		switch (c1.state)
		{
			case 0:
			case 1:
				c1.play = 0;
			break;
			case 2:
				c1.play = 1;
				c1.period = NORMALPERIOD;
				if (measure_acceleration() == 1)
					c1.state = 3;
			break;
			case 3:
				c1.play = 1;
			 	c1.period = ACCELPERIOD;
			 	if (measure_acceleration() == 0)
			 	{
			 		c1.state = 5;
			 		c1.acceltick=0;
			 	}
			 break;
			 case 4:
			 	 c1.play = 1;
				 c1.period = ACCELPERIOD2;
			 	if(measure_acceleration() == 0)
			 	{
			 		c1.state = 5;
			 		c1.acceltick= 0;
			 	}
			break;
			case 5:
				c1.play = 1;
				if (measure_acceleration() == 1)
				{
					c1.acceltick= 0;
					if (c1.period == ACCELPERIOD2)
					{
						
						c1.state = 4;
						
					}
					else if (c1.period == ACCELPERIOD)
					{
						
						c1.state = 3;
					}
				}
			break;
			case 6:
				c1.play = 1;
			break;
		}
	}
}
// *************************************************************************************************
// @fn          init_application
// @brief       Initialize the microcontroller.
// @param       none
// @return      none
// *************************************************************************************************
void init_application(void)
{
	volatile unsigned char *ptr;
	  
	// ---------------------------------------------------------------------
	// Enable watchdog
	
	// Watchdog triggers after 16 seconds when not cleared
#ifdef USE_WATCHDOG		
	WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK;
#else
	WDTCTL = WDTPW + WDTHOLD;
#endif
	
	// ---------------------------------------------------------------------
	// Configure PMM
	SetVCore(3);
	
	// Set global high power request enable
	PMMCTL0_H  = 0xA5;
	PMMCTL0_L |= PMMHPMRE;
	PMMCTL0_H  = 0x00;	

	// ---------------------------------------------------------------------
	// Enable 32kHz ACLK	
	P5SEL |= 0x03;                            // Select XIN, XOUT on P5.0 and P5.1
	UCSCTL6 &= ~XT1OFF;        				  // XT1 On, Highest drive strength
	UCSCTL6 |= XCAP_3;                        // Internal load cap

	UCSCTL3 = SELA__XT1CLK;                   // Select XT1 as FLL reference
	UCSCTL4 = SELA__XT1CLK | SELS__DCOCLKDIV | SELM__DCOCLKDIV;      
	
	// ---------------------------------------------------------------------
	// Configure CPU clock for 12MHz
	_BIS_SR(SCG0);                  // Disable the FLL control loop
	UCSCTL0 = 0x0000;          // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_5;       // Select suitable range
	UCSCTL2 = FLLD_1 + 0x16E;  // Set DCO Multiplier
	_BIC_SR(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
    __delay_cycles(250000);
  
	// Loop until XT1 & DCO stabilizes, use do-while to insure that 
	// body is executed at least once
	do
	{
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	} while ((SFRIFG1 & OFIFG));	

	
	// ---------------------------------------------------------------------
	// Configure port mapping
	
	// Disable all interrupts
	__disable_interrupt();
	// Get write-access to port mapping registers:
	PMAPPWD = 0x02D52;
	// Allow reconfiguration during runtime:
	PMAPCTL = PMAPRECFG;

	// P2.7 = TA0CCR1A or TA1CCR0A output (buzzer output)
	ptr  = &P2MAP0;
	*(ptr+7) = PM_TA1CCR0A;
	P2OUT &= ~BIT7;
	P2DIR |= BIT7;

	// P1.5 = SPI MISO input
	ptr  = &P1MAP0;
	*(ptr+5) = PM_UCA0SOMI;
	// P1.6 = SPI MOSI output
	*(ptr+6) = PM_UCA0SIMO;
	// P1.7 = SPI CLK output
	*(ptr+7) = PM_UCA0CLK;

	// Disable write-access to port mapping registers:
	PMAPPWD = 0;
	// Re-enable all interrupts
	__enable_interrupt();
	
	// ---------------------------------------------------------------------
	// Configure ports

	// ---------------------------------------------------------------------
	// Reset radio core
	radio_reset();
	radio_powerdown();	
	
	// ---------------------------------------------------------------------
	// Init acceleration sensor
	as_init();
	
	// ---------------------------------------------------------------------
	// Init LCD
	lcd_init();
  
	// ---------------------------------------------------------------------
	// Init buttons
	init_buttons();

	// ---------------------------------------------------------------------
	// Configure Timer0 for use by the clock and delay functions
	Timer0_Init();

	
	// ---------------------------------------------------------------------
	// Init pressure sensor
	ps_init();
}


// *************************************************************************************************
// @fn          init_global_variables
// @brief       Initialize global variables.
// @param       none
// @return      none
// *************************************************************************************************
void init_global_variables(void)
{

	// Init system flags
	button.all_flags 	= 0;
	sys.all_flags 		= 0;
	request.all_flags 	= 0;
	//display.all_flags 	= 0;
	//message.all_flags	= 0;
	
	// Force full display update when starting up
	//display.flag.full_update = 1;

#ifndef ISM_US
	// Use metric units when displaying values
	sys.flag.use_metric_units = 1;
#endif
	
	// Read calibration values from info memory
	read_calibration_values();
	
	// Set system time to default value
	reset_clock();
	
	// Set date to default value
	reset_date();
	
	// Set alarm time to default value 
	reset_alarm();
	
	// Set buzzer to default value
	reset_buzzer();
	
	// Reset stopwatch
	//reset_stopwatch();
	
	// Reset altitude measurement
	reset_altitude_measurement();
	
	// Reset acceleration measurement
	reset_acceleration();
	
	// Reset BlueRobin stack
	reset_bluerobin();

	// Reset SimpliciTI stack
	reset_rf();
	
	// Reset temperature measurement 
	reset_temp_measurement();

	// Reset battery measurement
	reset_batt_measurement();
	battery_measurement();
}


// *************************************************************************************************
// @fn          wakeup_event
// @brief       Process external / internal wakeup events.
// @param       none
// @return      none
// *************************************************************************************************
void wakeup_event(void)
{
	// Enable idle timeout
	
	sys.flag.idle_timeout_enabled = 1;
	
	if(button.flag.star_long ) 
	{
	
	}
	else if(button.flag.star)// SW1
	{
		display_symbol(LCD_ICON_ALARM, SEG_ON);
		switch (c1.state)
		{
			
		case 0:
		case 1:
			c1.state = 2;
							
			break;
		case 2:
			c1.state = 7;
			c1.sw1count++;
			c1.sw1tick = 0;
			if (c1.sw1count == 10)
				c1.state = 6;
			break;
		case 3:
			c1.state = 7;
			c1.sw1count++;
			c1.sw1tick = 0;
			if (c1.sw1count == 10)
				c1.state = 6;
			break;
		case 4:
			c1.state = 7;
			c1.sw1count++;
			c1.sw1tick = 0;
			if (c1.sw1count == 10)
				c1.state = 6;
			
			break;
		case 5:
			c1.state = 7;
			c1.sw1count++;
			c1.sw1tick = 0;
			if (c1.sw1count == 10)
				c1.state = 6;
			
			break;
		case 7:
			c1.sw1count++;
			c1.sw1tick = 0;
		break;
		}
		display_symbol(LCD_ICON_ALARM, SEG_OFF);
		button.flag.star = 0;
	}
	else if (button.flag.num) //SW2
	{
		// First part of SW2 button.
		switch (c1.state)
		{
			case 0:
			case 1:
			t1.CremainMin--;
			if(t1.CremainMin == 0)
				t1.CremainMin = 10;
			break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 7:
			if (c1.sbp ==0)
				c1.sbp = 1;
			else
				c1.sbp = 0;
			break;
			
		}
		
		
		button.flag.num = 0;
			
	}
	else if(button.flag.up) //button pause	
	{
		switch (c1.state)
		{
			
			case 2:
			case 3:
			case 4:
			case 5:
			case 7:
				stop_buzzer();
				display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
				display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
				display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
				c1.state = 8;
				break;
			case 8:
				c1.state = 2;
			break;
		}
	
				
		button.flag.up = 0;
	}
	else if(button.flag.down)
	{
		c1.Display++;
		if (c1.Display >=3)
			c1.Display = 0;
		button.flag.down = 0;
	
	}	
		
	
	//	 Process internal events
	if (sys.all_flags)
	{
		// Idle timeout ---------------------------------------------------------------------
		if (sys.flag.idle_timeout)
		{
			// Clear timeout flag	
			sys.flag.idle_timeout = 0;	
			
		}
	}
	
	
	sys.flag.idle_timeout_enabled = 0;

	}
	



// *************************************************************************************************
// @fn          idle_loop
// @brief       Go to LPM. Service watchdog timer when waking up.
// @param       none
// @return      none
// *************************************************************************************************
void idle_loop(void)
{

#ifdef USE_WATCHDOG		
	// Service watchdog
	WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK + WDTCNTCL;
#endif
}


// *************************************************************************************************
// @fn          read_calibration_values
// @brief       Read calibration values for temperature measurement, voltage measurement
//				and radio from INFO memory.
// @param       none
// @return      none
// *************************************************************************************************
void read_calibration_values(void)
{
	u8 cal_data[CALIBRATION_DATA_LENGTH];		// Temporary storage for constants
	u8 i;
	u8 * flash_mem;         					// Memory pointer
	
	// Read calibration data from Info D memory
	flash_mem = (u8 *)0x1800;
	for (i=0; i<CALIBRATION_DATA_LENGTH; i++)
	{
		cal_data[i] = *flash_mem++;
	}
	
	if (cal_data[0] == 0xFF) 
	{
		// If no values are available (i.e. INFO D memory has been erased by user), assign experimentally derived values	
		rf_frequoffset	= 4;
		sTemp.offset 	= -250;
		sBatt.offset 	= -10;	
		simpliciti_ed_address[0] = 0x79;
		simpliciti_ed_address[1] = 0x56;
		simpliciti_ed_address[2] = 0x34;
		simpliciti_ed_address[3] = 0x12;
		sAlt.altitude_offset	 = 0;
	}
	else
	{
		// Assign calibration data to global variables
		rf_frequoffset	= cal_data[1];	
		// Range check for calibrated FREQEST value (-20 .. + 20 is ok, else use default value)
		if ((rf_frequoffset > 20) && (rf_frequoffset < (256-20)))
		{
			rf_frequoffset = 0;
		} 
		sTemp.offset 	= (s16)((cal_data[2] << 8) + cal_data[3]);
		sBatt.offset 	= (s16)((cal_data[4] << 8) + cal_data[5]);
		simpliciti_ed_address[0] = cal_data[6];
		simpliciti_ed_address[1] = cal_data[7];
		simpliciti_ed_address[2] = cal_data[8];
		simpliciti_ed_address[3] = cal_data[9];
		// S/W version byte set during calibration?
		if (cal_data[12] != 0xFF)
		{
			sAlt.altitude_offset = (s16)((cal_data[10] << 8) + cal_data[11]);;
		}
		else
		{
			sAlt.altitude_offset = 0;	
		}
	}
}


void timer_function(void)
{
		switch (c1.state)
		{	
			case 2:
				timeelapsed();
				countremaining();
			break;
			case 3:
				accelerationtest();
				countremaining();
				accelerationtest();
			case 4:
				accelerationtest();
				countremaining();
				accelerationtest();
			case 5:
				timeelapsed();
				countremaining();
				accelerationtest();
			break;
			case 6:
				oneminutetone();
			break;
			case 7:
				sw1pressed();
				timeelapsed();
				countremaining();
			break;	
		}			
}

// *************************************************************************************************
// @fn          to_lpm
// @brief       Go to LPM0/3. 
// @param       none
// @return      none
// *************************************************************************************************
void to_lpm(void)
{
	// Go to LPM3
	_BIS_SR(LPM3_bits + GIE); 
	__no_operation();
}

void display_timeRafael(void)
{

	if (c1.Display == 0)
	{
		display_value1(LCD_SEG_L1_3_2, t1.TelapsedMin, 2, 0);
		display_value1(LCD_SEG_L1_1_0, t1.TelapsedSec, 2, 0);
	}
	else if (c1.Display == 1)
		display_value1(LCD_SEG_L1_3_0, c1.maxaccel, 4, 0);
	else
	{
		display_value1(LCD_SEG_L1_3_2, c1.state, 2, 0);
		display_value1(LCD_SEG_L1_1_0, c1.sw1count, 2, 0);
	}
	
		
	display_value1(LCD_SEG_L2_3_2, t1.CremainMin, 2, 0);
	display_value1(LCD_SEG_L2_1_0, t1.CremainSec, 2, 0);
}


u8 measure_acceleration(void)
{	    
	    u8 value;
	    u16 maxaccel;
	    u16 aux;
	    u8 i;
	    value = 0;
    	maxaccel = 0;
    	aux = 0;
    	as_get_data(sAccel.xyz);
    	for ( i=0, aux = 0; i<3; i++)
    	{
    		aux = convert_acceleration_value_to_mgrav(sAccel.xyz[i]);	
    		if (aux > maxaccel)
    		{
    			maxaccel = aux;
    		
    		}
    		
    	}
    		c1.maxaccel = maxaccel;
    
    	if (maxaccel > ACCELTHRESHOLD)
    		value = 1;
    	else
    		value = 0;
    	return value;
  	
}

void timeelapsed(void)
{
	t1.TickElapsed++;
	if (t1.TickElapsed >= NORMALPERIOD)
	{
			t1.TickElapsed = 0;
			if (t1.TelapsedSec <59)
				t1.TelapsedSec++;
			else
			{
				t1.TelapsedSec = 0;
				t1.TelapsedMin++;
			}
	}
}

void countremaining(void)
{
	t1.Tickcount++;
	if (t1.Tickcount >= c1.period) //if the number of ticks reaches the period
	{
			if (c1.hbp == 0)
			{
				display_symbol(LCD_ICON_HEART, SEG_ON);
				c1.hbp = 1;
			}
			else
			{
				display_symbol(LCD_ICON_HEART, SEG_OFF);
				c1.hbp = 0;
			}
			
			
			
			if (c1.sbp ==1)
			{
				
				if (c1.stg == 1)
				{
					c1.stg =0;
					display_symbol(LCD_ICON_BEEPER1, SEG_ON);
					display_symbol(LCD_ICON_BEEPER2, SEG_ON);
					display_symbol(LCD_ICON_BEEPER3, SEG_ON);
					start_buzzer(10, '20', '20');
				}
				
				else
				{
					display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
					display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
					display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
					c1.stg = 1;
					stop_buzzer();
				}
			}
			
			t1.Tickcount = 0;
			if(t1.CremainSec>0)
			{
				t1.CremainSec--;
				
			}
			else
			{
				if(t1.CremainMin>0)
				{
					t1.CremainSec = 59;
					t1.CremainMin--;
					
				}
				else
					c1.state = 6; // it means Finished
			}
	}
}

void accelerationtest(void)
{
	c1.acceltick++;
	
	switch (c1.state)
	{
		case 3:
			if (c1.acceltick > TOACCELPERIOD)
			{
				c1.state = 4;
				c1.acceltick = 0;
			}
		break;
		case 4:
			if (c1.acceltick > TOZEROVALUE)
			{
				c1.acceltick = 0;
				c1.state = 6;
				t1.CremainMin = 0;
				t1.CremainSec = 0;
				
			}
		break;
		case 5:
			if (c1.acceltick >TONORMALPERIOD)
			{
				c1.state = 2;
				c1.acceltick = 0;
				
			}
			
		break;
		
	}
						
}
void oneminutetone(void)
{
	t1.CremainMin = 0;
	t1.CremainSec = 0;
	start_buzzer (10,'20','0');
	display_symbol(LCD_ICON_BEEPER1, SEG_ON);
	display_symbol(LCD_ICON_BEEPER2, SEG_ON);
	display_symbol(LCD_ICON_BEEPER3, SEG_ON);
	t1.Oneminutetick++;
	if (t1.Oneminutetick >= NORMALPERIOD)
	{
		t1.Oneminutetick = 0;
		t1.Oneminutesec++;
		if (t1.Oneminutesec >59)
		{
			
			t1.Oneminutetick = 0;
			c1.state = 1;
			stop_buzzer();
			display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
			display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
			display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
			clear_variables();
			
		}
	}
	
	
}

void clear_variables(void)
{
	t1.TelapsedSec = 0; // Seconds of time elapsed
	t1.TelapsedMin = 0; // Minutes of time elapsed2*
	t1.CremainSec = 0;  // Seconds of count remaining
	t1.CremainMin = 1;  // Minutes of count remaining
	t1.Tickcount = 0;
	t1.TickElapsed =0;
	t1.Oneminutetick = 0;
	t1.tick = 328;
	t1.Oneminutesec = 0;
	c1.sw1tick = 0;
	c1.sw1count = 0; // Count the number of times that SW1 is pressed
	c1.period = NORMALPERIOD; // Define the period of counting according with some conditions
	c1.state = 1;	
	c1.hbp = 0; // Auxiliar variable to help the heart beep icon.
	c1.sbp = 1;
	c1.stg = 1;
	c1.play = 0;
	
	c1.acceltick = 0;
	stop_blink();
	
	c1.maxaccel = 0;
		
	clear_display();
	
	 sAccel.data = 0;
}

void sw1pressed (void)
{
	c1.sw1tick++;

	if (c1.sw1tick > SW1PERIOD)
	{
		c1.sw1tick = 0;
		switch (c1.state)
		{
			
			case 7:
				if (c1.sw1count == 10)
					c1.state = 6; 
				else if (c1.sw1count>=5)
					c1.state = 2;
				else
				{
					if( c1.period == ACCELPERIOD)
						c1.state = 3;
					else if( c1.period == ACCELPERIOD2)
						c1.state = 4;
					else
						c1.state = 2;
				}	
			break;
		}
		c1.sw1count = 0;
			
	}
}



