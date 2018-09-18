////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2018 The Krell Institute. All Rights Reserved.
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
/* These are coming from the Dyninst include directory */
#include "Symtab.h"
#include "LineInformation.h"
#include "Function.h"
#include <sstream>


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
SymtabAPISymbols::getSymbols(const std::set<Address>& addresses,
			     const LinkedObject& linkedobject,
			     SymbolTableMap& stm)
{
    std::string objname = linkedobject.getPath();
    AddressRange range_for_stm;
    AddressRange lrange;
    Framework::Address base_for_stm(0);

    //  NOTE: Important info on the addressranges follows.
    //  A linkedobject can be loaded in differing addressranges
    //  across processes.  So we cycle though the unique
    //  address ranges finding matching sample addresses and
    //  then resolving symbols. But, a symboltable itself is only
    //  associated with one address range.  So all functions
    //  and possibly statements need to be added to the symboltable
    //  using the base for the addressrange of the symboltable
    //  found.  Therefore we use range_for_stm and base_for_stm.
    bool found_symtab = false;
    std::set<AddressRange> lorange = linkedobject.getAddressRange();
    std::set<AddressRange>::iterator si;
    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	lrange = (*si);

        std::set<Address>::iterator begin = addresses.equal_range(lrange.getBegin()).first;
        std::set<Address>::iterator end = addresses.equal_range(lrange.getEnd()).second;
	int num_addresses_in_range = std::distance(begin,end);
// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "SymtabAPISymbols::getSymbols Processing linked object "
	    << objname << " range:" << lrange
	    << " addresses.size:" << addresses.size()
	    << " addresses in range:" << num_addresses_in_range
	    << " lorange.size:" << lorange.size()
	    << std::endl;
	}
#endif

	if (num_addresses_in_range == 0) {
	    continue;
	}

	if (!found_symtab || stm.find(*si) != stm.end()) {
	    range_for_stm = lrange;
	    found_symtab = true;
	} else {
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "SymtabAPISymbols::getSymbols Processing linked object "
	    << objname << " range:" << lrange
	    << " not in stm ranges."
	    << std::endl;
	    }
#endif
	}

    if (!found_symtab) {
	std::cerr << "SymtabAPISymbols::getSymbols NO STM " << objname << " with address range " << lrange
	<< std::endl;
    } else {
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "SymtabAPISymbols::getSymbols Resolving symbols for linked object "
	    << objname << " range:" << lrange
	    << std::endl;
	}
#endif
    }

    SymbolTable& st =  stm.find(range_for_stm)->second.first;

    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);

    if (err) {
	// do something with err
    }

    // The object may not have any symbols, so skip it.
    if (symtab == NULL) {
	std::cerr << "SymtabAPISymbols::getSymbols Symtab::openFile failed for " << objname
	<< std::endl;
	return;
    }

    Framework::Address image_offset(symtab->imageOffset());
    Framework::Address image_length(symtab->imageLength());
    AddressRange image_range(image_offset,image_offset+image_length);

    // base is used to normalize sampled addresses for symbol
    // lookups in symtabapi.
    Framework::Address base(0);
    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
	base = lrange.getBegin();
    }

    if ( (image_range.getBegin() - range_for_stm.getBegin()) < 0 ) {
	base_for_stm = range_for_stm.getBegin();
    }

    // Make sure we get the full filename
    if(symtab) {
	symtab->setTruncateLinePaths(false);
    }

    // set up interators to only iterate over addresses that
    // are within the range of the shared object we are processing.
    // Do this out side of the loop on all symtabpai function symbols.
    // The same will be done later on symtabpai statements. 
    std::set<Address>::iterator ai;
    std::set<Address>::iterator ai_begin = addresses.equal_range(lrange.getBegin()).first;
    std::set<Address>::iterator ai_end = addresses.equal_range(lrange.getEnd()).second;

// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
        std::cerr << "SymtabAPISymbols::getSymbols image offset:" << image_offset
	<< " length:" << image_length
	<< " range:" << image_range
	<< " base:" << base
	<< " base_for_stm:" << base_for_stm
	<< " st.dm_range:" << st.getAddressRange()
	<< std::endl;
    }
