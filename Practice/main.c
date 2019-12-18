/*
 * Practice1.c
 *
 * Created: 18/12/2019 10:45:22
 * Author : PowerStack
 */ 
//Diferencia entre #include "" e #include <> 
//#include "" debe estar en la raíz del proyecto, no esta en atmel
//#include <> propio de atmel studio
#include <avr/io.h>
#include <inttypes.h> //char == u_int8
#include <avr/interrupt.h> //set up easy ISR (interrup service routine)
#include <stdio.h> //Estandares oara comunicación serial
				   // Ejemplo: getchar, putchar
				   // Para usar USART
//#include "vart.h" //Control de errores en transmisión de datos
//las constantes tienen exactamente 1 byte
#define t1 250 //Para que el led titile a 1 Hz     
#define t2 125 //para 8 Hz
#define t3 60 //Para transicion de una tarea a otra

//volatile unsigned char time1, time2, time3 presentes en la RAM
volatile unsigned char time1, time2, time3; //Variables globales
unsigned char tsk2c, tsk3m, led;
//tsk2c contador
//tsk3m signal task1 -> task3
//led mantiene el estado del led
unsigned int time; //Almacenar el estado del tiempo.
//Las instrucciones deber ser lo más cortas posibles porque se toca hardware
ISR(TIMER0_COMPA_vect){
	//Construccion de 3 timers virtuales
	if (time1 > 0) --time1;
	if (time2 > 0) --time2;
	if (time3 > 0) --time3;
}

void initialize(void){
	DDRB = 0xF0; //Pin de entrada/salida 
				// Del registro B 4 bits de entrada y 4 bits de salida
	//PORTB = 0 //Esta enviando  0 a todo el puerto -- Debe enviar 0 solo a puertos de salida
	PORTB = 0x0F; //Esta cargando los pines con voltaje de 0v y 5v
	//Pasos para activar ISR
	TIMSK0 = (1<< OCIE0A); //activa bit OCIEA0 con 1 y desactiva con 0
	OCR0A = 249; //Tiempo para timer
	TCCR0A = (1<<WGM01); //Borra el registro cuando se hayan contado 249 tics // clear on Match
	TCCR0B = 0b00000101; //Se almacena info de pre escaler que se quiere usar (en este caso 64)
						//Se puede asignar directamente 3
						//Si se quiere usar otro preescaler se debe revisar hoja de datos.
	led = 0xFF; //Esta con 8 bist activados (255)
	time1 = t1;
	time2 = t2;
	time3 = t3;
	tsk3m = 0;
	tsk2c = 4;
	sei(); //Set Interrup enable
}

void task1(void){
	if(tsk3m !=0) time1 >>=2; //Si el boton esta precionado 
	led = led^0x20; //
	PORTB = led;
}
void task2(void){
	if(tsk2c =0){
		tsk2c=4;
		led=led^0x80;
		PORTB=led;
	}
}
void task3(void){
	tsk3m=~PINB & 0x80; //~Negación
}
int main(void)
{
    initialize();
    while (1) { //Mientras en microcontrolador este funcionando
		if(time1==0){time1=t1; task1();}
		if(0==time2){time2=t2; task2();}
		if(0==time3){time3=t3; task3();}
    }
}

