/*

    Comandos.h

*/

#ifndef __COMANDOS__
#define __COMANDOS__

#ifdef __cplusplus
 extern "C" {
#endif

/* COMANDOS HANDSHAKE */
#define Preambulo    0x01
#define Capture_Mod  0x42

/* BOTONES EN MODO DIGITAL */
#define PSOne_NByte_Digital 3
#define PSOne_Mode_Digital  0x41
#define PSOne_SELECT        0xFE   //0b11111110
#define PSOne_START         0xF7   //0b11110111
#define PSOne_Up            0xEF   //0b11101111
#define PSOne_Righ          0xDF   //0b11011111
#define PSOne_Down          0xBF   //0b10111111
#define PSOne_Left          0x7F   //0b01111111
#define PSOne_L2            0xFE   //0b11111110
#define PSOne_R2            0xFD   //0b11111101
#define PSOne_L1            0xFB   //0b11111011
#define PSOne_R1            0xF7   //0b11110111
#define PSOne_Triangle      0xEF   //0b11101111
#define PSOne_Circle        0xDF   //0b11011111
#define PSOne_X             0xBF   //0b10111111
#define PSOne_Square        0x7F   //0b01111111

/* BOTONES EN MODO ANALOGICO */
#define PSOne_NByte_Analog         7
#define PSOne_Mode_Analog          0x73  //0b01110011 11001110 0xCE
#define PSOne_SELECT_Analog        0xFE   //0b11111110
#define PSOne_JoyL_Analog          0xFD   //0b11111101
#define PSOne_JoyR_Analog          0xFB   //0b11111011
#define PSOne_START_Analog         0xF7   //0b11110111
#define PSOne_Up_Analog            0xEF   //0b11101111
#define PSOne_Righ_Analog          0xDF   //0b11011111
#define PSOne_Down_Analog          0xBF   //0b10111111
#define PSOne_Left_Analog          0x7F   //0b01111111
#define PSOne_L2_Analog            0xFE   //0b11111110
#define PSOne_R2_Analog            0xFD   //0b11111101
#define PSOne_L1_Analog            0xFB   //0b11111011
#define PSOne_R1_Analog            0xF7   //0b11110111
#define PSOne_Triangle_Analog      0xEF   //0b11101111
#define PSOne_Circle_Analog        0xDF   //0b11011111
#define PSOne_X_Analog             0xBF   //0b10111111
#define PSOne_Square_Analog        0x7F   //0b01111111

//////////////////////////////////////////////////////////////////////////////////////////////
/*  PSOne en SPI ; No se puede modificar envio en LSB y envia como MSB */

/* COMANDOS HANDSHAKE */
#define Preambulo_SPI    0x80
#define Capture_Mod_SPI  0x42

/* BOTONES EN MODO DIGITAL */
#define PSOne_NByte_Digital_SPI 3
#define PSOne_Mode_Digital_SPI  0x82   //0b01000001 ==> 0b10000010 0x82
#define PSOne_SELECT_SPI        0x7F   //0b11111110 ==> 0b01111111 0x7F
#define PSOne_START_SPI         0xE7   //0b11110111 ==> 0b11101111 0xE7
#define PSOne_Up_SPI            0x7F   //0b11101111 ==> 0b11110111 0xF7
#define PSOne_Righ_SPI          0xFB   //0b11011111 ==> 0b11111011 0xFB
#define PSOne_Down_SPI          0xFD   //0b10111111 ==> 0b11111101 0xFD
#define PSOne_Left_SPI          0xFE   //0b01111111 ==> 0b11111110 0xFE
#define PSOne_L2_SPI            0x7F   //0b11111110 ==> 0b01111111 0x7F
#define PSOne_R2_SPI            0xBF   //0b11111101 ==> 0b10111111 0xBF
#define PSOne_L1_SPI            0xDF   //0b11111011 ==> 0b11011111 0xDF
#define PSOne_R1_SPI            0xEF   //0b11110111 ==> 0b11101111 0xEF
#define PSOne_Triangle_SPI      0xF7   //0b11101111 ==> 0b11110111 0xF7
#define PSOne_Circle_SPI        0xFB   //0b11011111 ==> 0b11111011 0xFB
#define PSOne_X_SPI             0xFD   //0b10111111 ==> 0b11111101 0xFD
#define PSOne_Square_SPI        0xFE   //0b01111111 ==> 0b11111110 0xFE

/* BOTONES EN MODO ANALOGICO */
#define PSOne_NByte_Analog_SPI         7
#define PSOne_Mode_Analog_SPI          0xCE//0x73  0b01110011 11001110 0xCE
#define PSOne_SELECT_Analog_SPI        0x7F   //0b11111110 ==> 0b01111111 0x7F
#define PSOne_JoyL_Analog_SPI          0xBF   //0b11111101 ==> 0b10111111 0xBF
#define PSOne_JoyR_Analog_SPI          0xDF   //0b11111011 ==> 0b11011111 0xDF
#define PSOne_START_Analog_SPI         0xEF   //0b11110111 ==> 0b11101111 0xEF
#define PSOne_Up_Analog_SPI            0xF7   //0b11101111 ==> 0b11110111 0xF7
#define PSOne_Righ_Analog_SPI          0xFB   //0b11011111 ==> 0b11111011 0xFB
#define PSOne_Down_Analog_SPI          0xFD   //0b10111111 ==> 0b11111101 0xFD
#define PSOne_Left_Analog_SPI          0xFE   //0b01111111 ==> 0b11111110 0xFE

#define PSOne_L2_Analog_SPI            0x7F   //0b11111110 ==> 0b01111111 0x7F
#define PSOne_R2_Analog_SPI            0xBF   //0b11111101 ==> 0b10111111 0xBF
#define PSOne_L1_Analog_SPI            0xDF   //0b11111011 ==> 0b11011111 0xDF
#define PSOne_R1_Analog_SPI            0xEF   //0b11110111 ==> 0b11101111 0xEF
#define PSOne_Triangle_Analog_SPI      0xF7   //0b11101111 ==> 0b11110111 0xF7
#define PSOne_Circle_Analog_SPI        0xFB   //0b11011111 ==> 0b11111011 0xFB
#define PSOne_X_Analog_SPI             0xFD   //0b10111111 ==> 0b11111101 0xFD
#define PSOne_Square_Analog_SPI        0xFE   //0b01111111 ==> 0b11111110 0xFE

#ifdef __cplusplus
 }
#endif //__COMANDOS__
#endif