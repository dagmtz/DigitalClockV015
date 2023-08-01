/*
 * bcd.h
 *
 * Created: 28/07/2023 07:49:29 p. m.
 *  Author: dagmtz
 */ 


#ifndef BCD_H_
#define BCD_H_


#include <stdint.h>

/**
 * @brief   
 * @param[]   
 * @return  none
 */
uint8_t divideByTen( const uint8_t value )
{
	uint8_t q, r;
	q = (value >> 1) + (value >> 2);
	q = q + (q >> 4);
	q = q + (q >> 8);
	q = q >> 3;
	r = value - (((q << 2) + q) << 1);
	return q + (r > 9);
}

/**
 * @brief	Convert two-digit BCD number to decimal representation in 8 bits
 * @param	[value]		BCD number to convert
 * @return  [uint8_t]	Converted number
 */
uint8_t bcdToDec_u8( const uint8_t value )
{
	return ( ( (value >> 4) *10) + (value%16) );
}

/**
 * @brief   Convert 8-bit decimal number to two-digit BCD representation
 * @param	[value]			Decimal number to convert
 * @return  [uint8_t]		Converted number
 */
uint8_t decToBcd_u8( const uint8_t value )
{
	return ( ( (value/10) << 4) + (value%10) );
}

/*
uint16_t bcdToDec_u8( const uint8_t value );
uint16_t decToBcd_u8( const uint8_t value );
*/

#endif /* BCD_H_ */