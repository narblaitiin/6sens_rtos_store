/*
 * Copyright (c) 2025
 * Hugo Reymond, Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */
#include "app_download.h"
#include <zephyr/sys/base64.h>
//  ========== globals =====================================================================
char ram_buffer[DOWNLOAD_RAM_BUF_SIZE];

//  ========== setup_download() ============================================================
void setup_download()
{
    int not_found = 1;
    int rc = 0;

    // Check if flash has been mounted
    while (not_found)
    {
        int mount_index = 0;
        char name[30];
        char *new_name = name;
        rc = 0;
        while (rc == 0)
        {
            rc = fs_readmount(&mount_index, &new_name);
            if (new_name[0] != 0)
            {
                printk("Found mount : %s\n", new_name);
            }
            if (new_name[0] == '/' && new_name[1] == 'l' && new_name[2] == 'f' && new_name[3] == 's')
            {
                printk("LFS mount is here !\n");
                not_found = 0;
            }
        }
        if (not_found)
        {
            printk("LFS mount not found,  sleeping...\n");
            k_sleep(K_SECONDS(1));
        }
    }

    // Get the lfs folder
    struct fs_dir_t root_dir;
    fs_dir_t_init(&root_dir);
    rc = fs_opendir(&root_dir, "/lfs");
    switch (rc)
    {
    case -EINVAL:
        printk("Bad directory given...\n");
        break;
    case 0:
        printk("Opened root folder\n");
    default:
        break;
        printk("Error : error code=%d\n", rc);
    }

    printk("Reading content of lfs dir\n");
    struct fs_dirent dir_entry;
    while (true)
    {
        rc = fs_readdir(&root_dir, &dir_entry);
        if (dir_entry.name[0] == 0 || rc < 0)
        {
            break;
        }
        printk("FILE:%s\n", dir_entry.name);
        
        char file_path[32];
        snprintf(file_path, sizeof(file_path), "%s%s", "/lfs/", dir_entry.name);

        dump_file(file_path);
        printk("File dumped !\n");
    }
}

void dump_file(char * file_path)
{   
    int rc;
    char * buffer[100] = {0};
    char * base64_encoded[200] = {0};
    // printf("Opening file %s\n", file_path);
    struct fs_file_t file;
    fs_file_t_init(&file);
    rc = fs_open(&file, file_path, FS_O_READ);
    if (rc < 0)
    {
        printk("file open failed. error: %d\n", rc);
        return;
    }
    
    int total_encoded = 0;
    int written = 1;
    while(written > 0) {
        printk("D:");
        written = fs_read(&file, buffer, 100);
        size_t encoded = 0;
        rc = base64_encode(base64_encoded, 200, &encoded, buffer, written);
        base64_encoded[encoded] = 0;
        if(rc != 0) {
            printf("Error encoding to base 64\n");
            return;
        }
        printk(base64_encoded);
        total_encoded += encoded;
        printk("\n");
        k_sleep(K_MSEC(15));
    }

    printk("TOTAL_ENCODED:%d", total_encoded);
    
    rc = fs_close(&file);
    if (rc < 0)
    {
        printk("file closed failed. error: %d\n", rc);
    }
    return;
}
