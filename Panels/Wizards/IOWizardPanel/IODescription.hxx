////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#define vIODescription "The I/O trace experiment shows you the level of I/O activity in your program by tracing various I/O system calls, for example read(2) and write(2).\n\n\nThe prof output of an I/O trace experiment yields the following information:\n\n*  The number of I/O system calls executed.\n\n* The number of calls with an incomplete traceback.\n\n* The [index] column assigns a reference number to each function.\n\n* The number of I/O-related system calls from each function in the program.\n\n* The percentage of I/O-related system calls from each function in the program.\n\n* The percentage of I/O-related system calls encountered so far in the list of functions.\n\n* The number of I/O-related system calls made by a given function and by all the functions ultimately called by that given function. For example, the main function will probably include all of the program's I/O calls with complete tracebacks.\n\n* The percentage of I/O-related system calls made by a given function and by all the functions ultimately called by that given function.\n\n* The DSO, file name, and line number for each function."

#define eIODescription "Shows the level of I/O activity in your program by tracing I/O system calls."
