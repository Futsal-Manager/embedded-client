/*
*   SW_Marestro_Project_2_Ball_Tracker_FutSal_Manager
*
*  File Name : main.c
*  Author    : B W KANG
*  Version   : Ver 1.1
*  Date      : 2017.02.15
*
*/

#include<stm32f10x.h>
#include<stdio.h>
#include"delay.h"

////////////////////////////////////////////////////////////////////////////////
//                       Using for printf in UART                             //

// USART_1 & printf ���� define
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ascii)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ascii, FILE *f)
#endif 
#define BUFFER_SIZE 200

// BlueTooth ���� ������ ���� ����----------------------------------------------
volatile char control_data='S';
u8 RxBuf[BUFFER_SIZE];                    
u8 TxBuf[BUFFER_SIZE]; 
u8 TxInCnt=0;   
u8 TxOutCnt=0;     
u8 RxCnt=0;
volatile u16 a=0;

PUTCHAR_PROTOTYPE
{
    TxBuf[TxInCnt] = ascii;
    if(TxInCnt<BUFFER_SIZE-1) TxInCnt++;
    else TxInCnt = 0;     
    
    //Enable the USART1 Transmit interrupt     
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    return ascii; 
}
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                               Main                                   //

void Rcc_Initialize(void);
void Gpio_Initialize(void);
void Nvic_Initialize(void);
void UART1_Initialize(void);
void USART1_IRQHandler(void);
void UART2_Initialize(void);
void USART2_IRQHandler(void);
void TIM2_Initialize(void);
void TIM2_IRQHandler(void);
void UART4_Initialize(void);
void USART4_IRQHandler(void);
//func======================

void Timer2_Delay(u16 Delay);

//Motor_Position
void position_150(void);
void ID_setting_to_01(void);
void ID_setting_to_02(void);
void NO_Status_return(void);
void X_position_60(void);
void X_position(unsigned int Angle);
void X_position_2(unsigned int Angle,unsigned int RPM);

//==========================
/*
ù��° ���� 
 ���̵� : 1
 ���Ѱ� : 105 ~ 195 
 
�ι�° ���� 
 ���̵� : 2
 ���Ѱ� : 60 ~ 180 
*/
void main(void)
{  
  Rcc_Initialize();
  Gpio_Initialize();
  Nvic_Initialize();
  UART1_Initialize();
  UART2_Initialize();
  TIM2_Initialize();
  UART4_Initialize();
  
   //USART_SendData(USART2,0x41); //'A'
   Delay_ms(1000);
   X_position(150);  // Stand UP
   
  
  
  while(1)
  {
    /*
     Delay_ms(1000);
     X_position_2(60,30);
     
     Delay_ms(1000);
     X_position_2(120,30);
     
     Delay_ms(1000);
     X_position_2(180,30);
     
     Delay_ms(1000);
     X_position_2(120,30);
    */
    
    for(int i=60;i<180;i+=5)
    {
      Delay_ms(300);
      X_position_2(i,3);
    }
    
    for(int i=180;i>60;i-=5)
    {
      Delay_ms(300);
      X_position_2(i,3);
    }
  }
}

//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                           RCC Setting                                //
void Rcc_Initialize(void)
{
  ErrorStatus HSEStartUpStatus;
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if (HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK = SYSCLK
    
    RCC_PCLK2Config(RCC_HCLK_Div1); // 72Mhz
    RCC_PCLK1Config(RCC_HCLK_Div4); // 72/4 x2 = 36Mhz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLL ����8Mhz*9=72Mhz
    RCC_PLLCmd(ENABLE);
    
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);
  }
  
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //GPIO C 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //GPIO A 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);// FB ������� UART1 clock���.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);// Ÿ�̸�2 Ŭ����� (Delay)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);// HC ������� UART2 clock ��� 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);// ��������  UART4 clock ���

}


//                                                                      //
//////////////////////////////////////////////////////////////////////////


