timeserver:  timeserver.c timeserver.h ST7066 configEditor 
	gcc  -g timeserver.c configEditor.o ST7066.o -o output -lpigpio -lgps -lpthread -lcjson 

configEditor: configEditor.c configEditor.h ST7066
	gcc  -c configEditor.c -o configEditor.o 

ST7066: ST7066.c ST7066.h displayPages.h
	gcc  -c ST7066.c -o ST7066.o 
clean:
	rm *.o output