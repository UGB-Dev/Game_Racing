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
#include "ST7735.h"
#include "GC9A01.h"
#include "fonts.h"
#include "testimg.h"
#include <stdlib.h>
#include "ILI9341.h"
#include <math.h>

void DemoTFT(void);
void Prueba_Func(void);
void ILI9341_Demo(void);

/* SE CREAN LAS FUNCIONES PARA CONTROLAR EL SPRITE */
#define N_Sprite 1
#define Display_Width ILI9341_WIDTH
#define Display_Height ILI9341_HEIGHT
#define Mitad_Width ( Display_Width/2)
#define Mitad_Height ( Display_Height/2)

#define SEGMENT_ROAD 20
#define LENGTH_ROAD 100.0f
#define Segment_Length 2.0f
#define POV_Y Mitad_Width
#define POV_X Mitad_Height
#define PI 3.1415926535897932384626
#define FOV 125 // GRADOS DE VISTA DE LA PISTA EN EJE X
#define Theta (FOV*PI)/180
#define Dist (1/tan(Theta))
#define Coordenada_X(x) ((1+x)*Mitad_Width) 
#define Coordenada_Y(y) ((1-y)* Mitad_Height)
#define Coordenada_W(w) (w*Display_Width)


float Camara = 0.0f, Foco = 2.0f, Z_;
void Draw_Road(void);
void Draw_Road_Segment(float Z);

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
    float X;
    float Y;
    float Z;
    float W_road;
}Position;

typedef struct{
    Position Posicion;
    Position Velocidad;
    uint8_t R;
}BALL;

Position CAMARA  ; // X, Y, Z de la camara
Position MUNDO_3D[10]; 

// dx = 35
/*const uint16_t Pos_Sprite[][2] = {{0, 5}, {45, 5}, {85, 5}, {170, 172}, {127, 172},{85, 172}, {125, 5}, {169, 5},
                                  {0, 48}, {44, 48}, {83, 48}, {125, 48}, {168, 48},
                                  {0, 88}, {44, 88}, {85, 88}, {127, 88}, {168, 88},
                                  {0, 130}, {44, 130}, {86, 130}, {128, 130}, {169, 130},
                                  {0, 172}, {44, 172}, {40,30}};*/

const uint16_t Pos_Sprite[][2] = {{5, 5}, {42, 5}, {78, 5}, {109, 5}, 
                                  {111, 30}, {78, 30}, {43, 30}, {5, 30},
                                   {29,21}};

void Demo_Racing(void);
void GC9A01_Demo_Racing(void);

