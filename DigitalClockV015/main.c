/*
 * DigitalClockV015.c
 *
 * Created: 31/07/2023 02:52:53 p. m.
 * Author : dagmtz
 */ 

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "port.h"
#include "usart.h"
#include "clock.h"
#include "ds3231.h"
#include "oled.h"
#include <math.h>

#define RTC_RESPONSE_MAX_TRIES 200
#define COUNTER2_TOP_MS 10

#undef _INIT_TIME

#ifdef _INIT_TIME
#define _INIT_SECONDS	0x00
#define _INIT_MINUTES	0x45
#define _INIT_HOURS		0x18
#define _INIT_DATE		0x31
#define _INIT_MONTH		0x7
#define _INIT_YEAR		0x23
#endif // _INIT_TIME

/* Size for usartBuffer used in main() */
#define BUFF_SIZE   30

/* 1Hz heartbeat coming from RTC */
volatile bool g_heartbeat_1s = false;

/* Increases close to every 1ms. Counts up to 33.55s */
volatile uint16_t g_msCounter1 = 0;
volatile uint16_t g_msCounter2 = 0;
volatile uint16_t g_msCounter3 = 0;

//clock_control_t *gp_clockCtrl;
time_t g_time;
time_t *gp_time = &g_time;
struct tm rtc_time;
struct tm  *gp_rtcTime = &rtc_time;

