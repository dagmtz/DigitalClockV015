/*
 * clock.h
 *
 * Created: 24/07/2023 11:59:08 a. m.
 *  Author: dagmtz
 */ 


#ifndef CLOCK_H_
#define CLOCK_H_


#define SECONDS_PER_MINUTE  60U
#define MINUTES_PER_HOUR    60U
#define HOURS_PER_DAY       24U
#define DAYS_PER_MONTH_MAX  31U
#define MONTHS_PER_YEAR     12U
#define UPPER_TOP_YEARS     2999U
#define LOWER_TOP_YEARS     2000U
#define SECONDS_FROM_1970_TO_2000 946684800

#define DEBOUNCE_THRESHOLD  20U
#define HOLD_THRESHOLD     280U
#define REPEAT_THRESHOLD    66U

#define BUTTON_SETUP  4U
#define BUTTON_UP     2U
#define BUTTON_DOWN   1U
#define BUTTON_NONE   0U

#define BUTTON_SETUP_BIT    _BV( BUTTON_SETUP )
#define BUTTON_UP_BIT       _BV( BUTTON_UP )
#define BUTTON_DOWN_BIT     _BV( BUTTON_DOWN )
#define BUTTONS_MASK  ( _BV(PORTC2) | _BV(PORTC1) | _BV(PORTC0) )

#define UART_BAUD_RATE 19200

#include "ds3231.h"

typedef enum
{
	MINUS,
	NO_SIGN,
	PLUS
} sign_t;

typedef enum
{
	BRIGHTNESS,
	SECOND,
	MINUTE,
	HOUR,
	DAY,
	MONTH,
	YEAR,
} clock_settings_t;

typedef enum
{
	STOP,
	STANDBY,
	RUNNING,
	SETUP
} clock_state_t;

typedef enum
{
	NOT_PRESSED,
	SINGLE,
	HOLD1,
	REPEAT,
	HOLD2,
	INVALID
} button_state_t;

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} time_hms_t;

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t weekday;
} date_ymd_t;

typedef struct
{
	uint8_t accumulator;
	uint8_t lastReading;
	uint8_t currReading;
	button_state_t buttonState;
	clock_settings_t settingNow;
} settings_control_t;

typedef struct
{
	settings_control_t settings;
	clock_state_t clockState;
	time_hms_t time;
	date_ymd_t date;
	uint8_t daysInCurrentMonth;
	uint8_t brightness;
	float temperature;
	char timeString[30];
	char dateString[32];
	char weekdayString[10];
	char tempString[10];
} clock_control_t;


void clockToLED( clock_control_t *p_clock );
void clockToOLED( clock_control_t *p_clock );
void clockToUSART( clock_control_t *p_clock );
void settingsToUSART( clock_control_t *p_clock );
void timeToString( clock_control_t *p_clock );
void dateToString( clock_control_t *p_clock );
void weekdayToString( clock_control_t *p_clock );
void readButtons( settings_control_t *p_settings );
void updateConfig( clock_control_t *p_clock );
void update_clock( clock_control_t *p_clock, clock_settings_t setting, const sign_t sign, const bool affectNextUnit );

uint8_t getWeekday( date_ymd_t *date );
uint8_t tickSeconds( clock_control_t *p_clock );


#endif /* CLOCK_H_ */