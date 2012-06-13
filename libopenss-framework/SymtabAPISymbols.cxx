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
#include "dyninst/Function.h"


using namespace OpenSpeedShop::Framework;
using namespace Dyninst;
using namespace Dyninst::SymtabAPI;

#ifndef NDEBUG
/** Flag indicating if debuging for offline symbols is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_enabled =
    (getenv("OPENSS_DEBUG_SYMTABAPI_SYMBOLS") != NULL);
/** Flag indicating if debuging for detailed offline symbols is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_detailed_enabled =
    (getenv("OPENSS_DEBUG_SYMTABAPI_SYMBOLS_DETAILS") != NULL);
#endif

void
SymtabAPISymbols::getSymbols(PCBuffer* addrbuf,
			     const LinkedObject& linkedobject,
			     SymbolTableMap& stm)
{
    std::set<Address> addresses;
    std::set<Address>::iterator ai;
    for (unsigned ii = 0; ii < addrbuf->length; ++ii) {
	addresses.insert(Address(addrbuf->pc[ii]));
    }

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


	    Framework::Address image_offset(symtab->imageOffset());
	    Framework::Address image_length(symtab->imageLength());
	    AddressRange image_range(image_offset,image_offset+image_length);

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
	        std::cerr << "Image range of " << objname << " is " << image_range << std::endl;
	    }
#endif

	    Framework::Address base(0);
	    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
		base = lrange.getBegin();
	    }

	    std::vector <SymtabAPI::Function *>fsyms;

	    if(symtab && !symtab->getAllFunctions(fsyms)) {
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "Dyninst::SymtabAPI::Symbol::getAllFunctions unable to get functions\n`"
		    << "from " << objname << " range: " << image_range << "\n"
		    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
		    << std::endl;
		}
#endif

	    }


	    std::set<Framework::Address> function_begin_addresses;
	    std::vector<SymtabAPI::Function *>::iterator fsit;

	    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
		int sym_size = (*fsit)->getSize();
		Framework::Address begin((*fsit)->getOffset());
		Framework::Address end(begin + sym_size);

		// don't let an invalid range through...
		if (begin >= end) continue;

		AddressRange frange(begin,end);

		for (ai=addresses.equal_range(lrange.getBegin()).first;
	     		ai!=addresses.equal_range(lrange.getEnd()).second;ai++) {
		    Framework::Address theAddr(*ai - base.getValue()) ; 
		    if (frange.doesContain( theAddr )) {

			std::string out_name =
				(*fsit)->getFirstSymbol()->getPrettyName();
// DEBUG
#ifndef NDEBUG
			if(is_debug_symtabapi_symbols_detailed_enabled) {
		            std::cerr << "ADDING FUNCTION " << out_name
			    << " RANGE " << frange
			    << " for pc " << *ai
			    << " adjusted pc " << theAddr
			    << std::endl;
			}
#endif
			st.addFunction(begin+base,end+base,out_name);

			// Record the function begin addresses, This allows the
			// cli and gui to focus on or display the first
			// statement of a function.
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
	        if(is_debug_symtabapi_symbols_detailed_enabled) {
		    for(unsigned i = 0; i< mods.size();i++){
		        module_range =
			   AddressRange(mods[i]->addr(),
					mods[i]->addr() + symtab->imageLength());
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
			Framework::Address b(range.first);
			Framework::Address e(range.second);

			AddressRange srange(b,e);
			for (ai=addresses.equal_range(lrange.getBegin()).first;
	     		    ai!=addresses.equal_range(lrange.getEnd()).second;ai++) {
			    // Lookup address by subtracting base offset.
		    	    Framework::Address theAddr(*ai - base.getValue()) ; 
			    if(srange.doesContain(theAddr)) {
// DEBUG
#ifndef NDEBUG
				if(is_debug_symtabapi_symbols_detailed_enabled) {
				    std::cerr << "ADDING STATEMENT "
				    << b << ":" << e
				    <<" " << line.first << ":" << line.second
				    << std::endl;
				}
#endif
				// add the base offset back when recording statement.
				st.addStatement(b+base,e+base,line.first,line.second,line.column);
			    }

			}

			// Find any statements for the beginning of a function that
			// contained a valid sample address.
			for(std::set<Framework::Address>::const_iterator fi = function_begin_addresses.begin();
			                                      fi != function_begin_addresses.end();
			                                      ++fi) {
			    if(srange.doesContain(*fi) ) {
// DEBUG
#ifndef NDEBUG
				if(is_debug_symtabapi_symbols_detailed_enabled) {
				    std::cerr << "ADDING Function BEGIN STATEMENT "
				    << b << ":" << e
				    <<" " << line.first << ":" << line.second
				    << std::endl;
				}
#endif
				// add the base offset back when recording statement.
				st.addStatement(b+base,e+base,line.first,line.second,line.column);
			    }
			}
		    }
		}
	    }
	}
	// only look at first linkedobject since we normalize addresses
	// before lookup in symbols from symtabapi...
	break;
    }
}
