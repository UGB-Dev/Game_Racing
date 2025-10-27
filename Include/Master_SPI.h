/*

     Master_SPI.h

*/
#ifndef __DHT_11_H__
#define __DHT_11_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"

#define BUF_LEN         0x100
#ifndef LED_DELAY_MS
#define LED_DELAY_MS 500
#endif

void printbuf(uint8_t buf[], size_t len);
void pico_led_init(void);
void pico_set_led(bool led_on);

#ifdef __cplusplus
 }
#endif // __DHT_11_H__
#endif