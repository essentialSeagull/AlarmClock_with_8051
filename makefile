mainName = alarmClock
libName = myIO.lib

fname1 = keypad4x4
fname2 = LED_Display


all: $(mainName) clean	

$(fname1).rel: $(fname1).c $(fname1).h
	sdcc -c $(fname1).c

$(fname2).rel: $(fname2).c $(fname2).h
	sdcc -c $(fname2).c


$(libName): $(fname1).rel $(fname2).rel 
	sdar -o $(libName) -rc $(fname1).rel $(fname2).rel 
$(mainName): $(mainName).c $(libName)
	sdcc $(mainName).c $(libName)
	packihx $(mainName).ihx > $(mainName).hex
	
clean :
#相關檔案類型全清掉
	del *.asm *.ihx *.lk *.lst *.map *.mem *.rel *.rst *.sym


#-c : 只編譯不連結 