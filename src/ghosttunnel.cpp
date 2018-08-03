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

#include "gt_server.h"

int main(int argc, char* argv[])
{
	gt_server gts;

	if(argc != 3 && argc != 2)
	{
		printf("Usage: %s [interface]\n",  argv[0]);
		printf("Usage: %s [interface1] [interface2]\n",  argv[0]);
		gts.print_help();
		return 0;
	}

	if(argc == 2)
	{
		if(gts.initlize(argv[1], argv[1]))
		{
			gts.start();
		}
	}
	else
	{
		if(gts.initlize(argv[1], argv[2]))
		{
			gts.start();
		}		
	}


	return 0;
}
