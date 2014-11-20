////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

#include <boost/format.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "ToolAPI.hxx"
#include "CUDAQueries.hxx"

using namespace boost;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



template <typename T>
string sizeToString(const T& value)
{
    const struct { const double value; const char* label; } kUnits[] = {
        { 1024.0 * 1024.0 * 1024.0 * 1024.0, "TB" },
        {          1024.0 * 1024.0 * 1024.0, "GB" },
        {                   1024.0 * 1024.0, "MB" },
        {                            1024.0, "KB" },
        {                               0.0, NULL } // End-Of-Table
    };
    
    double x = static_cast<double>(value);
    string label = "Bytes";
    
    for (int i = 0; kUnits[i].label != NULL; ++i)
    {
        if (static_cast<double>(value) >= kUnits[i].value)
        {
            x = static_cast<double>(value) / kUnits[i].value;
            label = kUnits[i].label;
            break;
        }
    }
    
    return str(
        (x == floor(x)) ?
        (format("%1% %2%") % static_cast<int>(x) % label) :
        (format("%1$0.1f %2%") % x % label)
        );
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << endl 
             << "Usage: " << argv[0] << " <experiment-database>" << endl 
             << endl;
        return 1;
    }

    try
    {
        Experiment experiment(argv[1]);

        Collector collector = *experiment.getCollectors().begin();
        if (collector.getMetadata().getUniqueId() != "cuda")
        {
            throw runtime_error(
                "The specified experiment didn't use the CUDA collector."
                );
        }

        Thread thread = *experiment.getThreads().begin();

        CUDAData cuda_data(collector, thread);

        CUDAExecXferBalance balance = GetCUDAExecXferBalance(cuda_data);
        
        cout << endl;
        cout << "CUDAExecXferBalance.exec_time = " 
             << balance.exec_time << " Seconds" << endl;
        cout << "CUDAExecXferBalance.xfer_time = " 
             << balance.xfer_time << " Seconds" << endl;
        cout << "                        Ratio = " 
             << (balance.exec_time / balance.xfer_time) << endl;

        CUDAXferRate rate = GetCUDAXferRate(cuda_data);

        cout << endl;
        cout << "CUDAXferRate.time = " << rate.time << " Seconds" << endl;
        cout << "CUDAXferRate.size = " << sizeToString(rate.size) << endl;
        cout << "Average Xfer Rate = " 
             << sizeToString(static_cast<double>(rate.size) / rate.time)
             << "/Second" << endl;
        
        cout << endl << endl;           
    }
    catch (const std::exception& error)
    {
        cerr << endl << "ERROR: " << error.what() << endl << endl;
        return 1;        
    }
    catch (const Exception& error)
    {
        cerr << endl << "ERROR: " << error.getDescription() << endl << endl;
        return 1;
    }
    
    return 0;
}