#endif

    std::set<Framework::Address> function_begin_addresses;
    std::vector <SymtabAPI::Function *>fsyms;
    std::vector<SymtabAPI::Function *>::iterator fsit;
    bool ret = symtab->getAllFunctions(fsyms);
    if (!ret) {
    }

    // The iterator here is on addresses from the passed buffer.
    int count = 0;
    for (ai=ai_begin; ai!=ai_end; ++ai) {
	count++;
	SymtabAPI::Function* sample_function = 0;
	std::vector<Dyninst::SymtabAPI::Statement*> sample_stmts;

	// Adjust for symtabapi lookups by subtracting base.
	Offset offset = (*ai).getValue() - base.getValue();
	bool have_func = symtab->getContainingFunction(offset, sample_function);
	bool have_sample_stmts = symtab->getSourceLines(sample_stmts, offset);
	std::string fname = "unresolved function";

	// There may be an issue in symtabapi. With __nanosleep_nocancel from libc
	// the offset used to find the function does not exit with in the
	// range of the function returned by symtabapi getContainingFunction.
	if (have_func) {
	    fname = sample_function->getFirstSymbol()->getMangledName();
	    int sym_size = sample_function->getFirstSymbol()->getSize();
	    Framework::Address begin(sample_function->getFirstSymbol()->getOffset());
	    Framework::Address end(begin + sym_size);

	    // getContainingFunction can return a function with a sym_size of 0
	    // which results in a begin and end address being the same. In this case
	    // we will add 1.
	    if (begin == end) {
		end+=1;
	    } else if (begin > end) {
		std::cerr << "SymtabAPISymbols::getSymbols invalid range for " << fname
		<< " " << begin << ":" << end << std::endl;
		continue;
	    }

	    AddressRange frange(begin,end);
	    if (!frange.doesContain(offset)) {
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "SymtabAPISymbols::getSymbols"
		    << " sample:" << (*ai)
		    << " offset:" << Address(offset)
		    << " for " << fname
		    << " not in range:" << frange << " examining getAllFunctions."
		    << std::endl;
		}
#endif
		// The address lookup from getContainingFunction did not
		// return a Function symbol with a range that contained
		// the target address.  Need to look at the syms from getallFunctions.
		for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
		    // don't process an invalid range...
		    if ((*fsit)->getOffset() >=
			((*fsit)->getOffset() + (*fsit)->getSize())) {
			continue;
		    }
		    AddressRange fsymrange((*fsit)->getOffset(),
					   (*fsit)->getOffset() + (*fsit)->getSize());

		    if (fsymrange.doesContain( offset )) {
			fname = (*fsit)->getFirstSymbol()->getMangledName();
			begin = (*fsit)->getOffset();
			end = (*fsit)->getOffset() + (*fsit)->getSize();
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "SymtabAPISymbols::getSymbols fsym "
		    << " fsym name:" << fname
		    << " fsym offset:" << Address((*fsit)->getOffset())
		    << " fsym size:" << (*fsit)->getSize()
		    << std::endl;
		}
#endif
			//break;
		    }
		}
	    }

#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
		std::cerr << "SymtabAPISymbols::getSymbols function: "
		<< *ai << ":" << fname
		<< ":" << AddressRange(begin+base,end+base)
		<< std::endl;
	    }
#endif

	    st.addFunction(begin+base_for_stm,end+base_for_stm,fname);
	    // Record the function begin addresses, This allows the
	    // cli and gui to focus on or display the first
	    // statement of a function.
	    // The function begin addresses will be processed later
	    // for statement info and added to our statements.
	    function_begin_addresses.insert(begin+base_for_stm);

	    // CHECK FOR INLINES
	    // NOTE: the defining location for any function is the file
	    // and line where the function itself is defined for any
	    // address whether a sample or callstack frame.
	    // If the function is inlined and has parent function then
	    // we follow the parent chain using FunctionBase symbols
	    // to see the inlining performed. Essentially for the address
	    // in question there will be an inline chain of functions
	    // that represent the inlining. In this case we get a chain of:
	    // function_name:file:lineno which is an inining representation
	    // of this single address.  We are likely safe to record the
	    // the file into Files, and the inline function name, lineno
	    // into a separate Inline table of the OSS database.
	    // We need a new separate range table for the inline addresse of
	    // each inlined function to distinguish the inlining from
	    // the regular functions and statements so we do not interfere
	    // with existing OSS functionality and expectations.
	    FunctionBase *func, *parent;
	    bool has_inlines = false;
	    if (symtab->getContainingInlinedFunction(offset, func)) {
		parent = func->getInlinedParent();

#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled && parent) {
		    std::cerr << "SymtabAPISymbols::getSymbols: INLINES of "
		    << sample_function->getFirstSymbol()->getPrettyName()
		    << " at:" << *ai
		    << std::endl;
		}
