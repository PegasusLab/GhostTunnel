#include <windows.h>
#include <stdio.h>
#include "data_handler.h"


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	data_handler *handler = data_handler::get_instance();

	if (handler->initialize())
	{
		handler->start();
	}

	return 0;
}

int main(int argc, char* argv[])
{
	data_handler *handler = data_handler::get_instance();

	if (handler->initialize())
	{
		handler->start();
	}

	return 0;
}
