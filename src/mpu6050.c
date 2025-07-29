#include "mpu6050.h"


void mpu6050_init() {
    // Initialize I2C communication
    i2c_init(MPU6050_I2C_PORT, MPU6050_FREQ_HZ);
    gpio_set_function(MPU6050_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MPU6050_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MPU6050_I2C_SDA);
    gpio_pull_up(MPU6050_I2C_SCL);

    bi_decl(bi_2pins_with_func(MPU6050_I2C_SDA, MPU6050_I2C_SCL, GPIO_FUNC_I2C));

    // Reset the MPU6050 device
    mpu6050_reset();
}

static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, buf, 2, false);
    sleep_ms(100); // Allow device to reset and stabilize

    // Clear sleep mode (0x6B register, 0x00 value)
    buf[1] = 0x00;  // Clear sleep mode by writing 0x00 to the 0x6B register
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, buf, 2, false);
    sleep_ms(10); // Allow stabilization after waking up
}

void mpu6050_read_raw(mpu6050_data_t *data) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        data->accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, &val, 1, true);
    i2c_read_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        data->gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, &val, 1, true);
    i2c_read_blocking(MPU6050_I2C_PORT, MPU6050_ADDRESS, buffer, 2, false);  // False - finished with bus

    data->temp = buffer[0] << 8 | buffer[1];
}
