/*
 *  Copyright (C) 2018 E7mer of PegasusTeam <haimohk@gmail.com>
 *
 *  This file is a part of GhostTunnel
 *
*/

#include "os_func.h"

DWORD get_computer_name(char *name_buf, DWORD *length)
{
	if (GetComputerNameA(name_buf, length))
	{
		return 0;
	}
	else
	{
		return GetLastError();
	}

}