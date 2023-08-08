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
uint8_t segundo; //variable de segundos
uint8_t minuto; //variable de minutos
uint8_t hora; //variable de horas
uint8_t dia; //variable de dia
uint8_t mes; //variable de mes
uint8_t ano; //variable de año

// --------------- Prototipos ---------------
void setup(void); // funcion de configuracion
int map(unsigned char value, int inputmin, int inputmax, int outmin, int outmax){ //función para mapear valores
    return ((value - inputmin)*(outmax-outmin)) / (inputmax-inputmin)+outmin;}

uint8_t Read(uint8_t address); // Funcion para leer del RTC
void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h); // Funcion para leer el tiempo del RTC
void Read_Date(uint8_t *d, uint8_t *month, uint8_t *y); // Funcion para leer la fecha del RTC
uint8_t Dec_to_Bcd(uint8_t dec_number); //Función para pasar de decimal a bcd
uint8_t Bcd_to_Dec(uint8_t bcd); //Función para pasar de bcd a decimal


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
        
        Read_Time(&segundo, &minuto, &hora);
        Lcd_Set_Cursor(1,8); //Cursor en (1,8)
        sprintf(buffer, "%02u:%02u:%02u" , hora, minuto, segundo); //convertir variable a una cadena de caracteres
        Lcd_Write_String(buffer); //Mostrar cadena de caracteres en pantalla LCD
        
        Read_Date(&dia, &mes, &ano);
        Lcd_Set_Cursor(2,7); //Cursor en (2,7)
        sprintf(buffer, "%02u/%02u/20%02u" , dia, mes, ano); //convertir variable a una cadena de caracteres
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

// Funciones

// Funcion para leer del RTC
uint8_t Read(uint8_t address){ //Función para obtener datos
    uint8_t dato = 0; //Variable temporal
    I2C_Master_Start(); //Iniciar i2c
    I2C_Master_Write(0xD0); //Introducir dirección del esclavo
    I2C_Master_Write(address); //Introducir dirección 
    I2C_Master_RepeatedStart(); //Restart i2c
    I2C_Master_Write(0xD1); //Introducir dirección del esclavo más bit de escritura
    dato = I2C_Master_Read(0); //Almacenar dato en variable temporal
    I2C_Master_Stop(); //Stop i2c
    __delay_us(10); //delay de 10 us
    return dato; // Retornar dato leido
}

void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h){ //Función de obtener valores del tiempo
    *s = Bcd_to_Dec(Read(0x00)); //Obtener segundos
    *m = Bcd_to_Dec(Read(0x01)); //Obtener minutos
    *h = Bcd_to_Dec(Read(0x02)); //Obtener horas
}

void Read_Date(uint8_t *d, uint8_t *month, uint8_t *y){ //Función de obtener valores del tiempo
    *d = Bcd_to_Dec(Read(0x04)); //Obtener dia
    *month = Bcd_to_Dec(Read(0x05)); //Obtener mes
    *y = Bcd_to_Dec(Read(0x06)); //Obtener año
}

uint8_t Dec_to_Bcd(uint8_t dec_number){ //Función para pasar de numero decimal a bcd
    uint8_t bcd_number; //Variable para almacenar dato bcd
    bcd_number = 0; //Limpiar numero
    while(1){ //Loop
        if (dec_number >= 10){ //Convertir numero y repetir ciclo hasta que el numero sea menor que 10
            dec_number = dec_number - 10; //Restar 10
            bcd_number = bcd_number + 0b00010000; //Ir sumando diez en bcd
        }
        else { //Suma de números
            bcd_number = bcd_number + dec_number; //Suma
            break; //Salirse del loop
        }
    }
    return bcd_number; //Retornar valor BCD
}

uint8_t Bcd_to_Dec(uint8_t bcd){ //Función para pasar números de bcd a decimal
    uint8_t dec; //Variable para guardar valor
    dec = ((bcd>>4)*10)+(bcd & 0b00001111); // Hacer un corrimiento de bits y sumar con la unidad
    return dec; //Retornar valor
}