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

// USART_1 & printf 관련 define
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ascii)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ascii, FILE *f)
#endif 
#define BUFFER_SIZE 200

// BlueTooth 수신 데이터 저장 변수----------------------------------------------
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
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLL 설정8Mhz*9=72Mhz
    RCC_PLLCmd(ENABLE);
    
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);
  }
  
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //GPIO A
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //GPIO C 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//FB 블루투스 UART1 clock허용.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//HC-06 UART2 clock 허용 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);// 서보모터  UART4 clock 허용
 


}


//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                          GPIO Setting                                //
	
void Gpio_Initialize(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   
   
   //FB 블루투스 TX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//핀 9추가 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //FB 블루투스 RX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//핀 10추가 
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   

    //HC 블루투스 TX 
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //HC 블루투스 RX
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   
   //서보모터  TX 
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

void UART1_Initialize(void){ //PCLK2를 사용한다. 
  
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   
   
   USART_InitStructure.USART_BaudRate = 9600; 
   // baud rate= 데이터 통신에서 직렬 전송의 변조 속도를 1초간에 전송되는 신호의 수로 나타낸 값.
   
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   //8비트 또는 9비트로 설정 할 수있다. 
   
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   //하나의 단어(word)의 끝을 표시하기 위해 최후에 부가하는 2비트(1 1.5 2 로 설정 가능)
   
   USART_InitStructure.USART_Parity = USART_Parity_No;
   //1”의 비트의 개수가 짝수인지 홀수인지를 결정

   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   // 수신측에서 수신불가 상태인 경우 송신측에서 데이터 전송하지 않도록 하고 다시 수신가능 상태가 되었을 때만 데이터 전송하는 방식. 
   
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   //USART_CR1 레지스터에서 bit3 TE Transmitter enable 과 Bit 2 RE Receiver enable 를 설정하기 위함. 해당비트위치가 1이 되도록함. 
   
   USART_Init(USART1, &USART_InitStructure);
   //변수선언하고 0으로 초기화. 
   
   USART_Cmd(USART1, ENABLE); // 사용하려면 ENABLE 해줘야됨 
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
//UART1으로 수신된 값을 읽는 것. 
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)// 비트에 0이 아닐 경우에 데이터가 수신.
    {         
       
        control_data  = USART_ReceiveData(USART1); //0이 아닐 경우에 데이터를 읽는다. control_data= 버퍼에 값을 저장하는 역할을 하는 변수.
        
        //Buffer룰 구현해야하는 이유.
        
        //수신시간에비해 처리시간이 길어서 인터럽트 발생부분에서는 버퍼에 단순히 저장만하고 처리는 다른곳에서 한다.
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
        // pending bit를 clear.(안해주면 인터럽트가 처리되었는지 알수없고 다시 인터럽트가 발생한것으로 인지해서 계속 핸들러 호출) 
       
    }
    
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)//비트에 0이 아닐 경우에 데이터가 송신 
    {         
     
        USART_SendData(USART1,TxBuf[TxOutCnt]);
        
        if(TxOutCnt<BUFFER_SIZE-1) TxOutCnt++; // Txoutcount가 buffer size 보다 작으면 +1씩 샌다. 
        else TxOutCnt = 0;      
            
        if(TxOutCnt == TxInCnt)// Txoutcount가 Txincount가 되면 tx 끄고 RX 킨다. 
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


void UART2_Initialize(void)//HC-06 블루투스
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


void UART4_Initialize(void)//레이저 거리센서(뒤)
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

