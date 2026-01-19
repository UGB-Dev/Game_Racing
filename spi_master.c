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
#include "fonts.h"
#include "testimg.h"
#include <stdlib.h>
#include "ILI9341.h"
#include "PSOne.h"
#include "Comandos.h"
#include <math.h>

#define Cam_X 0 
#define Cam_Y 50
#define Cam_Z 10
#define N_Segmentos 20

/* SE CREAN LAS FUNCIONES PARA CONTROLAR EL SPRITE */
#define Mitad_Width ( ILI9341_WIDTH>>1)
#define Mitad_Height ( ILI9341_HEIGHT>>1)

#define PI 3.1415926535897932384626
#define FOV 125 // GRADOS DE VISTA DE LA PISTA EN EJE X
#define Theta (FOV*PI)/180
#define Dist (1/tan(Theta))

typedef struct{
    float X;
    float Y;
    float Z;
    float W_road;
    float W_Franja;
}Position;

Position Screen[50]; // X, Y, Z de la camara
//Position MUNDO_3D[50];
//Position Distancias[50];

/*const uint16_t Pos_Sprite[][2] = {{0, 5}, {45, 5}, {85, 5}, {170, 172}, {127, 172},{85, 172}, {125, 5}, {169, 5},
                                  {0, 48}, {44, 48}, {83, 48}, {125, 48}, {168, 48},
                                  {0, 88}, {44, 88}, {85, 88}, {127, 88}, {168, 88},
                                  {0, 130}, {44, 130}, {86, 130}, {128, 130}, {169, 130},
                                  {0, 172}, {44, 172}, {40,30}};*/

const uint16_t Pos_Sprite[][2] = {{5, 5}, {42, 5}, {78, 5}, {109, 5}, 
                                  {111, 30}, {78, 30}, {43, 30}, {5, 30},
                                   {29,21}};


void ILI9341_Trapecio_Solido(float x1, float y2, float x4, float x2, float y1, float x3, uint16_t Color);

/* RESPUESTA DEL PS1 A RP2040  
             ---------- ------- ------- ------- ------- ------- ------- -------
            |   BYTE0  | BYTE1 | BYTE2 | BYTE3 | BYTE4 | BYTE5 | BYTE6 | BYTE7 |
 ----------- ---------- ------- ------- ------- ------- ------- ------- ------- 
|  Digital  |   0x41   | 0x5A  | 0xFF  | 0xFF  | ----- | ----- | ----- | ----- |
| Analogico |   0x73   | 0x5A  | 0xFF  | 0xFF  | 0x7F  | 0x7F  | 0x7F  | 0x7F  |
 ----------- ---------- ------- ------- ------- ------- ------- ------- -------
*/
uint8_t Buffer_Rx_PSOne[] = {0,0,0,0,0,0,0,0};
uint16_t X1=0, Y1=0, X2=0, Y2=0;

/* CREANDO FUNCIONES PARA EL JUEGO */
void Game_Race_Road(void);

int main(){
    //intialize stdio
    stdio_init_all();

    /* INICIALIZACION DEL SPI */ 
    spi_init(SPI_PORT, 60000 * 1000); // SPI a " 60 MHz "
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);  // Pin 18 como SCK (Clock)
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI); // Pin 19 como MOSI (Master Output Slave Input)
    
    gpio_init(LCD_RST);                         // Pin 21 como RST (Reset)
    gpio_set_dir(LCD_RST, GPIO_OUT);

    gpio_init(LCD_CS);                          // Pin 17 como CS (Chip Select)
    gpio_set_dir(LCD_CS, GPIO_OUT);           
    
    gpio_init(LCD_DC);                          // Pin 20 como DC (Data/Comando)     
    gpio_set_dir(LCD_DC, GPIO_OUT);
    
    PSOne_Init(); // Inicializacion de Control PSOne por SPI1
    //PSOne_Software_Init(); // Inicializacion de Control PSOne por Software

    ILI9341_Init();
    ILI9341_Clear_Display(ILI9341_BLACK);
    
    while(1){
        //ILI9341_Clear_Display(ILI9341_GREEN);
        for(uint16_t i = Mitad_Height; i<(ILI9341_HEIGHT-1); i++ ){
            ILI9341_Linea(0, i, ILI9341_WIDTH, i, ILI9341_GREEN);
        }
        Game_Race_Road();
        ILI9341_Print_Buffer();
    }
}

