#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "adc.h"
#include "usart.h"
#include "timer.h"

void showFloat(u8 x, u8 y, float num);
void showFloat2(u8 x, u8 y, float num);
void showchs(void);

double pretem;

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
	
		printf("%lf", pretem);
		
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清出中断寄存器标志位，用于退出中断
	}
}


int main(void)
{
	delay_init();
	ADS1018_Init();
	uart_init(115200);
	OLED_Init();
	
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Refresh();
	
	Timer2_Init();
	
	while(1) {
		double obj_temp = 0.0;
		double temp = read_SensorTemp();
		int i;
		
		showchs();
	//	printf("obj=%lf\n", read_SensorObj(temp));
	//	printf("hello\n");
		showFloat(75,0,temp);
		
		for (i = 0; i < 5; ++i) {
			obj_temp += read_SensorObj(temp);
		} 
		
		pretem=obj_temp/5;
		/*printf("%lf", obj_temp/10); */
		
		showFloat(75,20,obj_temp/5);
		showFloat2(0,40,read_SensorTempR());
		showFloat2(60,40,read_SensorObjV(temp));
		

		OLED_Refresh();
		

	}
}

void showchs(void)
{
	OLED_ShowChinese(0,0,7,16,1);
	OLED_ShowChinese(18,0,8,16,1);
	OLED_ShowChinese(36,0,2,16,1);
	OLED_ShowChinese(54,0,3,16,1);
	OLED_ShowChinese(0,20,5,16,1);
	OLED_ShowChinese(18,20,6,16,1);
	OLED_ShowChinese(36,20,2,16,1);
	OLED_ShowChinese(54,20,3,16,1);
}

void showFloat(u8 x, u8 y, float num)
{
	OLED_ShowNum(x, y, num, 2, 16, 1);
	OLED_DrawPoint(x+17,y+13,1);
	OLED_DrawPoint(x+17,y+14,1);
	OLED_DrawPoint(x+18,y+13,1);
	OLED_DrawPoint(x+18,y+14,1);
	OLED_ShowNum(x+20, y, (num-((int)num))*100, 2, 16, 1);
	OLED_ShowChinese(x+40,y,4,16,1);
}

void showFloat2(u8 x, u8 y, float num)
{
	OLED_ShowNum(x, y, num, 4, 16, 1);
	OLED_DrawPoint(x+35,y+13,1);
	OLED_DrawPoint(x+35,y+14,1);
	OLED_DrawPoint(x+36,y+13,1);
	OLED_DrawPoint(x+36,y+14,1);
	OLED_ShowNum(x+38, y, (num-((int)num))*100, 2, 16, 1);
}
