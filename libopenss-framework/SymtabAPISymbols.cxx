////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2012 The Krell Institute. All Rights Reserved.
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
 * Declaration of the SymtabAPISymbols class.
 *
 */

#include "SymtabAPISymbols.hxx"
#include "Address.hxx"
#include "AddressRange.hxx"
#include "LinkedObject.hxx"
#include "Function.hxx"
#include "Statement.hxx"
#include "dyninst/Symtab.h"
#include "dyninst/LineInformation.h"

using namespace OpenSpeedShop::Framework;
using namespace Dyninst;
using namespace Dyninst::SymtabAPI;

#ifndef NDEBUG
/** Flag indicating if debuging for MPI jobs is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS") != NULL);
#endif

void
SymtabAPISymbols::getSymbols(PCBuffer* addrbuf,
			     const LinkedObject& linkedobject,
			     SymbolTableMap& stm)
{

    std::string objname = linkedobject.getPath();
    std::set<AddressRange> lorange = linkedobject.getAddressRange();
    std::set<AddressRange>::iterator si;

    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	AddressRange lrange;

// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "Processing linked object "
	    << objname << " with address range " << (*si)
	    << " addrbuf->length is " << addrbuf->length
	    << std::endl;
	}
#endif

	lrange = (*si);

	if (stm.find(*si) != stm.end()) {
	    SymbolTable& st =  stm.find(*si)->second.first;

	    Symtab *symtab;
	    bool err = Symtab::openFile(symtab, objname);

	    OpenSpeedShop::Framework::Address image_offset(symtab->imageOffset());
	    OpenSpeedShop::Framework::Address image_length(symtab->imageLength());
	    AddressRange image_range(image_offset,image_offset+image_length);

	    OpenSpeedShop::Framework::Address base(0);
	    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
		base = lrange.getBegin();
	    }

	    std::vector <Symbol *>fsyms;

	    if(symtab && !symtab->getAllSymbolsByType(fsyms,Symbol::ST_FUNCTION)) {
		std::cerr << "getAllSymbolsByType unable to get all Functions "
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
	    }


	    std::set<Address> function_begin_addresses;

	    for(unsigned i = 0; i< fsyms.size();i++){
		OpenSpeedShop::Framework::Address begin(fsyms[i]->getAddr());
		OpenSpeedShop::Framework::Address end(begin + fsyms[i]->getSize());
		begin = begin+base;
		end = end+base;
	     
		if (begin >= end) continue;
		AddressRange frange(begin,end);

		for (unsigned ii = 0; ii < addrbuf->length; ++ii) {
		    if (frange.doesContain(Address(addrbuf->pc[ii]))) {
// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_enabled) {
		            std::cerr << "ADDING FUNCTION " << fsyms[i]->getName()
			    << " RANGE " << begin << "," << end << std::endl;
			}
#endif
			st.addFunction(begin, end,fsyms[i]->getName());

			// Record the function begin addresses, This allows the cli and gui
			// to focus on or display the first statement of a function.
			// The function begin addresses will be processed later
			// for statement info and added to our statements.
			function_begin_addresses.insert(begin);

			break;
		    }
		}

	    }

	    std::vector <Module *>mods;
	    AddressRange module_range;
	    std::string module_name;
	    if(symtab && !symtab->getAllModules(mods)) {
		std::cerr << "getAllModules unable to get all modules  "
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
	    } else {
// DEBUG
#ifndef NDEBUG
	        if(is_debug_symtabapi_symbols_enabled) {
		    for(unsigned i = 0; i< mods.size();i++){
		        module_range =
			   AddressRange(mods[i]->addr(), mods[i]->addr() + symtab->imageLength());
		        module_name = mods[i]->fullName();
		        std::cerr << "getAllModules module name " << mods[i]->fullName()
			    << " Range " << module_range << std::endl;
		    }
		}
#endif
	    }

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {

	        std::cerr << "symtabAPISymbols: image_offset " << image_offset
		<< " image_length " << image_length
		<< " image_range " << image_range << std::endl;

		std::cerr << "USING BASE OFFSET " << base << std::endl;
	    }
#endif


	    for(unsigned i = 0; i< mods.size();i++) {
		LineInformation *lineInformation = mods[i]->getLineInformation();
		if(lineInformation) {
		    LineInformation::const_iterator iter = lineInformation->begin();
		    for(;iter!=lineInformation->end();iter++) {
			const std::pair<Offset, Offset> range = iter->first;
			LineNoTuple line = iter->second;
			OpenSpeedShop::Framework::Address b(range.first);
			OpenSpeedShop::Framework::Address e(range.second);

			AddressRange srange(b+base,e+base);
		        for (unsigned ii = 0; ii < addrbuf->length; ++ii) {

			    if(srange.doesContain(Address(addrbuf->pc[ii]))) {
// DEBUG
#ifndef NDEBUG
				if(is_debug_symtabapi_symbols_enabled) {
				    std::cerr << "ADDING STATEMENT "
				    << b << ":" << e
				    <<" " << line.first << ":" << line.second
				    << std::endl;
				}
#endif
				st.addStatement(b,e,line.first,line.second,line.column);
			    }

			}

			// Find any statements for the beginning of a function that
			// contained a valid sample address.
			for(std::set<Address>::const_iterator fi = function_begin_addresses.begin();
			                                      fi != function_begin_addresses.end();
			                                      ++fi) {
			    if(srange.doesContain(*fi) ) {
// DEBUG
#ifndef NDEBUG
				if(is_debug_symtabapi_symbols_enabled) {
				    std::cerr << "ADDING Function BEGIN STATEMENT "
				    << b << ":" << e
				    <<" " << line.first << ":" << line.second
				    << std::endl;
				}
#endif
				st.addStatement(b,e,line.first,line.second,line.column);
			    }
			}
		    }
		}
	    }
	    break;
	}
    }
}