const uint16_t Bird_UP[] = {
    11, 10, 0x00F8, 0x0104, 0x02E2, 0x0211, 0x0211, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

const uint16_t Bird_DOWN[] = {
    11, 10, 0x00F8, 0x0104, 0x0272, 0x0281, 0x0281, 0x0261, 0x0209, 0x0102, 0x00FC, 0x0070, 0x0020
};

uint8_t Toggle_Franja=0, set_Franja=0;

void ILI9341_Trapecio_Solido(uint16_t x1, uint16_t y2, uint16_t x4, uint16_t x2, uint16_t y1, uint16_t x3, uint16_t Color);
uint8_t avance=0;

int main(){
    //intialize stdio
    stdio_init_all();

    // intialize the SPI0 of Raspberry Pi
    // This example will use SPI0 at 4MHz. 
    spi_init(SPI_PORT, 20000 * 1000);
    //gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    //gpio_init(LCD_RST_2);
   // gpio_set_dir(LCD_RST_2, GPIO_OUT);
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    //gpio_init(LCD_CS_2);
    //gpio_set_dir(LCD_CS_2, GPIO_OUT);
    gpio_init(LCD_DC); //RS PIn
    gpio_set_dir(LCD_DC, GPIO_OUT);
    //gpio_init(LCD_DC_2); //RS PIn
    //gpio_set_dir(LCD_DC_2, GPIO_OUT);

    ILI9341_Init();
    ILI9341_Clear_Display(GC9A01_GREEN);

    float X_POS=5, x_POS=0, X_Linea=0;
    uint8_t avance=0;
    float X_TFT, Y_TFT;
    float Scale;

    for(uint8_t i = 0; i<10; i++ ){
            MUNDO_3D[i].Z =  100+ i*100; 
            Scale = Dist / MUNDO_3D[i].Z;
            MUNDO_3D[i].X = Scale * 143;
            MUNDO_3D[i].Y = Scale * 80;//60
            MUNDO_3D[i].W_road = Scale * 0.05;
        }
    float Pendiente_Der = (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)) - \
                           ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[0].X)+Coordenada_W(MUNDO_3D[0].W_road)))/ \
                           (Coordenada_Y( MUNDO_3D[9].Y)-Coordenada_Y( MUNDO_3D[0].Y)); //(float) (x2 - x1) / (y1 - y2);

    float Pendiente_Izq = ((Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)) - \
                           (Coordenada_X( MUNDO_3D[0].X)+Coordenada_W(MUNDO_3D[0].W_road)))/ \
                           (Coordenada_Y( MUNDO_3D[9].Y)-Coordenada_Y( MUNDO_3D[0].Y)); //(float) (x3 - x4) / (y1 - y2);

    while(1){

        /*ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[0].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[0].Y) ,ILI9341_GREEN);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[1].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[1].Y) ,ILI9341_GRAY);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[2].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[2].Y) ,ILI9341_GREEN);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[3].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[3].Y) ,ILI9341_GRAY);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[4].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[4].Y) ,ILI9341_GREEN);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[5].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[5].Y) ,ILI9341_GRAY);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[6].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[6].Y) ,ILI9341_GREEN);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[7].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[7].Y) ,ILI9341_GRAY);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[8].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[8].Y) ,ILI9341_GREEN);
        ILI9341_Linea(0, Coordenada_Y( MUNDO_3D[9].Y), ILI9341_WIDTH-1, Coordenada_Y( MUNDO_3D[9].Y) ,ILI9341_GRAY);*/
        ILI9341_Linea(0, 0, 20, 30,ILI9341_GRAY);

        for(uint8_t i = 10; i>0; i-- ){ //aqui**
            /*if(i==2){}
            else{
            if(i%2 == 0){

                ILI9341_Trapecio_Solido(ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[i-2].X)+Coordenada_W(MUNDO_3D[i-2].W_road)),
                                        Coordenada_Y( MUNDO_3D[i-2].Y),
                                        (Coordenada_X( MUNDO_3D[i-2].X)+Coordenada_W(MUNDO_3D[i-2].W_road)),
                                        ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[i-1].X)+Coordenada_W(MUNDO_3D[i-1].W_road)),
                                        Coordenada_Y( MUNDO_3D[i-1].Y),
                                        (Coordenada_X( MUNDO_3D[i-1].X)+Coordenada_W(MUNDO_3D[i-1].W_road)), ILI9341_GREEN);
            }
            else{
                ILI9341_Trapecio_Solido(ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[i-2].X)+Coordenada_W(MUNDO_3D[i-2].W_road)),
                                        Coordenada_Y( MUNDO_3D[i-2].Y),
                                        (Coordenada_X( MUNDO_3D[i-2].X)+Coordenada_W(MUNDO_3D[i-2].W_road)),
                                        ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[i-1].X)+Coordenada_W(MUNDO_3D[i-1].W_road)),
                                        Coordenada_Y( MUNDO_3D[i-1].Y),
                                        (Coordenada_X( MUNDO_3D[i-1].X)+Coordenada_W(MUNDO_3D[i-1].W_road)), ILI9341_GRAY);

            }
           }*/



            for(uint16_t y=Coordenada_Y( MUNDO_3D[i-1].Y); y<ILI9341_HEIGHT-1; y++){
            //for(uint16_t x=Coordenada_X( MUNDO_3D[i-1].X); x<(Coordenada_X( MUNDO_3D[i-1].X)+Coordenada_W(MUNDO_3D[i-1].W_road)); x++){
                if(avance%2 == 0){
                    if(i%2 == 0){
                        //ILI9341_Linea(Coordenada_X( MUNDO_3D[i-1].X), y, x , y ,ILI9341_GREEN);
                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y ,ILI9341_GRAY_1);
                    
                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                        (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der - 5, y ,ILI9341_RED);

                        ILI9341_Linea( (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq+5, y ,ILI9341_RED);
                    }
                    else{
                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y ,ILI9341_GRAY);

                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                        (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der - 5, y ,ILI9341_WHITE);

                        ILI9341_Linea( (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq+5, y ,ILI9341_WHITE);
                        //ILI9341_Linea(Coordenada_X( MUNDO_3D[i-1].X), y, x , y ,ILI9341_GRAY);
                        //ILI9341_Linea( ILI9341_WIDTH-x, y, Coordenada_X( MUNDO_3D[i-1].X) , y ,ILI9341_GRAY);
                    }
                }
                else{
                    if(i%2 == 0){
                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y ,ILI9341_GRAY);

                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                        (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der - 5, y ,ILI9341_WHITE);

                        ILI9341_Linea( (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq+5, y ,ILI9341_WHITE);
                        //ILI9341_Linea(Coordenada_X( MUNDO_3D[i-1].X), y, x , y ,ILI9341_GRAY);
                       // ILI9341_Linea( ILI9341_WIDTH-x, y, Coordenada_X( MUNDO_3D[i-1].X) , y ,ILI9341_GRAY);
                    }
                    else{
                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y ,ILI9341_GRAY_1);

                        ILI9341_Linea( (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der, y, 
                                        (ILI9341_WIDTH-(Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road)))+(y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Der - 5, y ,ILI9341_RED);

                        ILI9341_Linea( (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq, y, 
                                       (Coordenada_X( MUNDO_3D[9].X)+Coordenada_W(MUNDO_3D[9].W_road))+ (y-Coordenada_Y( MUNDO_3D[9].Y))*Pendiente_Izq+5, y ,ILI9341_RED);
                        //ILI9341_Linea(Coordenada_X( MUNDO_3D[i-1].X), y, x , y ,ILI9341_GREEN);
                       // ILI9341_Linea( ILI9341_WIDTH-x, y, Coordenada_X( MUNDO_3D[i-1].X) , y ,ILI9341_GREEN);
                    }
                
                }
                
            }
            //}
        } //aqui**

        /*for(uint16_t y=ILI9341_HEIGHT>>1; y<ILI9341_HEIGHT-1; y++){
            for(float x=(ILI9341_WIDTH>>1); x<(ILI9341_WIDTH>>1); x++){
                X_TFT = (x * 10.0f) / 8.0f;
                Y_TFT = (y * 10.0f) / 8.0f;
                ILI9341_Print_Pixel(X_TFT, Y_TFT, ILI9341_GRAY);
            }
        }*/

        ILI9341_Print_Buffer();
        x_POS = 0;
        avance++;
    }
}

