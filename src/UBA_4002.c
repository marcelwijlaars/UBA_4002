#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#include "ABE_IoPi.h"
#include "ABE_ADCPi.h"

int pwm(void);
float temperature(float V);


int main(void)
{
/* 	pwm();  */
	struct timeval t1,t2;
	double dt;


	int i=5;
	int pin2=0;
	int pin3=0;
	int pin4=0;

	IOPi_init(0x20); // initialise one of the io pi buses on i2c address 0x20, default address for bus 1
	set_port_direction(0x20,0, 0xFF); // set bank 0 to be inputs
	set_port_direction(0x20,1, 0x00); // set bank 1 to be inputs
	set_port_pullups(0x20, 0, 0xFF);  // enable internal pullups for bank 0
	invert_port(0x20,0,0xFF);         // invert output so bank will read as 0
	write_port(0x20,1, 0xFF);

	if (wiringPiSetup() == -1) return 1;
	pinMode(0,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	softPwmCreate(0, i,50);
	while(1)
	{

	//digitalWrite(0,1);
	//usleep(500000);
	//digitalWrite(0,0);
	sleep(2);
	if(i<50) i=i+1; 
	else {
		i=5;
		pin2^=1;
		digitalWrite(2,pin2);
	}
	if(i==10) {
		pin3^=1;
		digitalWrite(3,pin3);
	}
	if(i==30) {
		pin4^=1;
		digitalWrite(4,pin4);
	}

	softPwmWrite(0,i);
	printf("i: %i\n",i);
	}
	while(1)
	{
	gettimeofday(&t1,NULL);	
	/* ADC */
	printf("Read ADC pin 1-4: %G, ", temperature(read_voltage(0x68,1, 18, 1, 1))); 
	printf("%G, ", temperature(read_voltage(0x68,2, 18, 1, 1))); 
	printf("%G, ", temperature(read_voltage(0x68,3, 18, 1, 1))); 
	printf("%G \n", temperature(read_voltage(0x68,4, 18, 1, 1))); 
	/* DI */
	printf("Read DIO port 0 Pin 1: %x \n", read_pin(0x20,1)); // grounding a pins 1 to 8 will change the value
	/* DO */
	printf("Set DIO port 1 pin 1\n");
	write_pin(0x20,1, 1); // turn pin 1 on and off at 1 second intervals
	usleep(10000);
	printf("Clear DIO port 1 pin 1\n");
	write_pin(0x20,1, 0);
	usleep(10000);
	gettimeofday(&t2,NULL);
	dt=((t2.tv_sec-t1.tv_sec)*1000.0) + ((t2.tv_usec-t1.tv_usec)/1000.0);
	printf("dt: %.3f [ms]\n",dt);


	}
	return (0);
}


int pwm(void)
{
	const int led_pin = 1;
	if(wiringPiSetup() == -1)
		printf("Setup wiringPi Failed\n");

	pinMode (led_pin, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(200);
	pwmSetRange(100);
	delay(1);
	while(1)
	{
		int i;
		int list[]={5, 10, 20 ,30, 40, 50 ,60, 70, 80, 90, 100,0, -1};
		for(i=0;list[i]>0;i++)
		{
			pwmWrite(led_pin, list[i]);
			printf("pwm duty cycle: %d\n",list[i]);
			delay(1000);
		}
	}	
	return 0;
}



float temperature(float V)
{

  float Rref=32900.0;
  float R1=6800.0;
  float R2=10000.0;
  
  float Vref=12.0;
  float Rerr=((V*Rref)/Vref)/(1-(V/Vref));
  float R= 1/( (1/Rerr) - (1/(R1+R2)) );

  float R25 = 10e3;     /* Nominal Resistance at 25C */
  float B25=3553.96;    /* Nominal Beta Constant at 25C */
  float T25= 298.15;    /* Nominal Temperature 25C in Kelvin */
  float T=1/((1/B25)*log(R/R25)+ (1/T25));
  return T=T-273.15;
}
