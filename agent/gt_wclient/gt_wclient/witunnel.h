#ifndef _WIFI_TUNNEL_H_
#define _WIFI_TUNNEL_H_

#include <windows.h>
#include <wlanapi.h>

#define TUNNEL_MAX_BUFFER_SIZE 512

class wifitunnel
{
public:
	static wifitunnel* get_instance();
	~wifitunnel();

	bool initialize();
	int send(BYTE* data, DWORD length);
	int receive(BYTE* buffer, DWORD length);
	int receive_send(BYTE* rbuffer, DWORD rlength, BYTE* sdata, DWORD slength);

private:
	wifitunnel();

private:
	static wifitunnel * current_instance;
	DWORD ifaces_num;
	DWORD sel_ifaces_idx;
	WLAN_INTERFACE_INFO iface_info;
	GUID sel_iface_guid;
	HANDLE wlan_handle;
	CRITICAL_SECTION wlan_handle_critical;

};



#endif
