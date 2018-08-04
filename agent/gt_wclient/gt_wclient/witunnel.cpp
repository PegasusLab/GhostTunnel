/*
 *  Copyright (C) 2018 E7mer of PegasusTeam <haimohk@gmail.com>
 *
 *  This file is a part of GhostTunnel
 *
*/

#include <Shlwapi.h>
#include <stdio.h>
#include <VersionHelpers.h>
#include "witunnel.h"
#include "gt_common.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "Shlwapi.lib")

#pragma warning(disable: 4244)
#pragma warning(disable: 4996)  

wifitunnel* wifitunnel::current_instance = NULL;

wifitunnel* wifitunnel::get_instance()
{
	if (current_instance == NULL)
	{
		current_instance = new wifitunnel();
	}

	return current_instance;
}

wifitunnel::wifitunnel()
{
	wlan_handle = NULL;
	ifaces_num = 0;
	sel_ifaces_idx = 0;

	memset(&iface_info, 0, sizeof(iface_info));
	memset(&sel_iface_guid, 0, sizeof(sel_iface_guid));

	InitializeCriticalSection(&wlan_handle_critical);
}

wifitunnel::~wifitunnel()
{
	WlanCloseHandle(wlan_handle, NULL);
	DeleteCriticalSection(&wlan_handle_critical);
}

bool wifitunnel::initialize()
{
	DWORD result;
	PWLAN_INTERFACE_INFO_LIST piface_info_list = NULL;
	DWORD negotiated_version = 0;
	DWORD prev_notif_source = 0;

	if (!IsWindowsXPSP3OrGreater())
		return false;

	result = WlanOpenHandle(2, NULL, &negotiated_version, &wlan_handle);
	if (result != ERROR_SUCCESS)
		return false;

	result = WlanEnumInterfaces(wlan_handle, NULL, &piface_info_list);
	if (result != ERROR_SUCCESS)
	{
		WlanCloseHandle(wlan_handle, NULL);
		wlan_handle = NULL;
		return false;
	}

	ifaces_num = piface_info_list->dwNumberOfItems;
	if (ifaces_num == 0)
	{
		WlanFreeMemory(piface_info_list);
		WlanCloseHandle(wlan_handle, NULL);
		wlan_handle = NULL;
		return false;
	}

	memcpy(&iface_info, &piface_info_list->InterfaceInfo[0], sizeof(WLAN_INTERFACE_INFO));
	sel_iface_guid = iface_info.InterfaceGuid;

	WlanFreeMemory(piface_info_list);

	return true;
}

int wifitunnel::send(BYTE* data, DWORD length)
{
	DWORD result;
	DOT11_SSID tunnel_ssid = {0};
	WLAN_RAW_DATA *pwlan_raw_data = NULL;
	unsigned char raw_data_length;

	if (!wlan_handle || data == NULL || length > WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH)
		return -1;

	if (length > DOT11_SSID_MAX_LENGTH)
	{
		tunnel_ssid.uSSIDLength = 32;
		memcpy(tunnel_ssid.ucSSID, data, 32);

		raw_data_length = length - 32;
		pwlan_raw_data = (WLAN_RAW_DATA *)malloc(sizeof(DWORD) + 2 + raw_data_length);
		pwlan_raw_data->dwDataSize = 2 + raw_data_length;
		pwlan_raw_data->DataBlob[0] = 0xDD;
		pwlan_raw_data->DataBlob[1] = raw_data_length;
		memcpy(pwlan_raw_data->DataBlob + 2, data + 32 , raw_data_length);
	}
	else 
	{
		tunnel_ssid.uSSIDLength = length;
		memcpy(tunnel_ssid.ucSSID, data, length);
	}

	EnterCriticalSection(&wlan_handle_critical);
	result = WlanScan(wlan_handle, &sel_iface_guid, &tunnel_ssid, pwlan_raw_data, NULL);
	LeaveCriticalSection(&wlan_handle_critical);

	if(pwlan_raw_data)
		free(pwlan_raw_data);

	if (result != ERROR_SUCCESS)
	{
		if (result == ERROR_INVALID_PARAMETER)
		{
			printf("ERROR: WlanScan -> ERROR_INVALID_PARAMETER\n");
		}
		else if (result == ERROR_INVALID_HANDLE)
		{
			printf("ERROR: WlanScan -> ERROR_INVALID_HANDLE\n");
		}
		else if (result == ERROR_NOT_ENOUGH_MEMORY)
		{
			printf("ERROR: WlanScan -> ERROR_NOT_ENOUGH_MEMORY\n");
		}
		else
		{
			//printf("WlanScan unknow error: %d\n", result);
		}

		return -1;
	}

	return 0;
}

