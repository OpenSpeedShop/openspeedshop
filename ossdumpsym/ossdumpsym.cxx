////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Krell Institute. All Rights Reserved.
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

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/ref.hpp>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ToolAPI.hxx"

#include "Symbols.hxx"

using namespace boost;
using namespace boost::algorithm;
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
    bool needSemicolon = false;

    stringstream colorized;
    colorized << "\e[";

    if (foreground != None)
    {
        colorized << (29 + static_cast<int>(foreground));
        needSemicolon = true;
    }
    
    if (background != None)
    {
        colorized << (needSemicolon ? ";" : "")
                  << (39 + static_cast<int>(background));
        needSemicolon = true;
    }

    if (attributes & Bold)
    {
        colorized << (needSemicolon ? ";" : "") << "1";
        needSemicolon = true;
    }

    if (attributes & Underline)
    {
        colorized << (needSemicolon ? ";" : "") << "4";
        needSemicolon = true;
    }

    if (attributes & Blink)
    {
        colorized << (needSemicolon ? ";" : "") << "5";
        needSemicolon = true;
    }

    colorized << "m" << text << "\e[m";
    return colorized.str();
}



/**
 * Colorize the specified function row identifier.
 *
 * @param range      Address range of the instruction.
 * @param symbols    Symbols containing the function to be colorized.
 * @param rowid      Row identifier (from the original Open|SpeedShop
 *                   experiment database) of the function to colorize.
 * @return           String containing the colorized row identifier.
 */
string colorizerForFunctions(const AddressRange& range,
                             const Symbols& symbols, const int& rowid)
{
    stringstream text;
    text << rowid;
    
    if (!symbols.inFunction(range, rowid))
    {
        return colorize(text.str(), Red);
    }
    
    return text.str();
}



/**
 * Colorize the specified loop row identifier.
 *
 * @param range      Address range of the instruction.
 * @param symbols    Symbols containing the loop to be colorized.
 * @param rowid      Row identifier (from the original Open|SpeedShop
 *                   experiment database) of the loop to colorize.
 * @return           String containing the colorized row identifier.
 */
string colorizerForLoops(const AddressRange& range,
                         const Symbols& symbols, const int& rowid)
{
    stringstream text;
    text << rowid;

    if (!symbols.inLoop(range, rowid))
    {
        return colorize(text.str(), Red);
    }
    else if (symbols.isLoopHead(range.getBegin(), rowid))
    {
        return colorize(text.str(), None, None, Underline);
    }
    
    return text.str();
}



/**
 * Colorize the specified statement row identifier.
 *
 * @param range      Address range of the instruction.
 * @param symbols    Symbols containing the statement to be colorized.
 * @param rowid      Row identifier (from the original Open|SpeedShop
 *                   experiment database) of the statement to colorize.
 * @return           String containing the colorized row identifier.
 */
string colorizerForStatements(const AddressRange& range,
                              const Symbols& symbols, const int& rowid)
{
    stringstream text;
    text << rowid;
    
    if (!symbols.inStatement(range, rowid))
    {
        return colorize(text.str(), Red);
    }
    
    return text.str();
}



/**
 * Convert the specified set of row identifiers into colorized strings.
 *
 * @param rowid        Row identifiers (from the original Open|SpeedShop
 *                     experiment database) to convert.
 * @param colorizer    Function for colorizing the row identifiers.
 * @param width        Maximum width of each string.
 * @return             List of colorized strings.
 */
vector<string> toStrings(const set<int>& rowids,
                         const boost::function<string (const int&)>& colorizer,
                         const int& width)
{
    vector<string> strings;
    
    string raw, colorized;
    for (set<int>::const_iterator i = rowids.begin(); i != rowids.end(); ++i)
    {
        stringstream r, c;
        r << *i << ((i != --rowids.end()) ? "," : "");
        c << colorizer(*i) << ((i != --rowids.end()) ? "," : "");
        
        if ((raw.size() + r.str().size()) > width)
        {
            colorized.append(width - raw.size(), ' ');
            strings.push_back(colorized);
            raw.clear();
            colorized.clear();
        }
        
        raw += r.str();
        colorized += c.str();
    }
    
    if (!colorized.empty())
    {
        colorized.append(width - raw.size(), ' ');
        strings.push_back(colorized);
    }

    if (strings.empty())
    {
        colorized = colorize("?", Yellow);
        colorized.append(width - 1, ' ');
        strings.push_back(colorized);
    }
    
    return strings;
}
                         


