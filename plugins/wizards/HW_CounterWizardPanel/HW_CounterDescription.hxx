////////////////////////////////////////////////////////////////////////////////// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under// the terms of the GNU Lesser General Public License as published by the Free
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


#define vHW_CounterDescription  "There are two hardware counter experiments that may be run from this wizard.  They are hardware counter event monitoring (HWC) and hardware counter event monitoring with sampling time gathering (HWCTIME).  In these two Open|Speedshop hardware counter experiments, overflows of a particular hardware counter are recorded. Each hardware counter is configured to count from zero to a number designated as the overflow value. When the counter reaches the overflow value, the system resets it to zero and increments the number of overflows at the present program instruction address. Each experiment provides two possible overflow values; the values are prime numbers, so any profiles that seem the same for both overflow values should be statistically valid.\n\nThe experiments described in this section are available for systems that have hardware counters class machines.  Hardware counters allow you to count various types of events, such as cache misses and counts of issued and graduated instructions.\n\nA hardware counter works as follows: for each event, the appropriate hardware counter is incremented on the processor clock cycle. For example, when a floating-point instruction is graduated in a cycle, the graduated floating-point instruction counter is incremented by 1.\n\nThese experiments are detailed by nature. They return information gathered at the hardware level. You probably want to run a higher level experiment first.  Once you have narrowed the scope, you can use hardware counter experiments to pinpoint the area to be tuned."
#define eHW_CounterDescription  "Records particular hardware counter(s)."
