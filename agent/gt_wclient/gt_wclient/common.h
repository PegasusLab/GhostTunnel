#ifndef _COMMON_H_
#define _COMMON_H_

#pragma pack(1)

typedef struct _information_element
{
	unsigned char id;
	unsigned char length;
}information_element;

typedef struct _tunnel_data_header
{
	unsigned char id;
	unsigned char length;
	unsigned char flag;
	unsigned int packet_count;

}tunnel_data_header;

typedef enum _cmd_type
{
	TUNNEL_ONLINE = 1,
	TUNNEL_HEARTBEAT,

	TUNNEL_CMD_INIT,
	TUNNEL_CMD_DATA,
	TUNNEL_CMD_QUIT,
	TUNNEL_CMD_OK,

	TUNNEL_FILE,
	TUNNEL_SCREEN_CAPTURE,
	TUNNEL_UNKNOW

}cmd_type;

typedef struct _tunnel_cmd_header
{
	unsigned char cmd_type;
	unsigned char length;
}tunnel_cmd_header;

#define UNKNOW_OS 0
#define WIN_XP 1
#define	WIN_VISTA 2
#define	WIN_7 3
#define	WIN_8 4
#define	WIN_81 5
#define	WIN_10 6
#define	MAC_OSX 7

typedef struct _tunnel_online_data
{
	char computer_name[27];
	char os;
}tunnel_online_data;

/*typedef struct _tunnel_online_response
{
	char computer_name[27];
	unsigned char client_id;
}tunnel_online_response;*/

#pragma pack()






#endif // !_COMMON_H_
