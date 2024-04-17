#include "stdlib.h"
#include "delay.h"
#include "adc.h"

#define DIN0 100
#define DIN1 101
#define PGA2048 010
#define PGA4096 001
#define MODE_SS 1
#define MODE_C 0
#define MODE_TS 0
#define MODE_ADC 1



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
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
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

u16 ADS1018_ReadData(void)   
{  
	u8 byte[2];
	u16 data = 0x0000;
	u8 config[2] = {0x04,0x9B};
	
	for(u8 i=0;i<2;i++){
			byte[i] = SPI2_ReadWriteByte(config[i]);
	}
	
	data = (byte[0]<<8) | byte[1];
	
	return data>>4;
}

