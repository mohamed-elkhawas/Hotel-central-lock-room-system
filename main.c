#include <string.h>
#include <stdint.h>
#include "E:/keil/Labware/inc/tm4c123gh6pm.h"
void SystemInit(){}
///varibles///
unsigned char ch ;
char * input;
uint32_t I=0,temp[2][4];
char * * room;// id cond pass{4}
int roomsnumber ,ww ,w,b,f,uarti=0;
int * passi;	
///drivers///
///UART///		
#define UART_FR_TXFF            0x00000020  
#define UART_FR_RXFE            0x00000010
#define UART_LCRH_WLEN_8        0x00000060
#define UART_LCRH_FEN           0x00000010
#define UART_CTL_UARTEN         0x00000001
void myuart_Init(void){
	UART0_CTL_R = 0;
	SYSCTL_RCGCUART_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x01;
	while((SYSCTL_PRGPIO_R & 0x01) == 0){}
  UART0_CTL_R &= ~UART_CTL_UARTEN;      
  UART0_IBRD_R = 43;// IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
  UART0_FBRD_R = 26;// FBRD = round(0.402778 * 64) = 26
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= UART_CTL_UARTEN;       	
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTA_CR_R |= 0x01;
	GPIO_PORTA_DIR_R &= 0xFE;
	GPIO_PORTA_AFSEL_R |= 0x01;
	GPIO_PORTA_PCTL_R |= 0x01;
	GPIO_PORTA_DEN_R |= 0x01;
	GPIO_PORTA_AMSEL_R &= 0xFE;	
}	
unsigned char myUART_InChar(void){
  while((UART0_FR_R&UART_FR_RXFE) != 0); // not empty
  return((unsigned char)(UART0_DR_R&0xFF));
}
void uart_String(char *string) {
int l=0;
char c;
c = myUART_InChar();
  while(c != 'e'){  string[l] = c;  I++;
c = myUART_InChar(); }
string[l] = '\0';}
///KEYPAD0///
void keypad0_init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000002; /* enable clock to GPIOB */ //c as output
	SYSCTL_RCGCGPIO_R |= 0x00000008; /* enable clock to GPIOD */ //e as input
	while((SYSCTL_PRGPIO_R&0x00000008) == 0){};
	GPIO_PORTB_LOCK_R = 0x4C4F434B;
	GPIO_PORTB_CR_R |= 0x0F;
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R |= 0xF0;
	GPIO_PORTB_DIR_R |= 0x0F;
	GPIO_PORTB_DEN_R |= 0x0F;
	GPIO_PORTB_ODR_R |= 0x0F;
	GPIO_PORTD_DIR_R &= ~0xF0; 
	GPIO_PORTD_DEN_R |= 0xF0;
	GPIO_PORTD_PUR_R |= 0xF0;
	GPIO_PORTD_ICR_R = 0xFF;
	GPIO_PORTD_IM_R |= 0xFF;
	NVIC_EN1_R |=0X00000008 ;
}
unsigned char keypad0_getkey(void){
	const unsigned char keymap[4][3] = {
	{ '1', '2', '3'},{ '4', '5', '6' },{ '7', '8', '9' },{ 'n', '0', 'n' } };
	int row, col;
	GPIO_PORTB_DATA_R = 0;
	col = GPIO_PORTE_DATA_R & 0xF0;
	if (col == 0xF0) return 'n';
	while (1){
		row = 0;
		GPIO_PORTB_DATA_R = 0x0E;
		col = GPIO_PORTD_DATA_R & 0xF0;
		if (col != 0xF0) break;
		row = 1;
		GPIO_PORTB_DATA_R = 0x0D;
		col = GPIO_PORTD_DATA_R & 0xF0;
		if (col != 0xF0) break;
		row = 2;
		GPIO_PORTB_DATA_R = 0x0B;
		col = GPIO_PORTD_DATA_R & 0xF0;
		if (col != 0xF0) break;
		row = 3;
		GPIO_PORTB_DATA_R = 0x07;
		col = GPIO_PORTD_DATA_R & 0xF0;
		if (col != 0xF0) break;
	return 'n';
	}
	if (col == 0xE0) return keymap[row][0];
	if (col == 0xD0) return keymap[row][1];
	if (col == 0xB0) return keymap[row][2];
	return 'n';
}
void GPIOD_Handler(){ // keypad0handler  0 = ocuipped , 1 = clean , 2 = closed
	if(room[0][1] == '0'){ // ocuipped 
			ch = keypad0_getkey();
				if(ch != 'n'){I = (int) ch;
				temp[0][passi[0]]=I;
				passi[0] ++;
				if(passi[0] == 4){
					passi[0]=0;f=0;
				for(b=0;b<4;b++){
						 if(temp[0][b] != room[0][b+2]){				f=1;	break;	 }	} 
						if(f==0){GPIO_PORTF_DATA_R |= 0x01;} // port f open door1
					}
				else{GPIO_PORTF_DATA_R &= 0xFE; }//CLOSE
				}
			}
		else if(room[0][1] == 1){ // cleaning 
				GPIO_PORTF_DATA_R |= 0x01;
			} 
		else{GPIO_PORTF_DATA_R &= 0xFE;} //colse
	GPIO_PORTE_ICR_R = 0xFF;
}	
///OUTPUT///
void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x00000020;
  while((SYSCTL_PRGPIO_R&0x20) == 0){}
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0x1F;
  GPIO_PORTF_DIR_R = 0x1F;
  GPIO_PORTF_DEN_R = 0x1F; }
void init_all(){
	keypad0_init(); myuart_Init();	PortF_Init();
}
int main(void){ 
	init_all();
	GPIO_PORTF_DATA_R = 0x00; // close all doors
	__enable_irq(); //enable inturrepts
	uart_String(input);
	roomsnumber = (int)input;
//intialize room
	for( ww=0;ww<roomsnumber;ww++){
		uart_String(input);
		strcpy(room[ww], input); 
		passi[ww]=0;
	}	
	while(1){ uart_String(input);
		if(strlen(input) != 0){
	for( ww=0;ww<roomsnumber;ww++){
		if(room[ww][0] == input[0]){
strcpy(room[ww], input);
if(room[ww][1] == '1') {GPIO_PORTF_DATA_R |= 0x01;} if(room[ww][1] == '2') {GPIO_PORTF_DATA_R &= 0xFE;}	}}}}}
