#ifndef SDCARD_LOCAL_H
#define SDCARD_LOCAL_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hardware/rtc.h"
#include "pico/stdlib.h"

#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "rtc.h"
#include "sd_card.h"


/* ------------- PROTOTIPO DE FUNCOES ------------- */

sd_card_t *sd_get_by_name(const char *const name);
FATFS *sd_get_fs_by_name(const char *name);
void run_setrtc();
void run_format();
int run_mount();
int run_unmount();
void run_getfree();
void run_ls();
void run_cat();
void read_file(const char *filename);

#endif // SDCARD_LOCAL_H