// uLCD-144-G2 basic text demo program for uLCD-4GL LCD driver library
//
#include "mbed.h"
#include "uLCD_4DGL.h"
#include <iostream>
#include <stdio.h>
using namespace std::chrono;
#define freqModeMax 11
#define freqModeMin 0
#define sampleAmount 200

uLCD_4DGL uLCD(D1, D0, D2);                         // serial tx, serial rx, reset pin;
BusIn control(D12, D11, D10);                       // buttom: user control
AnalogOut aout(D7);
AnalogIn ain(A0);

Thread sampleThread;
Thread screenThread;
EventQueue sampleQueue; 
EventQueue screenQueue;

bool if_initial = 1;                                   // for monitor_display
bool if_generate = 0;
int freqTable[freqModeMax+1] = {1, 5, 10, 20, 30, 40, 50, 60, 70, 80 ,90 ,100}; // frequency for user choosing
int freqMode = 0;                                      // wave frequency mode
int freq = 0;
float delta_aout_up = 0;
float delta_aout_down = 0;
int sampleCount = 0;
float sample[sampleAmount] = {0};


void monitor_display();
void set_parameter();
void wave_generate();
void wave_sample();
void sample_print();

int main()
{
    sampleThread.start(callback(&sampleQueue, &EventQueue::dispatch_forever));
    screenThread.start(callback(&screenQueue, &EventQueue::dispatch_forever));
    while (1) 
    {
        monitor_display();
        wave_generate(); 
    }
}

void monitor_display()
{
    if (if_initial)
    {
        if_initial = 0;
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
            if_generate = !if_generate;
            if (if_generate)
            {
                set_parameter();
                sampleQueue.call(&wave_sample);
                uLCD.cls();
                uLCD.text_width(1);
                uLCD.text_height(1);
                uLCD.color(BLACK);
                uLCD.textbackground_color(RED);
                uLCD.locate(7, 15);
                uLCD.printf("Select"); 
                ThisThread::sleep_for(10ms);
                uLCD.textbackground_color(RED); 
                uLCD.locate(7, 15);
                uLCD.printf(" Stop ");  
                uLCD.text_width(3);
                uLCD.text_height(3);
                uLCD.color(RED);
                uLCD.textbackground_color(BLACK);
                uLCD.locate(0, 2);
                uLCD.printf("%4dHz", freqTable[freqMode]);
            }
            else
            {
                uLCD.text_width(1);
                uLCD.text_height(1);
                uLCD.color(BLACK);
                uLCD.textbackground_color(RED);
                uLCD.locate(7, 15);
                uLCD.printf("Select"); 
                ThisThread::sleep_for(10ms);
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
            break;
        
        case 0x4:
            if (if_generate) break;
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
            if (if_generate) break;
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
    delta_aout_up = 9*(float)freq/10000.0f;
    delta_aout_down = (float)freq/10000.0f;
}

void wave_generate()
{
    aout = 0.0f;
    if (if_generate)
    {
        while (aout <= 0.89f)
        {
            aout = aout + delta_aout_up;
            wait_us(80);
        }
        while (aout > 0.0f)
        {
            aout = aout - delta_aout_down;
            wait_us(80);
        }
    }
}

void wave_sample()
{
    if (!if_generate) return;
    sampleCount = 0;
    while (sampleCount < sampleAmount && if_generate)
    {
        sample[sampleCount++] = ain;
        ThisThread::sleep_for(1000ms/sampleAmount);
    }
    printQueue.call(&sample_print);
}

void sample_print()
{
    for (int j = 1; j <= 2; j++)
    {
        for (int i = 0; i < sampleAmount && if_generate; i++)
        {
            printf("%f\r\n", sample[i]);
            ThisThread::sleep_for(10ms);
        }
    }
    
    ThisThread::sleep_for(3s);
    sampleCount = 0;
    sampleQueue.call(&wave_sample);
}
