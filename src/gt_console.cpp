/*
 *  Copyright (C) 2018 E7mer of PegasusTeam <haimohk@gmail.com>
 *
 *  This file is a part of GhostTunnel
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gt_console.h"

static int ping_interval = 5;


gt_console::gt_console()
{

}

gt_console::~gt_console()
{
	
}

int gt_console::initlize(void (*cmd_cb)(void *ctx, char *cmd),
	      void (*eof_cb)(void *ctx),
	      char ** (*completion_cb)(void *ctx, const char *cmd, int pos),
	      void *ctx, const char *history_file, const char *ps)
{


	eloop_init();

	memset(prompt, 0, 256);
	memset(prompt_org, 0, 64);
	
	strncpy(prompt_org, ps, 64);
	strncpy(prompt, ps, 256);

	edit_init(cmd_cb, eof_cb, completion_cb, ctx, history_file, ps);

	eloop_run();

	return 0;
}

void gt_console::destroy()
{
	edit_deinit(NULL, NULL);
	eloop_terminate();
	eloop_destroy();
}

void gt_console::set_prompt(char *ps)
{

	memset(prompt, 0, 256);

	strncpy(prompt, prompt_org, 256);
	strcat(prompt, ">");
	strcat(prompt, ps);

	edit_set_psstr(prompt);

}