void Gpio_Initialize(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
   //FB ������� TX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //FB ������� RX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   

    //HC ������� TX 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //HC ������� RX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //��������  TX 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
   
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                          NVIC Setting                                //
void Nvic_Initialize(void)
{ 
   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   
   //USART1_IRQ enable
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;  //FB
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
    //TIM2_IRQn enable
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;     //Delay
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
    //USART2_IRQn enable
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  //HC
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   
   //USART4_IRQn enable
   NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  //Servo
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////  



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             UART1 Setting                            //

void UART1_Initialize(void){ //PCLK2�� ����Ѵ�. 
  
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   
   
   USART_InitStructure.USART_BaudRate = 19200; 
   // baud rate= ������ ��ſ��� ���� ������ ���� �ӵ��� 1�ʰ��� ���۵Ǵ� ��ȣ�� ���� ��Ÿ�� ��.
   
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   //8��Ʈ �Ǵ� 9��Ʈ�� ���� �� ���ִ�. 
   
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   //�ϳ��� �ܾ�(word)�� ���� ǥ���ϱ� ���� ���Ŀ� �ΰ��ϴ� 2��Ʈ(1 1.5 2 �� ���� ����)
   
   USART_InitStructure.USART_Parity = USART_Parity_No;
   //1���� ��Ʈ�� ������ ¦������ Ȧ�������� ����

   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   // ���������� ���źҰ� ������ ��� �۽������� ������ �������� �ʵ��� �ϰ� �ٽ� ���Ű��� ���°� �Ǿ��� ���� ������ �����ϴ� ���. 
   
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   //USART_CR1 �������Ϳ��� bit3 TE Transmitter enable �� Bit 2 RE Receiver enable �� �����ϱ� ����. �ش��Ʈ��ġ�� 1�� �ǵ�����. 
   
   USART_Init(USART1, &USART_InitStructure);
   //���������ϰ� 0���� �ʱ�ȭ. 
   
   USART_Cmd(USART1, ENABLE); // ����Ϸ��� ENABLE ����ߵ� 
   //USART1 Enable
   
   
   NVIC_InitStructure.NVIC_IRQChannel = 37;  //USART1_IRQChannel
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             UART1 Handler                            //


void USART1_IRQHandler(void)
//UART1���� ���ŵ� ���� �д� ��. 
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)// ��Ʈ�� 0�� �ƴ� ��쿡 �����Ͱ� ����.
    {         
       
        control_data  = USART_ReceiveData(USART1); //0�� �ƴ� ��쿡 �����͸� �д´�. control_data= ���ۿ� ���� �����ϴ� ������ �ϴ� ����.
        
        //Buffer�� �����ؾ��ϴ� ����.
        
        //���Žð������� ó���ð��� �� ���ͷ�Ʈ �߻��κп����� ���ۿ� �ܼ��� ���常�ϰ� ó���� �ٸ������� �Ѵ�.
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
        // pending bit�� clear.(�����ָ� ���ͷ�Ʈ�� ó���Ǿ����� �˼����� �ٽ� ���ͷ�Ʈ�� �߻��Ѱ����� �����ؼ� ��� �ڵ鷯 ȣ��) 
       
    }
    
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)//��Ʈ�� 0�� �ƴ� ��쿡 �����Ͱ� �۽� 
    {         
     
        USART_SendData(USART1,TxBuf[TxOutCnt]);
        
        if(TxOutCnt<BUFFER_SIZE-1) TxOutCnt++; // Txoutcount�� buffer size ���� ������ +1�� ����. 
        else TxOutCnt = 0;      
            
        if(TxOutCnt == TxInCnt)// Txoutcount�� Txincount�� �Ǹ� tx ���� RX Ų��. 
        {
          USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
          USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        }         
    }        
   
}



//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                            UART2 Setting                             //


void UART2_Initialize(void)//HC ������� 
{
 
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate = 9600 ;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode =USART_Mode_Rx |USART_Mode_Tx;
   USART_Init(USART2, &USART_InitStructure);
   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
   USART_Cmd(USART2, ENABLE);  
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             UART2 Handler                            //

void USART2_IRQHandler(void)

{
   volatile char USART2_Temp =0;
  
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
   {
      USART_ClearITPendingBit(USART2, USART_IT_RXNE);
      //printf("%c",USART_ReceiveData(USART2));
      
      USART2_Temp = USART_ReceiveData(USART2);
      
      if(USART2_Temp == 0x41)
      {
        position_150();
        Delay_ms(1000);
      }
      
      
   }
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             TIM_2 Settings                           //


void TIM2_Initialize(void) //0.1ms
{   
   
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

   TIM_TimeBaseStructure.TIM_Period = 99; //1~65535  
   TIM_TimeBaseStructure.TIM_Prescaler = 35;     
   //�ð� ��� �� = 36MHZ ���޹޴´�.36000,000  x 1/period  x 1/prescaler  [1200, 300=   10ms]
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  
   TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); //TIM enable
   TIM_Cmd(TIM2, DISABLE); //TIM2 enable

}

//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             TIM_2 Handler                            //



void TIM2_IRQHandler(void) //per 0.1s
{
 
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);               // Ÿ�̸� ����
  a++;                                      //Ÿ�̸� ī��Ʈ ������Ų��.
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);         //���ͷ�Ʈ ����
}

//                                                                      //
//////////////////////////////////////////////////////////////////////////

//(Packet Delay) TIM2 Delay �Լ�------------------------------------------------

void Timer2_Delay(u16 Delay)
{
   

  a=0;
   TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
   TIM_Cmd(TIM2, ENABLE);
 
   while(1)
   {
     if(a>Delay) break;
      //printf("T\n");
   }
 
   TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
   TIM_Cmd(TIM2, DISABLE);

}

//------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                            UART4 Setting                             //


