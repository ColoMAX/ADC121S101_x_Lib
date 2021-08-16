CC = g++

CPPFLAGS = -g3 -I lib/ADC121S101x/ 

OBJ = src/main.cpp lib/ADC121S101x/ADC121S101x.cpp

TARGET = test_ADC.out

default: compile

compile: $(OBJ)
	$(CC) $(OBJ) $(CPPFLAGS) -o $(TARGET) 

clean: 
	-rm -f *.o
	-rm -f $(TARGET)