int wifitunnel::receive(BYTE* buffer, DWORD length)
{
	DWORD result, ret = -1;
	PWLAN_BSS_LIST pwlan_bss_list = NULL;
	tunnel_data_header *ptdh = NULL;
	unsigned char *tags;
	int left;

	if (!wlan_handle)
		return ret;

	EnterCriticalSection(&wlan_handle_critical);
	result = WlanScan(wlan_handle, &sel_iface_guid, NULL, NULL, NULL);
	if (result != ERROR_SUCCESS)
	{
		LeaveCriticalSection(&wlan_handle_critical);
		//printf("ERROR: receive WlanScan: %d\n", result);
		return ret;
	}

	ret = 0;

	result = WlanGetNetworkBssList(wlan_handle, &sel_iface_guid, NULL, dot11_BSS_type_infrastructure, TRUE, NULL, &pwlan_bss_list);
	if (result != ERROR_SUCCESS || pwlan_bss_list->dwNumberOfItems <= 0)
	{
		LeaveCriticalSection(&wlan_handle_critical);
		//printf("ERROR: receive WlanGetNetworkBssList: %d\n", result);
		return ret;
	}

	//printf("INFO: receive data success, item number: %d.\n", pwlan_bss_list->dwNumberOfItems);
	for (DWORD i = 0; i < pwlan_bss_list->dwNumberOfItems; i++)
	{
		if (pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID[0] == 0xFE)  // ghosttunnel data
		{
			ptdh = (tunnel_data_header*)pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID;
			if (ptdh->data_type & DATA_IN_VENDOR)
			{
				tags = (BYTE*)&pwlan_bss_list->wlanBssEntries[i] + pwlan_bss_list->wlanBssEntries[i].ulIeOffset;
				left = pwlan_bss_list->wlanBssEntries[0].ulIeSize;
				left = left - 2 - tags[1];
				while (left > 0)   // the last one
				{
					tags = tags + 2 + tags[1];
					left = left - 2 - tags[1];
				}

				if (tags[0] != 0xDD)  //vendor specific
				{
					break;
				}

				//ptdh->length += tags[1];     // total data length
				memcpy(buffer, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID, /*32*/pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
				memcpy(buffer + 32, tags + 2, tags[1]);

				ret = 32 + tags[1];
			}
			else
			{
				memcpy(buffer, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
				ret = pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength;
			}
			
			break;
		}
	}

	WlanFreeMemory(pwlan_bss_list);
	LeaveCriticalSection(&wlan_handle_critical);

	return ret;
}

int wifitunnel::receive_send(BYTE* rbuffer, DWORD rlength, BYTE* sdata, DWORD slength)
{
	DWORD result, ret = -1;
	PWLAN_BSS_LIST pwlan_bss_list = NULL;
	tunnel_data_header *ptdh = NULL;
	unsigned char *tags;
	int left;

	DOT11_SSID tunnel_ssid = { 0 };
	WLAN_RAW_DATA *pwlan_raw_data = NULL;
	unsigned char raw_data_length;

	if (!wlan_handle)
		return ret;

	// send
	EnterCriticalSection(&wlan_handle_critical);
	if (sdata != NULL && slength > 0  && slength <= WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH)
	{
		if (slength > DOT11_SSID_MAX_LENGTH)
		{
			tunnel_ssid.uSSIDLength = 32;
			memcpy(tunnel_ssid.ucSSID, sdata, 32);

			raw_data_length = slength - 32;
			pwlan_raw_data = (WLAN_RAW_DATA *)malloc(sizeof(DWORD) + 2 + raw_data_length);
			pwlan_raw_data->dwDataSize = 2 + raw_data_length;
			pwlan_raw_data->DataBlob[0] = 0xDD;
			pwlan_raw_data->DataBlob[1] = raw_data_length;
			memcpy(pwlan_raw_data->DataBlob + 2, sdata + 32, raw_data_length);
		}
		else
		{
			tunnel_ssid.uSSIDLength = slength;
			memcpy(tunnel_ssid.ucSSID, sdata, slength);
		}
		result = WlanScan(wlan_handle, &sel_iface_guid, &tunnel_ssid, pwlan_raw_data, NULL);
	}
	else
	{
		result = WlanScan(wlan_handle, &sel_iface_guid, NULL, NULL, NULL);
	}

	if (result != ERROR_SUCCESS)
	{
		LeaveCriticalSection(&wlan_handle_critical);
		if (pwlan_raw_data)
			free(pwlan_raw_data);
		//printf("ERROR: receive WlanScan: %d\n", result);
		return ret;
	}

	if (pwlan_raw_data)
		free(pwlan_raw_data);

	ret = 0;

	result = WlanGetNetworkBssList(wlan_handle, &sel_iface_guid, NULL, dot11_BSS_type_infrastructure, TRUE, NULL, &pwlan_bss_list);
	if (result != ERROR_SUCCESS || pwlan_bss_list->dwNumberOfItems <= 0)
	{
		LeaveCriticalSection(&wlan_handle_critical);
		//printf("ERROR: receive WlanGetNetworkBssList: %d\n", result);
		return ret;
	}

	//printf("INFO: receive data success, item number: %d.\n", pwlan_bss_list->dwNumberOfItems);
	for (DWORD i = 0; i < pwlan_bss_list->dwNumberOfItems; i++)
	{
		if (pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID[0] == 0xFE)  // ghosttunnel data
		{
			ptdh = (tunnel_data_header*)pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID;
			if (ptdh->data_type & DATA_IN_VENDOR)
			{
				tags = (BYTE*)&pwlan_bss_list->wlanBssEntries[i] + pwlan_bss_list->wlanBssEntries[i].ulIeOffset;
				left = pwlan_bss_list->wlanBssEntries[0].ulIeSize;
				left = left - 2 - tags[1];
				while (left > 0)   // the last one
				{
					tags = tags + 2 + tags[1];
					left = left - 2 - tags[1];
				}

				if (tags[0] != 0xDD)  //vendor specific
				{
					break;
				}

				//ptdh->length += tags[1];     // total data length
				memcpy(rbuffer, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID, /*32*/pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
				memcpy(rbuffer + 32, tags + 2, tags[1]);

				ret = 32 + tags[1];
			}
			else
			{
				memcpy(rbuffer, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.ucSSID, pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
				ret = pwlan_bss_list->wlanBssEntries[i].dot11Ssid.uSSIDLength;
			}

			break;
		}
	}

	WlanFreeMemory(pwlan_bss_list);
	LeaveCriticalSection(&wlan_handle_critical);

	return ret;
}





