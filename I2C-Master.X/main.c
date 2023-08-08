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
#include "IOCB.h" // librería IOCB

// --------------- Frecuencia ---------------
#define _XTAL_FREQ 8000000 // Frecuencia 8 MHz

// --------------- Variables ---------------
uint8_t selector = 0; // Variable para selector de modos con botones
uint8_t bandera = 0; //Variable para antirrebotes
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
void Set_sec(uint8_t sec); //Función para setear segundos
void Set_min(uint8_t min); //Función para setear minutos
void Set_hour(uint8_t hour); //Función para setear horas
void Set_day(uint8_t day); //Función para setear día
void Set_month(uint8_t month); //Función para setear mes
void Set_year(uint8_t year); //Función para setear año
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
    
    Set_sec(0); //Setear segundos en 0
    Set_min(0); //Setear minutos en 0
    Set_hour(12); //Setear hora en 12
    Set_day(8); //Setear día en 8
    Set_month(8); //Setear mes en agosto
    Set_year(23); //Setear año en 23
    
    while(1){
        //I2C_Master_Start();
        //I2C_Master_Write(0x50);
        //I2C_Master_Write(PORTB);
        //I2C_Master_Stop();
        //__delay_ms(200);
        if (selector == 0){ //Chequear el modo
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
            
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("N "); //Mostrar D de modo RELOJ NORMAL en LCD
        }
        else if (selector == 1){ //Chequar modo
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //MostraR en LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("D "); //Mostrar D de modo día en LCD
        }
        else if (selector == 2){
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar el LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("M "); //Mostrar M de modo mes en LCD
        }
        else if (selector == 3){ //Chequear modo
            Lcd_Set_Cursor(2,7); //Cursor en 2,7
            sprintf(buffer, "%02u/%02u/20%02u ", dia, mes, ano); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("A "); //Mostrar A de modo año en LCD
        }
        else if (selector == 4){ //Chequear modo
            Lcd_Set_Cursor(1,8); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("H "); //Mostrar h de modo hora en LCD
        }
        else if (selector == 5){ //Chequear modo
            Lcd_Set_Cursor(1,8); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("m "); //Mostrar m de modo minuto en LCD
        }
        else if (selector == 6){ //Chequar modo
            Lcd_Set_Cursor(1,8); //Cursor en 1,7
            sprintf(buffer, "%02u:%02u:%02u ", hora, minuto, segundo); //Función para pasar variables a cadena de caracteres
            Lcd_Write_String(buffer); //Mostrar en LCD
            Lcd_Set_Cursor(1,5); //Cursor en (1,5)
            Lcd_Write_String("S "); //Mostrar s de modo segundo
        }
        
    }
}

