/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_adc.h"
#include "app_storage.h"
#include "app_download.h"

//  ========== defines =====================================================================
#define STACK_SIZE 					4096
#define ADC_THREAD_PRIORITY 		2
#define STORAGE_THREAD_PRIORITY		3
RING_BUF_DECLARE(adc_ringbuf, BUFFER_SIZE_SAMPLES * SAMPLE_SIZE_BYTES);

//  ========== globals =====================================================================
K_THREAD_STACK_DEFINE(adc_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(storage_stack, STACK_SIZE);

static struct k_thread adc_thread_data;
static struct k_thread storage_thread_data;

// ========== main =========================================================================
int8_t main(void)
{
	// initialize ADC device
	int8_t ret = app_nrf52_adc_init();
	if (ret != 1) {
		printk("failed to initialize ADC device\n");
		return 0;
	}

	printk("Geophone Acquisition Example\n");
	
    // start threads
    k_thread_create(&adc_thread_data, adc_stack, STACK_SIZE,
                    (k_thread_entry_t)app_nrf52_adc_thread, NULL, NULL, NULL,
                    ADC_THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&storage_thread_data, storage_stack, STACK_SIZE,
                    (k_thread_entry_t)app_storage_thread, NULL, NULL, NULL,
                    STORAGE_THREAD_PRIORITY, 0, K_NO_WAIT);

	setup_download();

	while (1) {
        k_sleep(K_SECONDS(5));
		printk("Alive\n");
	}
	return 0;
}