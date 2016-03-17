////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014-2016 Argo Navis Technologies. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <KrellInstitute/Messages/CUDA_data.h>

#include <ArgoNavis/CUDA/stringify.hpp>

#include "ToolAPI.hxx"

using namespace boost;
using namespace OpenSpeedShop::Framework;
using namespace std;



/** Enumeration of the attributes supported by colorize(). */
enum Attributes { Normal = 0, Bold = 1, Underline = 2, Blink = 4 };

/** Enumeration of the colors supported by colorize(). */
enum Color { None, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };



/**
 * Colorize the specified text via ASCII escape sequences.
 *
 * @param text          String containing the text to be colorized.
 * @param foreground    Foreground color for this text.
 * @param background    Background color for this text.
 * @param attributes    Attributes (bold, underline, etc.) for this text.
 * @return              String containing the colorized text.
 */
string colorize(const string& text,
                const Color& foreground = None,
                const Color& background = None,
                const Attributes& attributes = Normal)
{
    bool semicolon = false;

    stringstream colorized;
    colorized << "\e[";

    if (foreground != None)
    {
        colorized << (29 + static_cast<int>(foreground));
        semicolon = true;
    }
    
    if (background != None)
    {
        colorized << (semicolon ? ";" : "")
                  << (39 + static_cast<int>(background));
        semicolon = true;
    }

    if (attributes & Bold)
    {
        colorized << (semicolon ? ";" : "") << "1";
        semicolon = true;
    }

    if (attributes & Underline)
    {
        colorized << (semicolon ? ";" : "") << "4";
        semicolon = true;
    }

    if (attributes & Blink)
    {
        colorized << (semicolon ? ";" : "") << "5";
        semicolon = true;
    }

    colorized << "m" << text << "\e[m";
    return colorized.str();
}



/**
 * Parse the command-line arguments and dump the requested CUDA performance
 * data blobs.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Command-line arguments.
 * @return        Exit code. Either 1 if a failure occurred, or 0 otherwise.
 */
int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    stringstream stream;
    stream << endl
           << "This tool scans the specified Open|SpeedShop experiment "
           << "database for CUDA " << endl
           << "performance data blobs and dumps their contents to the "
           << "standard output stream." << endl
           << endl;
    string kExtraHelp = stream.str();

    // Parse and validate the command-line arguments
    
    options_description kNonPositionalOptions("ossdumpcuda options");
    kNonPositionalOptions.add_options()
 
        ("blob", value< vector<int> >(),
         "Restrict the dump to the CUDA performance data blob with this "
         "id. Multiple blobs may be specified.")
        
        ("database", value<string>(),
         "Open|SpeedShop experiment database to be dumped. May also be "
         "specified as a positional argument.")
        
        ("help", "Display this help message.")
        
        ;
    
    positional_options_description kPositionalOptions;
    kPositionalOptions.add("database", 1);
    
    variables_map values;
    
    try
    {
        store(command_line_parser(argc, argv).options(kNonPositionalOptions).
              positional(kPositionalOptions).run(), values);       
        notify(values);
    }
    catch (const std::exception& error)
    {
        cout << endl << "ERROR: " << error.what() << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (values.count("help") > 0)
    {
        cout << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (values.count("database") == 0)
    {
        cout << endl << "ERROR: database must be specified" << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }
    
    if (!Experiment::isAccessible(values["database"].as<string>()))
    {
        cout << endl << "ERROR: " << values["database"].as<string>()
             << " isn't a database" << endl 
             << endl << kNonPositionalOptions << kExtraHelp;
        return 1;
    }

    set<int> blobs;
    if (values.count("blob") > 0)
    {
        vector<int> temp = values["blob"].as< vector<int> >();
        blobs = set<int>(temp.begin(), temp.end());
    }

    // Iterate over the CUDA performance data blobs in the specified database

    SmartPtr<Database> database(new Database(values["database"].as<string>()));
    
    BEGIN_TRANSACTION(database);

    database->prepareStatement(
        "SELECT Data.id, "
        "       Data.collector, "
        "       Data.thread, "
        "       Data.time_begin, "
        "       Data.time_end, "
        "       Data.addr_begin, "
        "       Data.addr_end, "
        "       Data.data "
        "FROM Data "
        "  JOIN Collectors "
        "ON Data.collector = Collectors.id "
        "WHERE Collectors.unique_id = \"cuda\";"
        );

    while (database->executeStatement())
    {
        int id = database->getResultAsInteger(1);

        if (!blobs.empty() && (blobs.find(id) == blobs.end()))
        {
            continue;
        }

        int collector = database->getResultAsInteger(2);
        int thread = database->getResultAsInteger(3);

        TimeInterval interval(database->getResultAsTime(4),
                              database->getResultAsTime(5));
        
        AddressRange range(database->getResultAsAddress(6),
                           database->getResultAsAddress(7));

        Blob blob = database->getResultAsBlob(8);

        CBTF_cuda_data data;
        memset(&data, 0, sizeof(data));
        blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &data
            );

        cout << endl << endl
             << colorize(
                 str(format("CUDA Performance Data Blob ID %1% from %2%") %
                     id % database->getName()),
                 None, Green, Bold) << endl
             << colorize(str(format("Collector ID : %1%") % collector),
                         None, Green, Bold) << endl
             << colorize(str(format("Thread ID    : %1%") % thread),
                         None, Green, Bold) << endl
             << colorize(str(format("Time Interval: %1%") % interval),
                         None, Green, Bold) << endl
             << colorize(str(format("Address Range: %1%") % range),
                         None, Green, Bold) << endl
             << ArgoNavis::CUDA::stringify(data);
        
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
                 reinterpret_cast<char*>(&data));        
    }

    END_TRANSACTION(database);

    cout << endl << endl;
    return 0;
}
