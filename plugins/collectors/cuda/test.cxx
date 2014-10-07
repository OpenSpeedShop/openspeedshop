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

#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include "ToolAPI.hxx"
#include "Queries.hxx"

using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;
using namespace std;



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
        
        SmartPtr<map<Function, map<Thread, double> > > individual;

        Queries::GetMetricValues(
            collector, "exec_time",
            TimeInterval(Time::TheBeginning(), Time::TheEnd()),
            experiment.getThreads(),
            experiment.getThreads().getFunctions(),
            individual
            );
        
        SmartPtr<map<Function, double> > data = Queries::Reduction::Apply(
            individual, Queries::Reduction::Summation
            );

        individual = SmartPtr<map<Function, map<Thread, double> > >();
        
        multimap<double, Function> sorted;
        for (map<Function, double>::const_iterator
                 i = data->begin(); i != data->end(); ++i)
            sorted.insert(make_pair(i->second, i->first));     
        
        cout << endl << endl
             << setw(10) << "Time" << "    " << "Function" << endl
             << endl;
        
        for (multimap<double, Function>::reverse_iterator
                 i = sorted.rbegin(); i != sorted.rend(); ++i)
        {        
            cout << setw(10) << fixed << setprecision(3)
                 << i->first << "    " << i->second.getName();
        
            set<Statement> definitions = i->second.getDefinitions();
            for (set<Statement>::const_iterator
                     i = definitions.begin(); i != definitions.end(); ++i)
                cout << " (" << i->getPath().getBaseName()
                     << ", " << i->getLine() << ")";
            
            cout << endl;
        }
    
        cout << endl << endl;           
    }
    catch (const exception& error)
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
