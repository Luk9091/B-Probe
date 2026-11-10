#ifndef MLX90395_H
#define MLX90395_H

#include <stdio.h>
#include "pico/stdlib.h"

#define MLX90395_DEFAULT_ADDRESS    (0x0C)
#define MLX90395_AXIS_Z             (1 << 4)
#define MLX90395_AXIS_Y             (1 << 3)
#define MLX90395_AXIS_X             (1 << 2)
#define MLX90395_AXIS_T             (1 << 1)
#define MLX90395_AXIS_ALL           (MLX90395_AXIS_Z | MLX90395_AXIS_Y | MLX90395_AXIS_X | MLX90395_AXIS_T)

typedef enum {
    MLX90395_RANGE_50mT  = MLX90395_DEFAULT_ADDRESS + 0,
    MLX90395_RANGE_120mT = MLX90395_DEFAULT_ADDRESS + 1,
} MLX90395_range_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} MLX90395_axis_t;

typedef struct{
    uint8_t status;
    uint8_t crc;
    MLX90395_axis_t magnetic;
    uint16_t temperature;
    uint16_t voltage;
} MLX90395_reads_t;


uint64_t MLX90395_getID(uint channel, MLX90395_range_t range);

MLX90395_reads_t MLX90395_readMeasurements(uint channel, MLX90395_range_t range);
MLX90395_reads_t MLX90395_singleMeasure(uint channel, MLX90395_range_t range, uint8_t measureAxis);


static inline void MLX90395_printMeasurements(MLX90395_reads_t reads){
    printf("Status: %i, CRC: %i\n", reads.status, reads.crc);
    printf("Magnetic field: %iuT, %iuT, %iuT\n", reads.magnetic.x, reads.magnetic.y, reads.magnetic.x);
    printf("Temperature: %iC\n", reads.temperature);
    printf("Voltage: %iV\n", reads.voltage);
}

#endif
