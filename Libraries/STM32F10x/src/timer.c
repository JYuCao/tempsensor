#include "timer.h"
#include "stm32f10x.h"   
//��ʼ����ʱ��2������ʱ�ж϶�ʱ����
void Timer2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_InternalClockConfig(TIM2);//ѡ���ĸ��жϾ�д�ĸ�
	
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //�޸ķ�Ƶ����ʵ�����Ӱ�첻�󣬿��Բ��޸�,�����ǲ���Ƶ����ѡ1~72��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���϶���ģʽ��ͬʱ�������¶��룬�������ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 200 - 1;							  //���������ڡ��ò��������˼������������ǰ�����ֵ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000 - 1;							//��Ƶ��Ԥ��Ƶϵ�����ò��������˼�����ʱ��Ƶ�ʵı仯�̶ȡ�
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;          //�߼���������Ҫ������Ҫ�õ���ֱ�Ӹ�0�ͺ�
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);                           //���ڽ��һ��λʱ���Ƚ�һ���жϵ����
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;       //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              //��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	
}