/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_storage.h"

//  ========== globals =====================================================================
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage_partition_lfs);
static struct fs_mount_t lfs_storage_mount = {
    .type = FS_LITTLEFS,
    .mnt_point = "/lfs",
    .fs_data = &storage_partition_lfs,
    .storage_dev = (void *)FIXED_PARTITION_ID(storage_partition_lfs),
};

//  ========== app_storage_thread ==========================================================
void app_storage_thread(void *arg1, void *arg2, void *arg3)
{
    int rc = fs_mount(&lfs_mount);
    if (rc < 0) {
        printk("Mount failed (%d)", rc);
        return;
    }

    struct fs_file_t file;
    fs_file_t_init(&file);

    static int file_index = 0;
    char file_path[32];
    snprintf(file_path, sizeof(file_path), "%s_%03d%s", FILE_PREFIX, file_index, FILE_EXT);

    rc = fs_open(&file, file_path, FS_O_CREATE | FS_O_WRITE | FS_O_APPEND);
    if (rc < 0) {
        printk("File open failed (%d)", rc);
        return;
    }

    size_t current_file_size = fs_tell(&file);
    uint8_t buf[STORAGE_BUFFER_SIZE];

    while (1) {
        // pull data from ring buffer
        uint32_t len = ring_buf_get(&adc_ringbuf, buf, sizeof(buf));
        if (len > 0) {
            rc = fs_write(&file, buf, len);
            if (rc < 0) {
                printk("flash write error (%d)", rc);
            } else {
                current_file_size += len;
            }

            // rotate file if max size reached
            if (current_file_size >= MAX_FILE_SIZE) {
                fs_close(&file);
                file_index++;
                snprintf(file_path, sizeof(file_path), "%s_%03d%s", FILE_PREFIX, file_index, FILE_EXT);
                fs_file_t_init(&file);
                rc = fs_open(&file, file_path, FS_O_CREATE | FS_O_WRITE | FS_O_APPEND);
                if (rc < 0) {
                    printk("failed to open new file (%d)", rc);
                    return;
                }
                current_file_size = 0;
                printk("rotated to new file: %s", file_path);
            }
        } else {
            k_sleep(K_MSEC(5));
        }
    }

    fs_close(&file);
}