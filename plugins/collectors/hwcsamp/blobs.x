/*******************************************************************************
** Copyright (c) 2010 The Krell Institute. All Rights Reserved.
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

/** @file
 *
 * Specification of the HWC sampling collector's blobs.
 *
 */


/** Structure of the blob containing our parameters. */
struct hwcsamp_parameters {
    unsigned sampling_rate;  /**< Sampling rate in samples/second. */
    char hwcsamp_event [512];      /**< hwc event (PAPI event code) to sample*/
};


/** Structure of the blob containing our performance data. */
typedef uint64_t evcounts[6];

struct hwcsamp_event {
    uint64_t hwccounts[6];
};

struct hwcsamp_data {
    uint64_t interval;    /**< Sampling interval in nanoseconds. */
    uint64_t pc<>;        /**< Program counter (PC) addresses. */
    uint8_t count<>;      /**< Sample counts at those addresses. */    
    hwcsamp_event events<>;
    float  clock_mhz;
};


/** Structure of the blob containing pcsamp_start_sampling()'s arguments. */
struct hwcsamp_start_sampling_args {

    unsigned sampling_rate;  /**< Sampling rate in samples/second. */
    char hwcsamp_event [512];      /**< hwc event (PAPI event code) to sample*/
    
    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
    
};
