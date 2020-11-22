/*
 * KMP.c
 *
 * Created: 14.11.2020 13:32:30
 * Author : Black_DEXP
 */ 

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#define LCD_PORT PORTA
#define LCD_DDR DDRA
#define RS PORTA4
#define EN PORTA5



#define size_mas 20
typedef struct {
	uint8_t x;
	uint8_t y;
} position;
int speed=1;
position mass[size_mas];
position run_man_position;
void LCDPrint(char *str, uint8_t row, uint8_t col) {
	
	if (row == 0) {
		LCDSendCommand(0x80 | col);
	}
	else {
		LCDSendCommand(0xC0 | col);
	}

	for (char *ptr = str; *ptr != '\0'; ptr++) {
		//отправляем данные -> lcd
		LCD_PORT |= (1 << RS);
		LCDWriteByte(*ptr);
	}
}

void LCDWriteByte(uint8_t data) {
	LCD_PORT &= 0xF0;
	
	LCD_PORT |= ((data & 0xF0) >> 4);
	LCD_PORT |= (1 << EN);
	_delay_ms(40);
	LCD_PORT &= ~(1 << EN);
	_delay_ms(40);
	
	LCD_PORT &= 0xF0;
	
	LCD_PORT |=(data & 0x0F);
	LCD_PORT |= (1 << EN);
	_delay_ms(40);
	LCD_PORT &= ~(1 << EN);
	_delay_ms(40);
}

inline void LCDSendCommand(uint8_t cmd) {
	LCD_PORT &= ~(1 << RS);
	LCDWriteByte(cmd);
}

void LCDInit(){
	LCD_DDR = 0xFF;
	LCD_PORT = 0x00;

	LCDSendCommand(0x02); //Режим 4bit
	LCDSendCommand(0x28); //Инициализация ЖК-дисплея 16X2 в 4-битном режиме
	LCDSendCommand(0x0C); //Дисплей->ВКЛ/Курсор->ВЫКЛ
	LCDSendCommand(0x06); //Курсор автоматического увеличения
}


int main(void)
{
    LCDInit();

	MCUCR = (0 << ISC01) | (1 << ISC00) | (0 << ISC11) | (1 << ISC10);
	GICR |= (1 << INT0) | (1 << INT1);
	TCCR1B |= (1 << WGM12);
	TIMSK |= (1 << OCIE1A);

	Game();
	sei();
    while (1) 
    {

    }
}




void kill_run_man(int x,int y)
{

		
		LCDPrint(" ", y, x);
		
	
}
uint16_t play_delay = 500;
void Game(){
	LCDSendCommand(0x01);
	LCDPrint("RUN MAN GAME", 0, 2);
	_delay_ms(1000);
	LCDSendCommand(0x01);
	LCDPrint("LETS GO!", 0, 4);
	_delay_ms(1000);
	LCDSendCommand(0x01);
	
	run_man_position.x=1;
	run_man_position.y=1;
	LCDPrint(".", run_man_position.x, run_man_position.y);
	//mass[0].x = 1;
	//for(int i=0;i<size_mas;i++)
	//{
		//if(i!=0)
		//mass[i].x=mass[i-1].x+2;
		//else
		//mass[i].x=3;
		//mass[i].y=rand() % 2;
		//LCDPrint("|", rand() % 2,mass[i].x);
	//}
	//generator_of_obstacles();
	lvl_2();


}
//вверх
ISR(INT0_vect) {

	OCR1A = speed * 2;
	if(run_man_position.y==1)
	{
		
		kill_run_man(run_man_position.x,run_man_position.y);
		run_man_position.y=0;
		LCDPrint(".", run_man_position.y, run_man_position.x);
	}
}

//вниз
ISR(INT1_vect) {
	
	if(run_man_position.y==0)
	{
		kill_run_man(run_man_position.x,run_man_position.y);
		run_man_position.y=1;
		LCDPrint(".", run_man_position.y, run_man_position.x);
	}

}
int pos_x=13;
int pos_y=0;
void endGame()
{
	cli();
	LCDSendCommand(0x01);
	LCDPrint("you lose", 0, 2);
	TCCR1B = 0;
}
void chekPixel()
{
	
}



int counter=0;
int temp_counter=0;
int temp_counter_2=0;
void displeyScrollOnCounter(int n){

	for(int i=0;i<n;i++)
	{
		LCDSendCommand(0x1C);
		
	}
}
//////////////////////////////////
void generator_of_obstacles(){
		int temp_x=0;
		int temp_y=0;
		//speed = 500;
		OCR1AH = 0b01111010;
		OCR1AL = 0b00010010;
		//TCNT1 = 0;
		TCCR1B |= (1 << CS11) | (1 << CS10) ; // div 1024
		for(int i=0;i<size_mas;i++)
		{
			temp_x+=3;
			temp_y=rand() % 2;
			mass[i].x=temp_x;
			mass[i].y=temp_y;
			LCDPrint("|", temp_y,temp_x);
		}
		
}

void lvl_2(){
			int temp_x=0;
			int temp_y=0;
			//speed = 500;
			OCR1AH = 0b01111010;
			OCR1AL = 0b00010010;
			//TCNT1 = 0;
			TCCR1B |= (1 << CS10); // div 256
			for(int i=0;i<size_mas;i++)
			{
				temp_x+=3;
				temp_y=rand() % 2;
				mass[i].x=temp_x;
				mass[i].y=temp_y;
				LCDPrint("|", temp_y,temp_x);
			}
}
void RunGame(){
	
	
	if(counter==40)
	{
		LCDSendCommand(0x01);
		generator_of_obstacles();
		//lvl_2();
		run_man_position.x=1;
		counter=0;
		 
	}
	kill_run_man(run_man_position.x,run_man_position.y);
	run_man_position.x++;
	if(run_man_position.x==mass[temp_counter].x)
	{
		if(run_man_position.y==mass[temp_counter].y)
		{
			//endGame();
		}
		else
		temp_counter++;
	}

	LCDPrint(".", run_man_position.y, run_man_position.x);
	LCDSendCommand(0x18);
	counter++;

	
}


ISR(TIMER1_COMPA_vect)
{
	cli();
	RunGame();
	sei();
}