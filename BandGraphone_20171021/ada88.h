#pragma once

void write_i2cDevice( unsigned char i2c, unsigned char* data, int cnt );
void ada88_init( void );
void ada88_write( int letter );
void ada88_writeNumber( int num );	//	num 1999 .. -1999
void ada88_writeDot( int dotNum, bool onoff );
