/*
 * Copyright (C) 2017, Marvell International Ltd.
 * All Rights Reserved.
 */

/*
 * This is a board specific file for HAP Smart Switch application.
 *
 * This file contains the helper functions to retrieve the
 * GPIO numbers and other information specific to the board.
 *
 * It is recommended to edit or replace this file for new board.
 */

#include <smart_switch_board.h>

input_gpio_cfg_t ps_board_reset_to_factory_button = {
	.gpio = GPIO_22, .type = GPIO_ACTIVE_LOW };

input_gpio_cfg_t ps_board_power_switch_button = {
	.gpio = GPIO_22, .type = GPIO_ACTIVE_LOW };

input_gpio_cfg_t ps_board_reset_pairings_button = {
	.gpio = GPIO_24, .type = GPIO_ACTIVE_LOW };

output_gpio_cfg_t ps_board_load = {
	.gpio = GPIO_11, .type = GPIO_ACTIVE_LOW };

output_gpio_cfg_t ps_board_load_led = {
	.gpio = GPIO_40, .type = GPIO_ACTIVE_LOW };

output_gpio_cfg_t ps_board_conn_led = {
	.gpio = GPIO_41, .type = GPIO_ACTIVE_LOW };

output_gpio_cfg_t ps_board_reset_led = {
	.gpio = GPIO_41, .type = GPIO_ACTIVE_LOW };
