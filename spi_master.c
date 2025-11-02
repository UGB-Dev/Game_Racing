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
#include "GC9A01.h"
#include "fonts.h"
#include "testimg.h"
#include <stdlib.h>

void DemoTFT(void);

/* SE CREAN LAS FUNCIONES PARA CONTROLAR EL SPRITE */
#define N_Sprite 1
#define Display_Width 160
#define Display_Height 80
#define Mitad_Width ( GC9A01_WIDTH/2)
#define Mitad_Height ( GC9A01_HEIGHT/2)
#define SEGMENT_ROAD 20
#define LENGTH_ROAD 0.6
#define POV_Y 60.0
#define POV_X 10.0


typedef struct{
    uint16_t Width;
    uint16_t Height;
    uint16_t X;
    uint16_t Y;
    //uint16_t Buf;
}Ventana;

Ventana Sprite[N_Sprite] = {29, 21, 0, 0}; // Se crea N sprite

//Sprite = {29,21,0,0};
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

// dx = 35
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
void Demo_Racing(void);
void GC9A01_Demo_Racing(void);
void ST7735_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color);
void ST7735_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
void ST7735_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
void GC9A01_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint8_t X_Pos_Destino, uint8_t Y_Pos_Destino, const uint16_t* Sprite);


const uint16_t Bird_UP[] = {
    11, 10, 0x00F8, 0x0104, 0x02E2, 0x0211, 0x0211, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

const uint16_t Bird_DOWN[] = {
    11, 10, 0x00F8, 0x0104, 0x0272, 0x0281, 0x0281, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

uint8_t Toggle_Franja=0, set_Franja=0;

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
    gpio_init(LCD_RST_2);
    gpio_set_dir(LCD_RST_2, GPIO_OUT);
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_init(LCD_CS_2);
    gpio_set_dir(LCD_CS_2, GPIO_OUT);
    gpio_init(LCD_DC); //RS PIn
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_init(LCD_DC_2); //RS PIn
    gpio_set_dir(LCD_DC_2, GPIO_OUT);


    // call the LCD initialization
    ST7735_Init();
    ST7735_SetRotation(1);
    ST7735_Clear_Display(ST7735_CYAN);
    //ST7735_Clear_Buffer(BackGround);

    //GC9A01_Init();
    

    //GC9A01_Clear_Display(GC9A01_CYAN);
    //GC9A01_Print_Buffer();
    //ST7735_Init();

    float X_POS=5, x_POS=0, X_Linea=0;
    uint8_t avance=0;

    while(1){
        for(uint16_t Y_Pos=40; Y_Pos < 80-1; Y_Pos++, X_POS+=0.8, x_POS+=0.2, X_Linea+=0.05 ){
            ST7735_Linea(0, Y_Pos, 160-1, Y_Pos, ST7735_GREEN);
            

            if(avance%2 == 0){
                (Y_Pos % 10 < 5 )? ST7735_Linea(80-X_POS-x_POS, Y_Pos, 80+X_POS+x_POS, Y_Pos, ST7735_RED): 
                               ST7735_Linea(80-X_POS-x_POS, Y_Pos, 80+X_POS+x_POS, Y_Pos, ST7735_WHITE);
            }
            else{
                (Y_Pos % 10 < 5 )? ST7735_Linea(80-X_POS-x_POS, Y_Pos, 80+X_POS+x_POS, Y_Pos, ST7735_WHITE): 
                               ST7735_Linea(80-X_POS-x_POS, Y_Pos, 80+X_POS+x_POS, Y_Pos, ST7735_RED);
            }

            ST7735_Linea(80-X_POS, Y_Pos, 80+X_POS, Y_Pos, ST7735_GRAY);
            
            if(avance%2 == 0){
                (Y_Pos % 20 < 10 )? ST7735_Linea(80-X_Linea, Y_Pos, 80+X_Linea, Y_Pos, ST7735_WHITE):
                               ST7735_Linea(80-X_Linea, Y_Pos, 80+X_Linea, Y_Pos, 0xFFFF);
            }
            else{
                (Y_Pos % 20 < 10 )? ST7735_Linea(80-X_Linea, Y_Pos, 80+X_Linea, Y_Pos, 0xFFFF):
                               ST7735_Linea(80-X_Linea, Y_Pos, 80+X_Linea, Y_Pos, ST7735_WHITE);
            }
        }
        X_POS=5;
        x_POS= X_Linea=0;

        ST7735_Print_Sprite_Trans(Pos_Sprite[avance%8][0], Pos_Sprite[avance%8][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 65, 57, gg );
        ST7735_Print_Buffer();
        //ST7735_Set_BG(BG_, 0, 52, 240, 70);

        avance++;
        sleep_ms(100);

   
         //AQUI
        /*for(uint16_t Y_Pos=120; Y_Pos < GC9A01_HEIGHT-1; Y_Pos++, X_POS+=0.8, x_POS+=0.2, X_Linea+=0.05 ){
            GC9A01_Linea(0, Y_Pos, GC9A01_WIDTH-1, Y_Pos, GC9A01_GREEN);
            GC9A01_Linea(120-X_POS, Y_Pos, 120+X_POS, Y_Pos, GC9A01_GRAY);

            if(avance%2 == 0){
                (Y_Pos % 10 < 5 )? GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_RED): 
                               GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_WHITE);

                (Y_Pos % 10 < 5 )? GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos, GC9A01_RED): 
                               GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos, GC9A01_WHITE);

                (Y_Pos % 20 < 10 )? GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, GC9A01_WHITE):
                               GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, 0xFFFF);
            }
            else{
                (Y_Pos % 10 < 5 )? GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_WHITE): 
                               GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_RED);

                (Y_Pos % 10 < 5 )? GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos,GC9A01_WHITE ): 
                               GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos, GC9A01_RED);

                (Y_Pos % 20 < 10 )? GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, 0xFFFF):
                               GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, GC9A01_WHITE);
            }// AQUI


            /*(Y_Pos % 10 < 5 )? GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_RED): 
                               GC9A01_Linea(120-X_POS-x_POS, Y_Pos, 120-X_POS, Y_Pos, GC9A01_WHITE);

            (Y_Pos % 10 < 5 )? GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos, GC9A01_RED): 
                               GC9A01_Linea(120+X_POS, Y_Pos, 120+X_POS+x_POS, Y_Pos, GC9A01_WHITE);

            //GC9A01_Linea(120-X_POS-5, Y_Pos, 120-X_POS, Y_Pos, GC9A01_RED);
            
            /*(Y_Pos % 15 < 10 )? GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, GC9A01_WHITE):
                               GC9A01_Linea(120-X_Linea, Y_Pos, 120+X_Linea, Y_Pos, 0xFFFF);
        }
        X_POS=5;
        x_POS= X_Linea=0;

        GC9A01_Print_Sprite_Trans(Pos_Sprite[avance%8][0], Pos_Sprite[avance%8][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 104, 210, gg );
        GC9A01_Print_Buffer();
        GC9A01_Set_BG(BG_, 0, 52, 240, 70);
        //ST7735_Clear_Display(ST7735_BLACK);
        //sleep_ms(100);

        avance++;*/ //AQUI
        
        //GC9A01_Linea(0, 0, GC9A01_WIDTH-1, GC9A01_HEIGHT-1, GC9A01_WHITE);
       // GC9A01_Print_Buffer();
        //GC9A01_Clear_Display(GC9A01_CYAN);
        //sleep_ms(1000);
        
        //GC9A01_Demo_Racing();
        /*GC9A01_Linea(0, 0, GC9A01_WIDTH-1, GC9A01_HEIGHT-1, GC9A01_CYAN);
        GC9A01_Linea(0, GC9A01_HEIGHT-1, GC9A01_WIDTH-1, 0, GC9A01_CYAN);
        GC9A01_Cuadro_Solido(100, 100, 40, 40,GC9A01_RED);
        GC9A01_Print_Buffer();
        GC9A01_Clear_Display(GC9A01_BLACK);
        sleep_ms(100);*/
        //DemoTFT();
        //Demo_Racing();
    }
    return 0;
}

void GC9A01_Modo_Avance(uint16_t RED, uint16_t WHITE, uint16_t Transparente){

}

void GC9A01_Demo_Racing(void){
    for(uint32_t i=0; i<8; i++){
        for(uint16_t Y_Pos=set_Franja; Y_Pos < (GC9A01_HEIGHT+set_Franja); Y_Pos++ ){
            for(uint16_t X_Pos=0; X_Pos < GC9A01_WIDTH; X_Pos++ ){

                if(Y_Pos < 120){GC9A01_Print_Pixel( X_Pos, Y_Pos, GC9A01_CYAN);}
                else{
                    if(X_Pos < 75 || X_Pos > 240-75){
                        (Y_Pos % 20 < 10 )? GC9A01_Print_Pixel( X_Pos, Y_Pos-set_Franja, GC9A01_GREEN) : 
                                            GC9A01_Print_Pixel( X_Pos, Y_Pos-set_Franja, GC9A01_DARK_GREEN);
                    }

                    if( (X_Pos < 80 && X_Pos >= 75) || (X_Pos > 160 && X_Pos <= 165)){
                        (Y_Pos % 10 < 5 )? GC9A01_Print_Pixel( X_Pos, Y_Pos-set_Franja, GC9A01_RED) : 
                                           GC9A01_Print_Pixel( X_Pos, Y_Pos-set_Franja, GC9A01_WHITE);
                    }
                    if(X_Pos >= 80 && X_Pos<=160){
                        GC9A01_Print_Pixel( X_Pos, Y_Pos, GC9A01_GRAY);
                    }
                }
            }
        }

        GC9A01_Print_Sprite_Trans(Pos_Sprite[i][0], Pos_Sprite[i][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 104, 210, gg );
        GC9A01_Print_Buffer();
        //ST7735_Clear_Display(ST7735_BLACK);
        sleep_ms(10);

        if(Toggle_Franja ==  1){
            set_Franja=0;
            Toggle_Franja=0;
        }
        else{
            Toggle_Franja++;
            set_Franja=5;
        }
    }
}


void Demo_Racing(void){
    for(uint32_t i=0; i<8; i++){
        for(uint16_t Y_Pos=set_Franja; Y_Pos < (80+set_Franja); Y_Pos++ ){
            for(uint16_t X_Pos=0; X_Pos < 160; X_Pos++ ){

                if(X_Pos < 35 || X_Pos > 125){
                    (Y_Pos % 20 < 10 )? ST7735_Print_Pixel( X_Pos, Y_Pos-set_Franja, ST7735_GREEN) : ST7735_Print_Pixel( X_Pos, Y_Pos-set_Franja, ST7735_DARK_GREEN);
                }

                if( (X_Pos < 40 && X_Pos >= 35) || (X_Pos > 120 && X_Pos <= 125)){
                    (Y_Pos % 10 < 5 )? ST7735_Print_Pixel( X_Pos, Y_Pos-set_Franja, ST7735_RED) : ST7735_Print_Pixel( X_Pos, Y_Pos-set_Franja, ST7735_WHITE);
                }
                if(X_Pos >= 40 && X_Pos<=120){
                    ST7735_Print_Pixel( X_Pos, Y_Pos, ST7735_GRAY);
                }
            }
        }

        ST7735_Print_Sprite_Trans(Pos_Sprite[i][0], Pos_Sprite[i][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 65, 57, gg );
        ST7735_Print_Buffer();
        //ST7735_Clear_Display(ST7735_BLACK);
        sleep_ms(100);

        if(Toggle_Franja ==  2){
            set_Franja=0;
            Toggle_Franja=0;
        }
        else{
            Toggle_Franja++;
            set_Franja=5;
        }
    }
    

   /*ST7735_Cuadro_Solido(0, 0, 35, 80, ST7735_GREEN);
    ST7735_Cuadro_Solido(35, 0, 5, 80, ST7735_RED);
    ST7735_Cuadro_Solido(70, 0, 35, 80, ST7735_BLACK);
    ST7735_Cuadro_Solido(120, 0, 5, 80, ST7735_RED);
    ST7735_Cuadro_Solido(125, 0, 35, 80, ST7735_GREEN);*/
    //ST7735_Print_Buffer();
    //sleep_ms(100);
}

void ST7735_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color){
    int dx = abs(X1-X0);
    int dy = -abs(Y1-Y0);
    int sx = (X0 < X1)? 1 : -1;
    int sy = (Y0 < Y1)? 1 : -1;
    int err = dx + dy;

    while (1) {
        ST7735_Print_Pixel(X0, Y0, Color);
        if (X0 == X1  && Y0 == Y1) break;
        int e2 = 2 * err;
        if(e2 >= dy){ err += dy; X0 += sx; }
        if(e2 <= dx){ err += dx; Y0 += sy; }
    }
}

void ST7735_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
    ST7735_Linea(X, Y, X+Width, Y, Color);
    ST7735_Linea(X, Y, X, Y+Height, Color);
    ST7735_Linea(X+Width, Y, X+Width, Y+Height, Color);
    ST7735_Linea(X, Y+Height, X+Width, Y+Height, Color);
}

void ST7735_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
    for(uint32_t Y_Pos = 0; Y_Pos < Height; Y_Pos++){
        for(uint32_t X_Pos = 0; X_Pos < Width; X_Pos++){
            ST7735_Print_Pixel(X+X_Pos, Y+Y_Pos, Color); 
        }
    }
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

void GC9A01_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint8_t X_Pos_Destino, uint8_t Y_Pos_Destino, const uint16_t* Sprite){
    for (uint16_t Y=0; Y < Y_Height; Y++) {
        for (uint16_t X=0; X < X_Width; X++) {
            Buf[X+(Y*X_Width)] = Sprite[(X_Pos+X)+((Y_Pos+Y)*143)]; //Se almacena la ubicacion del sprite
        }
    }

    for(uint32_t Y=0; Y<X_Width; Y++){
        for(uint32_t X=0; X<Y_Height; X++){
        if(Buf[Y+(X*X_Width)] != 0xFFFF){
            GC9A01_Print_Pixel(X_Pos_Destino+Y, Y_Pos_Destino+X, Buf[Y+(X*X_Width)]);
        }
        }
    }
}