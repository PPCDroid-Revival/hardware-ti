/*
 * Author: Matthew Ranostay <mranostay@embeddedalley.com>
 * Copyright (C) 2009 Embedded Alley Solution Inc
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

#define LOG_TAG "Omap3EvmLights"

#include <hardware/hardware.h>
#include <hardware/lights.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>

#define BACKLIGHT_PATH "/sys/class/backlight/omap-backlight/brightness"
#define BRIGHTNESS_SCALED_SETTING(x)	(int) ((x & 0xff) / 2.55)

static int omap3evm_set_light(struct light_device_t* dev,
		struct light_state_t const* state)
{
	int fd = open(BACKLIGHT_PATH, O_RDWR);
	int level = BRIGHTNESS_SCALED_SETTING(state->color);
	unsigned int i;
	char buf[4];

	struct light_state_t *test = const_cast<struct light_state_t*>(state);

	if (fd < 0)
		return -EINVAL;

	sprintf(buf, "%3d", level);
	for (i = 0; i < strlen(buf); i++)
		if (buf[i] != ' ')
			break;

	write(fd, buf + i, strlen(buf) - i);
	close(fd);

	return 0;
}

static int omap3evm_close_device(struct hw_device_t *dev)
{
	return 0;
}

static int lights_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
	int status = -EINVAL;

	if (!strcmp(name, LIGHT_ID_BACKLIGHT)) {
		struct light_device_t *item;
		item = (struct light_device_t*) malloc(sizeof(*item));
		if (item == NULL)
			return status;

		item->set_light = &omap3evm_set_light;
		item->common.tag = HARDWARE_DEVICE_TAG;
		item->common.version = 0;
		item->common.module = const_cast<hw_module_t*>(module);
		item->common.close = &omap3evm_close_device;

		*device = (struct hw_device_t*) item;
		status = 0;
	}
	return status;
}

static struct hw_module_methods_t light_module_methods = {
    open: lights_device_open,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	tag: HARDWARE_MODULE_TAG,
	version_major: 1,
	version_minor: 0,
	id: LIGHTS_HARDWARE_MODULE_ID,
	name: "OMAP3EVM lights modules",
	author: "Matthew Ranostay",
	methods: &light_module_methods,
};
