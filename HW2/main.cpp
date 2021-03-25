// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library
//
#include "mbed.h"
#include "uLCD_4DGL.h"
#include <stdio.h>
#define freqModeMax 9
#define freqModeMin 0

uLCD_4DGL uLCD(D1, D0, D2);                         // serial tx, serial rx, reset pin;
BusIn control(D12, D11, D10);                       // buttom: user control
AnalogOut aout(D7);


int freqTable[freqModeMax+1] = {100, 200, 300, 
                                400, 500, 600,
                                700, 800, 900,
                                1000};              // frequency for user choosing
int freqMode = 1;                                   // wave frequency mode
bool initial = 1;                                   // for monitor_display
bool construct = 0;
int freq = 0;
float delta_aout_up = 0;
float delta_aout_down = 0;

void monitor_display();
void set_parameter();
void wave_generate();


int main()
{
    while (1) 
    {
        monitor_display();
        set_parameter();
        wave_generate();
    }
}

void monitor_display()
{
    if (initial)
    {
        initial = 0;
        uLCD.background_color(BLACK);
        uLCD.cls();
        uLCD.textbackground_color(GREEN);
        uLCD.color(BLACK);
        uLCD.locate(0, 15);
        uLCD.printf(" Down "); 
        uLCD.locate(7, 15);
        uLCD.printf("Select");
        uLCD.locate(14, 15);
        uLCD.printf(" Up "); 
        uLCD.text_width(3);
        uLCD.text_height(3);
        uLCD.color(WHITE);
        uLCD.textbackground_color(BLACK);
        uLCD.locate(0, 2);
        uLCD.printf("%4dHz", freqTable[freqMode]);
    }

    switch (control)
        {
        case 0x2: 
            construct = 1;
            uLCD.text_width(1);
            uLCD.text_height(1);
            uLCD.color(BLACK);
            uLCD.textbackground_color(RED);
            uLCD.locate(7, 15);
            uLCD.printf("Select"); 
            ThisThread::sleep_for(10ms);
            uLCD.textbackground_color(GREEN);
            uLCD.locate(7, 15);
            uLCD.printf("Select"); 
            uLCD.text_width(3);
            uLCD.text_height(3);
            uLCD.color(RED);
            uLCD.textbackground_color(BLACK);
            uLCD.locate(0, 2);
            uLCD.printf("%4dHz", freqTable[freqMode]);
            break;
        
        case 0x4:
            construct = 0;
            if (freqMode+1 <= freqModeMax) freqMode++;
            uLCD.text_width(1);
            uLCD.text_height(1);
            uLCD.color(BLACK); 
            uLCD.textbackground_color(BLUE);
            uLCD.locate(14, 15);
            uLCD.printf(" Up "); 
            ThisThread::sleep_for(10ms);
            uLCD.textbackground_color(GREEN);
            uLCD.locate(14, 15);
            uLCD.printf(" Up ");
            uLCD.text_width(3);
            uLCD.text_height(3);
            uLCD.color(WHITE);
            uLCD.textbackground_color(BLACK);
            uLCD.locate(0, 2);
            uLCD.printf("%4dHz", freqTable[freqMode]); 
            break;
        
        case 0x1:
            construct = 0;
            if (freqMode-1 >= freqModeMin) freqMode--;
            uLCD.text_width(1);
            uLCD.text_height(1);
            uLCD.color(BLACK); 
            uLCD.textbackground_color(BLUE);
            uLCD.locate(0, 15);
            uLCD.printf(" Down "); 
            ThisThread::sleep_for(10ms);
            uLCD.textbackground_color(GREEN);
            uLCD.locate(0, 15);
            uLCD.printf(" Down "); 
            uLCD.text_width(3);
            uLCD.text_height(3);
            uLCD.color(WHITE);
            uLCD.textbackground_color(BLACK);
            uLCD.locate(0, 2);
            uLCD.printf("%4dHz", freqTable[freqMode]);
            break;

        default:
            break;
        }
}

void set_parameter()
{
    freq = freqTable[freqMode];
    delta_aout_up = 6*(float)freq/10000.0f;
    delta_aout_down = 4*(float)freq/10000.0f;
}

void wave_generate()
{
    aout = 0.0f;
    while (construct && n < 10000)
    {
        monitor_display();
        while (aout <= 0.89f && n < 10000)
        {
            aout = aout + delta_aout_up;
            wait_us(100);
        }
        while (aout > 0.0f && n < 10000)
        {
            aout = aout - delta_aout_down;
            wait_us(100);
        }
    }
}