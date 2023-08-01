/*
* ds3231.c
*
* Created: 24/07/2023 12:39:21 p. m.
*  Author: dagmtz
*/

#include "ds3231.h"
#include "i2c.h"
#include "bcd.h"

void rtc_getTime( struct tm *rtc_time )
{
	bool format = rtc_getTwelveHourFormat();
	if( format == TWELVE_HOUR )
	{
		rtc_setTwelveHourFormat( false );
	}
	
	rtc_time->tm_sec = rtc_getSeconds();
	rtc_time->tm_min = rtc_getMinutes();
	rtc_time->tm_hour = rtc_getHours();
	rtc_time->tm_mday = rtc_getDay();
	rtc_time->tm_mon = rtc_getMonth() - 1;
	rtc_time->tm_year = rtc_getYear() - 1900;
	
	if( format == TWELVE_HOUR )
	{
		rtc_setTwelveHourFormat( true );
	}
}

void rtc_setTime( struct tm *rtc_time )
{
	i2c_start_sla( TW_SLA_W( DS3231_ADDRESS ) );
	i2c_write( DS3231_SECOND );
	i2c_write( rtc_time->tm_sec );
	i2c_stop();
}

void rtc_setByte( uint8_t byteToSet, uint8_t value )
{
	
	/* Set DS3231 Address Pointer to the desired address */
	i2c_start_sla( TW_SLA_W( DS3231_ADDRESS ) );
	i2c_write( byteToSet );
	
	/* Set the desired value to the address set */
	i2c_write( value );
	i2c_stop();
	
}

void rtc_setTwelveHourFormat( bool value )
{
	bitfield8_t buffer;
	buffer.byte = rtc_getByte( DS3231_HOUR );
	
	if ( buffer.bits.b6 != value )
	{
		buffer.bits.b6 = ~buffer.bits.b6;
		/* Set DS3231 Address Pointer to the desired address */
		i2c_start_sla( TW_SLA_W( DS3231_ADDRESS ) );
		i2c_write( DS3231_HOUR );
		
		/* Set the desired value to the address set */
		i2c_write( buffer.byte );
		i2c_stop();
		
	}
}

bool rtc_getTwelveHourFormat( void )
{
	bitfield8_t buffer;
	buffer.byte = rtc_getByte( DS3231_HOUR );
	
	/* 0 = 24h, 1 = 12h */
	return buffer.bits.b6;
}

bool rtc_getAMPM( void )
{
	bitfield8_t buffer;
	buffer.byte = rtc_getByte( DS3231_HOUR );
	
	/* 0 = AM, 1 = PM */
	return buffer.bits.b5;
}

bool rtc_getCentury( void )
{
	bitfield8_t buffer;
	buffer.byte = rtc_getByte( DS3231_MONTH );
	
	/* 0 = XXI, 1 = XXII */
	return buffer.bits.b7;
}

uint8_t rtc_getByte( uint8_t byteToGet )
{
	uint8_t buffer = 0;

	/* Set DS3231 Address Pointer to the desired address */
	i2c_start_sla( TW_SLA_W( DS3231_ADDRESS ) );
	i2c_write( byteToGet );
	i2c_stop();
	
	/* Request one byte and send NACK to end transmission */
	i2c_start_sla( TW_SLA_R( DS3231_ADDRESS ) );
	buffer = i2c_readNAck();
	i2c_stop();
	
	return buffer;
}

uint8_t rtc_getSeconds( void )
{
	uint8_t buffer;
	buffer = rtc_getByte( DS3231_SECOND );

	return bcdToDec_u8( buffer );
}

uint8_t rtc_getMinutes( void )
{
	uint8_t buffer;
	buffer = rtc_getByte( DS3231_MINUTE );

	return bcdToDec_u8( buffer );
}

uint8_t rtc_getHours( void )
{
	bitfield8_t buffer;
	
	buffer.byte = rtc_getByte( DS3231_HOUR );
	if( rtc_getTwelveHourFormat() )
	{
		buffer.nibbles.ms_nibble &= 0x1;
	}
	
	return bcdToDec_u8( buffer.byte );
}

uint8_t rtc_getWeekday( void )
{
	uint8_t buffer;
	buffer = rtc_getByte( DS3231_DAY ) - 1;

	return bcdToDec_u8( buffer );
}

uint8_t rtc_getDay( void )
{
	uint8_t buffer;
	buffer = rtc_getByte( DS3231_DATE );

	return bcdToDec_u8( buffer );
}

uint8_t rtc_getMonth( void )
{
	bitfield8_t buffer;
	buffer.byte = rtc_getByte( DS3231_MONTH );
	buffer.bits.b7 = 0;

	return buffer.byte;
}

uint8_t rtc_getYear_8b( void )
{
	uint8_t buffer;
	buffer = rtc_getByte( DS3231_YEAR );

	return bcdToDec_u8( buffer );
}

uint16_t rtc_getYear( void )
{
	uint16_t year = rtc_formatYear( rtc_getYear_8b() );
	if ( rtc_getCentury() )
	{
		year += 100;
	}
	
	return year;
}

uint16_t rtc_formatYear( uint8_t value )
{
	uint16_t year = value + 2000;
	
	return year;
}

float rtc_getTemp( void )
{

	uint8_t tMSB, tLSB;
	float temp;

	i2c_start_sla( TW_SLA_W( DS3231_ADDRESS ) );
	i2c_write( DS3231_TEMP_MSB );
	i2c_stop();

	i2c_start_sla( TW_SLA_R( DS3231_ADDRESS ) );
	tMSB = i2c_readAck();
	tLSB = i2c_readNAck();
	i2c_stop();
	
	int16_t _temp = ( tMSB << 8 | ( tLSB & 0xC0 ) );  // Shift upper byte, add lower
	temp = ( ( float ) _temp / 256.0 );              // Scale and return
	
	return temp;
}