#endif

		// Within this while loop we need to store for this sample offset
		// any inlines found as a chain into an InlineEntry which
		// is the inline function name,file,line and column of 0.
		// In addition, we store the address for that chain. It is
		// entirely possible that multiple addresses will point to
		// the same inline chain. So a set of addresses will potentially
		// be represented by the same vector of InlineEntry.
		// In a callstack based experiment, this inline chain would be
		// inserted before the symbol representing the existing addresses
		// in the callstack.  IE. Adding the inline chain as an additionl
		// set of frames before the frame containing the address of an inline.
		int inline_count = 0;
		while (parent != NULL) {
		    ++inline_count;
		    InlinedFunction *inline_func = static_cast <InlinedFunction *> (func);
		    std::pair <std::string, Offset> callsite = inline_func->getCallsite();
		    std::string inline_fname = func->getName();
		    if (inline_fname == "") {
			inline_fname = "UNKNOWN_INLINED_FUNCTION";
		    }
		    std::string filenm = callsite.first;
		    long lineno = callsite.second;

		    func = parent;
		    parent = func->getInlinedParent();
		    has_inlines = true;

		    // At this point we have a function name, file, and line number
		    // which are added to a new database table Inlines.
		    // Given that some inline entries will have an offset size of 0
		    // we will use the sample address to create the address range.
#ifndef NDEBUG
		    if(is_debug_symtabapi_symbols_enabled) {
			std::cerr << "    INLINE " << inline_count << " " << inline_fname
			<< " sample:" << *ai
			<< " file:" << filenm
			<< " lineno:" << lineno
			<< std::endl;
		    }
#endif
		    st.addInlinedFunction(inline_fname,*ai+base, *ai+base+1, filenm, lineno, 0);
		}
	    } // inlines

	} else { // have_func
#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
		std::cerr << "SymtabAPISymbols::getSymbols"
		<< " sample:" << (*ai)
		<< " offset:" << Address(offset)
		<< " not found by getContainingFunction ... examining getAllFunctions."
		<< std::endl;
	    }
#endif
	    Address begin,end;
	    // The address lookup from getContainingFunction did not
	    // return a Function symbol the target address.
	    // Need to look at the syms from getallFunctions.
	    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
		// don't process an invalid range...
		if ((*fsit)->getOffset() >=
		    ((*fsit)->getOffset() + (*fsit)->getSize())) {
		    continue;
		}
		AddressRange fsymrange((*fsit)->getOffset(),
					(*fsit)->getOffset() + (*fsit)->getSize());

		if (fsymrange.doesContain( offset )) {
		    fname = (*fsit)->getFirstSymbol()->getMangledName();
		    begin = (*fsit)->getOffset();
		    end = (*fsit)->getOffset() + (*fsit)->getSize();
#ifndef NDEBUG
		    if(is_debug_symtabapi_symbols_enabled) {
		    std::cerr << "SymtabAPISymbols::getSymbols allFunctions 2 fsym "
		    << " fsym name:" << fname
		    << " fsym offset:" << Address((*fsit)->getOffset())
		    << " fsym size:" << (*fsit)->getSize()
		    << std::endl;
		    }
#endif
			//break;
		}
	    }
	}

	// Will do same for any function begin addresses.
	if (have_sample_stmts) {
	    Dyninst::SymtabAPI::Statement *real_stmt = NULL;
	    for (std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = sample_stmts.begin();
		 si != sample_stmts.end(); si++) {
		Offset real_stmt_size = 0;
		Offset stmt_size = (*si)->endAddr() - (*si)->startAddr();
		if (real_stmt_size == 0) {
		    // this sets real_stmt to first stmt from mylines.
		    // If there is only one statment in mylines, this is used.
		    real_stmt = *si;
		    real_stmt_size = stmt_size;
		} else if (stmt_size < real_stmt_size) {
		    // if more that one statement is in mylines, we use the
		    // statement with the smallest address range.
		    real_stmt = *si;
		    real_stmt_size = stmt_size;
		}
	    }

#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
		std::cerr << "SymtabAPISymbols::getSymbols sample statement: "
		<< *ai << ":" << real_stmt->getFile()
		<< ":" << real_stmt->getLine()
		<< ":" << real_stmt->getColumn()
		<< ":" << AddressRange(Framework::Address(real_stmt->startAddr()) +base,
				       Framework::Address(real_stmt->endAddr()) +base)
		<< std::endl;
	    }
