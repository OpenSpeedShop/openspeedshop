/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "support.h"

command_t command;

/**
 * Function: cmd_init
 * 
 * 
 *     
 * @param   void
 *
 * @return  void.
 *
 */
void
cmd_init()
{
    command.lineno_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.address_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.pid_table.entry_size    	= sizeof(arg_desc_t);
    command.break_table.entry_size  	= sizeof(arg_desc_t);
    command.rank_table.entry_size   	= sizeof(arg_desc_t);
    command.thread_table.entry_size 	= sizeof(arg_desc_t);
    command.file_table.entry_size   	= sizeof(arg_desc_t);
    command.experiment_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.name_table.entry_size	= sizeof(name_tab_t);	/*  */
    command.host_table.entry_size   	= sizeof(host_id_t);	/*  */
    command.help_table.entry_size   	= sizeof(help_desc_t);
    command.param_table.entry_size   	= sizeof(arg_desc_t);
    command.view_table.entry_size   	= sizeof(arg_desc_t);

}


