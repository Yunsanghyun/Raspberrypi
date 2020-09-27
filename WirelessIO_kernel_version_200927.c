#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>

char device[] = "/dev/rfcomm0";
int fd = 0;
unsigned long baud = 9600;
//unsigned long time = 0;


void setup(){
    printf("%s \n","Raspberry Startup!");
    fflush(stdout);

    if((fd = serialOpen(device,baud)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: \n");
        exit(1);
    }

    if(wiringPiSetup() == -1)
    {
        fprintf(stdout,"Unable to start wiringPi: \n");
        exit(1);
    }
}

int main(void){
    char output_signal[1] = "0";
    char input_signal;
    int input_on_flag = 0;
    int input_off_flag = 0;

    setup();
    printf("Wireless Test is operating \n");
    
    serialFlush(fd);
    while(1)     
    {
	// send to arduino
	serialPuts(fd, output_signal);
	sleep(1);

	// recv from arduino
        if(serialDataAvail(fd)){
	    printf("recv data %d\n",serialDataAvail(fd));

            input_signal = serialGetchar(fd);
	    printf("signal %c\n",input_signal);
	    if(input_signal == '0'){
	   	 if(input_off_flag == 0){ 
            		printf("input signal OFF \n");
	    		input_on_flag = 0;
			input_off_flag = 1;
		 }
	    }
	    else if(input_signal == '1'){
	  	if(input_on_flag == 0){ 
			printf("input signal ON \n");
			input_on_flag = 1;
			input_off_flag = 0;
		}
	    }
            fflush(stdout);
        }
    }
    
    return 0;
}
