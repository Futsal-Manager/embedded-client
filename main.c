/*
*   SW_Marestro_Project_2_Ball_Tracker_FutSal_Manager
*
*  File Name : main.c
*  Author    : B W KANG
*  Version   : Ver 1.0
*  Date      : 2017.02.09
*
*/

#include<stm32f10x.h>
#include<stdio.h>
#include"delay.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                       Using for printf in UART                       //

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
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void Rcc_Initialize(void);
void Gpio_Initialize(void);
void Nvic_Initialize(void);
void UART1_Initialize(void);
void USART1_IRQHandler(void);
void UART2_Initialize(void);
void USART2_IRQHandler(void);
void UART4_Initialize(void);
void USART4_IRQHandler(void);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                               Main                                   //

void main(void)
{
  Rcc_Initialize();
  Gpio_Initialize();
  
  while(1)
  {
 
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
  
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //GPIO A
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //GPIO C 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//FB ������� UART1 clock���.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//HC-06 UART2 clock ��� 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);// ��������  UART4 clock ���
 


}


//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                          GPIO Setting                                //
	
void Gpio_Initialize(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
   
   //FB ������� TX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//�� 9�߰� 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //FB ������� RX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//�� 10�߰� 
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
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   //USART2_IRQn enable
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //enable the USART2 Interrupt
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   //USART4_IRQn enable
   NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //enable the USART4 Interrupt
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
   
   
   USART_InitStructure.USART_BaudRate = 9600; 
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


void UART2_Initialize(void)//HC-06 �������
{
 
   USART_InitTypeDef USART_InitStructure;
   USART_InitStructure.USART_BaudRate = 19200 ;
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
   volatile char temp;
  
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
   {
      USART_ClearITPendingBit(USART2, USART_IT_RXNE);
      //printf("%c",USART_ReceiveData(USART2));
      
      temp = USART_ReceiveData(USART2);
      
   }
   
}
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                            UART4 Setting                             //


void UART4_Initialize(void)//������ �Ÿ�����(��)
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

