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


#define vUserTimeDescription "The usertime experiment is a versatile experiment to start the performance analysis of your application. The usertime experiment returns CPU time for each function while your program runs.  It also gives call tree information.\n\nThis experiment uses statistical call stack profiling to measure inclusive and exclusive user time. A call stack sample is taken periodically based on the sample rate input parameter.  The program's call stack data is used to do the following:\n\n* Attribute exclusive user time to the function at the bottom of each call stack (that is, the function being executed at the time of the sample).\n\n* Attribute inclusive user time to all the functions above the one currently being executed (those involved in the chain of calls that led to the function at the bottom of the call stack executing).\n\nThe time spent in a function/procedure is determined by multiplying the number of times an instruction for that procedure appears in the stack by the sampling time interval between call stack samples. Call stacks are gathered when the program is running; hence, the time computed represents user time, not time spent when the program is waiting for a CPU. User time shows both the time the program itself is executing and the time the operating system is performing services for the program, such as I/O.\n\nThe usertime experiment should incur a program execution slow down of no more than 15%.  Performance data obtained from the usertime experiment is statistical in nature and shows some variance from run to run."

#define eUserTimeDescription "Returns CPU time for each function while your program runs."
