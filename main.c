 /******************************************************************************
 * main.c
 * UART example for HM-10 BLE module
 * serial terminal setting: 8N1, 115200
 * UART command: 
 * "1#" turn off LED, 
 * "2#" turn on LED, 
 * "3#" display hello message.
 ******************************************************************************/
 #include <iocc2541.h>
 #include <string.h>
 
 typedef unsigned char uchar;
 typedef unsigned int  uint;
 
 #define LED1 P1_2

 char Rxdata[2] = " " ;  //rx buffer
 char Txdata[] = "Hello from CC2541 UART1\n\r" ;  //tx buffer
 int  txnum = sizeof(Txdata);
 
 char temp;             
 uchar datanum = 0;    
 uchar RXflag = 1;
 uchar TXflag = 0;
 
 /****************************************************************
 *InitClock
 *****************************************************************/
 void InitClock(void)
 {
 CLKCONCMD &= ~0x40; // use crystal 32MHZ
 while(CLKCONSTA & 0x40); // wait for clock to stabilize 
 CLKCONCMD &= ~0x47; // set clock as 32MHZ
 }
 
 /******************************************************************************
 *?InitLED
 ******************************************************************************/
 void InitLED(uchar On_Off)
 {
   P1SEL &= ~0x04;      //P1.2 io mode
   P1DIR |=  0x04;      ///p1.2 led output
   LED1 = On_Off;       ///
 }
 
 /******************************************************************************
 *InitUART
 *UART0 115200 8N1
 ******************************************************************************/
 void InitUART(void)
 {
   PERCFG &= ~0x01;     //USART0 alt1
   P2DIR &= ~0xc0;      //IO priority USART0 >USART1 >timer1
   P0SEL |= 0x0c;       //P0_2,P0_3 set as UART
   U0CSR |= 0x80;       //set UART mode
   U0CSR |= 0x40;       //enable USART0 receive
   
   U0UCR = 0x02;        //8N1
   U0GCR |=11;          //32MHz BAUD_E:11, 115200
   U0BAUD |= 216;       //32MHz BAUD_M:216 11520
   UTX0IF = 0;          //UART0 TX interrupt reset
   IEN0 |= 0x04;        //USART0 RX interrupt enable
   EA = 1;              //enable interrupt
 }
 
 /****************************************************************************
 * UartSendString()
 ****************************************************************************/
 void UartSendString(char *Data, int len)
 {
   uint i;
   U0CSR &= ~0x40;  //disable receive
   
   for(i=0; i<len; i++)
   {
     U0DBUF = *Data++;
     while(UTX0IF == 0);
     UTX0IF = 0;
   }
   TXflag = 0;  
   U0CSR |= 0x40;       //enable receiving
 }
 
 /******************************************************************************
 *UART0_ISR
 ******************************************************************************/
 #pragma vector = URX0_VECTOR 
 __interrupt void UART0_ISR(void) 
 { 
   URX0IF = 0;           //UART0 RX interrupt reset
   temp = U0DBUF;        //U0DBUF
 }
 
 /******************************************************************************
 *InitUART1
 * 8N1, 115200 alt2 location
 ******************************************************************************/
 void InitUART1(void)
 {
   PERCFG |= 0x02;     //USART1 at alt location 2 TX P1.6, RXP1.7
   P2DIR &= ~0x80;      //priority USART1>USART0 >TIMER1
   P2DIR |=0x40;
   P1SEL |= 0xc0;       //P1.6, P1.7 as peripheral
   U1CSR |= 0x80;       //set UART mode
   U1CSR |= 0x40;       //enable USART1 receive
   
   U1UCR = 0x02;        //8N1
   U1GCR |=11;          //32MHz BAUD_E:11, 115200
   U1BAUD |= 216;       //32MHz BAUD_M:216 11520
   UTX1IF = 0;          //UART1 TX interrupt reset
   IEN0 |= 0x08;        //IEN0.URX1IE=1
   EA = 1;              //enable interrupt
 }
 
 /****************************************************************************
 * Uart1SendString()
 ****************************************************************************/
 void Uart1SendString(char *Data, int len)
 {
   uint i;
   U1CSR &= ~0x40;  
   
   for(i=0; i<len; i++)
   {
     U1DBUF = *Data++;
     while(UTX1IF == 0);
     UTX1IF = 0;
   }
   TXflag = 0;  
   U1CSR |= 0x40;       //enable
 }
 
 /******************************************************************************
 * UART1_ISR
 ******************************************************************************/
 #pragma vector = URX1_VECTOR 
 __interrupt void UART1_ISR(void) 
 { 
   URX1IF = 0;           //UART1 RX interrupt reset
   temp = U1DBUF;        //read from U1DBUF
 }
 /******************************************************************************
 * main
 ******************************************************************************/
 int main(void)
 {  
   InitClock();
   InitLED(0);          //LED on
   InitUART1();          //UART1 init
   
   Uart1SendString(Txdata,txnum); 
   //uart receiving & command processing
   while(1)
   {
     if(RXflag == 1)
     {
       if(temp !=0)
       {
         if((temp != '#') && (datanum < 2))
         {
           Rxdata[datanum++] = temp;
         }
         else
         {
           RXflag = 2;
         }
 
           temp = 0;
       }
     }
     if(RXflag == 2)
     {
       switch(Rxdata[0])
       {
         case '1':LED1 = 0;break;
         case '2':LED1 = 1;break;
         case '3':
         {
            TXflag = 1;
            break;        
         }
         default: 
         {//LED1 = 0;
         }
       }
       RXflag = 1;
       memset(Rxdata, 0, 2);
       datanum = 0;
     }
     if(TXflag == 1)
     {
        Uart1SendString(Txdata,txnum);         
     }
   }
 }