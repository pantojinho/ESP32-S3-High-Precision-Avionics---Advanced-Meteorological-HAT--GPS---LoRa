#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
#define LV_COLOR_16_SWAP 1

/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: use custom malloc/free, 0: use the built-in `lv_malloc()` and `lv_free()`*/
#define LV_USE_STDLIB_MALLOC    0
#define LV_USE_STDLIB_STRING    0
#define LV_USE_STDLIB_SPRINTF   0

/*Size of the memory available for `lv_malloc()` in bytes (>= 2kB)*/
#define LV_MEM_SIZE (64 * 1024U)          /*[bytes]*/
#define LV_MEM_ADR 0                      /*0: unused*/

/*=========================
   HAL SETTINGS
 *=========================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
#define LV_DEF_REFR_PERIOD  33      /*[ms]*/

/*Input device read period in milliseconds*/
#define LV_DEF_INDEV_READ_PERIOD 33 /*[ms]*/

/*====================
   FEATURE CONFIGURATION
 *====================*/

/*-------------
 * Drawing
 *-----------*/

/*Enable complex draw engine*/
#define LV_USE_DRAW_MASKS 1

/*-------------
 * Others
 *-----------*/

/*1: Enable the log module*/
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO

/*=================
 * DEMO USAGE
 =================*/

/*Show some widget. It might be required to increase `LV_MEM_SIZE` */
#define LV_USE_DEMO_WIDGETS 0
#define LV_USE_DEMO_BENCHMARK 0

#endif /*LV_CONF_H*/
