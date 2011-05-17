//
// Copyright (c) 2011 Krell Institute, Inc. All Rights Reserved.
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


#define vHW_CounterSampDescription  "There is one hardware counter sampling experiment that may be run from this wizard.  The hardware counter sampling event monitoring (HWCSAMP) experiment is enabled by using this wizard.  In this Open|Speedshop hardware counter experiment, a timer is used to periodically interrupt the application and read the hardware counter values of the selected hardware counter events and record them.\n\nThe experiment described in this section are available for systems that have hardware counters class machines.  Hardware counters allow you to count various types of events, such as cache misses and counts of issued and graduated instructions.\n\nA hardware counter works as follows: for each event, the appropriate hardware counter is incremented on the processor clock cycle. For example, when a floating-point instruction is graduated in a cycle, the graduated floating-point instruction counter is incremented by 1.\n\nThese experiments are detailed by nature. They return information gathered at the hardware level. You may want to run a higher level experiment first.  Once you have narrowed the scope, you can use hardware counter experiments to pinpoint the area to be tuned."
#define eHW_CounterSampDescription  "Uses timer based sampling to record the counts of selected hardware counter(s)."
