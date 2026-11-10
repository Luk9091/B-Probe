#ifndef I2C_H
#define I2C_H

#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C i2c0
#define I2C_SPEED (100e3)
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

static inline void I2C_init(){
    i2c_init(i2c0, I2C_SPEED);
    i2c_init(i2c1, I2C_SPEED);
}

static inline i2c_inst_t* I2C_getChannelInstance(uint channel){
    return channel % 2 == 0 ? i2c0 : i2c1;
}

static inline void I2C_initChannel(uint channel){
    uint sda = (channel * 2) + 0;
    uint scl = (channel * 2) + 1;
    I2C_init();
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
}

static inline void I2C_deinitChannel(uint channel){
    uint sda = (channel * 2) + 0;
    uint scl = (channel * 2) + 1;

    i2c_deinit(I2C_getChannelInstance(channel));
    gpio_deinit(sda);
    gpio_deinit(scl);
    // gpio_put(sda, 1);
    // gpio_put(scl, 1);
    // gpio_set_dir(sda, GPIO_OUT);
    // gpio_set_dir(scl, GPIO_OUT);
    // gpio_set_function((channel * 2) + 0, GPIO_FUNC_SIO);
    // gpio_set_function((channel * 2) + 1, GPIO_FUNC_SIO);
}

#endif