/**
 * Dump the specified function.
 *
 * @param symbols     Symbols containing the function to be dumped.
 * @param function    Function to be dumped.
 *
 * @note    The Symbols object is passed in as a parameter in order to improve
 *          performance in the common case where a large number of functions
 *          from a single linked object are being dumped.
 */
void dump(Symbols& symbols, const Function& function)
{
    const int kRowIDsWidth = 10;
    const int kAddressRangeWidth = 19;
    const int kInstructionWidth = 40;

    cout << endl 
         << colorize(function.getDemangledName(), None, None, Bold)
         << " from " 
         << colorize(function.getLinkedObject().getPath(), None, None, Bold)
         << ":" << endl;
    
    set<AddressRange> ranges = symbols.ranges(function);
    
    for (set<AddressRange>::const_iterator
             i = ranges.begin(); i != ranges.end(); ++i)
    {
        vector< pair<AddressRange, string> > disassembly = 
            symbols.disassemble(*i);

        cout << endl << setiosflags(ios::left)
             << setw(kRowIDsWidth) << "Func(s)" << " "
             << setw(kRowIDsWidth) << "Stmt(s)" << " "
             << setw(kRowIDsWidth) << "Loop(s)" << "  "
             << setw(kAddressRangeWidth) << "Address Range" << "  "
             << setw(kInstructionWidth) << "Instruction"
             << endl << setfill('-')
             << setw(6 /* Spaces */ + (3 * kRowIDsWidth) +
                     kAddressRangeWidth + kInstructionWidth) << ""
             << setfill(' ') << endl;

        set<int> seen;
        
        for (vector< pair<AddressRange, string> >::const_iterator
                 j = disassembly.begin(); j != disassembly.end(); ++j)
        {
            set<int> functions = symbols.functions(j->first);
            set<int> loops = symbols.loops(j->first);
            set<int> statements = symbols.statements(j->first);

            // Unseen Statements
            for (set<int>::const_iterator
                     s = statements.begin(); s != statements.end(); ++s)
            {
                if (seen.find(*s) == seen.end())
                {
                    seen.insert(*s);

                    stringstream rowid;
                    rowid << *s;
                    
                    cout << setw(kRowIDsWidth) << "" << " "
                         << colorize(rowid.str(), Cyan, None, Bold) 
                         << setw(kRowIDsWidth - rowid.str().size()) << ""
                         << " "
                         << setw(kRowIDsWidth) << "" << "  "
                         << colorize(symbols.source(*s), Cyan, None, Bold) 
                         << endl;
                }
            }

            // Disassembled Instruction

            vector< vector<string> > rowids;
            
            rowids.push_back(toStrings(
                                 functions,
                                 bind(&colorizerForFunctions,
                                      boost::cref(j->first), boost::cref(symbols), _1),
                                 kRowIDsWidth
                                 ));
            
            rowids.push_back(toStrings(
                                 statements,
                                 bind(&colorizerForStatements,
                                      boost::cref(j->first), boost::cref(symbols), _1),
                                 kRowIDsWidth
                                 ));
            
            rowids.push_back(toStrings(
                                 loops,
                                 bind(&colorizerForLoops,
                                      boost::cref(j->first), boost::cref(symbols), _1),
                                 kRowIDsWidth
                                 ));

            int M = max(max<int>(1, rowids[0].size()),
                        max<int>(rowids[1].size(), rowids[2].size()));

            for (int m = 0; m < M; ++m)
            {
                // Functions, Statements, Loops
                for (int n = 0; n < rowids.size(); ++n)
                {
                    if (m < rowids[n].size())
                    {
                        cout << rowids[n][m] << " ";
                    }
                    else
                    {
                        cout << setw(kRowIDsWidth) << "" << " ";
                    }
                }
                cout << " ";
                
                if (m == 0)
                {
                    // Address Range
                    stringstream range;
                    range << "[" << str(format("%08X") %
                                        j->first.getBegin().getValue())
                          << "," << str(format("%08X") %
                                        j->first.getEnd().getValue())
                          << ")";
                    cout << setw(kAddressRangeWidth) << range.str() << "  ";
                    
                    // Disassembled Instruction
                    cout << setw(kInstructionWidth) << j->second;
                }
                
                cout << endl;
                
            } // m
        } // j
    } // i
}



