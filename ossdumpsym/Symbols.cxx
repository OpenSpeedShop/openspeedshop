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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <set>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "EntrySpy.hxx"
#include "Database.hxx"
#include "SmartPtr.hxx"

#include "Symbols.hxx"

using namespace boost;
using namespace boost::algorithm;
using namespace OpenSpeedShop::Framework;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {
    
    /**
     * Convert the specified string, which is assumed to contain a hexadecimal
     * address, into an Address.
     *
     * @param input    String to be converted.
     * @return         Address conversion of that string.
     */
    Address address(const string& input)
    {
        stringstream stream;
        stream << hex << input;
        Address::value_type value;
        stream >> value;
        return Address(value);
    }
    
    /**
     * Execute the specified command and return the contents of its standard
     * output stream (stdout) as a list of strings.
     *
     * @param command    Command to be executed.
     * @return           Contents of that command's stdout.
     */
    vector<string> execute(const string& command)
    {
        vector<string> stdout;
        
        FILE* pipe = popen(command.c_str(), "r");
        
        if (pipe == NULL)
        {
            return stdout;
        }
        
        char* ptr = NULL;
        size_t length = 0;
        ssize_t read = 0;
        
        while ((read = getline(&ptr, &length, pipe)) != -1)
        {
            stdout.push_back(string(ptr));
        }
        
        free(ptr);
        pclose(pipe);
        
        return stdout;
    }

    /**
     * Disassemble the specified address range of the given linked object.
     *
     * @param linked_object    Path of the linked object to be disassembled.
     * @param range            Address range to be disassembled.
     * @return                 Disassembled instructions.
     *
     * @sa https://sourceware.org/binutils/docs-2.24/binutils/objdump.html
     */
    vector< pair<AddressRange, string> > disassemble(const Path& linked_object,
                                                     const AddressRange& range)
    {
        ostringstream command;
        command << "objdump --wide --disassemble --no-show-raw-insn"
                << " --start-address=" << range.getBegin()
                << " --stop-address=" << range.getEnd()
                << " " << linked_object;
        vector<string> stdout = execute(command.str());
        
        vector< pair<AddressRange, string> > disassembly;
        
        Address begin;
        string instruction;
        
        for (vector<string>::size_type i = 0; i < stdout.size(); ++i)
        {
            string::size_type at = stdout[i].find(":");
            
            if (!starts_with(stdout[i], "  ") || (at == string::npos))
            {
                continue;
            }
            
            Address current = address(stdout[i].substr(0, at));
            
            if (!instruction.empty())
            {
                disassembly.push_back(
                    make_pair(AddressRange(begin, current), instruction)
                    );
            }
            
            begin = current;
            instruction = trim_copy(stdout[i].substr(at + 1));
        }
        
        disassembly.push_back(
            make_pair(AddressRange(begin, range.getEnd()), instruction)
            );
        
        return disassembly;
    }
    
    /**
     * Get the virtual memory address (VMA) of the specified linked object's
     * text section. This address, obtained by executing the readelf tool, is
     * needed in order to convert between the linked object relative addresses
     * found in the Open|SpeedShop experiment database and the VMA addresses
     * used by the objdump tool.
     *
     * @param linked_object    Path of the linked object.
     * @return                 VMA of that linked object's text section.
     *
     * @sa https://sourceware.org/binutils/docs-2.24/binutils/readelf.html
     */
    Address vma(const Path& linked_object)
    {
        ostringstream command;
        command << "readelf --wide --segments " << linked_object;
        vector<string> stdout = execute(command.str());
        
        vector<Address> vma;
        
        for (vector<string>::size_type i = 0; i < stdout.size(); ++i)
        {
            if (trim_copy(stdout[i]) == "Program Headers:")
            {
                for (i += 2 /* Skip Title & Header */; i < stdout.size(); ++i)
                {
                    string line = trim_copy(stdout[i]);
                    
                    if (line.empty())
                    {
                        break;
                    }
                    
                    if (starts_with(line, "[Requesting program interpreter:"))
                    {
                        continue;
                    }
                    
                    // Type, Offset, VirtAddr, PhysAddr,
                    // FileSiz, MemSiz, Flg, Align
                    tokenizer<> parser(line);
                    vector<string> fields;
                    copy(parser.begin(), parser.end(), back_inserter<>(fields));
                    
                    vma.push_back(address(fields[2 /* VirtAddr */]));
                }
            }
            
            if (trim_copy(stdout[i]) == "Section to Segment mapping:")
            {
                for (i += 2 /* Skip Title & Header */; i < stdout.size(); ++i)
                {
                    string line = trim_copy(stdout[i]);
                    
                    if (!contains(line, ".text"))
                    {
                        continue;
                    }
                    
                    // Segment, Sections...
                    tokenizer<> parser(line);
                    vector<string> fields;
                    copy(parser.begin(), parser.end(), back_inserter<>(fields));
                    
                    return vma[lexical_cast<int>(fields[0 /* Segment */])];
                }
            }
        }
        
        return Address(); // Assume VMA=0 because parsing failed!
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Symbols::Symbols(const LinkedObject& linked_object) :
    dm_path(linked_object.getPath()),
    dm_vma(::vma(dm_path)),
    dm_functions(),
    dm_functions_index(),
    dm_loops(),
    dm_loops_index(),
    dm_statements(),
    dm_statements_index()
{
    SmartPtr<Database> database = EntrySpy(linked_object).getDatabase();
    int id = EntrySpy(linked_object).getEntry();
    
    BEGIN_TRANSACTION(database);

    int previous_rowid;

    database->prepareStatement(
        "SELECT Functions.id, "
        "       Functions.name, "
        "       FunctionRanges.addr_begin, "
        "       FunctionRanges.addr_end, "
        "       FunctionRanges.valid_bitmap "
        "FROM Functions "
        "JOIN FunctionRanges ON FunctionRanges.function = Functions.id "
        "WHERE Functions.linked_object = ? "
        "ORDER BY Functions.id;"
        );
    database->bindArgument(1, id);
    previous_rowid = -1;
    while (database->executeStatement())
    {
        int rowid = database->getResultAsInteger(1);
        string name = database->getResultAsString(2);
        
        if (rowid != previous_rowid)
        {
            dm_functions.push_back(FunctionItem(rowid, name));
            previous_rowid = rowid;
        }
        
        set<AddressRange> ranges =
            AddressBitmap(AddressRange(database->getResultAsAddress(3),
                                       database->getResultAsAddress(4)),
                          database->getResultAsBlob(5)).
            getContiguousRanges(true);
        
        for (set<AddressRange>::const_iterator
                 i = ranges.begin(); i != ranges.end(); ++i)
        {
            dm_functions_index.insert(
                AddressRangeIndexRow(dm_functions.size() - 1, rowid, 
                                     AddressRange(dm_vma + i->getBegin(),
                                                  dm_vma + i->getEnd()))
                );
        }
    }
    
    database->prepareStatement(
        "SELECT Loops.id, "
        "       Loops.addr_head, "
        "       LoopRanges.addr_begin, "
        "       LoopRanges.addr_end, "
        "       LoopRanges.valid_bitmap "
        "FROM Loops "
        "JOIN LoopRanges ON LoopRanges.loop = Loops.id "
        "WHERE Loops.linked_object = ? "
        "ORDER BY Loops.id;"
        );
    database->bindArgument(1, id);
    previous_rowid = -1;
    while (database->executeStatement())
    {
        int rowid = database->getResultAsInteger(1);
        Address head = dm_vma + Address(database->getResultAsAddress(2));
        
        if (rowid != previous_rowid)
        {
            dm_loops.push_back(LoopItem(rowid, head));
            previous_rowid = rowid;
        }
        
        set<AddressRange> ranges =
            AddressBitmap(AddressRange(database->getResultAsAddress(3),
                                       database->getResultAsAddress(4)),
                          database->getResultAsBlob(5)).
            getContiguousRanges(true);
        
        for (set<AddressRange>::const_iterator
                 i = ranges.begin(); i != ranges.end(); ++i)
        {
            dm_loops_index.insert(
                AddressRangeIndexRow(dm_loops.size() - 1, rowid,
                                     AddressRange(dm_vma + i->getBegin(),
                                                  dm_vma + i->getEnd()))
                );
        }
    }

    database->prepareStatement(
        "SELECT Statements.id, "
        "       Files.path, "
        "       Statements.line, "
        "       Statements.column, "
        "       StatementRanges.addr_begin, "
        "       StatementRanges.addr_end, "
        "       StatementRanges.valid_bitmap "
        "FROM Statements "
        "JOIN Files ON Files.id = Statements.file "
        "JOIN StatementRanges ON StatementRanges.statement = Statements.id "
        "WHERE Statements.linked_object = ? "
        "ORDER BY Statements.id;"
        );
    database->bindArgument(1, id);
    previous_rowid = -1;
    while (database->executeStatement())
    {
        int rowid = database->getResultAsInteger(1);
        Path path = Path(database->getResultAsString(2));
        int line = database->getResultAsInteger(3);
        int column = database->getResultAsInteger(4);
        
        if (rowid != previous_rowid)
        {
            dm_statements.push_back(StatementItem(rowid, path, line, column));
            previous_rowid = rowid;
        }
        
        set<AddressRange> ranges =
            AddressBitmap(AddressRange(database->getResultAsAddress(5),
                                       database->getResultAsAddress(6)),
                          database->getResultAsBlob(7)).
            getContiguousRanges(true);
        
        for (set<AddressRange>::const_iterator
                 i = ranges.begin(); i != ranges.end(); ++i)
        {
            dm_statements_index.insert(
                AddressRangeIndexRow(dm_statements.size() - 1, rowid,
                                     AddressRange(dm_vma + i->getBegin(),
                                                  dm_vma + i->getEnd()))
                );
        }
    }

    END_TRANSACTION(database);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vector< pair<AddressRange, string> > Symbols::disassemble(
    const AddressRange& range
    ) const
{
    return ::disassemble(dm_path, range);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
set<int> Symbols::functions(const AddressRange& range) const
{
    return rowids(range, dm_functions_index);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Symbols::inFunction(const AddressRange& range, const int& rowid) const
{
    return in(range, dm_functions_index, rowid);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Symbols::inLoop(const AddressRange& range, const int& rowid) const
{
    return in(range, dm_loops_index, rowid);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Symbols::inStatement(const AddressRange& range, const int& rowid) const
{
    return in(range, dm_statements_index, rowid);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Symbols::isLoopHead(const OpenSpeedShop::Framework::Address& address,
                         const int& rowid) const
{
    AddressRangeIndex::index<ByRowID>::type::const_iterator i =
        dm_loops_index.get<ByRowID>().lower_bound(rowid);
    
    AddressRangeIndex::index<ByRowID>::type::const_iterator iEnd =
        dm_loops_index.get<ByRowID>().upper_bound(rowid);
    
    for (; i != iEnd; ++i)
    {
        if (dm_loops[i->dm_index].dm_head == address)
        {
            return true;
        }
    }
    
    return false;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
set<int> Symbols::loops(const AddressRange& range) const
{
    return rowids(range, dm_loops_index);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
set<AddressRange> Symbols::ranges(const Function& function) const
{
    set<AddressRange> result;

    int rowid = EntrySpy(function).getEntry();

    AddressRangeIndex::index<ByRowID>::type::const_iterator i =
        dm_functions_index.get<ByRowID>().lower_bound(rowid);
    
    AddressRangeIndex::index<ByRowID>::type::const_iterator iEnd =
        dm_functions_index.get<ByRowID>().upper_bound(rowid);
    
    for (; i != iEnd; ++i)
    {
        result.insert(i->dm_range);
    }
    
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string Symbols::source(const int& rowid)
{
    AddressRangeIndex::index<ByRowID>::type::iterator i =
        dm_statements_index.get<ByRowID>().lower_bound(rowid);
    
    AddressRangeIndex::index<ByRowID>::type::iterator iEnd =
        dm_statements_index.get<ByRowID>().upper_bound(rowid);
    
    for (; i != iEnd; ++i)
    {
        StatementItem& statement = dm_statements[i->dm_index];

        if (statement.dm_source.empty())
        {
            if (statement.dm_file.doesExist())
            {
                vector<string> lines;
                
                ifstream stream(statement.dm_file.c_str());
                for (string line; getline(stream, line);)
                {
                    lines.push_back(line);
                }
                
                for (vector<StatementItem>::iterator
                         j = dm_statements.begin(); 
                     j != dm_statements.end();
                     ++j)
                {
                    if (j->dm_file == statement.dm_file)
                    {
                        if (j->dm_line <= lines.size())
                        {
                            j->dm_source = lines[j->dm_line - 1];
                        }
                        else
                        {
                            j->dm_source = str(format("<%1%, %2%>") %
                                               j->dm_file.getBaseName() %
                                               j->dm_line);
                        }
                    }
                } // j
            }
            else /* !statement.dm_file.doesExist() */
            {
                for (vector<StatementItem>::iterator
                         j = dm_statements.begin(); 
                     j != dm_statements.end();
                     ++j)
                {
                    if (j->dm_file == statement.dm_file)
                    {
                        j->dm_source = str(format("<%1%, %2%>") %
                                           j->dm_file.getBaseName() %
                                           j->dm_line);
                    }
                } // j
            }
        }

        return statement.dm_source;
    }

    return "";
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
set<int> Symbols::statements(const AddressRange& range) const
{
    return rowids(range, dm_statements_index);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
set<int> Symbols::rowids(const AddressRange& range,
                         const AddressRangeIndex& index)
{
    set<int> result;
    
    AddressRangeIndex::index<ByAddressRange>::type::const_iterator i =
        index.get<ByAddressRange>().lower_bound(range.getBegin());
    
    if (i != index.get<ByAddressRange>().begin())
    {
        --i;
    }
    
    AddressRangeIndex::index<ByAddressRange>::type::const_iterator iEnd =
        index.get<ByAddressRange>().upper_bound(range.getEnd());
    
    for (; i != iEnd; ++i)
    {
        if (i->dm_range.doesIntersect(range))
        {
            result.insert(i->dm_rowid);
        }
    }
    
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool Symbols::in(const OpenSpeedShop::Framework::AddressRange& range,
                 const AddressRangeIndex& index,
                 const int& rowid)
{
    AddressRangeIndex::index<ByRowID>::type::const_iterator i =
        index.get<ByRowID>().lower_bound(rowid);
    
    AddressRangeIndex::index<ByRowID>::type::const_iterator iEnd =
        index.get<ByRowID>().upper_bound(rowid);
    
    for (; i != iEnd; ++i)
    {
        if (i->dm_range.doesContain(range))
        {
            return true;
        }
    }
        
    return false;
}
