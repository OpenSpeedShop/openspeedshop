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


#define vpcSampleDescription "The Program Counter (PC) Sample experiment estimates the actual CPU time for each function, source code line, and machine code line in your program. The performance results report listing for this experiment shows exclusive program counter sample time. This experiment is lightweight and high-speed and therefore has a low impact on the application being monitored.\n\nCPU time is calculated by multiplying the number of times an instruction representing a function or source code line address appears in the program counter by the interval specified for the experiment.\n\nTo collect the data, the process is periodically stopped and a counter corresponding to the current value of the program counter is incremented, and then the executable is resumed.\n\nProgram counter sample experiment runs should slow the execution time of the program down no more than 5 percent. The measurements are statistical in nature, meaning they exhibit variance inversely proportional to the running time."


#define epcSampleDescription "Estimates the actual CPU time for each function, source code line and machine code line in your program."
