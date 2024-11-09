#include "stdlib.h"
#include "delay.h"
#include "adc.h"

void CS_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOB,&GPIO_InitStructure);				
	GPIO_SetBits(GPIOB,GPIO_Pin_5);						
	
}

void CS_Reset(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void CS_Set(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void ADS1018_Init(void)
{ 
	SPI_InitTypeDef SPI_InitStructure; 
	SPI_I2S_DeInit(SPI2); 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master; 
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge; 
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial=7;
	SPI_Init(SPI2,&SPI_InitStructure); 
	SPI_Cmd(SPI2,ENABLE);
	CS_Init();
	CS_Set();
}

void ADS1018_Config(u8 *config, ADC_ConfigDef ADC_Config)
{
	config[0] = 0x00;
	config[1] = 0x03;
	
	config[0] |= (ADC_Config.ADC_SS << 7);
	config[0] |= (ADC_Config.ADC_MUX << 4);
	config[0] |= (ADC_Config.ADC_PGA << 1);
	config[0] |= ADC_Config.ADC_MODE;
	config[1] |= (ADC_Config.ADC_DR << 5);
	config[1] |= (ADC_Config.ADC_TS_MODE << 4);
	config[1] |= (ADC_Config.ADC_PULL_UP_EN << 3);
}

u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
  u16 retry=0; 	
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) 
  {
		retry++; 
		if (retry>=0xfffe) return 0;		
  }			  
  SPI_I2S_SendData(SPI2, TxData);     
  retry=0;

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) 
  {
		retry++;  
		if (retry>=0xfffe) return 0;			
  }	  						 
  return SPI_I2S_ReceiveData(SPI2); 
}

u16 ADS1018_ReadData(u8 ch)   
{  
	u8 byte[2];
	u16 data = 0x0000;
	u8 config[2] = {0b10000011,0b10011011};
	
	ADC_ConfigDef ADC_ConfigStructure;
	
	ADC_ConfigStructure.ADC_TS_MODE = ADC_TS_MODE_TemperatureSensor;
	ADC_ConfigStructure.ADC_SS = ADC_SS_NoEffect;
	ADC_ConfigStructure.ADC_MUX = ADC_MUX_AIN0to1;
	ADC_ConfigStructure.ADC_PGA = ADC_PGA_4096;
	ADC_ConfigStructure.ADC_DR = ADC_DR_1600;
	ADC_ConfigStructure.ADC_PULL_UP_EN = ADC_PULL_UP_Enable;
	
	ADC_ConfigStructure.ADC_MODE = ADC_MODE_SingleShot;
	
	switch(ch) {
		case ADC_TEMP:
			ADC_ConfigStructure.ADC_TS_MODE = ADC_TS_MODE_TemperatureSensor;
			break;
		case SENSOR_OBJ:
			ADC_ConfigStructure.ADC_TS_MODE = ADC_TS_MODE_ADC;
			ADC_ConfigStructure.ADC_MUX = ADC_MUX_AIN0toGND;
			break;
		case SENSOR_TEMP:
			ADC_ConfigStructure.ADC_TS_MODE = ADC_TS_MODE_ADC;
			ADC_ConfigStructure.ADC_MUX = ADC_MUX_AIN1toGND;
			break;
	}
#if DEBUG
	if (ADC_ConfigStructure.ADC_MODE == ADC_MODE_SingleShot)
#endif
		ADC_ConfigStructure.ADC_SS = ADC_SS_Start;
	
	ADS1018_Config(config, ADC_ConfigStructure);
	
	CS_Reset();
	for(u8 i=0;i<2;i++){
			SPI2_ReadWriteByte(config[i]);
	}
	delay_ms(20);
	for(u8 i=0;i<2;i++){
			byte[i] = SPI2_ReadWriteByte(config[i]);
	}
	CS_Set();
	delay_ms(5);
	
	data = (byte[0]<<8) | byte[1];

	return data>>4;
}

double read_ADCTemp(void)
{
	return ADS1018_ReadData(ADC_TEMP)*0.125-7;
}

double read_SensorTempR(void)
{
	int new_data = complement_to_signed(ADS1018_ReadData(SENSOR_TEMP));
	double resist = new_data*2/1000.0;
	resist = 100*resist/(4.096-resist);
	
	return resist;
}

double read_SensorTemp(void)
{
	double x = read_SensorTempR();
	double temp = -3.786e-06*x*x*x + 0.002572*x*x - 0.6579*x + 68.82;
	
	return temp;
}

int complement_to_signed(u16 complement) {
    if (complement & (1 << 11)) {
        complement &= 0x7FF;
        complement = (~complement + 1) & 0x7FF;
        return -complement;
    }
    return complement;
}

double read_SensorObjV(float temp)
{
	int new_data = complement_to_signed(ADS1018_ReadData(SENSOR_OBJ));
	double obj_temp;
	
	obj_temp = new_data*2;
	
	return obj_temp;
}

double read_SensorObj(float temp)
{
	double obj_temp = (read_SensorObjV(temp)-1120)*PARAM/511;
	int stemp_index = (int)temp;
	obj_temp = sensor_obj_params[stemp_index][0] * obj_temp * obj_temp + sensor_obj_params[stemp_index][1] * obj_temp + sensor_obj_params[stemp_index][2];
	
	return obj_temp+1.5;
}
