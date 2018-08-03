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

#ifndef _GT_SERVER_H
#define _GT_SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <map>
#include <queue>
#include "gt_console.h"
#include "gt_common.h"

extern "C"
{
#include "./libwifi/kismet/kismet_wifi_control.h"
#include "./libwifi/aircrack-ng/osdep_wifi_transmit.h"	
#include "./packet/packet.h"
#include "./edit/cli.h"
#include "./edit/os.h"
}


using std::string;
using std::map;
using std::queue;


#define DEFAULT_TIM "\x05\x04\x00\x01\x00\x00"
#define DEFAULT_ERP "\x2a\x01\x00"
#define DEFAULT_CHANNEL "\x03\x01\x0b"
#define DEFAULT_VENDOR "\xdd\x18\x00\x50\xf2\x02\x01\x01\x00\x00\x03\xa4\x00\x00\x27\xa4\x00\x00\x42\x43\x5e\x00\x62\x32\x2f\x00"


typedef enum _OP_MODE
{
	OP_INIT,
	OP_SELECTED,
	OP_SHELL,

}OP_MODE;


typedef struct _client_session
{
	unsigned char id;
	unsigned char seq;      // receive seq
	unsigned char send_seq; // send seq
	char os_version;
	char device_name[128];
	struct ether_addr mac_addr;
	bool is_connected;

}client_session; 


typedef struct _send_pkt
{
	unsigned char client_id;
	unsigned char seq;
	struct packet pkt;
}send_pkt;


// client_id,  
typedef map<unsigned char, client_session> client_manager_map;


class gt_server
{
public:
	gt_server();
	~gt_server();

	bool initlize(const char *rinterface, const char *winterface);
	int start();

	static int cmd_list(void *param, int argc, char *argv[]);
	static int cmd_use(void *param, int argc, char *argv[]);
	static int cmd_exit(void *param, int argc, char *argv[]);
	static int cmd_wget(void *param, int argc, char *argv[]);
	static int cmd_quit(void *param, int argc, char *argv[]);
	static int cmd_help(void *param, int argc, char *argv[]);
	void print_help();

private:

	static void* receive_thread(void *param);
	static void* send_thread(void *param);
	static void* handle_conn_thread(void *param);

	static void edit_cmd_command_cb(void *ctx, char *cmd_str);
	static void edit_eof_cb(void *ctx);
	static char ** edit_completion_cb(void *ctx, const char *str, int pos);

	int cmd_command(char *cmd);


	int send(unsigned char client_id, TUNNEL_DATA_TYPE data_type, unsigned char *data, unsigned int len);

	int parse_data_from_probe_request(struct packet *pkt, unsigned char *data, int *len);
	void handle_data(struct packet *pkt, unsigned char *data, int len);

	void handle_connect(struct packet *pkt, unsigned char *data, int len);
	void handle_shell(struct packet *pkt, unsigned char *data, int len);
	void handle_file(struct packet *pkt, unsigned char *data, int len);

	//
	void client_session_insert(client_session *cs);
	void client_session_delete(unsigned char client_id);
	bool client_session_query(unsigned char client_id, client_session **cs);
	bool client_session_query(struct ether_addr *mac, client_session **cs);
	bool client_session_exist(struct ether_addr *mac);

	struct packet create_tunnel_beacon_packet(unsigned char *ssid, unsigned char ssid_len, unsigned char *vendor, unsigned char vendor_len);
	int to_utf8(char *src, size_t *src_len, char *dest, size_t *dest_len);

	int download_progress(int rates);

private:
	unsigned char server_id;
	unsigned char cur_client_id;
	gt_console gt_cnsl;
	string riface;
	string wiface;
	bool running;

	char file_name[256];
	unsigned int file_size;
	unsigned int cur_size;
	FILE *fp;

	unsigned char cur_op_client_id;
	client_session *cur_op_client;
	unsigned int shell_acp;
	char acp_name[32];

	pthread_mutex_t mutex;
	client_manager_map client_session_mgr;
	unsigned char sequence_number;

	queue<send_pkt> send_queue;
	pthread_mutex_t send_queue_mutex;

};







#endif
