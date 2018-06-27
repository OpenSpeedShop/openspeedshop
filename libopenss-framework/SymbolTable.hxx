////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2013-2018 The Krell Institue. All Rights Reserved.
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

/** @file
 *
 * Declaration of the SymbolTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_SymbolTable_
#define _OpenSpeedShop_Framework_SymbolTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"

#include <map>
#include <set>
#include <string>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    class Address;
    class Database;
    class LinkedObject;
    class Path;

    /**
     * Symbol table.
     * 
     * The symbol table information for linked objects must be processed a fair
     * amount before it is stored into the database: overlapping and redundant
     * entries must be removed; source statement information is scattered across
     * the linked object and must be condensed; and finally the formatting of
     * the information in the database is signficantly different from the form
     * in which it is typically found. This class encapsulates these various
     * processing steps.
     *
     * @ingroup Implementation
     */
    class SymbolTable
    {
	
    public:
	
	SymbolTable(const AddressRange&);
	
	void addFunction(const Address&, const Address&, const std::string&);
        void addLoop(const Address&, const Address&, const Address&);
	void addStatement(const Address&, const Address&,
			  const Path&, const int&, const int&);
	void addVectorInstr(const Address&, const int32_t&, const int32_t&, const std::string&);
	
	void processAndStore(const LinkedObject&);

	// Offline uses this while checking that all sampled
	// addresses have been resolved for function symbols.
	std::map<AddressRange, std::string> getFunctions() {
		return dm_functions;
	};

	void removeFunctions(std::map<AddressRange, std::string>& other)
	{
		//dm_functions = other;
	    for (std::map<AddressRange, std::string>::iterator i = other.begin();
                i != other.end(); ++i) {
		     std::map<AddressRange, std::string>::iterator it = dm_functions.find(i->first);
		     if (it == dm_functions.end()) {
#if 0
			std::cerr << "DID not find " << i->second
			    << "range: " << i->first << " in dm_functions" << std::endl;
#endif
		     } else {
#if 0
			std::cerr << "ERASE " << i->second
			    << "range: " << i->first << " in dm_functions" << std::endl;
#endif
		         dm_functions.erase(it);
		     }
	    }
	};
	
    private:

	static std::vector<std::set<Address> >
	partitionAddressSet(const std::set<Address>&);
	
	/** Address range occupied by this symbol table. */
	AddressRange dm_range;

	/** Functions in this symbol table. */
	std::map<AddressRange, std::string> dm_functions;
	
	/**
	 * Statement entry.
	 *
	 * Structure for a source statement's entry in the symbol table's
	 * internal tables. Contains the full path name, line number, and column
	 * number of the source statement.
	 */
	struct StatementEntry
	{
	    
	    Path dm_path;   /**< Path name of this statement's source file. */
	    int dm_line;    /**< Line number of this statement. */
	    int dm_column;  /**< Column number of this statement. */
	    
	    /** Constructor from fields. */
	    StatementEntry(const Path& path, 
			   const int& line, 
			   const int& column) :
		dm_path(path),
		dm_line(line),
		dm_column(column)
	    {
	    }

	    /** Operator "<" defined for two StatementEntry objects. */
	    bool operator<(const StatementEntry& other) const
	    {
		if(dm_path < other.dm_path)
		    return true;
		if(dm_path > other.dm_path)
		    return false;
		if(dm_line < other.dm_line)
		    return true;
		if(dm_line > other.dm_line)
		    return false;
		return dm_column < other.dm_column;		
	    }

	};

	/**
	 * Vector Instruction entry.
	 *
	 * Structure for a vector instruction entry in the symbol table's
	 * internal tables. Contains the instruction op-code, maximum 
         * physical instruction vector length, and the actual vector length
         * used (deferred implementation).
	 */
	struct VectorInstrEntry
	{
	    
	    std::string dm_instr_opcode; /**< This vector instructions opcode renderred for string viewing */
	    int dm_max_instr_vl;    /**< Maximum hardware instruction vector length value. */
	    int dm_actual_vl;       /**< Actual vector length for this instruction detected by O|SS */

	    /** Constructor from fields. */
	    VectorInstrEntry(const std::string& vopcode, 
			   const int& max_vinstr_vl, 
			   const int& actual_vinstr_vl) :
		dm_instr_opcode(vopcode),
		dm_max_instr_vl(max_vinstr_vl),
		dm_actual_vl(actual_vinstr_vl)
	    {
	    }
        };
	    

    /** Loops in this symbol table. */
    std::map<Address, std::vector<AddressRange> > dm_loops;
	
    /** Statements in this symbol table. */
    std::map<StatementEntry, std::vector<AddressRange> > dm_statements;

    /** Vector Instructions in this symbol table. */
    std::map<Address, VectorInstrEntry > dm_vectorInstrs;
	
    };

} }



#endif
