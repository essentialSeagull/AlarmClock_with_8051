#include <8051.h>
#include "LED_Display.h"

unsigned char keyscan()
{
	unsigned char  row,r,c,temp;
	
	for(c=0; c<4; c++) 
	{	
		//P0 = 0xf0;
		P2 = 0xff & ~(0x10 << c); 
		
		if ((P2& 0xf0) != 0xf0)
		{
			delay(3);
			for (r=0; r<4; r++)
			{
				row = (0x01 << r);
				
				if (( P2 & row) == 0)
				{
					temp = r + c*4;
					return temp;
				}
			}
		}
	}
    return 16;

}

