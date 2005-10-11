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


#define vpcSampleDescription "The pc Sample experiment estimates the actual CPU time for each source code line, machine code line, and function in your program. The report listing of this experiment shows exclusive pc Sample time. This experiment is a lightweight, high-speed operation that makes use of the operating system.\n\nCPU time is calculated by multiplying the number of times an instruction or function appears in the PC by the interval specified for the experiment.\n\nTo collect the data, the operating system regularly stops the process, increments a counter corresponding to the current value of the PC, and resumes the process.\n\npc Sample runs should slow the execution time of the program down no more than 5 percent. The measurements are statistical in nature, meaning they exhibit variance inversely proportional to the running time."


#define epcSampleDescription "Estimates the actual CPU time for each source code line, machine code line, and function in your program."
