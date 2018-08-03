/*
 * Command line editing and history
 * Copyright (c) 2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EDIT_H
#define EDIT_H


int edit_init(void (*cmd_cb)(void *ctx, char *cmd),
	      void (*eof_cb)(void *ctx),
	      char ** (*completion_cb)(void *ctx, const char *cmd, int pos),
	      void *ctx, const char *history_file, const char *ps);
void edit_deinit(const char *history_file,
		 int (*filter_cb)(void *ctx, const char *cmd));
void edit_clear_line(void);
void edit_redraw(void);
void edit_set_psstr(const char *ps);
void edit_wait_reply(int wait);
void edit_print_ps(void);
void edit_print_redraw(void);
void edit_input_redraw(void);



#endif /* EDIT_H */
