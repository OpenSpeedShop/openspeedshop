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

#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/noncopyable.hpp>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "Address.hxx"
#include "AddressRange.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"

/**
 * Symbols for a single linked object. Provides in-memory caching of symbols
 * obtained from an Open|SpeedShop experiment database using data structures
 * similar to those in the CBTF symbol table library.
 */
class Symbols :
    private boost::noncopyable
{
    
public:

    /**
     * Construct a Symbols for the specified linked object.
     *
     * @param linked_object    Linked object for which to construct a Symbols.
     */
    Symbols(const OpenSpeedShop::Framework::LinkedObject& linked_object);
    
    /**
     * Disassemble the specified address range.
     *
     * @param range    Address range to be disassembled.
     * @return         Disassembled instructions.
     */
    std::vector<std::pair<OpenSpeedShop::Framework::AddressRange, std::string> >
    disassemble(const OpenSpeedShop::Framework::AddressRange& range) const;
    
    /**
     * Get the row identifiers (from the original Open|SpeedShop experiment
     * database) of the functions that intersect the specified address range.
     *
     * @param range    Address range to intersect.
     * @return         Identifiers of the functions intersecting that range.
     */
    std::set<int> functions(
        const OpenSpeedShop::Framework::AddressRange& range
        ) const;

    /**
     * Does the specified function completely contain the given address range?
     *
     * @param range    Address range to query.
     * @param rowid    Row identifier (from the original Open|SpeedShop
     *                 experiment database) of the function to query.
     * @return         Boolean "true" if the specified function completely
     *                 contains the given address range, or "false" otherwise.
     */
    bool inFunction(const OpenSpeedShop::Framework::AddressRange& range,
                    const int& rowid) const;
    
    /**
     * Does the specified loop completely contain the given address range?
     *
     * @param range    Address range to query.
     * @param rowid    Row identifier (from the original Open|SpeedShop
     *                 experiment database) of the loop to query.
     * @return         Boolean "true" if the specified loop completely
     *                 contains the given address range, or "false" otherwise.
     */
    bool inLoop(const OpenSpeedShop::Framework::AddressRange& range,
                const int& rowid) const;

    /**
     * Does the specified statement completely contain the given address range?
     *
     * @param range   Address range to query.
     * @param rowid   Row identifier (from the original Open|SpeedShop
     *                experiment database) of the statement to query.
     * @return        Boolean "true" if the specified statement completely
     *                contains the given address range, or "false" otherwise.
     */
    bool inStatement(const OpenSpeedShop::Framework::AddressRange& range,
                     const int& rowid) const;

    /**
     * Is the specified address the head of the given loop?
     *
     * @param address    Address range to query.
     * @param rowid      Row identifier (from the original Open|SpeedShop
     *                   experiment database) of the loop to query.
     * @return           Boolean "true" if the specified address is the
     *                   head of the given loop, or "false" otherwise.
     */    
    bool isLoopHead(const OpenSpeedShop::Framework::Address& address,
                    const int& rowid) const;
    
    /**
     * Get the row identifiers (from the original Open|SpeedShop experiment
     * database) of the loops that intersect the specified address range.
     *
     * @param range    Address range to intersect.
     * @return         Identifiers of the loops intersecting that range.
     */
    std::set<int> loops(
        const OpenSpeedShop::Framework::AddressRange& range
        ) const;
    
    /**
     * Get the address ranges of the specified function.
     *
     * @param function    Subject of the query.
     * @return            Address ranges of that function.
     */
    std::set<OpenSpeedShop::Framework::AddressRange> ranges(
        const OpenSpeedShop::Framework::Function& function
        ) const;

    /**
     * Get the text of the source line for the specified statement.
     *
     * @param rowid   Row identifier (from the original Open|SpeedShop
     *                experiment database) of the statement to query.
     * @return        Text of the source line corresponding to this statement.
     */
    std::string source(const int& rowid);
    
    /**
     * Get the row identifiers (from the original Open|SpeedShop experiment
     * database) of the statements that intersect the specified address range.
     *
     * @param range    Address range to intersect.
     * @return         Identifiers of the statements intersecting that range.
     */
    std::set<int> statements(
        const OpenSpeedShop::Framework::AddressRange& range
        ) const;
    
private:

    /** Structure representing one row in an address range index. */
    struct AddressRangeIndexRow
    {
        /** Index for an entity (function, loop, statement). */
        int dm_index;
        
        /** Row identifier for that entity within the Open|SS database. */
        int dm_rowid;
        
        /** Address range for that entity. */
        OpenSpeedShop::Framework::AddressRange dm_range;
        
        /** Constructor from initial fields. */
        AddressRangeIndexRow(
            const int& index, const int& rowid,
            const OpenSpeedShop::Framework::AddressRange& range
            ) :
            dm_index(index),
            dm_rowid(rowid),
            dm_range(range)
        {
        }
        
        /** Key extractor for the address range's beginning. */
        struct range_begin
        {
            typedef OpenSpeedShop::Framework::Address result_type;
            
            result_type operator()(const AddressRangeIndexRow& row) const
            {
                return row.dm_range.getBegin();
            }
        };
        
    }; // struct AddressRangeIndexRow

    /** Tag for the row identifier index. */
    struct ByRowID { };

    /** Tag for the address range index. */
    struct ByAddressRange { };
    
    /**
     * Type of associative container used to search for entities (functions,
     * loops, statements) by their unique identifiers or address ranges.
     */
    typedef boost::multi_index_container<
        AddressRangeIndexRow,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<ByRowID>,
                boost::multi_index::member<
                    AddressRangeIndexRow,
                    int,
                    &AddressRangeIndexRow::dm_rowid
                    >
                >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<ByAddressRange>,
                AddressRangeIndexRow::range_begin
                >
            >
    > AddressRangeIndex;
 
    /** Structure representing one function. */
    struct FunctionItem
    {
        /** Row identifier for this function within the Open|SS database. */
        int dm_rowid;
        
        /** Demangled name of this function. */
        std::string dm_name;
        
        /** Constructor from initial fields. */
        FunctionItem(const int& rowid, const std::string& name) :
            dm_rowid(rowid),
            dm_name(name)
        {
        }
        
    }; // struct FunctionItem
        
    /** Structure representing one loop. */
    struct LoopItem
    {
        /** Row identifier for this loop within the Open|SS database. */
        int dm_rowid;
        
        /** Head address of this loop. */
        OpenSpeedShop::Framework::Address dm_head;
        
        /** Constructor from initial fields. */
        LoopItem(const int& rowid,
                 const OpenSpeedShop::Framework::Address& head) :
            dm_rowid(rowid),
            dm_head(head)
        {
        }
        
    }; // struct LoopItem
    
    /** Structure representing one statement. */
    struct StatementItem
    {
        /** Row identifier for this statement within the Open|SS database. */
        int dm_rowid;
        
        /** Path of this statement's source file. */
        OpenSpeedShop::Framework::Path dm_file;
        
        /** Line number of this statement. */
        int dm_line;
        
        /** Column number of this statement. */
        int dm_column;

        /** Text of the source line corresponding to this statement. */
        std::string dm_source;
        
        /** Constructor from initial fields. */
        StatementItem(const int& rowid,
                      const OpenSpeedShop::Framework::Path& file,
                      const int& line,
                      const int& column) :
            dm_rowid(rowid),
            dm_file(file),
            dm_line(line),
            dm_column(column),
            dm_source(/* Initially empty - filled on demand */)
        {
        }
        
    }; // struct StatementItem

    /** Get the row identifiers intersecting the specified address range. */
    static std::set<int> rowids(
        const OpenSpeedShop::Framework::AddressRange& range,
        const AddressRangeIndex& index
        );

    /** Does the specified row identifier contain the given address range? */
    static bool in(const OpenSpeedShop::Framework::AddressRange& range,
                   const AddressRangeIndex& index,
                   const int& rowid);
    
    /** Path of the linked object. */
    OpenSpeedShop::Framework::Path dm_path;
    
    /** Virtual memory address (VMA) of the linked object's text section. */
    OpenSpeedShop::Framework::Address dm_vma;
    
    /** List of functions. */
    std::vector<FunctionItem> dm_functions;
    
    /** Index used to find functions by addresses. */
    AddressRangeIndex dm_functions_index;
    
    /** List of loops. */
    std::vector<LoopItem> dm_loops;
    
    /** Index used to find functions by addresses. */
    AddressRangeIndex dm_loops_index;
    
    /** List of statements in this symbol table. */
    std::vector<StatementItem> dm_statements;
    
    /** Index used to find statements by addresses. */
    AddressRangeIndex dm_statements_index;
    
}; // class Symbols
