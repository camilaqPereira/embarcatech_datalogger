#ifndef MPU6050_H
#define MPU6050_H


#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include <math.h>

#define MPU6050_I2C_PORT i2c0
#define MPU6050_I2C_SDA 0
#define MPU6050_I2C_SCL 1
#define MPU6050_ADDRESS 0x68
#define MPU6050_FREQ_HZ 400000 // 400 kHz I2C speed

#define MPU6050_ACCEL_SCALE 16384.0f
/* -------------- STRUCTS E ENUMS -------------- */
typedef struct {
    int16_t accel[3]; // X, Y, Z aceleracao
    int16_t gyro[3];  // X, Y, Z giro
    int16_t temp;     // Temperatura
} mpu6050_data_t;

/* -------------- PROTOTIPO DE FUNCOES -------------- */
void mpu6050_init();
static void mpu6050_reset();
void mpu6050_read_raw(mpu6050_data_t *data);



#endif // MPU6050_H