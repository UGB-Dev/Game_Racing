/*
    SPI_MASTER.C 
    _______
   |        |--- MOSI (PIN 19)
   |        |--- SCK  (PIN 18)
   |        |--- RST  (PIN 21)
   | RP2040 |--- CS   (PIN 17)
   |        |--- DC   (PIN 20)
   |        | 
   |________|

*/

#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "testimg.h"

void DemoTFT(void);
uint8_t r=0;

typedef struct{
    int X;
    int Y;
}Position;

typedef struct{
    Position Posicion;
    Position Velocidad;
    uint8_t R;
}BALL;

BALL Pelota;

uint16_t Buf[40*30];

/// dx = 35
/*const uint16_t Pos_Sprite[][2] = {{0, 5}, {45, 5}, {85, 5}, {170, 172}, {127, 172},{85, 172}, {125, 5}, {169, 5},
                                  {0, 48}, {44, 48}, {83, 48}, {125, 48}, {168, 48},
                                  {0, 88}, {44, 88}, {85, 88}, {127, 88}, {168, 88},
                                  {0, 130}, {44, 130}, {86, 130}, {128, 130}, {169, 130},
                                  {0, 172}, {44, 172}, {40,30}};*/

const uint16_t Pos_Sprite[][2] = {{5, 5}, {42, 5}, {78, 5}, {109, 5}, 
                                  {111, 30}, {78, 30}, {43, 30}, {5, 30},
                                   {29,21}};


void ST7735_Print_Sprite(uint8_t X_pos, uint8_t Y_pos, const uint16_t* Sprite, uint16_t Color);
void ST7735_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint8_t X_Pos_Destino, uint8_t Y_Pos_Destino, const uint16_t* Sprite);

const uint16_t Bird_UP[] = {
    11, 10, 0x00F8, 0x0104, 0x02E2, 0x0211, 0x0211, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

const uint16_t Bird_DOWN[] = {
    11, 10, 0x00F8, 0x0104, 0x0272, 0x0281, 0x0281, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

int main(){
    //intialize stdio
    stdio_init_all();

    // intialize the SPI0 of Raspberry Pi
    // This example will use SPI0 at 4MHz. 
    spi_init(SPI_PORT, 4000 * 1000);
    //gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_init(LCD_DC); //RS PIn
    gpio_set_dir(LCD_DC, GPIO_OUT);

    Pelota = (BALL){{15, 15}, {2, 2}, 6};

    // call the LCD initialization
    ST7735_Init();
    ST7735_SetRotation(1);
    ST7735_Clear_Buffer(BackGround);

    while(1){
        DemoTFT();
    }
    return 0;
}


void DemoTFT(void){
    for(uint32_t i=0; i<8; i++){
        ST7735_Print_Sprite_Trans(Pos_Sprite[i][0], Pos_Sprite[i][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 59, 40, gg );
        ST7735_Print_Buffer();
        ST7735_Clear_Buffer(BackGround);
        sleep_ms(100);
    }

    /*for(uint32_t i=0; i<80; i++){
        for(uint32_t J=0; J<160; J++){
            ST7735_Print_Pixel(J,i, gg[J+(i*208)]);
        }
    }

    ST7735_Print_Buffer();
    ST7735_Clear_Buffer(ST7735_CYAN);*/



    /* CALCULA LA POSICION (X, Y) DE ACUERDO AL VECTOR VELOCIDAD */
    /*Pelota.Posicion.X += Pelota.Velocidad.X;
    Pelota.Posicion.Y += Pelota.Velocidad.Y;

    if(Pelota.Posicion.X < Pelota.R || Pelota.Posicion.X > (159 - Pelota.R)){
        Pelota.Velocidad.X = -Pelota.Velocidad.X;
    }

    if(Pelota.Posicion.Y < Pelota.R || Pelota.Posicion.Y > (79 - Pelota.R)){
        Pelota.Velocidad.Y = -Pelota.Velocidad.Y;
    }*/

    //ST7735_FillScreen(ST7735_BLACK);
    
    //ST7735_Print_Sprite(0, 0, Bird_DOWN, ST7735_MAGENTA);
    /*for(uint32_t i=0; i<160; i++){
        for(uint32_t j=0; j<80; j++)
        if(gg[i+(j*160)] != 0){
            ST7735_DrawPixel(i, j, gg[i+(j*160)]);
        }
    }*/

   //ST7735_DrawImage(0,0,160,80,gg);

  //aqui
    /*for(uint32_t i=0; i<25; i++){
        ST7735_Print_Sprite_Trans(Pos_Sprite[i][0], Pos_Sprite[i][1], Pos_Sprite[25][0], Pos_Sprite[25][1], 80, 40, gg );
        ST7735_Print_Sprite_Trans(Pos_Sprite[24-i][0], Pos_Sprite[24-i][1], Pos_Sprite[25][0], Pos_Sprite[25][1], 30, 15, gg );
        ST7735_Print_Buffer();
        ST7735_Clear_Buffer(BackGround);
        //sleep_ms(500);
    }*/
   // aqui
}


void ST7735_Print_Sprite(uint8_t X_pos, uint8_t Y_pos, const uint16_t* Sprite, uint16_t Color){
    uint16_t Iterador;
    for (uint8_t X=2; X < Sprite[0]+2; X++) {
        Iterador=1;
        for (uint8_t Y=0; Y < Sprite[1]; Y++) {
            if(Sprite[X] & Iterador){ 
                ST7735_DrawPixel(X_pos+X-2, Y_pos+Y, Color);
            }
            Iterador<<=1; 
        }
    }
    
}

void ST7735_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint8_t X_Pos_Destino, uint8_t Y_Pos_Destino, const uint16_t* Sprite){
    for (uint16_t Y=0; Y < Y_Height; Y++) {
        for (uint16_t X=0; X < X_Width; X++) {
            Buf[X+(Y*X_Width)] = Sprite[(X_Pos+X)+((Y_Pos+Y)*143)]; //Se almacena la ubicacion del sprite
        }
    }

    for(uint32_t Y=0; Y<X_Width; Y++){
        for(uint32_t X=0; X<Y_Height; X++){
        if(Buf[Y+(X*X_Width)] != 0xFFFF){
            ST7735_Print_Pixel(X_Pos_Destino+Y, Y_Pos_Destino+X, Buf[Y+(X*X_Width)]);
        }
        }
    }
}