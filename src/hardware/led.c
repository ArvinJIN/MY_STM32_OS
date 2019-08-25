#include "hardware/led.h"

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
	RCC->APB2ENR|=1<<3;    //使能PORTB时钟（IO端口B时钟开启）	   	 
	RCC->APB2ENR|=1<<6;    //使能PORTE时钟	
	//CRL:20~23这四位控制的是PB5的输入输出模式
	GPIOB->CRL&=0XFF0FFFFF; 
	GPIOB->CRL|=0X00300000;//PB.5推挽输出   	 
  GPIOB->ODR|=1<<5;      //PB.5输出高
											  
	GPIOE->CRL&=0XFF0FFFFF;
	GPIOE->CRL|=0X00300000;//PE.5推挽输出
	GPIOE->ODR|=1<<5;      //PE.5输出高 
}


void LED0TURN(){
    LED0=!LED0;
}

void LED1TURN(){
    LED1=!LED1;
}