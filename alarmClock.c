//#include "": 引號表示前置處理器會先搜尋包含父原始程式檔的目錄。
//#include <>: 角括弧會使預處理器搜尋包含環境變數所指定的目錄

#include <8051.h>
#include "LED_Display.h"
#include "Keypad4x4.h"

#define PERIOD 15536

void entry_clock();
void Timeclock();
void entry_alarm();
void delay_Mu(unsigned char ms);
void period_cal(unsigned  HZ);

unsigned char keypadIn, keypadTmp, keyStatus, keyMode;
unsigned char Initclock[4] = {10, 10, 10, 10};
unsigned char InitAlar[4] = {10, 10, 10, 10};
unsigned char move = 0;
unsigned timecount = 0;

unsigned char clock_InitHR, clock_InitMin;
unsigned char num_th, num_h, num_ten, num_unit; //顯示7段的數字
unsigned char alarm_hr, alarm_min, alarm_sec;
unsigned char sec_ten, sec_unit;
unsigned char sec, min,hour;	//計算秒分時

char speaker = 0;
unsigned char high = 0xfc, low = 0x8f;

unsigned musicScale[14] = {524,555,587,622,659,698,739,783,830,880,932,988,391,0};
		//					1   1#  2   2#  3   4   4#  5   5#  6   6#  7  .5
		//					0	1	2	3	4	5	6	7	8	9	10	11 12  13    
//樂譜
unsigned const char music[] = {	0,13,2,13,4,13,0,13,0,13,2,13,4,13,0,13,
								4,13,5,13,7,13,4,13,5,13,7,13,
								7,13,9,13,7,13,5,13,4,13,0,13,
								7,13,9,13,7,13,5,13,4,13,0,13,
								2,13,12,13,0,13,2,13,12,13,0,13	};
unsigned const char clap[] =  {	2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,
								2,1,2,1,4,1,2,1,2,1,4,1,
								1,1,1,1,1,1,1,1,2,1,2,1,
								1,1,1,1,1,1,1,1,2,1,2,1,
								2,1,2,1,4,1,2,1,2,1,4,2     };

//interrupt timer

//時間
void time0_ISR(void) __interrupt(1) __using(1)
{
	static __bit bEven = 1;
	TL0 = PERIOD & 0xff;
	TH0 = PERIOD >> 8;
	timecount ++;
}

//音樂頻率
void time1_ISR(void) __interrupt(3) __using(2)
{
	TL1 = low;
	TH1 = high;
	P3_0 = speaker;
	speaker = ~speaker;
}

//程式
void main()
{
    while(1)
    {
        while(keyStatus == 0)
        {
            entry_clock();
        }
        while (keyStatus == 1)
        {
            //display(9,10,9,11);
            TMOD = 0x11;	 //Timer0 mode 1 
	        IE = 0x8f;		//中斷致能暫存器EA,ET0,ET1 = 1
	        TR0 = 1;		//turn ON Timer zero
           
            Timeclock();    //時鐘
             while (keyMode == 11)
            {
                entry_alarm();  
                if (keypadIn == 15 && move >= 4)
                {
                    keyMode = 0;
                    alarm_hr = InitAlar[0] * 10 + InitAlar[1];
                    alarm_min = InitAlar[2] * 10 + InitAlar[3];
                    alarm_sec = 00;
                    move = 0;
                }
            }
            
            while (keyMode == 17) //音樂響起
            {
                keypadIn = keyscan();
                unsigned char musicNum;
                musicNum = 0;
                TR1 = 1;
                display(num_th, num_h, num_ten, num_unit);
                while(1)
                {
                    keypadIn = keyscan();
                    delay(5);
                    display(num_th, num_h, num_ten, num_unit);
                    if (keypadIn == 12)
                    {   //C: 停止
                        
                        TR1 = 0;
                        keyMode = 0;
                        break;
                    }
                    if (keypadIn == 14)
                    {   //E: 貪睡5分鐘
                        TR1 = 0;
                        keyMode = 0;
                        alarm_min += 5;
                        break;
                    }
                    period_cal(musicScale[music[musicNum]]);
                    delay_Mu(clap[musicNum]);
                    musicNum++;
                    if (musicNum > 62)
                    {
                        musicNum = 0;
                    }
                    
                }  
            }
        }
    }
}

