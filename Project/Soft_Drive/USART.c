#include "stm32f10x.h"
#include "USART.h"
/*******************************************************************************
* Function Name  : USART1_Configuration
* Description    : Configures the USART1 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control Disabled
        - Receive and transmit enabled
*/
void USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  		  
   /* Configure the USART1*/ 
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);                           /* Enable the USART1 */
}
/*******************************************************************************
* Function Name  : USART2_Configuration
* Description    : Configures the USART2 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control Disabled
        - Receive and transmit 
*/
void USART2_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  		  
   /* Configure the USART2*/ 
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);   /* Enable the USART2 */
}
/*******************************************************************************
* Function Name  : USART2_GPIO_Configuration
* Description    : Configures the uart1 GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure USART2_Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure USART2_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
/*******************************************************************************
* Function Name  : USART1_GPIO_Configuration
* Description    : Configures the uart1 GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // Configure USART1_Tx as push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure USART1_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}
