/*
 * ds3231.h
 *
 * Created: 24/07/2023 12:25:33 p. m.
 *  Author: dagmtz
 */ 


#ifndef DS3231_H_
#define DS3231_H_


#define DS3231_ADDRESS		0x68
#define DS3231_SECOND		0x00
#define DS3231_MINUTE		0x01
#define DS3231_HOUR 		0x02
#define DS3231_DAY			0x03
#define DS3231_DATE			0x04
#define DS3231_MONTH		0x05
#define DS3231_YEAR			0x06
#define DS3231_A1S			0x07
#define DS3231_A1M			0x08
#define DS3231_A1H			0x09
#define DS3231_A1D			0x0A
#define DS3231_A2M			0x0B
#define DS3231_A2H			0x0C
#define DS3231_A2D			0x0D
#define DS3231_CONTROL		0x0E
#define DS3231_STATUS		0x0F
#define DS3231_AGING		0x10
#define DS3231_TEMP_MSB		0x11
#define DS3231_TEMP_LSB		0x12

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef enum
{
	TWENTY_FOUR_HOUR = 0,
	TWELVE_HOUR = 1
}hour_format;

typedef union {
	uint8_t byte;
	struct {
		uint8_t ls_nibble : 4;
		uint8_t ms_nibble : 4;
	} nibbles;
	struct {
		bool b0 : 1;
		bool b1 : 1;
		bool b2 : 1;
		bool b3 : 1;
		bool b4 : 1;
		bool b5 : 1;
		bool b6 : 1;
		bool b7 : 1;
	} bits;
} bitfield8_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t days;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t a1seconds;
	uint8_t a1minutes;
	uint8_t a1hour;
	uint8_t a1day;
	uint8_t a2minutes;
	uint8_t a2hour;
	uint8_t a2day;
	uint8_t control;
	uint8_t status;
	uint8_t aging;
	uint8_t temp_msb;
	uint8_t temp_lsb;
} DS3231_buffer_t;

/**
 * @brief   
 * @param[]   
 * @return  none
 */
void rtc_getTime( struct tm *rtc_time );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
void rtc_setTime( struct tm *rtc_time );

/**
 * @brief	Set AM/PM bit to DS3231
 *			A logic 1 in bit 6 of the byte
 *
 * @param	[value]		True means 12h format
 * @return	none
 */
void rtc_setTwelveHourFormat( bool value );

/**
 * @brief	Sets century configuration in DS3231
 * @param	[value]		True sets Century to XXII, false to XXI
 * @return  none
 */
void rtc_setCentury( bool value );

/**
 * @brief   Write byte to DS3231
 * @param   [byteToSet]	The address to write to
 * @return  none
 */
void rtc_setByte( uint8_t byteToSet, uint8_t value );

/**
 * @brief   Sets the seconds
 * @param	[seconds]	Value to be set (0-59)
 * @return  none
 */
void rtc_setSeconds( uint8_t seconds );

/**
 * @brief   Sets the minutes
 * @param	[minutes]	Value to be set (0-59)
 * @return  none
 */
void rtc_setMinutes( uint8_t minutes );

/**
 * @brief   Sets the hours
 * @param	[hours]		Value to be set (0-24)
 * @return  none
 */
void rtc_setHours( uint8_t hours );

/**
 * @brief   Sets week of the day
 * @param	[weekday]	The number of the day (1-7)
 * @return  none
 */
void rtc_setWeekday( uint8_t weekday );

/**
 * @brief   Sets day of the month
 * @param	[day]		The day of the month (1-31)		   
 * @return  none
 */
void rtc_setDay( uint8_t day );

/**
 * @brief   Sets month of the year
 * @param	[month]		Value to be set (1-12)
 * @return  none
 */
void rtc_setMonth( uint8_t month );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
void rtc_setYear_u8( uint8_t year );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
void rtc_setYear( uint16_t year );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
bool rtc_getTwelveHourFormat( void );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
bool rtc_getAMPM( void );

/**
 * @brief	Returns value saved as century	
 * @param[]	none
 * @return	true if XXII century is saved, false if XXI century si saved
 */
bool rtc_getCentury( void );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
uint8_t rtc_getByte( uint8_t byteToGet );

/**
 * @brief   Get seconds value
 * @param	none   
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getSeconds( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getMinutes( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getHours( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getWeekday( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getDay( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint8_t rtc_getMonth( void );

/**
 * @brief	
 * @param[]   
 * @return  none
 */
uint8_t rtc_getYear_8b( void );

/**
 * @brief   Get seconds value
 * @param	none
 * @return  [uint8_t]	Seconds in decimal representation
 */
uint16_t rtc_getYear( void );

/**
 * @brief   Format a given year value from two-digit to four-digit
 * @param	none
 * @return  [uint16_t]	The year in four digit representation
 */
uint16_t rtc_formatYear( uint8_t value );

/**
 * @brief   
 * @param[]   
 * @return  none
 */
float rtc_getTemp( void );


#endif /* DS3231_H_ */