void ILI9341_Trapecio_Solido(uint16_t x1, uint16_t y2, uint16_t x4, uint16_t x2, uint16_t y1, uint16_t x3, uint16_t Color){
    float Pendiente_Der = (float) (x2 - x1) / (y1 - y2);
    float Pendiente_Izq = (float) (x3 - x4) / (y1 - y2);
    for(int y = y1; y<= y2; y++){
        int x_Izq = x2 + (y-y1)*Pendiente_Der;
        int x_Der = x3 + (y-y1)*Pendiente_Izq;
        ILI9341_Linea(x_Der, y, x_Izq, y, Color );
    }
}

void ILI9341_Demo(void){
    float X_POS=5, x_POS=0, X_Linea=0;
    
    
     for(float Y_Pos=ILI9341_HEIGHT>>1; Y_Pos < ILI9341_HEIGHT-1; Y_Pos+=0.8, X_POS+=0.8, x_POS+=0.2, X_Linea+=0.01 ){
            ILI9341_Linea(0, Y_Pos, ILI9341_WIDTH-1, Y_Pos, ILI9341_GREEN);
            
            if(avance%2 == 0){
                ((uint8_t)Y_Pos % 10 < 5 )? ILI9341_Linea((ILI9341_WIDTH>>1)-X_POS-x_POS, Y_Pos, (ILI9341_WIDTH>>1)+X_POS+x_POS, Y_Pos, ILI9341_RED): 
                               ILI9341_Linea((ILI9341_WIDTH>>1)-X_POS-x_POS, Y_Pos, (ILI9341_WIDTH>>1)+X_POS+x_POS, Y_Pos, ILI9341_WHITE);
            }
            else{
                ((uint8_t)Y_Pos % 10 < 5 )? ILI9341_Linea((ILI9341_WIDTH>>1)-X_POS-x_POS, Y_Pos, (ILI9341_WIDTH>>1)+X_POS+x_POS, Y_Pos, ILI9341_WHITE): 
                               ILI9341_Linea((ILI9341_WIDTH>>1)-X_POS-x_POS, Y_Pos, (ILI9341_WIDTH>>1)+X_POS+x_POS, Y_Pos, ILI9341_RED);
            }

            ILI9341_Linea((ILI9341_WIDTH>>1)-X_POS, Y_Pos, (ILI9341_WIDTH>>1)+X_POS, Y_Pos, ILI9341_GRAY);

            if(avance%2 == 0){
                ((uint8_t)Y_Pos % 10 < 5 )?ILI9341_Linea((ILI9341_WIDTH>>1)-X_Linea, Y_Pos, (ILI9341_WIDTH>>1)+X_Linea, Y_Pos, ILI9341_WHITE):
                               ILI9341_Linea((ILI9341_WIDTH>>1)-X_Linea, Y_Pos, (ILI9341_WIDTH>>1)+X_Linea, Y_Pos, 0xFFFF);
            }
            else{
                ((uint8_t)Y_Pos % 10 < 5 )? ILI9341_Linea((ILI9341_WIDTH>>1)-X_Linea, Y_Pos, (ILI9341_WIDTH>>1)+X_Linea, Y_Pos, 0xFFFF):
                               ILI9341_Linea((ILI9341_WIDTH>>1)-X_Linea, Y_Pos, (ILI9341_WIDTH>>1)+X_Linea, Y_Pos, ILI9341_WHITE);
            }
        }
        X_POS=5;
        x_POS= X_Linea=0;

        ILI9341_Print_Sprite_Trans(Pos_Sprite[avance%1][0], Pos_Sprite[avance%1][1], Pos_Sprite[8][0], Pos_Sprite[8][1], (ILI9341_WIDTH>>1)+28, ILI9341_HEIGHT-23, gg );
        ILI9341_Print_Sprite_Trans(Pos_Sprite[avance%1][0], Pos_Sprite[avance%1][1], Pos_Sprite[8][0], Pos_Sprite[8][1], (ILI9341_WIDTH>>1)-28, ILI9341_HEIGHT-23, gg );
        ILI9341_Print_Buffer();

        avance++;
        sleep_ms(50);
}