void entry_clock()
{

    display(Initclock[0],Initclock[1],Initclock[2],Initclock[3]);
    keypadTmp = keypadIn;
    keypadIn = keyscan();
    if ((keypadTmp != keypadIn) && (keypadIn != 16))
    {
        if ((keypadIn <= 9) && (move <4))
        {
            if ((move == 0 && keypadIn < 3) || (move == 1 && Initclock[0] != 2) || (move == 1 && Initclock[0] == 2 && keypadIn < 4) ||
             (move == 2 && keypadIn < 6) || (move == 3))
            {
                Initclock[move] = keypadIn;
                display(Initclock[0],Initclock[1],Initclock[2],Initclock[3]);
                move ++;
            }
        }
        if (keypadIn == 12)
        {
            Initclock[0] = Initclock[1] = Initclock[2] = Initclock[3] = 10;
            move = 0;
        }
        
        if ((keypadTmp != keypadIn) && (keypadIn == 15) && (move >= 4))
        {
            keyStatus = 1;
            move = 0;
        }
    }

}

void entry_alarm()
{
    display(InitAlar[0],InitAlar[1],InitAlar[2],InitAlar[3]);
    keypadTmp = keypadIn;
    keypadIn = keyscan();
    if ((keypadTmp != keypadIn) && (keypadIn != 16))
    {
        if ((keypadIn <= 9) && (move <4))
        {
            
            if ((move == 0 && keypadIn < 3) || (move == 1 && InitAlar[0] != 2) || (move == 1 && InitAlar[0] == 2 && keypadIn < 4) ||
             (move == 2 && keypadIn < 6) || (move == 3))
            {
                InitAlar[move] = keypadIn;
                display(InitAlar[0],InitAlar[1],InitAlar[2],InitAlar[3]);
                move ++;
            }
            
        }
        if (keypadIn == 12)
        {
            InitAlar[0] = InitAlar[1] = InitAlar[2] = InitAlar[3] = 10;
            move = 0;
        }
        
        
    }
    
}

/********Clock時鐘***********/
void Timeclock()
{
    keypadTmp = keypadIn;
    keypadIn = keyscan();
    
    clock_InitHR = Initclock[0] * 10 + Initclock[1];
    clock_InitMin = Initclock[2] * 10 + Initclock[3];

    sec = (timecount / 10) % 60;
    min = (timecount + clock_InitMin*600) / 600; 	//分的計算
    hour = clock_InitHR % 24;
    
    if (min > 59){  //59分後下一分(整點)小時會加1
        hour++;
        min = 0;
        timecount = 0;
        clock_InitMin = 0;
    }
    if (hour > 23){
        hour = 0;
    } 

    if (keypadIn != 16 && keypadIn != 10 && keypadIn != 11 && keypadIn != 13 && keypadIn != 12){
        display(num_th, num_h, num_ten, num_unit);
    }
    if (keypadIn == 16) //default
    {
        num_unit = min % 10;
        num_ten = min /10;
        num_h = hour % 10;
        num_th = hour / 10;
        display(num_th, num_h, num_ten, num_unit);

        if ((alarm_hr == hour) && (alarm_min == min) && (alarm_sec == sec))
        {
            keyMode = 17;
        }
    }
    delay(10);
    //切換模式
    if (keypadIn == 10){
        //A: 查看鬧鐘時間
        num_unit = alarm_min % 10;
        num_ten = alarm_min / 10;
        num_h = alarm_hr % 10;
        num_th = alarm_hr / 10;
        display(num_th, num_h, num_ten, num_unit);
    }
    if (keypadIn == 11){
        //B: 設定鬧鐘
        InitAlar[0] = InitAlar[1] = InitAlar[2] = InitAlar[3] = 10;
        keyMode= 11;
    }
    if (keypadIn == 13){
        //D: 查看秒數
        sec_ten = sec /10 ;
        sec_unit = sec % 10;
        display(11, 10, sec_ten, sec_unit);
    }
    
}

/********music***********/
void period_cal(unsigned  HZ)
{
	unsigned int temp;
	unsigned long PERIOD_Mu;
	PERIOD_Mu = 1000000/HZ;
	temp = 65536 - PERIOD_Mu/2;
	high = temp >> 8;
	low = temp % 256;
}

void delay_Mu(unsigned char ms)	//音樂的delay
{	
	unsigned i;	
	while(ms--)
	{
		for(i = 0; i < 60000; i++);
	}
}


/*

10	A:	查看鬧鐘時間	V
11	B:	設定鬧鐘 V
12	C:	關鬧鐘	V
13	D:	查看秒數	V
14	E:	貪睡    V
15	F:	確定/跳離模式   V
*/

//會燒壞喇叭
    /*if (keypadIn == 12){
        //C: 重啟時鐘
        keyStatus = 0;
        TR0 = 0;
        timecount = 0;
        Initclock[0] = Initclock[1] = Initclock[2] = Initclock[3] = 10;
        InitAlar[0] = InitAlar[1] = InitAlar[2] = InitAlar[3] = 10;
        alarm_hr = alarm_min = 00;
    }*/