/**
 * Return a boolean flag indiciating whether the specified input string matches
 * the given restrictions (if any).
 *
 * @param input           String to be tested.

 * @param restrictions    List of restriction strings.
 * @return                Boolean "true" if the specified input string matches
 *                        the given restrictions (if any), or "false" otherwise.
 */
bool matches(const string& input, const vector<string>& restrictions)
{
    using namespace boost::algorithm;
    
    if (restrictions.empty())
    {
        return true;
    }
    
    for (vector<string>::const_iterator
             i = restrictions.begin(); i != restrictions.end(); ++i)
    {
        if (contains(input, *i))
        {
            return true;
        }
    }
    
    return false;
}



/**
 * Parse the command-line arguments and dump the requested symbol information.
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
           << "database for functions," << endl
           << "restricting to those requested, and dumps each located "
           << "function to the standard" << endl
           << "output stream. The dump contains a disassembly of all "
           << "address ranges associated" << endl
           << "with the function. Function, statement, and loop row "
           << "identifiers (found within" << endl
           << "the database) for each disassembled instruction are " 
           << "shown." << endl
           << endl
           << "A lack of function, statement, or loop row identifiers "
           << "for a given instruction" << endl
           << "is indicated by a "
           << colorize("?", Yellow)
           << ". Function, statements, or loops whose address ranges "
           << "within" << endl
           << "the database do not completely contain a given "
           << "instruction's address range are" << endl
           << "highlighted in red (e.g. "
           << colorize("15", Red)
           << "). Instructions whose first address is also the head" << endl
           << "address of a given loop are indicated by underlining "
           << "that loop's row identifier" << endl
           << "(e.g. "
           << colorize("10", None, None, Underline)
           << "). The source code corresponding to the disassembled "
           << "instructions will" << endl
           << "also be interspersed when available, highlighted in cyan (e.g. "
           << colorize("a = b * c;", Cyan, None, Bold)
           << ")." << endl
           << endl
           << "Remember that this tool only shows the symbol information "
           << "actually contained in" << endl
           << "the specified database. In some cases Open|SpeedShop "
           << "pares down that information" << endl
           << "to those instructions for which performance data was "
           << "collected, leaving a large" << endl
           << "number of instructions for which there is no available "
           << "symbol information." << endl
           << endl;
    string kExtraHelp = stream.str();
    
    // Parse and validate the command-line arguments

    options_description kNonPositionalOptions("ossdumpsym options");
    kNonPositionalOptions.add_options()
        
        ("database", value<string>(),
         "Open|SpeedShop experiment database to be dumped. May also be "
         "specified as a positional argument.")
        
        ("function", value< vector<string> >(),
         "Restrict the dump to the function(s) containing this string. "
         "Multiple restrictions are joined rather than intersected.")
        
        ("linked-object", value< vector<string> >(),
         "Restrict the dump to linked object(s) containing this string. "
         "Multiple restrictions are joined rather than intersected.")
        
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
    
    // Iterate over the linked objects in the specified database
    
    Experiment experiment(values["database"].as<string>());
    
    const set<LinkedObject> linked_objects =
        experiment.getThreads().getLinkedObjects();
    
    for (set<LinkedObject>::const_iterator
             l = linked_objects.begin(); l != linked_objects.end(); ++l)
    {
        if ((values.count("linked-object") == 0) ||
            matches(l->getPath(),
                    values["linked-object"].as< vector<string> >()))
        {
            Symbols symbols(*l);
            
            // Iterate over the functions in this linked object
            
            const set<Function> functions = l->getFunctions();
            
            for (set<Function>::const_iterator
                     f = functions.begin(); f != functions.end(); ++f)
            {
                if ((values.count("function") == 0) ||
                    matches(f->getDemangledName(),
                            values["function"].as< vector<string> >()))
                {
                    // Dump this function
                    dump(symbols, *f);
                }
            } // f
        }
    } // l
    
    cout << endl;
    return 0;
}
