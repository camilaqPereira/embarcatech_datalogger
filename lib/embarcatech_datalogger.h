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

#define DEBOUNCE_TIME 200 // Debounce time em milisegundos
#define MAX_NUM_SAMPLES 100
extern const uint32_t LED_MASK;     // Mascara para os LEDs
extern const uint32_t COLOR_RED;    // Mascara para a cor vermelha
extern const uint32_t COLOR_GREEN;  // Mascara para a cor verde
extern const uint32_t COLOR_BLUE;   // Mascara para a cor azul
extern const uint32_t COLOR_WHITE;  // Mascara para a cor branca
extern const uint32_t COLOR_YELLOW; // Mascara para a cor amarela
extern const uint32_t COLOR_PURPLE; // Mascara para a cor roxa

/* --------- STRUCTS E ENUMS --------- */



/**
 * @enum System_state_t
 * @brief Representa os vários estados do sistema.
 *
 * Esta enumeração define os possíveis estados operacionais do sistema:
 * - SYSTEM_STATE_IDLE: O sistema está ocioso e não executa operações.
 * - SYSTEM_STATE_MOUNTING: O sistema está montando o microSD.
 * - SYSTEM_STATE_UNMOUNTING: O sistema está desmontando o microSD.
 * - SYSTEM_STATE_READING: O sistema está lendo dados ativamente.
 */
typedef enum {
    SYSTEM_STATE_IDLE,
    SYSTEM_STATE_MOUNTING,
    SYSTEM_STATE_UNMOUNTING,
    SYSTEM_STATE_READING
} System_state_t;

/* --------- PROTOTIPOS DE FUNCOES --------- */

/**
 * @brief Manipulador de interrupção para eventos de GPIO.
 * 
 * Esta função é chamada quando ocorre um evento de interrupção em um pino GPIO específico.
 * 
 * @param gpio Número do pino GPIO que gerou a interrupção.
 * @param events Máscara de eventos que ocorreram no pino.
 */

void gpio_irq_handler(uint gpio, uint32_t events);

/**
 * @brief Limpa a tela e desenha uma string na tela nas coordenadas especificadas.
 * 
 * @param str Ponteiro para a string a ser desenhada.
 * @param x Posição horizontal onde a string será desenhada.
 * @param y Posição vertical onde a string será desenhada.
 */
static void draw_string(const char *str, uint8_t x, uint8_t y);

/**
 * @brief Desenha a interface de usuário em estado ocioso.
 * 
 * Esta função é responsável por exibir a interface gráfica quando o sistema está em modo idle.
 */

static void draw_idle_interface();
/**
 * @brief Desenha a interface de captura de dados.
 * 
 * Esta função exibe a interface gráfica utilizada durante o processo de captura de dados.
 */

static void draw_capture_interface();
/**
 * @brief Realiza a captura de dados e salva os resultados.
 * 
 * Esta função executa a aquisição dos dados necessários e realiza o salvamento dos mesmos.
 * 
 * @return Retorna 0 em caso de sucesso ou um valor negativo em caso de erro.
 */
static int capture_data_and_save();


#endif // EMBARCA_TECH_DATALOGGER_H