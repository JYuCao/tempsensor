#include "timer.h"
#include "stm32f10x.h"   
//初始化定时器2用作计时中断定时器：
void Timer2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);//选择哪个中断就写哪个
	
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //修改分频，对实际情况影响不大，可以不修改,这里是不分频（可选1~72）
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上对齐模式，同时还有向下对齐，中央对齐模式
	TIM_TimeBaseInitStructure.TIM_Period = 200 - 1;							  //计数器周期。该参数决定了计数器计数溢出前的最大值。
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000 - 1;							//分频器预分频系数。该参数决定了计数器时钟频率的变化程度。
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;          //高级计数器需要，不需要用到的直接给0就好
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);                           //用于解决一复位时就先进一次中断的情况
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;       //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              //响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	
}