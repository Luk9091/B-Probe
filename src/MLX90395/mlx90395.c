#include "mlx90395.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pico/error.h>
#include <hardware/i2c.h>
#include "i2c.h"

#define I2C_TIMEOUT     1000

#define COMMAND_STATUS_ADDRESS 0x40

typedef enum {
    START_BURST_MODE            = 0x01 << 4,
    START_WOC_MODE              = 0x02 << 4,
    START_SINGLE_MEASURE_MODE   = 0x03 << 4,
    READ_MEASURMENTS            = 0x04 << 4,
    SPI_READ_REGISTER           = 0x05 << 4,
    SPI_WRITE_REGISTER          = 0x06 << 4,
    EXIT_MODE                   = 0x08 << 4,
    READ_VOLTAGE                = 0x0C << 4,
    MEMORY_RECAL                = 0x0D << 4,
    MEMORY_STORE                = 0x0E << 4,
    RESET                       = 0x0F << 4,
} MLX90395_COMMANDS_t;


static error_t MLX90395_readMultipleRegisters(uint channel, MLX90395_range_t devAddress, uint8_t address, uint16_t *data, const uint size, bool skipRegAddr){
    i2c_inst_t* i2c = I2C_getChannelInstance(channel);
    error_t errorCode;
    if (skipRegAddr == false){
        address = address << 1;
        errorCode = i2c_write_timeout_per_char_us(i2c, devAddress, &address, 1, true, I2C_TIMEOUT);
        if (errorCode < 0) return errorCode;
    }

    uint8_t recv[32];
    errorCode = i2c_read_timeout_per_char_us(i2c, devAddress, recv, size, false, I2C_TIMEOUT);
    for (uint i = 0 ; i < size/2; i++){
        *(data + i) = recv[i*2 + 0] << 8 | recv[i*2 + 1];
    }
    return errorCode;
}

static error_t MLX90395_readRegister(uint channel, MLX90395_range_t devAddress, uint8_t address, uint16_t *data){
    return MLX90395_readMultipleRegisters(channel, devAddress, address, data, 2, false);
}

static error_t MLX90395_writeRegister(uint channel, MLX90395_range_t devAddress, uint8_t address, uint16_t data){
    i2c_inst_t* i2c = I2C_getChannelInstance(channel);
    address = address << 1;
    uint8_t toSend[3] = {address, data >> 8, data};
    error_t errorCode = i2c_write_timeout_per_char_us(i2c, devAddress, toSend, 3, true, I2C_TIMEOUT);
    return errorCode;
}

static error_t MLX90395_writeCommand(uint channel, MLX90395_range_t devAddress, uint8_t command){
    i2c_inst_t* i2c = I2C_getChannelInstance(channel);

    uint8_t toSend[2] = {COMMAND_STATUS_ADDRESS << 1, command};
    error_t errorCode = i2c_write_timeout_per_char_us(i2c, devAddress, toSend, 2, true, I2C_TIMEOUT);
    return errorCode;
}


uint64_t MLX90395_getID(uint channel, MLX90395_range_t range){
    I2C_initChannel(channel);
    uint16_t read;
    uint64_t id = 0;
    for (uint i = 0; i < 3; i++){
        error_t errorCode = MLX90395_readRegister(channel, range, (0x26 + i), &read);
        if (errorCode < 0){
            printf("Error transmission: %i\n", errorCode);
            return 0;
        }
        id = (id << 16) | read;
    }
    I2C_deinitChannel(channel);

    return id;
}

static MLX90395_reads_t MLX90395_convertRawReadToReads(uint16_t *recv){
    MLX90395_reads_t reads = {
        .status         = *(recv + 0) >> 8,
        .crc            = *(recv + 0) >> 0,
        .magnetic.x     = *(recv + 1),
        .magnetic.y     = *(recv + 2),
        .magnetic.z     = *(recv + 3),
        .temperature    = *(recv + 4),
        .voltage        = *(recv + 5)
    };

    return reads;
}

MLX90395_reads_t MLX90395_readMeasurements(uint channel, MLX90395_range_t range){
    uint16_t recv[sizeof(MLX90395_reads_t) / 2] = {0};
    I2C_initChannel(channel);
    MLX90395_readMultipleRegisters(channel, range, COMMAND_STATUS_ADDRESS, recv, sizeof(MLX90395_reads_t), false);
    I2C_deinitChannel(channel);

    return MLX90395_convertRawReadToReads(recv);
}

MLX90395_reads_t MLX90395_singleMeasure(uint channel, MLX90395_range_t range, uint8_t measureAxis){
    I2C_initChannel(channel);
    error_t errorCode = MLX90395_writeCommand(channel, range, START_SINGLE_MEASURE_MODE | (measureAxis & 0x0F));

    MLX90395_reads_t reads = {0, 0, 0, 0, 0};
    if (errorCode >= 0){
        sleep_ms(20);
        uint16_t recv[sizeof(MLX90395_reads_t) / 2] = {0};
        MLX90395_readMultipleRegisters(channel, range, COMMAND_STATUS_ADDRESS, recv, sizeof(MLX90395_reads_t), true);
        reads = MLX90395_convertRawReadToReads(recv);
    }
    I2C_deinitChannel(channel);
    return reads;
}
