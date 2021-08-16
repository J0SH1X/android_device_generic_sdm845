/*
 * Copyright (C) 2014, 2017-2018 The  Linux Foundation. All rights reserved.
 * Not a contribution
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// #define LOG_NDEBUG 0

#include <log/log.h>
#include <cutils/properties.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#define CG_COLOR_ID_PROPERTY "ro.boot.hardware.color"

/******************************************************************************/
static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static int rgb_brightness_ratio = 255;

char const*const BACKLIGHT_CLASS
        = "/sys/class/backlight";
char const*const BACKLIGHT_CLASS_FSTRING
        = "/sys/class/backlight/%s/brightness";

/**
 * device methods
 */

void init_globals(void)
{
    char color_id_prop[PROPERTY_VALUE_MAX] = {""};

    // init the mutex
    pthread_mutex_init(&g_lock, NULL);

    // check CG color
    property_get(CG_COLOR_ID_PROPERTY, color_id_prop, "DEF00");
    if (strcmp(color_id_prop, "GRA00") == 0) {
        rgb_brightness_ratio = 25;
    } else if (strcmp(color_id_prop, "SLV00") == 0) {
        rgb_brightness_ratio = 15;
    } else if (strcmp(color_id_prop, "BLU00") == 0) {
        rgb_brightness_ratio = 15;
    } else {
        rgb_brightness_ratio = 20;
    }
}

static bool findFirstBacklightDevice(char *filename) {
    struct dirent *de;
    DIR *dir = opendir("/sys/class/backlight");
    if (dir == NULL)
        return NULL;
    while((de = readdir(dir))) {
        if(de->d_name[0] == '.' &&
           (de->d_name[1] == '\0' ||
            (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        closedir(dir);
        return true;
    }
    closedir(dir);
    return false;
}

static int
write_brightness(int value)
{
    int fd;
    char brightness_path[4096];
    char filename[256];
    if (!findFirstBacklightDevice(filename)) {
        ALOGE("CA:: Couldn't find backlight brightness path");
        return -EINVAL;
    }

    snprintf(brightness_path, 4096, BACKLIGHT_CLASS_FSTRING, filename);
    fd = open(brightness_path, O_WRONLY);
    if (fd >= 0) {
        char buffer[20];
        ssize_t amt;
        size_t bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
        if(bytes < sizeof(buffer)) {
            amt = write(fd, buffer, bytes);
        } else {
            amt = -1;
            errno = -EINVAL;
        }
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        ALOGE("write_brightness failed to open %s, fd = %d, errno = %d\n", brightness_path, fd, errno);
        return -errno;
    }
}

static int
rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int
set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    if(!dev) {
        return -1;
    }

    int brightness = rgb_to_brightness(state) << 4; // Scale up to 4095
    write_brightness(brightness);

    return 0;
}

/** Close the lights device */
static int
close_lights(struct light_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    (void)name;
    pthread_once(&g_init, init_globals);

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));

    if(!dev)
        return -ENOMEM;

    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = LIGHTS_DEVICE_API_VERSION_2_0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light_backlight;

    *device = (struct hw_device_t*)dev;
    return 0;
}

static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

/*
 * The lights Module
 */
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "lights Module",
    .author = "Google, Inc.",
    .methods = &lights_module_methods,
};
