/*
 * Userspace Backlight Driver
 * Modified from Driver for the Cirrus EP93xx lcd backlight
 * by 2010 H Hartley Sweeten <hsweeten@visionengravers.com>
 * 
 * Copyright (c) 2015 Icenowy Zheng <icenowy@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/kmod.h>

static int userspace_bl_brightness;
static struct backlight_device *userspace_bl_device;

static int userspace_bl_set(struct backlight_device *bl, int brightness)
{

	char brightness_chars[4];
	static char *envp[] = {"PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
	char *argv[] = {"/sbin/backlight_helper", brightness_chars, NULL};
	
	if(brightness < 0 || brightness > 255)
		return -EINVAL;

	snprintf(brightness_chars, 4, "%d", brightness);

	if(call_usermodehelper(argv[0], argv, envp, 1))
		return -ENOSYS;

	userspace_bl_brightness = brightness;

	return 0;
}

static int userspace_bl_update_status(struct backlight_device *bl)
{
	int brightness = bl->props.brightness;

	if (bl->props.power != FB_BLANK_UNBLANK ||
	    bl->props.fb_blank != FB_BLANK_UNBLANK)
		brightness = 0;

	return userspace_bl_set(bl, brightness);
}

static int userspace_bl_get_brightness(struct backlight_device *bl)
{
	return userspace_bl_brightness;
}

static const struct backlight_ops userspace_bl_ops = {
	.update_status	= userspace_bl_update_status,
	.get_brightness	= userspace_bl_get_brightness,
};

static int __init userspace_bl_create(void)
{
	struct backlight_properties props;

	printk("Initializing Userspace Backlight Driver.\n");

	memset(&props, 0, sizeof(struct backlight_properties));
	props.type = BACKLIGHT_RAW;
	props.max_brightness = 255;
	userspace_bl_device = backlight_device_register("userspace-bl", NULL, NULL,
				       &userspace_bl_ops, &props);
	if (IS_ERR(userspace_bl_device))
		return PTR_ERR(userspace_bl_device);

	userspace_bl_device->props.brightness = 255;

	userspace_bl_update_status(userspace_bl_device);

	return 0;
}

static int userspace_bl_remove(void)
{
	backlight_device_unregister(userspace_bl_device);
	return 0;
}

static int __init userspace_bl_init(void)
{
	printk("userspace_bl_init\n");
        return userspace_bl_create();
}
module_init(userspace_bl_init);

static void __exit userspace_bl_exit(void)
{
        userspace_bl_remove();
}
module_exit(userspace_bl_exit);

MODULE_DESCRIPTION("Userspace Backlight Driver");
MODULE_AUTHOR("Icenowy Zheng <icenowy@outlook.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:userspace-bl");
