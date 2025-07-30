#include "embarcatech_datalogger.h"



/* ------------ VARIAVEIS E CONSTANTES ------------ */

const uint32_t LED_MASK = (1u << GPIO_RED_LED) | (1u << GPIO_GREEN_LED) | (1u << GPIO_BLUE_LED);
const uint32_t COLOR_RED = 1u << GPIO_RED_LED;
const uint32_t COLOR_GREEN = 1u << GPIO_GREEN_LED;
const uint32_t COLOR_BLUE = 1u << GPIO_BLUE_LED;
const uint32_t COLOR_WHITE = 1u << GPIO_RED_LED | 1u << GPIO_GREEN_LED | 1u << GPIO_BLUE_LED;
const uint32_t COLOR_YELLOW = 1u << GPIO_RED_LED | 1u << GPIO_GREEN_LED;
const uint32_t COLOR_PURPLE = 1u << GPIO_RED_LED | 1u << GPIO_BLUE_LED;

ssd1306_t ssd;

volatile bool is_sd_mounted = false;
volatile char mount_sd_card = 'i'; // 'i' for idle, 'm' for mount, 'u' for unmount
volatile bool is_capturing_data = false;

const char filename[20] = "data.csv"; // Default filename for data capture
const char csv_title[] = "id,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z\n"; // CSV header



int main()
{
    /* ---------------------- INICIALIZACAO DE PERIFERICOS ---------------------- */
    gpio_init_mask(LED_MASK | (1u << GPIO_BUTTONA) | (1u << GPIO_BUTTONB));
    gpio_set_dir_masked(LED_MASK | (1u << GPIO_BUTTONA) | (1u << GPIO_BUTTONB), LED_MASK);
    
    // LEDS
    gpio_put_masked(LED_MASK, 0); // Turn off all LEDs initially

    //Push Buttons
    gpio_pull_up(GPIO_BUTTONA);
    gpio_pull_up(GPIO_BUTTONB);

    gpio_set_irq_enabled_with_callback(GPIO_BUTTONB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(GPIO_BUTTONA, GPIO_IRQ_EDGE_FALL, true);

    stdio_init_all();
    sleep_ms(2000); 

    // Buzzer
    buzzer_init();
    buzzer_setup_pwm(GPIO_BUZZER, BUZZER_FREQ); // Initialize buzzer with 1kHz frequency

    // Display OLED
    i2c_init(DISPLAY_I2C_PORT, DISPLAY_FREQUENCY); // Initialize I2C at 400kHz
    gpio_set_function(DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_I2C_SDA);
    gpio_pull_up(DISPLAY_I2C_SCL);

    
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_ADDRESS, DISPLAY_I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd); 

    draw_string("Iniciando sistema...", 0, 0);

    // MPU6050
    mpu6050_init();
    sleep_ms(4000);

    System_state_t current_state = SYSTEM_STATE_IDLE;
    
    while (true) {
        switch (current_state) {
            case SYSTEM_STATE_IDLE:

                if(!is_sd_mounted && is_capturing_data){
                    gpio_put_masked(LED_MASK, COLOR_PURPLE);
                    draw_string("Captura de dados interrompida. SD nao montado.", 0, 0);
                    sleep_ms(2000);
                    is_capturing_data = false; 
                }else if (is_sd_mounted) {
                    gpio_put_masked(LED_MASK, COLOR_GREEN);
                    draw_idle_interface();
                } else {
                    gpio_put_masked(LED_MASK, COLOR_WHITE);
                    draw_idle_interface();
                    
                }

                if (is_sd_mounted && is_capturing_data) {
                    current_state = SYSTEM_STATE_READING;
                } else if (mount_sd_card != 'i') {
                    current_state = (mount_sd_card == 'm') ? SYSTEM_STATE_MOUNTING : SYSTEM_STATE_UNMOUNTING;
                }

                break;
            
            case SYSTEM_STATE_MOUNTING:
                draw_string("Montando SD...", 0, 0);
                gpio_put_masked(LED_MASK, COLOR_YELLOW);

                if(!is_sd_mounted && run_mount() == 0) {
                    is_sd_mounted = true;
                }else {
                    gpio_put_masked(LED_MASK, COLOR_PURPLE);
                    draw_string("Erro ao montar SD. Verifique o log.", 0, 0);
                    sleep_ms(2000);
                }
                
                current_state = SYSTEM_STATE_IDLE;
                mount_sd_card = 'i';
                break;

            case SYSTEM_STATE_UNMOUNTING:
                draw_string("Desmontando SD...", 0, 0);
                gpio_put_masked(LED_MASK, 1u << GPIO_RED_LED | 1u << GPIO_GREEN_LED);

                if(is_sd_mounted && run_unmount() == 0) {
                   is_sd_mounted = false;
                }else{   
                    gpio_put_masked(LED_MASK, COLOR_PURPLE);               
                    draw_string("Erro ao desmontar SD. Verifique log do sistema", 0, 0);
                    sleep_ms(2000);
                }

                mount_sd_card = 'i';
                current_state = SYSTEM_STATE_IDLE;
                break;
            case SYSTEM_STATE_READING:
                draw_capture_interface();
                gpio_set_irq_enabled(GPIO_BUTTONA, GPIO_IRQ_EDGE_FALL,false); // Disable button A interrupt during capture
                
                buzzer_play(2, 1000, 200);

                int res = capture_data_and_save();
                
                if(res < 0) {
                    gpio_put_masked(LED_MASK, COLOR_PURPLE);
                    draw_string("Erro ao capturar dados. Verifique o log.", 0, 0);
                    buzzer_play(1, 1000, 700);
                    sleep_ms(1500);
                }
                
                
                // Fim da captura
                current_state = SYSTEM_STATE_IDLE;
                is_capturing_data = false; // Reset capture state
                gpio_set_irq_enabled(GPIO_BUTTONA, GPIO_IRQ_EDGE_FALL,true); // Re-enable button A interrupt

                buzzer_play(4, 1000, 200);
                break;
            default:
                break;

        }
        sleep_ms(500); 
    }
}


