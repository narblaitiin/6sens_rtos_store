/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_STORAGE_H
#define APP_STORAGE_H

//  ========== includes ====================================================================
#include "app_adc.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/ring_buffer.h>

//  ========== defines =====================================================================
#define FILE_PREFIX             "/lfs/geophone"
#define FILE_EXT                ".dat"
#define MAX_FILE_SIZE           (512 * 1024)   // 512 KB per file (adjustable)
#define STORAGE_BUFFER_SIZE     64

//  ========== prototypes ==================================================================
void app_storage_thread();

#endif /* APP_STORAGE_H */