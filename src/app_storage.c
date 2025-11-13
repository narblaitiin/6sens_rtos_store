/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_storage.h"

//  ========== app_storage_thread ==========================================================
void app_storage_thread(void *arg1, void *arg2, void *arg3)
{
    struct fs_file_t file;
    fs_file_t_init(&file);

    static int file_index = 0;
    char file_path[32];
    snprintf(file_path, sizeof(file_path), "%s_%03d%s", FILE_PREFIX, file_index, FILE_EXT);

    int rc;    
    rc = fs_open(&file, file_path, FS_O_CREATE | FS_O_WRITE | FS_O_APPEND);
    if (rc < 0) {
        printk("file open failed. error: %d\n", rc);
        return;
    }
    // printf("\npartition address beginning: flash erase page at 0x%x\n", TEST_PARTITION_OFFSET);

    size_t current_file_size = fs_tell(&file);
    uint8_t buf[STORAGE_BUFFER_SIZE];

    while (1) {
        // pull data from ring buffer
        uint32_t len = ring_buf_get(&adc_ringbuf, buf, sizeof(buf));
        if (len > 0) {
            //printk("writing %u bytes\n", len);
            rc = fs_write(&file, buf, len);
            if (rc < 0) {
                printk("flash write failed. error: %d\n", rc);
            } else {
                current_file_size += len;
                //printk("current size: %zu / %d\n", current_file_size, MAX_FILE_SIZE);
            }

            // rotate file if max size reached
            if (current_file_size >= MAX_FILE_SIZE) {
                fs_close(&file);
                file_index++;
                snprintf(file_path, sizeof(file_path), "%s_%03d%s", FILE_PREFIX, file_index, FILE_EXT);
                fs_file_t_init(&file);
                rc = fs_open(&file, file_path, FS_O_CREATE | FS_O_WRITE | FS_O_APPEND);
                if (rc < 0) {
                    printk("failed to open new file. error: %d\n", rc);
                    return;
                }
                current_file_size = 0;
                printk("rotated to new file: %s\n", file_path);
            }
        } else {
            k_sleep(K_MSEC(5));
        }
    }

    fs_close(&file);
}