// --------------- Rutina de  interrupciones ---------------
void __interrupt() isr(void){ // interrupciones
    if (INTCONbits.RBIF == 1){ //Chequear interrupciones del puerto B
        INTCONbits.RBIF = 0; //Limpiar bandera
        if (PORTBbits.RB5 == 0){ //Ver si se presiono botón RB3
            __delay_ms(2); //delay de 1 ms
            bandera = 1; //Bandera en 1 
        }
        if (PORTBbits.RB5 == 1 && bandera == 1){ //Esperar a que se suelte botón RB5
            if (selector == 0){ //Chequear el modo 
                selector = 1; //Cambiar a 1 - CAMBIO DIA -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 1){ //Chequear modo
                selector = 2; //Cambiar a modo a 2- CAMBIO MES -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                selector = 3; //Cambiar modo a 3 - CAMBIO AÑO -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 3){ //Chequear modo
                selector = 4; //Cambiar a modo 4 - CAMBIO HORA -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                selector = 5; //Cambiar modo a 5 - CAMBIO MINUTO -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear modo
                selector = 6; //Cambiar modo a 6 - CAMBIO SEGUNDO -
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                selector = 0; //Cambiar a modo 0
                bandera = 0; //Limpiar bandera
            }
        }
        if (PORTBbits.RB7 == 0){ //Ver si se presionó RB2
            bandera = 2; //Bandera en 2
            __delay_ms(1); //delay de 1ms
        }
        if (PORTBbits.RB7 == 1 && bandera == 2){ //Ver si se dejó de presionar
            if (selector == 1){ //Chequear modo
                if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12){ //Chequear el mes
                    if (dia == 31){ //Si el día es 31 no sumar más
                        dia = 31; //Día igual a 31
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día
                        Set_day(dia); //Setear día
                    }
                }
                else if (mes == 4 || mes == 6 || mes == 9 || mes == 11){ //Chequear mes si es de 30 días
                    if (dia == 30){ //Si el día es 30 no sumar más
                        dia = 30; //Día en 30
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día normalmente
                        Set_day(dia); //Setear día
                    }
                }
                else if (mes == 2){ //Chequear si es febrero
                    if (dia == 28){ //Si el día es 28 no sumar más
                        dia = 28; //Día en 28
                        Set_day(dia); //Setear día
                    }
                    else { //Sino sumar normalmente
                        dia++; //Sumar día
                        Set_day(dia); //Setear día
                    }
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                if (mes == 12){ //Ver si el mes es 12
                    mes = 12; //No sumar más 
                    Set_month(mes); //Setear mes
                }
                else { //Sino sumar normalmente
                    mes++; //
                    Set_month(mes);   //Setear mes
                    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && (dia >> 30)){ //Chequear día 
                        Set_day(30); //Setear día 
                    }
                    else if (mes == 2 && dia >> 28){ //Chequear si es febrero y el día es mayor a 28
                        Set_day(28); //Setear día en 28
                    }
                    else {
                        Set_day(dia);
                    }
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 3){ //Chequear modo
                if (ano == 99){ //Ver si año es 99
                    ano = 99; //No sumar más y que se quede en 99
                    Set_year(ano); //Setear año
                }
                else { //Sino sumar normalmente
                    ano++; //Sumar año
                    Set_year(ano); //Setear año
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                if (hora == 23){ //Si hora es 23
                    hora = 23; //Hora en 23
                    Set_hour(hora); //Setear en hora
                }
                else { //Sino sumar normalmente
                    hora++; //Sumar hora
                    Set_hour(hora); //Setear hora
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear modo
                if (minuto == 59){ //Si el minuto es 59 no sumar más
                    minuto = 59; //Minuto en 59
                    Set_min(minuto); //Setear minuto
                }
                else { //Sino sumar normalmente
                    minuto++; //Sumar minuto
                    Set_min(minuto); //Setar minuto
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                if (segundo == 59){ //Chequer segundo si es 59
                    segundo = 59; //Segundo en 59
                    Set_sec(segundo); //Setear segundo
                }
                else { //Sino sumar normalmente
                    segundo++; //Sumar segundo
                    Set_sec(segundo); //Setear segundo
                }
                bandera = 0; //Limpiar bandera
            }
        }
        if (PORTBbits.RB6 == 0){ //Ver si se presiono al botón RB1
            bandera = 3; //Bandera en 3
            __delay_ms(1);
        }
        if (PORTBbits.RB6 == 1 && bandera == 3){ //Ver si se presióno el botón
            if (selector == 1){ //Chequear el modo
                if (dia == 1){ //Si día 1 no sumar más 
                    dia = 1; //Día en 1
                    Set_day(dia); //Setear día en 1
                }
                else { //Sino sumar normalmente
                    dia--; //Restar día
                    Set_day(dia); //Setear día
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 2){ //Chequear modo
                if (mes == 1){ //Si el mes es 1 no restar más
                    mes = 1; //Mes en 1
                    Set_month(mes); //Setear en 1
                }
                else { //Sino restar normalmente
                    mes--; //Restar mes
                    Set_month(mes); //Setear mes
                }
                bandera = 0;
            }
            else if (selector == 3){ //Chequear modo
                if (ano == 0){ //Ver si año 0 es 0 para no restar más
                    ano = 0; //Año en 0
                    Set_year(ano); //Setear año
                }
                else { //Sino restar normalmente
                    ano--; //Restar año
                    Set_year(ano); //Setear año
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 4){ //Chequear modo
                if (hora == 0){ //Si hora es 0 no restar más
                    hora = 0; //Hora en 0
                    Set_hour(hora); //Setear hora
                }
                else { //Sino restar normalmente
                    hora--; //Restar hora
                    Set_hour(hora); //Setear hora
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 5){ //Chequear bandera
                if (minuto == 0){ //Ver si minutos es 0 para no restar más
                    minuto = 0; //Minutos en 0
                    Set_min(minuto); //Setear minutos
                }
                else { //Sino no restar normalmente
                    minuto--; //Restar normalmente
                    Set_min(minuto); //Setear minutos
                }
                bandera = 0; //Limpiar bandera
            }
            else if (selector == 6){ //Chequear modo
                if (segundo == 0){ //Ver si segundo es 0
                    segundo = 0; //Segundo en 0
                    Set_sec(segundo); //Setear segundos
                }
                else { //Sino restar normalmente
                    segundo--; //Restar segundos
                    Set_sec(segundo); //Setear segundos
                }
                bandera = 0; //Limpiar bandera
            }
        }
    }
}

// --------------- Setup General ---------------
void setup(void){
    
// --------------- Definir analogicas ---------------
    ANSEL = 0; //Puertos como I/O digitales
    ANSELH = 0; //Puertos como I/O digitales
    
    // --------------- Configurar puertos --------------- 
    
    TRISBbits.TRISB0 = 0; // PIN B0 como salida - LCD
    TRISBbits.TRISB1 = 0; // PIN B1 como salida - LCD
    TRISBbits.TRISB5 = 1; // PIN B5 como entrada - botones
    TRISBbits.TRISB6 = 1; // PIN B6 como entrada - botones
    TRISBbits.TRISB7 = 1; // PIN B7 como entrada - botones
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
    INTCONbits.GIE = 1;   // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;  // Habilitar interrupciones de perifericas
    INTCONbits.RBIE = 1;  // Habilitar interrupciones en PORTB
    
    //IOCBbits.IOCB5 = 1;   // Habilitar interrupciones en RB5
    //IOCBbits.IOCB6 = 1;   // Habilitar interrupciones en RB6
    //IOCBbits.IOCB7 = 1;   // Habilitar interrupciones en RB7
    
    // Utilizar la libreria para habilitar pullup e IOCB de cada boton deseado
    ioc_init(7);
    ioc_init(6);
    ioc_init(5);
    
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

// Funcion para leer tiempo del RTC
void Read_Time(uint8_t *s, uint8_t *m, uint8_t *h){ //Función de obtener valores del tiempo
    *s = Bcd_to_Dec(Read(0x00)); //Obtener segundos
    *m = Bcd_to_Dec(Read(0x01)); //Obtener minutos
    *h = Bcd_to_Dec(Read(0x02)); //Obtener horas
}

// Funcion para leer fecha del RTC
void Read_Date(uint8_t *d, uint8_t *month, uint8_t *y){ //Función de obtener valores del tiempo
    *d = Bcd_to_Dec(Read(0x04)); //Obtener dia
    *month = Bcd_to_Dec(Read(0x05)); //Obtener mes
    *y = Bcd_to_Dec(Read(0x06)); //Obtener año
}

// Funcion para escribir segundos al RTC
void Set_sec(uint8_t sec){ //Función para setear segundo
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x00); //Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(sec)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para escribir minutos al RTC
void Set_min(uint8_t min){ //Función para setear minutos
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x01); //Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(min)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para escribir horas al RTC
void Set_hour(uint8_t hour){ //Función para setear horas
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x02); //Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(hour)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para escribir dia al RTC
void Set_day(uint8_t day){ //Función para setear numero de día
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x04); //Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(day)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para escribir mes al RTC
void Set_month(uint8_t month){ //Función para setear mes
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x05); //Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(month)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para escribir año al RTC
void Set_year(uint8_t year){ //Función para setear año
    I2C_Master_Start(); //Iniciar I2C
    I2C_Master_Write(0xD0); //Dirección del esclavo y bit de escritura
    I2C_Master_Write(0x06); // Dirección del registro a modificar
    I2C_Master_Write(Dec_to_Bcd(year)); //Mandar dato en BCD
    I2C_Master_Stop(); //Terminar i2c
}

// Funcion para traducir de decimales a BCD del RTC
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

// Funcion para traducir de BCD del RTC a decimales
uint8_t Bcd_to_Dec(uint8_t bcd){ //Función para pasar números de bcd a decimal
    uint8_t dec; //Variable para guardar valor
    dec = ((bcd>>4)*10)+(bcd & 0b00001111); // Hacer un corrimiento de bits y sumar con la unidad
    return dec; //Retornar valor
}