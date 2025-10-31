/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_ADC_H
#define APP_ADC_H

//  ========== includes ====================================================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/ring_buffer.h> 

//  ========== defines =====================================================================
#define ADC_REFERENCE_VOLTAGE       3300    // 3.3V reference voltage of the board
#define ADC_RESOLUTION              4096    // 12-bit resolution
#define SAMPLE_RATE_MS              10
#define BUFFER_SIZE_SAMPLES         512
#define SAMPLE_SIZE_BYTES           sizeof(int16_t)

//  ========== globals =====================================================================
extern struct ring_buf adc_ringbuf;

//  ========== prototypes ==================================================================
int8_t app_nrf52_adc_init();
int16_t app_nrf52_adc_get();
void app_nrf52_adc_thread();

#endif /* APP_ADC_H */