void Prueba_Func(void){
    float Camara_Y = 40.0f, Camara_Z = 40.0f, H_Y[20];
    uint8_t N_Segments = 20;
    float L = 0.5;
    float scala=0.5;
    float H_Y_SCALA = 80.0f * scala;

    for(uint8_t i=0; i<N_Segments; i++){
        H_Y[i] = 80-((((i*L*Camara_Y)/Camara_Z)/ (1+(i*L)))*H_Y_SCALA);
    }

    for(uint8_t Secciones=0; Secciones<20; Secciones++){
        for(uint8_t i=H_Y[Secciones]; i>H_Y[Secciones%19+1]; i--){
            if(Toggle_Franja%2==0){
                if(Secciones%2 == 0){
                    ST7735_Linea(0, i, 159, i, ST7735_GRAY);
                }
                else{
                    ST7735_Linea(0, i, 159, i, ST7735_BLACK);
                }
            }
            else{
                if(Secciones%2 == 0){
                    ST7735_Linea(0, i, 159, i, ST7735_BLACK);
                }
                else{
                    ST7735_Linea(0, i, 159, i, ST7735_GRAY);
                }
            }
        }
    }
    Toggle_Franja++;
}

void Draw_Road(void){
    for(uint16_t i=0; i<SEGMENT_ROAD; i++){
        Z_ = 1.0f *(i*Segment_Length);
        Draw_Road_Segment(Z_);
    }
}

void Draw_Road_Segment(float Z){
    float scale = Foco / Z;
    int Screen_Y = Display_Height - (Z*2.0f);
    float Ancho_Carretera = 4.0f;
    int Alto_Carretera = Ancho_Carretera * scale * 1.0f;
    int Borde_Der = Mitad_Width - (Ancho_Carretera/2);
    int Borde_Izq = Mitad_Width + (Ancho_Carretera/2);

    if(Screen_Y >= 0 && Screen_Y < Display_Height){
        ST7735_Linea(Borde_Izq, Screen_Y, Ancho_Carretera, Screen_Y, ST7735_GRAY);
    }
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
}

void DemoTFT(void){
    for(uint32_t i=0; i<8; i++){
        ST7735_Print_Sprite_Trans(Pos_Sprite[i][0], Pos_Sprite[i][1], Pos_Sprite[8][0], Pos_Sprite[8][1], 59, 40, gg );
        ST7735_Print_Buffer();
        ST7735_Clear_Buffer(BackGround);
        sleep_ms(100);
    }
}