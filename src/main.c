#include <stdio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include "mlx90395.h"

#define GPIO_LED    25

static inline void LED_init(){
    gpio_set_function(GPIO_LED, GPIO_FUNC_SIO);
    gpio_init(GPIO_LED);
    gpio_set_dir(GPIO_LED, GPIO_OUT);
}


int main(){
    stdio_init_all();
    // stdio_usb_init();
    LED_init();
    printf("Hello world!\n");

    uint64_t id = MLX90395_getID(2, MLX90395_RANGE_50mT);
    printf("Sensor ID: 0x%012llX\n", id);

    // MLX90395_reads_t reads = MLX90395_readMeasurements(2, MLX90395_RANGE_50mT);

    while(1){
        MLX90395_reads_t reads = MLX90395_singleMeasure(2, MLX90395_RANGE_50mT, MLX90395_AXIS_ALL);
        MLX90395_printMeasurements(reads);

        gpio_put(GPIO_LED, 1);
        sleep_ms(200);
        gpio_put(GPIO_LED, 0);
        sleep_ms(200);
    }
    return 0;
}