void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint buttonA_last_press_time, buttonB_last_press_time = 0;

    uint32_t current_time = us_to_ms(get_absolute_time());

    if (gpio == GPIO_BUTTONA && (current_time - buttonA_last_press_time > DEBOUNCE_TIME)) {
        buttonA_last_press_time = current_time;

        if (mount_sd_card == 'i') {
            mount_sd_card = !is_sd_mounted ? 'm' : 'u'; // Set the pointer to the is_sd_mounted variable
        }
    } else if (gpio == GPIO_BUTTONB && (current_time - buttonB_last_press_time > DEBOUNCE_TIME)) {
        buttonB_last_press_time = current_time;
        is_capturing_data = !is_capturing_data; // Toggle data capture state
    }
}

static void draw_string(const char *str, uint8_t x, uint8_t y) {
    ssd1306_fill(&ssd, false); // Clear the display
    ssd1306_draw_string(&ssd, str, x, y);
    ssd1306_send_data(&ssd);
}

static void draw_idle_interface(){
    ssd1306_fill(&ssd, false); // Clear the display
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);
    ssd1306_draw_string(&ssd, "IMU DATALOGGER", 5, 5);
    ssd1306_hline(&ssd, 3, 125, 15, true);

    if(is_sd_mounted){
        ssd1306_draw_string(&ssd, "SD: montado", 5, 25);
        ssd1306_draw_string(&ssd, "MSG: inicie", 5, 40);
        ssd1306_draw_string(&ssd, "ou desmonte SD", 5, 50);

    }else{
        ssd1306_draw_string(&ssd, "SD: desmontado", 5, 25);
        ssd1306_draw_string(&ssd, "MSG: montar SD", 5, 40);
        ssd1306_draw_string(&ssd, "para iniciar", 5, 50);
    }
    ssd1306_send_data(&ssd);
}

static void draw_capture_interface(){
    ssd1306_fill(&ssd, false); // Clear the display
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);
    ssd1306_draw_string(&ssd, "IMU DATALOGGER", 5, 5);
    ssd1306_hline(&ssd, 3, 125, 15, true);

    ssd1306_draw_string(&ssd, " * Coletando *", 5, 25);
    ssd1306_draw_string(&ssd, "dados do IMU", 5, 35);
    ssd1306_draw_string(&ssd, "ID: ", 5, 45);
    ssd1306_send_data(&ssd);
}

static int capture_data_and_save(){
    FIL file;
    FRESULT res = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    
    if (res != FR_OK) {
        return -1; // Erro ao abrir o arquivo
    }

    UINT bw;
    res = f_write(&file, csv_title, strlen(csv_title), &bw);

    if(res != FR_OK) {
        f_close(&file);
        return -1; // Erro ao escrever o cabeçalho
    }
    int id = 0;
    while(is_capturing_data) {
        gpio_put_masked(LED_MASK, COLOR_RED); // Acende o LED vermelho enquanto captura dados
        
        // Captura os dados do MPU6050
        mpu6050_data_t data;
        mpu6050_read_raw(&data);

        float ax = data.accel[0] / MPU6050_ACCEL_SCALE;
        float ay = data.accel[1] / MPU6050_ACCEL_SCALE;
        float az = data.accel[2] / MPU6050_ACCEL_SCALE;
        float gx = (float)data.gyro[0];
        float gy = (float)data.gyro[1];
        float gz = (float)data.gyro[2];
        

        char buffer[120];
        char id_buffer[10];
        sprintf(buffer, "%d,%d,%d,%d,%d,%d,%d\n", id, (int)ax, (int)ay, (int)az, (int)gx, (int)gy, (int)gz);
        snprintf(id_buffer, sizeof(id_buffer), "%d", id++);

        
        ssd1306_draw_string(&ssd, id_buffer, 37, 45); // Display the current ID on the OLED
        ssd1306_send_data(&ssd); // Update the display

        res = f_write(&file, buffer, strlen(buffer), &bw);

        if (res != FR_OK)
        {
            f_close(&file);
            return -1;
        }

        
        sleep_ms(250); // Intervalo entre capturas 
    }

    f_close(&file);
    return 0; // Sucesso
}



