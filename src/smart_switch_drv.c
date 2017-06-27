/*
 * Copyright (C) 2017, Marvell International Ltd.
 * All Rights Reserved.
 */

#include <mdev_gpio.h>
#include <lowlevel_drivers.h>
#include <mdev_pinmux.h>
#include <push_button.h>
#include <led_indicator.h>
#include "smart_switch_board.h"

#define GPIO_ON		1
#define GPIO_OFF	0

void my_switch_on()
{
	mdev_t *gpio_dev = gpio_drv_open("MDEV_GPIO");
	if (ps_board_load.type == GPIO_ACTIVE_LOW)
		gpio_drv_write(gpio_dev, ps_board_load.gpio,
				GPIO_OFF);
	else
		gpio_drv_write(gpio_dev, ps_board_load.gpio,
				GPIO_ON);
	led_on(ps_board_load_led);
	gpio_drv_close(gpio_dev);
}

void my_switch_off()
{
	mdev_t *gpio_dev = gpio_drv_open("MDEV_GPIO");
	if (ps_board_load.type == GPIO_ACTIVE_LOW)
		gpio_drv_write(gpio_dev, ps_board_load.gpio,
				GPIO_ON);
	else
		gpio_drv_write(gpio_dev, ps_board_load.gpio,
				GPIO_OFF);
	led_off(ps_board_load_led);
	gpio_drv_close(gpio_dev);
}

void my_switch_drv_init(gpio_irq_cb pb_callback)
{
	mdev_t *pinmux_dev, *gpio_dev;

	pinmux_drv_init();
	gpio_drv_init();

	pinmux_dev = pinmux_drv_open("MDEV_PINMUX");
	gpio_dev = gpio_drv_open("MDEV_GPIO");

	/* Load GPIO */
	pinmux_drv_setfunc(pinmux_dev, ps_board_load.gpio,
			   pinmux_drv_get_gpio_func(ps_board_load.gpio));
	gpio_drv_setdir(gpio_dev, ps_board_load.gpio,
			GPIO_OUTPUT);

	pinmux_drv_close(pinmux_dev);
	gpio_drv_close(gpio_dev);

	/* Push Button */
	push_button_set_cb(ps_board_power_switch_button,
			   pb_callback, 0, 0, NULL);

	/* Load LED */
	/* Nothing special needs to be done as the settings are managed by
	 * the led_indicator functions (led_on, led_off, etc.)
	 */
}