#endif
	    // add statement for actual sample to our symboltable
	    // FIXME: Verify that base works here. We use base_for_stm with
	    // the addFunction call.
	    st.addStatement(Framework::Address(real_stmt->startAddr()) +base,
			    Framework::Address(real_stmt->endAddr()) +base,
			    real_stmt->getFile(),
			    real_stmt->getLine(),
			    (int) real_stmt->getColumn()
			   );
	} // have_sample_stmts
    } // unique addresses iterator loop.


    // Find any statements for the beginning of a function that
    // contained a valid sample address.
    for(std::set<Framework::Address>::const_iterator fi = function_begin_addresses.begin();
		                                     fi != function_begin_addresses.end();
			                             ++fi) {
	std::vector<Dyninst::SymtabAPI::Statement*> func_begin_stmts;
	bool have_func_begin_stmts = symtab->getSourceLines(func_begin_stmts, (*fi).getValue());
	if (have_func_begin_stmts) {
	    Dyninst::SymtabAPI::Statement *real_stmt = NULL;
	    for (std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = func_begin_stmts.begin();
		     si != func_begin_stmts.end(); si++) {
		Offset real_stmt_size = 0;
		Offset stmt_size = (*si)->endAddr() - (*si)->startAddr();
		if (real_stmt_size == 0) {
		    // this sets real_stmt to first stmt from mylines.
		    // If there is only one statment in mylines, this is used.
		    real_stmt = *si;
		    real_stmt_size = stmt_size;
		} else if (stmt_size < real_stmt_size) {
		    // if more that one statement is in mylines, we use the
		    // statement with the smallest address range.
		    real_stmt = *si;
		    real_stmt_size = stmt_size;
		}
	    }

#ifndef NDEBUG
	    if(is_debug_symtabapi_symbols_enabled) {
		std::cerr << "SymtabAPISymbols::getSymbols function begin statement: "
		<< real_stmt->getFile() << ":" << real_stmt->getLine()
		<< ":" << real_stmt->getColumn()
		<< ":" << AddressRange(Framework::Address(real_stmt->startAddr()) +base,
				       Framework::Address(real_stmt->endAddr()) +base)
		<< std::endl;
	    }
#endif

	    // statement for function begin
	    st.addStatement(Framework::Address(real_stmt->startAddr()) +base,
			    Framework::Address(real_stmt->endAddr()) +base,
			    real_stmt->getFile(),
			    real_stmt->getLine(),
			    (int) real_stmt->getColumn()
			   );
	} // end if have_func_begin_stmts
    } // end func begin addr loop
  } // end of lorange loop. 
}

// NOTE: This is deprecated an could be removed in the future.
// For now convert PCBuffer to std::set<Address> and simply call the
// current getSymbols that uses a std::set<Address>.
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
    getSymbols(addresses,linkedobject,stm);

}


void
SymtabAPISymbols::getDepenentLibs(const std::string& objname,
           std::vector<std::string>& dependencies)
{
    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);
    if (err) {
	// do something with err
    }

    if (symtab) {
        dependencies = symtab->getDependencies();
        symtab->setTruncateLinePaths(false);
    }
}


bool SymtabAPISymbols::foundLibrary(const std::string& exename, const std::string& libname)
{

    std::vector<std::string> depends;
    getDepenentLibs(exename,depends);
    for (std::vector<std::string>::iterator curDep = depends.begin(); curDep != depends.end(); curDep++) {
      if (curDep->find(libname) != std::string::npos) {
        return true;
      }
    }
   return false;
}
