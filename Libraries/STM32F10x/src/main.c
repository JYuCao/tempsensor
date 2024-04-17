#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"

int main(void)
{
	//OLED_Init();
	//OLED_ColorTurn(0);//0正常显示，1 反色显示
  //OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	delay_init();
	led_init();
	ADS1018_Init();
	uart_init(115200);
	
	while(1) {
		//OLED_ShowChinese(0,0,0,16,1);
		//OLED_ShowChinese(20,0,1,16,1);
		//OLED_Refresh();
		printf("data16=%.2f\n", ADS1018_ReadData()*0.125-10);
		delay_ms(200);
	}
}
