/*
 * Copyright (c) 2025
 * Hugo Reymond, Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef APP_DOWLOAD_H
#define APP_DOWLOAD_H

//  ========== includes ====================================================================
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>


//  ========== defines =====================================================================
#define DOWNLOAD_RAM_BUF_SIZE 1024  

//  ========== prototypes ==================================================================
void setup_download();
void dump_file(char * file_path);


#endif // APP_DOWNLOAD_H