/*
 * File:   main.c
 * Author: Gabriel Carrera 21216
 * Created on August 7, 15:42 PM
 */

// Código de PIC Maestro - i2C

// --------------- Palabra de Configuracion ---------------
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// --------------- Librerias ---------------
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD.h" // librería LCD
#include "I2C.h" // librería I2C

// --------------- Frecuencia ---------------
#define _XTAL_FREQ 8000000 // Frecuencia 8 MHz

// --------------- Variables ---------------
unsigned char voltaje1; //Variable para guardar valor del voltaje del potenciómetro 1
int vol1; //Variable para guardar valor del voltaje del potenciómetro 1 mapeado
unsigned int unidad1; //Variable para guardar unidades del voltaje del AN0
unsigned int decima1; //Variable para guardar decimas del voltaje del AN0
unsigned int centesima1; //Variable para guardar centesimas del voltaje del AN0
char buffer[20]; //Arreglo para guardar valores de voltajes

// --------------- Prototipos ---------------
void setup(void); // funcion de configuracion
int map(unsigned char value, int inputmin, int inputmax, int outmin, int outmax){ //función para mapear valores
    return ((value - inputmin)*(outmax-outmin)) / (inputmax-inputmin)+outmin;}


// --------------- Loop principal ---------------
void main(void) {
    setup(); // Llamada a la funcion de configuracion
    
    //------------LCD------------------------
    Lcd_Init(); // Iniciar la LCD
    Lcd_Clear(); // Limpiar la LCD
    
    Lcd_Set_Cursor(1,1); //Cursor en (1,1)
    Lcd_Write_String("S1:"); //Escribir S1 en pantalla
    
    while(1){
        //I2C_Master_Start();
        //I2C_Master_Write(0x50);
        //I2C_Master_Write(PORTB);
        //I2C_Master_Stop();
        //__delay_ms(200);
       
        I2C_Master_Start(); // Iniciar I2C como maestro
        I2C_Master_Write(0x51); // Enviar direccion del esclavo y enviar bit para que el esclavo mande datos
        voltaje1 = I2C_Master_Read(0); // Leer el valor del esclavo y guardarlo en la variable
        I2C_Master_Stop(); // Terminar I2C
        __delay_ms(200);  // delay
        
        vol1 = map(voltaje1, 0, 255, 0, 100); //mapear valor del voltaje de 0 a 100
        unidad1 = (vol1*5)/100; //Separar las unidades del valor del voltaje
        decima1 = ((vol1*5)/10)%10; //Separar las decimas del valor del voltaje
        centesima1 = (vol1*5)%10; //Separar las centesimas del valor del voltaje
        Lcd_Set_Cursor(2,1); //Cursor en (1,7)
        sprintf(buffer, "%d.%d%dV" , unidad1 , decima1 , centesima1 ); //convertir variable a una cadena de caracteres
        Lcd_Write_String(buffer); //Mostrar cadena de caracteres en pantalla LCD
    }
}

// --------------- Rutina de  interrupciones ---------------
void __interrupt() isr(void){ // interrupciones
    
}

// --------------- Setup General ---------------
void setup(void){
    
// --------------- Definir analogicas ---------------
    ANSEL = 0; //Puertos como I/O digitales
    ANSELH = 0; //Puertos como I/O digitales
    
    // --------------- Configurar puertos --------------- 
    
    TRISB = 0; // Puerto B como salida - LCD
    TRISD = 0; // Puerto D como salida - LCD
    //TRISE = 0;
    
    
// --------------- Limpiar puertos ---------------    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;    
    
    // --------------- Habilitar pullups --------------- 
    //OPTION_REGbits.nRBPU = 0; 
    //WPUBbits.WPUB6 = 1;
    //WPUBbits.WPUB7 = 1; 

// --------------- Banderas e interrupciones --------------- 
    //INTCONbits.GIE = 1;   // Habilitar interrupciones globales
    //INTCONbits.PEIE = 1;  // Habilitar interrupciones de perifericas
    //INTCONbits.RBIE = 1;  // Habilitar interrupciones en PORTB
    
    //IOCBbits.IOCB6 = 1;   // Habilitar interrupciones en RB6
    //IOCBbits.IOCB7 = 1;   // Habilitar interrupciones en RB7
    
    // Utilizar la libreria para habilitar pullup e IOCB de cada boton deseado
    //ioc_init(7);
    //ioc_init(6);
    
    //INTCONbits.RBIF = 0;  // Limpiar bandera de interrupcion de PORTB
    
// --------------- Oscilador --------------- 
    OSCCONbits.IRCF = 0b111 ; // establecerlo en 8 MHz
    OSCCONbits.SCS = 1; // utilizar oscilador interno
    
    // Inicializacion del i2C con la libreria
    I2C_Master_Init(100000); // Inicializar Comuncación I2C en 100 kHz - MAESTRO
}