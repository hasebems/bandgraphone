/*

*/
#include <Wire.h>
#include "ada88.h"


static unsigned char dot[8];

//---------------------------------------------------------
//		<< ADA88 >>
//---------------------------------------------------------
#define		ADA88_I2C_ADRS		0x70

void write_i2cDevice( unsigned char i2c, unsigned char* data, int cnt )
{
  Wire.beginTransmission(i2c);
  for (int i=0; i<cnt; i++ ){
    Wire.write(*(data+i));
  }
  Wire.endTransmission();
}
//---------------------------------------------------------
//		Initialize ADA88 LED Matrix
//---------------------------------------------------------
void ada88_init( void )
{
  Wire.begin();

	unsigned char i2cBuf[2] = {0,0};
	i2cBuf[0] = 0x21;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

	i2cBuf[0] = 0x81;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

	i2cBuf[0] = 0xef;
	write_i2cDevice( ADA88_I2C_ADRS, i2cBuf, 2 );

  for (int i=0;i<8;i++){ dot[i] = 0;}

	ada88_write(0);
}
//---------------------------------------------------------
//	write Character to Ada LED-matrix
//---------------------------------------------------------
void ada88_write( int letter )
{
	int	i;
	unsigned char i2cBufx[17];
	const unsigned char letters[21][8] = {

//  **#***** [0]:7, [0]:0, [0]:1 ... [0]:6
//  *#*#**** [1]:7, [1]:0, [1]:1 ... [1]:6
//  #***#*** [2]:7, [2]:0, [2]:1 ... [2]:6
//  #***#*** [3]:7, [3]:0, [3]:1 ... [3]:6
//  #####*** [4]:7, [4]:0, [4]:1 ... [4]:6
//  #***#*** [5]:7, [5]:0, [5]:1 ... [5]:6
//  #***#*** [6]:7, [6]:0, [6]:1 ... [6]:6
//  #***#*** [7]:7, [7]:0, [7]:1 ... [7]:6
	
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	//	0:nothing
		{0x02,0x05,0x88,0x88,0x8f,0x88,0x88,0x88},	//	1:A
		{0x87,0x88,0x88,0x87,0x88,0x88,0x88,0x87},	//	2:B
		{0x07,0x88,0x88,0x80,0x80,0x88,0x88,0x07},	//	3:C
		{0x87,0x88,0x88,0x88,0x88,0x88,0x88,0x87},	//	4:D
		{0x87,0x80,0x80,0x87,0x80,0x80,0x80,0x8f},	//	5:E
		{0x87,0x80,0x80,0x87,0x80,0x80,0x80,0x80},	//	6:F
		{0x07,0x88,0x80,0x80,0x8e,0x88,0x88,0x07},	//	7:G
		{0x88,0x88,0x88,0x8f,0x88,0x88,0x88,0x88},	//	8:H

		{0x02,0x05,0x88,0xe8,0xaf,0xe8,0xa8,0xa8},	//	9:AF
		{0x87,0x88,0x88,0xe7,0xa8,0xe8,0xa8,0xa7},	//	10:BF
		{0x07,0x88,0x88,0xe0,0xa0,0xe8,0xa8,0x27},	//	11:CF
		{0x87,0x88,0x88,0xe8,0xa8,0xe8,0xa8,0xa7},	//	12:DF
		{0x87,0x80,0x80,0xe7,0xa0,0xe0,0xa0,0xaf},	//	13:EF
		{0x87,0x80,0x80,0xe7,0xa0,0xe0,0xa0,0xa0},	//	14:FF
		{0x07,0x88,0x80,0xe0,0xae,0xe8,0xa8,0x27},	//	15:GF

		{0x97,0x90,0x90,0x97,0x90,0x90,0x90,0xd0},	//	16:Fl.
		{0x13,0x94,0x94,0xf4,0xd4,0xd4,0xb4,0x53},	//	17:Ob.

		{0x04,0x1f,0x04,0x1f,0x04,0x0f,0x15,0x22},	//	18:MA
		{0x04,0x0f,0x04,0x1e,0x08,0x12,0x01,0x0e},	//	19:KI
		{0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa}	//	20:
	};

	i2cBufx[0] = 0;
	for ( i=0; i<8; i++ ){
		i2cBufx[i*2+1] = letters[letter][i];
		i2cBufx[i*2+2] = 0;
	}
	write_i2cDevice( ADA88_I2C_ADRS, i2cBufx, 17 );
}
//---------------------------------------------------------
void ada88_writeNumber( int num )	//	num 1999 .. -1999
{
	int i;
	unsigned char i2cBufx[17];
	unsigned char ledPtn[8] = {0};
	const unsigned char numletter[10][5] = {
		{ 0x07, 0x05, 0x05, 0x05, 0x07 },
		{ 0x04, 0x04, 0x04, 0x04, 0x04 },
		{ 0x07, 0x04, 0x07, 0x01, 0x07 },
		{ 0x07, 0x04, 0x07, 0x04, 0x07 },
		{ 0x05, 0x05, 0x07, 0x04, 0x04 },
		{ 0x07, 0x01, 0x07, 0x04, 0x07 },
		{ 0x07, 0x01, 0x07, 0x05, 0x07 },
		{ 0x07, 0x04, 0x04, 0x04, 0x04 },
		{ 0x07, 0x05, 0x07, 0x05, 0x07 },
		{ 0x07, 0x05, 0x07, 0x04, 0x07 }
	};
	const unsigned char graph[10][2] = {
		{ 0x00, 0x00 },
		{ 0x00, 0x40 },
		{ 0x40, 0x60 },
		{ 0x60, 0x70 },
		{ 0x70, 0x78 },
		{ 0x78, 0x7c },
		{ 0x7c, 0x7e },
		{ 0x7e, 0x7f },
		{ 0x7f, 0xff },
		{ 0xff, 0xff }
	};	

	if ( num > 1999 ){ num = 1999; }
	else if ( num < -1999 ){ num = -1999;}
	
	//	+/-, over 1000 or not
	if ( num/1000 ){ ledPtn[5] |= 0x80; }
	if ( num<0 ){
		ledPtn[2] |= 0x80;
		num = -num;
	}

	int num3digits = num%1000;
	int hundred = num3digits/100;
	int num2degits = num3digits%100;
	int deci = num2degits/10;
	int z2n = num2degits%10;

	for ( i=0; i<5; i++ ){
		ledPtn[i] |= numletter[hundred][i];
	}
	for ( i=0; i<5; i++ ){
		ledPtn[i] |= (numletter[deci][i]<<4);
	}
	for ( i=0; i<2; i++ ){
		ledPtn[i+6] = graph[z2n][i];
	}

	i2cBufx[0] = 0;
	for ( i=0; i<8; i++ ){
		i2cBufx[i*2+1] = ledPtn[i];
		i2cBufx[i*2+2] = 0;
	}
	write_i2cDevice( ADA88_I2C_ADRS, i2cBufx, 17 );
}
//---------------------------------------------------------
//	write dot directry to Ada LED-matrix
//---------------------------------------------------------
#define   ONE_LINE_DOT    8
//---------------------------------------------------------
void ada88_writeDot( int dotNum, bool onoff )
{
	int	i;
	unsigned char i2cBufx[17];

  if ( onoff == true ){
    if ((dotNum%ONE_LINE_DOT) != 0){ dot[dotNum/ONE_LINE_DOT] |= (0x01<<((dotNum%ONE_LINE_DOT)-1));}
    else { dot[dotNum/ONE_LINE_DOT] |= 0x80; }
  }
  else {
    if ((dotNum%ONE_LINE_DOT) != 0){ dot[dotNum/ONE_LINE_DOT] &= ~(0x01<<((dotNum%ONE_LINE_DOT)-1));}
    else { dot[dotNum/ONE_LINE_DOT] &= 0x7f; }
  }

	i2cBufx[0] = 0;
	for ( i=0; i<8; i++ ){
		i2cBufx[i*2+1] = dot[i];
		i2cBufx[i*2+2] = 0;
	}
	write_i2cDevice( ADA88_I2C_ADRS, i2cBufx, 17 );
}