int main(void)
{	
	/* ============================== SETUP ============================== */
	
	/* Set up external interrupt ( 1Hz from RTC ) */
	PORTD_set_pin_dir( PORTD2, PORT_DIR_IN );
	EICRA = ( _BV( ISC01 ) | _BV( ISC00 ) );
	
	/* Set up Timer/Counter 0 in normal mode with prescaler 64 */
	TCCR0A = 0;
	TCCR0B = ( _BV( CS00 ) | _BV( CS01 ) );

	/* Enable INT0 external interrupt */
	EIMSK = _BV( INT0 );
	
	/* Enable Timer/Counter 0 Overflow interrupt */
	TIMSK0 = _BV( TOIE0 );
	
	PORTB_set_pin_dir( PORTB5, PORT_DIR_OUT );

	DS3231_buffer_t rtcBuffer;
	//clock_control_t clockControl = {
		//{0, 0, BUTTON_NONE, NOT_PRESSED, BRIGHTNESS},	// accumulator, lastReading, buttonPressed, buttonState, settingNow
		//STOP,											// clockState
		//{0, 0, 0},										// time
		//{2000, 1, 1, SATURDAY},							// date
		//DAYS_PER_MONTH_MAX,								// daynsInCurrentMonth
		//9,												// brightness
		//25.5,											// temperature
		//"00:00:00",										// timeString
		//"2000-01-01",									// dateString
		//"------",										// weekdayString
		//"------"										// tempString
	//};
	
#ifdef USART_DEBUG
	/* ---------- USART initialization ---------- */
	uart_init( BAUD_CALC( 115200 ) ); // 8n1 transmission is set as default
	
	stdout = &uart0_io; // attach uart stream to stdout & stdin
	stdin = &uart0_io; // uart0_in and uart0_out are only available if NO_USART_RX or NO_USART_TX is defined
	
	char usartBuffer[BUFF_SIZE];
#endif /* USART_DEBUG */

	sei();
	
#ifdef USART_DEBUG
	uart_puts_P( "\e[2J\e[H" );
	uart_puts_P( "\e[1;32m>USART Ready\r\n" );
#endif /* USART_DEBUG */
	
	/* ---------- I2C initialization ---------- */
#ifdef USART_DEBUG
	uart_puts_P( "\e[1;33m>Initializing I2C\r\n" );
#endif /* USART_DEBUG */
	i2c_init();
	
	/* ---------- RTC initialization ---------- */
	//DS3231_buffer_t *p_rtcBuffer = &rtcBuffer;
	//clock_control_t *p_clock = &clockControl;
	
#ifdef USART_DEBUG
	uart_puts_P( "\e[1;33m>Initializing DS3231 RTC\r\n" );
	uart_puts_P( "\e[0;33m>Waiting for module response...\r\n" );
#endif /* USART_DEBUG */
	
	rtcBuffer.control = rtc_getByte( DS3231_CONTROL );
	rtcBuffer.status = rtc_getByte( DS3231_STATUS );
	
	if ( rtcBuffer.control != 0x00 )
	{
		rtc_setByte( DS3231_CONTROL, 0x00 );
		g_heartbeat_1s = false;
		g_msCounter2 = 0;
		
		/*  If the control register from RTC is different from 0, set it to 0 and 
		wait in a loop until the 1s square wave	triggers the interrupt. If the 
		module doesn't respond, print each second the error to USART. */
		
		while( g_heartbeat_1s == false )
		{
			if ( g_msCounter2 > 1000 )
			{
				g_msCounter2 = 0;
				/* ===== TO DO ===== */
				/* Add an LED to signal errors and turn it on here */
#ifdef USART_DEBUG
				uart_puts_P( "\e[1;31m>RTC is not responding\r\n" );
#endif /* USART_DEBUG */
			}
		}
		
		rtcBuffer.control = rtc_getByte( DS3231_CONTROL );
	}
	
	rtc_setByte( DS3231_CONTROL, 0x00 );
	
	uart_puts_P( "\e[0;33m>Control register value: 0x" );
	utoa( rtcBuffer.control, usartBuffer, 16);
	uart_puts( usartBuffer );
	uart_puts_P( "\r\n" );
	
	uart_puts_P( "\e[0;33m>Status register value: 0x" );
	utoa( rtcBuffer.status, usartBuffer, 16);
	uart_puts( usartBuffer );
	uart_puts_P( "\r\n" );

	/*	When the program gets here, the RTC module should be responding */
	/* ===== TO DO ===== */
	/* Improve method to ensure RTC is working (as well as other modules) */
	
#ifdef _INIT_TIME
	/*	In case this is defined, set initial values to RTC */
	rtc_setByte( DS3231_SECOND, _INIT_SECONDS );
	rtc_setByte( DS3231_MINUTE, _INIT_MINUTES );
	rtc_setByte( DS3231_HOUR, _INIT_HOURS );
	rtc_setByte( DS3231_DATE, _INIT_DATE );
	rtc_setByte( DS3231_MONTH, _INIT_MONTH );
	rtc_setByte( DS3231_YEAR, _INIT_YEAR );
#endif // _INIT_TIME

#ifdef USART_DEBUG
	uart_puts_P( "\e[1;32m>RTC module Ready\r\n" );
#endif /* USART_DEBUG */
	
	
	/* ---------- OLED display initialization ---------- */
#ifdef USART_DEBUG
	uart_puts_P( "\e[1;33m>Initializing OLED Display\r\n" );
	
#endif /* USART_DEBUG */

	oled_init( LCD_DISP_ON ); 
	oled_puts_p( PSTR( "OLED ready!" ) );
	
#ifdef USART_DEBUG
	uart_puts_P( "\e[0m" );
#endif /* USART_DEBUG */
	oled_clrscr();
	
	/* ---------- Core time initialization ---------- */
	rtc_getTime( gp_rtcTime );
	rtc_time.tm_isdst = 0;
	
	g_time = mktime( gp_rtcTime );
	set_system_time( g_time );

	/*	Wait for a heartbeat from RTC to initialize values	*/
	g_heartbeat_1s = false;
	while ( g_heartbeat_1s == false);
	
	/* ============================== MAIN LOOP ============================== */
	
	while (1)
	{	
		if ( g_heartbeat_1s )
		{
			g_heartbeat_1s = false;
			
			uart_puts_P( "\r\e[2K" );
			printf("%s\t%.2fC", ctime(gp_time) , rtc_getTemp());
		}
	}
	
}

ISR( INT0_vect )
{
	g_heartbeat_1s = true;
	g_time++;
	//system_tick();
	PORTB ^= _BV( PORTB5 );	
}

ISR( TIMER0_OVF_vect )
{
	g_msCounter1++;
	g_msCounter2++;
	g_msCounter3++;
}