void UART4_Initialize(void)// servo motor
{
 
    USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate = 1000000 ;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode =USART_Mode_Tx;
   USART_Init(UART4, &USART_InitStructure);
   USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
   USART_Cmd(UART4, ENABLE);  
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                             UART4 Handler                            //

void USART4_IRQHandler(void)
{
   
}


//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//                          ���� ���� �Լ�                              // 
/*
30RPM =0x10D
114RPM = 0x3ff = 1023

Position 150 = 0x1ff
Position 300 = 0x3ff

*/

void position_150(void) // center 150 150
{
   //��ü��  RPM30(269)�ӵ��� 150�� �� ��ġ ��Ų��.
 
 
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //ID
  USART_SendData(UART4,0xFE);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //LENGTH 
  USART_SendData(UART4,0x07);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //INSTRUCTION   DATA_WRITE
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //PARAMETERS  GOAL_POSITON
  USART_SendData(UART4,0x1E);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //PARAMETERS
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //PARAMETERS  
  USART_SendData(UART4,0x01);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  //PARAMETERS
  USART_SendData(UART4,0x0D);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  //PARAMETERS
  USART_SendData(UART4,0x01);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);

  //CHECKSUM
  USART_SendData(UART4,0xCB);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

//--------------------------- 

void X_position_60(void)
{
  unsigned char Table[18]={0xFF,0xFF,0xFE,0x0E,0x83,0x1E,0x04,0x01,0xFF,0x01,0x0D,0x01,0x02,0xCD,0x00,0x0D,0x01,0};
  unsigned char sum=0;
  unsigned char check_sum = 0;
  for(int i=0;i<17;i++)
  {
    USART_SendData(UART4,Table[i]);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    
    if (i > 1)
      sum += Table[i];  
  }
  check_sum = ~sum;  
  
  USART_SendData(UART4,check_sum);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    
}

//-----------------------------------------------------------------------------

void X_position(unsigned int Angle)
{
  unsigned char sum=0;
  unsigned char check_sum = 0;
  unsigned char Angle_FO =0;
  unsigned char Angle_BA =0;
  
  unsigned int temp = 1024 * Angle / 300;
  Angle_FO = (unsigned char)(temp / 255);
  Angle_BA = (unsigned char)temp;
  
  unsigned char Table[18]={0xFF,0xFF,0xFE,0x0E,0x83,0x1E,0x04,0x01,0xFF,0x01,0x0D,0x01,0x02,Angle_BA,Angle_FO,0x0D,0x01,0};
  
  for(int i=0;i<17;i++)
  {
    USART_SendData(UART4,Table[i]);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    
    if (i > 1)
      sum += Table[i];  
  }
  check_sum = ~sum; 
  
  USART_SendData(UART4,check_sum);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

//--------------------------------------------------------------------------


void X_position_2(unsigned int Angle,unsigned int RPM)
{
  unsigned char sum=0;
  unsigned char check_sum = 0;
  unsigned char Angle_FO =0;
  unsigned char Angle_BA =0;
  unsigned char RPM_FO =0;
  unsigned char RPM_BA =0;
  
  unsigned int temp = 1024 * Angle / 300;
  Angle_FO = (unsigned char)(temp / 255);
  Angle_BA = (unsigned char)temp;
  
  unsigned int temp_rp = 1024 * RPM / 114;
  RPM_FO = (unsigned char)(temp_rp / 255);
  RPM_BA = (unsigned char)(temp_rp);
  
  unsigned char Table[18]={0xFF,0xFF,0xFE,0x0E,0x83,0x1E,0x04,0x01,0xFF,0x01,RPM_BA,RPM_FO,0x02,Angle_BA,Angle_FO,RPM_BA,RPM_FO,0};
  
  for(int i=0;i<17;i++)
  {
    USART_SendData(UART4,Table[i]);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
    
    if (i > 1)
      sum += Table[i];  
  }
  check_sum = ~sum; 
  
  USART_SendData(UART4,check_sum);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  
}


//=========================================================================
void ID_setting_to_01(void)
{
  //��ü ID �� 1�� �����Ѵ�.
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //ID
  USART_SendData(UART4,0xFE);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Length
  USART_SendData(UART4,0x04);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Write
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Motor_ID_Adress
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Motor_ID
  USART_SendData(UART4,0x01);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);  
  
  //C S
  USART_SendData(UART4,0xF6);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
 
}

//----------------------------------
void ID_setting_to_02(void)
{
  //��ü ID �� 2�� �����Ѵ�.
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //ID
  USART_SendData(UART4,0xFE);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Length
  USART_SendData(UART4,0x04);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Write
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Motor_ID_Adress
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Motor_ID
  USART_SendData(UART4,0x02);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);  
  
  //C S
  USART_SendData(UART4,0xF5);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
 
}
//------------------------

void NO_Status_return(void)
{
  //status rturn ���� �ʰ� �Ѵ� 
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  USART_SendData(UART4,0xFF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //ID
  USART_SendData(UART4,0xFE);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Length
  USART_SendData(UART4,0x04);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //Write
  USART_SendData(UART4,0x03);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //STATUS_ID_Adress
  USART_SendData(UART4,0x10);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
  
  //false
  USART_SendData(UART4,0x00);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);  
  
  //C S
  USART_SendData(UART4,0xEB);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
 
}

