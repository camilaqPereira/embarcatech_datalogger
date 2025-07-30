#ifndef EMBARCA_TECH_DATALOGGER_H
#define EMBARCA_TECH_DATALOGGER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "ssd1306.h"
#include "mpu6050.h"
#include "sdcard_local.h"
#include "buzzer.h"

/* --------- DEFINICOES DE GPIO --------- */

#define GPIO_RED_LED 13
#define GPIO_GREEN_LED 11
#define GPIO_BLUE_LED 12
#define GPIO_BUTTONA 5
#define GPIO_BUTTONB 6

/* --------- VARIAVEIS E CONSTANTES --------- */

#define DEBOUNCE_TIME 200 // Debounce time in milliseconds
#define MAX_NUM_SAMPLES 100
extern const uint32_t LED_MASK;
extern const uint32_t COLOR_RED;
extern const uint32_t COLOR_GREEN;
extern const uint32_t COLOR_BLUE;
extern const uint32_t COLOR_WHITE;
extern const uint32_t COLOR_YELLOW;
extern const uint32_t COLOR_PURPLE;

/* --------- STRUCTS E ENUMS --------- */

typedef enum {
    SYSTEM_STATE_IDLE,
    SYSTEM_STATE_MOUNTING,
    SYSTEM_STATE_UNMOUNTING,
    SYSTEM_STATE_FORMATTING,
    SYSTEM_STATE_READING,
    SYSTEM_STATE_WRITING
} System_state_t;

/* --------- PROTOTIPOS DE FUNCOES --------- */

void gpio_irq_handler(uint gpio, uint32_t events);
static void draw_string(const char *str, uint8_t x, uint8_t y);
static void draw_idle_interface();
static void draw_capture_interface();
static int capture_data_and_save();
#endif // EMBARCA_TECH_DATALOGGER_H