void Game_Race_Road(void){
    float Scala=0;
    uint16_t Col=0;
    uint8_t pos=0, Expancion=1;
    int16_t ConversionX=0, ConversionY=0, ConversionZ=0;
    PSOne_Capturar_Trama(Buffer_Rx_PSOne);
    
    /* VARIABLES PARA AJUSTAR CON LOS JOYSTICK LA POSICION DE LA CAMARA X, Y y Z*/
    ConversionX = ((Buffer_Rx_PSOne[4] * (ILI9341_WIDTH+15))/255)-(ILI9341_WIDTH>>1); 
    ConversionY = 1+(Buffer_Rx_PSOne[5] * (N_Segmentos-1))/255;/*10+ (Buffer_Rx_PSOne[5] * ILI9341_HEIGHT)/255;*/
    ConversionZ = 10+ (Buffer_Rx_PSOne[7] * 100)/255; 

    /* ciclos for para pruebas */
    /*for(uint8_t i = 0; i<N_Segmentos; i++ ){ // <=== 
        Distancias[i].X = ConversionX -Cam_X;
        Distancias[i].Y = Mitad_Height -Cam_Y ;
        Distancias[i].Z = (i*60)-Cam_Z ;
    }

    for(uint8_t i = 0; i<N_Segmentos; i++ ){
        Scala = Dist/Distancias[i].Z;
        MUNDO_3D[i].X = Scala*Distancias[i].X;
        MUNDO_3D[i].Y = Scala*Distancias[i].Y;
        MUNDO_3D[i].W_road = Scala * 100;
        MUNDO_3D[i].W_Franja = Scala * 100;
    }
    
    for(uint8_t i=0; i<N_Segmentos; i++){
        Screen[i].X = (1+MUNDO_3D[i].X)*Mitad_Width;
        Screen[i].Y = (1-MUNDO_3D[i].Y)*Mitad_Height;
        Screen[i].W_road = (float)fabs(MUNDO_3D[i].W_road * 200);  
        Screen[i].W_Franja = (float)fabs(MUNDO_3D[i].W_Franja * 20);
    }*/
    
    /* PSEUDO 3D DE CARRETERA */
    for(uint8_t i=0; i<N_Segmentos; i++){
        Scala = Dist/((i*60)-Cam_Z);
        Screen[i].X = (1+(Scala*(ConversionX - Cam_X)))*Mitad_Width;
        Screen[i].Y = (1-(Scala*(Mitad_Height - Cam_Y)))*Mitad_Height;
        Screen[i].W_road = (float)fabs(Scala * 100 * 200);  
        Screen[i].W_Franja = (float)fabs(Scala * 100 * 20);
    }

    for(uint8_t i=1; i<(N_Segmentos-1); i++){
        ILI9341_Trapecio_Solido(Screen[i].X - Screen[i].W_road - Screen[i].W_Franja, Screen[i].Y, Screen[i].X + Screen[i].W_road + Screen[i].W_Franja, 
                                Screen[i+1].X - Screen[i+1].W_road - Screen[i+1].W_Franja, Screen[i+1].Y, Screen[i+1].X + Screen[i+1].W_road + Screen[i+1].W_Franja, ILI9341_RED);
        ILI9341_Trapecio_Solido(Screen[i].X-Screen[i].W_road, Screen[i].Y, Screen[i].X + Screen[i].W_road, Screen[i+1].X-Screen[i+1].W_road, Screen[i+1].Y, Screen[i+1].X + Screen[i+1].W_road, ILI9341_GRAY);
        ILI9341_Linea(Screen[i].X - (Screen[i].W_road), Screen[i].Y, Screen[i].X+Screen[i].W_road, Screen[i].Y, ILI9341_CYAN);
    }
    if (ConversionY < 3){
        Expancion = 3;
    }
    else if(ConversionY < 5 && ConversionY>=3){
        Expancion = 2;
    }
    else{
        Expancion = 1;
    }
    
    ILI9341_Print_Sprite_Trans_Expand(Pos_Sprite[0][0], Pos_Sprite[0][1],                                                         // Coordenadas del Sprite
                                      Pos_Sprite[8][0], Pos_Sprite[8][1],                                                         // Dimenciones del Sprite
                                      ConversionX + 30 - (Pos_Sprite[8][0]>>1),  Screen[ConversionY].Y - (Pos_Sprite[8][0]>>1),   // Posicion del Sprite dentro del buffer
                                      gg, Expancion);                                                                             // Matriz de Sprite y escala de Sprite respectivamente
}

void ILI9341_Trapecio_Solido(float x1, float y2, float x4, float x2, float y1, float x3, uint16_t Color){
    float Pendiente_Der = (float) (x2 - x1) / (y1 - y2);
    float Pendiente_Izq = (float) (x3 - x4) / (y1 - y2);
    for(uint16_t y = y1; y<= y2; y++){
        float x_Izq = x2 + (y-y1)*Pendiente_Der;
        float x_Der = x3 + (y-y1)*Pendiente_Izq;
        ILI9341_Linea(x_Der, y, x_Izq, y, Color );
    }
}