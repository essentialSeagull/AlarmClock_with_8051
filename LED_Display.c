#include <8051.h>

void delay(unsigned int count)
{
	unsigned d1, d2;
	for (d1 = 0; d1 < count; d1 ++) {
		for (d2 = 0; d2 < 100; d2 ++) {}
	}
}
void display(unsigned char seg1,unsigned char seg2,unsigned char seg3,unsigned char seg4)
{	
	unsigned char arrayin[] = {seg1,seg2,seg3,seg4};
	unsigned char scanline[] = {0xfe, 0xfd, 0xfb, 0xf7};
	unsigned char seg7[13] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x77,0x73}; //0123456789-AP
	unsigned char seg[4];

	for (int i=0; i<4; i++)
	{
		P0 = 0xff;
		P0 = scanline[i];
		seg[i] = seg7[arrayin[i]];
		P1 =~ seg[i];
		delay(10);
	}
	
}

