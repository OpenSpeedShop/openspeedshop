/*******************************************************************************
** Copyright (c) 2007,2008,2009 The Krell Institue. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Definition of the OpenSS BFD symbol support functions.
 * This code is based on examples (directly copied) from binutils
 * tools like objdump, addr2line, nm.  We use the bfd and
 * code examples from these tools to match addresses gathered
 * from offline collectors to symbol information.
 *
 */

//
// Note that on Ubuntu 14.04.2 systems, the inclusion of <libiberty.h> MUST
// be before the inclusion of <bfd.h>, or <bfd.h> will pick up the version of
// <ansidecl.h> found in /usr/include instead of /usr/include/libiberty.h, and
// the former is missing the declaration of ATTRIBUTE_RETURNS_NONNULL, which
// results in a compilation error. WDH 2015-06-17
//
#define HAVE_DECL_BASENAME 1
#include <libiberty.h>

#include "ToolAPI.hxx"
#include "BFDSymbols.hxx"
#include "Path.hxx"

#include <bfd.h>
#include "bfd.h"
#include <string.h>

#include <stdint.h>
#include <dis-asm.h>
#include <algorithm>


// bfd symbols for current linkedobject
static asymbol **syms;
static long numsyms;

// Sorted bfd symbols for current linkedobject
// Used to find function begin and end addresses.
static asymbol **sortedsyms;
static long numsortedsyms;

static int init_done = 0;

// statics used by find_address_in_section and bfd_find_nearest_line.
static bfd  *theBFD;
static bfd_vma pc;
static bfd_vma obj_base;
static bfd_boolean found;
static bool debug_symbols = false;

using namespace OpenSpeedShop::Framework;

static StatementsVec statementvec;
static FunctionsVec functionvec;

#ifndef NDEBUG
/** Flag indicating if debuging for offline symbols is enabled. */
bool BFDSymbols::is_debug_symbols_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE_SYMBOLS") != NULL);

/** Flag indicating if debuging for offline experiments is enabled. */
bool BFDSymbols::is_debug_offline_enabled =
    (getenv("OPENSS_DEBUG_OFFLINE") != NULL);
#endif

#if 0
/* binutils 2.23/2.24 use this definition */
/* 2.28 replaces ... with va_list, but there is no */
/* binutils version string to create a version specific */
/* routine, so commenting out for now */
static void bfdErrHandler (const char *s, ...)
{
    // empty.  TODO.
}
#endif

// lifted from objdump
void BFDSymbols::slurp_symtab (bfd *abfd)
{
  long symcount;
  unsigned int size;

  if ((bfd_get_file_flags (abfd) & HAS_SYMS) == 0)
    return;

  symcount = bfd_read_minisymbols (abfd, FALSE, (void **)&syms, &size);
  if (symcount == 0) {
    symcount = bfd_read_minisymbols (abfd, TRUE /* dynamic */,
					(void **) &syms, &size);
  }

  if (symcount < 0) {
    std::cerr << "slurp_symtab: Cound not get symbols for "
	<< bfd_get_filename (abfd) << std::endl;
  }
  numsyms = symcount;
}

// lifted from objdump
/* Sort symbols into value order. This function is passed to qsort */
static int
compare_symbols (const void *ap, const void *bp)
{
  const asymbol *a = * (const asymbol **) ap;
  const asymbol *b = * (const asymbol **) bp;
  const char *an;
  const char *bn;
  size_t anl;
  size_t bnl;
  bfd_boolean af;
  bfd_boolean bf;
  flagword aflags;
  flagword bflags;

  if (bfd_asymbol_value (a) > bfd_asymbol_value (b))
    return 1;
  else if (bfd_asymbol_value (a) < bfd_asymbol_value (b))
    return -1;

  if (a->section > b->section)
    return 1;
  else if (a->section < b->section)
    return -1;
  an = bfd_asymbol_name (a);
  bn = bfd_asymbol_name (b);
  anl = strlen (an);
  bnl = strlen (bn);

  /* The symbols gnu_compiled and gcc2_compiled convey no real
     information, so put them after other symbols with the same value.  */
  af = (strstr (an, "gnu_compiled") != NULL
	|| strstr (an, "gcc2_compiled") != NULL);
  bf = (strstr (bn, "gnu_compiled") != NULL
	|| strstr (bn, "gcc2_compiled") != NULL);

  if (af && ! bf)
    return 1;
  if (! af && bf)
    return -1;

  /* We use a heuristic for the file name, to try to sort it after
     more useful symbols.  It may not work on non Unix systems, but it
     doesn't really matter; the only difference is precisely which
     symbol names get printed.  */

#define file_symbol(s, sn, snl)			\
  (((s)->flags & BSF_FILE) != 0			\
   || ((sn)[(snl) - 2] == '.'			\
       && ((sn)[(snl) - 1] == 'o'		\
	   || (sn)[(snl) - 1] == 'a')))

  af = file_symbol (a, an, anl);
  bf = file_symbol (b, bn, bnl);

  if (af && ! bf)
    return 1;
  if (! af && bf)
    return -1;

  /* Try to sort global symbols before local symbols before function
     symbols before debugging symbols.  */

  aflags = a->flags;
  bflags = b->flags;

  if ((aflags & BSF_DEBUGGING) != (bflags & BSF_DEBUGGING))
    {
      if ((aflags & BSF_DEBUGGING) != 0)
	return 1;
      else
	return -1;
    }
  if ((aflags & BSF_FUNCTION) != (bflags & BSF_FUNCTION))
    {
      if ((aflags & BSF_FUNCTION) != 0)
	return -1;
      else
	return 1;
    }
  if ((aflags & BSF_LOCAL) != (bflags & BSF_LOCAL))
    {
      if ((aflags & BSF_LOCAL) != 0)
	return 1;
      else
	return -1;
    }
  if ((aflags & BSF_GLOBAL) != (bflags & BSF_GLOBAL))
    {
      if ((aflags & BSF_GLOBAL) != 0)
	return -1;
      else
	return 1;
    }

  /* Symbols that start with '.' might be section names, so sort them
     after symbols that don't start with '.'.  */
  if (an[0] == '.' && bn[0] != '.')
    return 1;
  if (an[0] != '.' && bn[0] == '.')
    return -1;

  /* Finally, if we can't distinguish them in any other way, try to
     get consistent results by sorting the symbols by name.  */
  return strcmp (an, bn);
}

// lifted from objdump
long BFDSymbols::remove_useless_symbols(asymbol **symbols, long count)
{
    asymbol **in_ptr = symbols, **out_ptr = symbols;

    while (--count >= 0) {
	 asymbol *sym = *in_ptr++;

	if (sym->name == NULL || sym->name[0] == '\0')
	    continue;
	if (sym->flags & (BSF_DEBUGGING))
	    continue;
	if (bfd_is_und_section (sym->section) ||
	    bfd_is_com_section (sym->section))
	    continue;

	*out_ptr++ = sym;
    }

    return out_ptr - symbols;
}

// Some of this code is lifted from objdump.
//
// Find functions symbols for this object.
// Restrict search to only those functions whose addresses
// are found within the range of sampled addresses for
// this object. i.e. prune addrbuf of addresses not within
// the passed address range of [obj_load_addr, obj_end_addr).
int
BFDSymbols::getFunctionSyms (PCBuffer *addrbuf,
			     bfd_vma obj_load_addr,
			     bfd_vma obj_end_addr)
{
    std::vector<uint64_t> addrvec;

    AddressRange range_in_this_obj(Address((uint64_t)obj_load_addr),
			Address((uint64_t)obj_end_addr));
#ifndef NDEBUG
            if(is_debug_symbols_enabled) {
	      std::cerr << "BFDSymbols::getFunctionSyms ENTERED"
                << " Address((uint64_t)obj_load_addr)= "
                << Address((uint64_t)obj_load_addr)
                << " Address((uint64_t)obj_end_addr)= "
                << Address((uint64_t)obj_end_addr)
		<< " to pc's found" << std::endl;
	    }
#endif

    for (unsigned ii = 0; ii < addrbuf->length; ii++) {
	if (range_in_this_obj.doesContain(static_cast<Address>(addrbuf->pc[ii]))) {
// DEBUG
#ifndef NDEBUG
            if(is_debug_symbols_enabled) {
	      std::cerr << "BFDSymbols::getFunctionSyms addrvec adds "
		<< Address(addrbuf->pc[ii])
		<< " to pc's found" << std::endl;
	    }
#endif
	    addrvec.push_back(addrbuf->pc[ii]);
	}
    }

    // No sampled addresses from this object so nothing to do. Just return 0.
    if (addrvec.size() == 0 ) {
	return 0;
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "BFDSymbols::getFunctionSyms addrvec found " << addrvec.size()
	<< " addresses in this object out of " << addrbuf->length << std::endl;
    }
#endif

    std::sort(addrvec.begin(), addrvec.end());

    /* We make a copy of syms to sort.  We don't want to sort syms
       because that will screw up any relocs.  */
    sortedsyms = (asymbol**)xmalloc (numsyms * sizeof (asymbol *));
    memcpy (sortedsyms, syms, numsyms * sizeof (asymbol *));

    numsortedsyms = remove_useless_symbols (sortedsyms, numsyms);

    /* Sort the symbols into section and symbol order.  */
    qsort (sortedsyms, numsortedsyms, sizeof (asymbol *), compare_symbols);

    // begining of text (entry).
    bfd_vma bfd_text_begin = bfd_get_start_address(theBFD);

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: Examining " << numsortedsyms
	<< " sortedsyms for functions with data"
	<< std::endl;

      std::cerr << "getFunctionSyms: start address for this bfd "
	<< Address(bfd_text_begin)
	<< ", load address for object is " << (Address)obj_load_addr
	<< std::endl;

      std::cerr << "getFunctionSyms: offset "
	<< (bfd_signed_vma ) ((bfd_signed_vma) bfd_text_begin - (bfd_signed_vma) obj_load_addr)
	<< std::endl;
    }
#endif

    // Compute offset object was loaded at.
    // Use load address from /proc/self/maps if offset is
    // less than 0.
    Address base = Address(0);
    if ( (Address(bfd_text_begin) - Address(obj_load_addr)) < 0 ) {
        base = Address(obj_load_addr);
    }

    obj_base = base.getValue();

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: using base address " << base << " for function offsets" << std::endl;
    }
#endif

    int foundpcs = 0;
    for (int i = 0; i < numsortedsyms; i++) {
	asymbol* sym = sortedsyms[i];
#ifndef NDEBUG
        if(is_debug_symbols_enabled) {
            std::cerr << "getFunctionSyms: before if sym->flags= " << sym->flags
                      << " BSF_FUNCTION= " << BSF_FUNCTION
                      << " (sym->flags & BSF_FUNCTION)= " << (sym->flags & BSF_FUNCTION)
                      << " sym->section= " << sym->section
                      << " !bfd_is_und_section(sym->section)= " << !bfd_is_und_section(sym->section)
      	              << std::endl;
          }
#endif
	if ((sym->flags & BSF_FUNCTION) && !bfd_is_und_section(sym->section)) {
	
	    bfd_size_type size = bfd_get_section_size(sym->section);
	    bfd_vma section_vma = bfd_get_section_vma (theBFD, sym->section);
	    bfd_vma begin_addr = bfd_asymbol_value(sym);
#ifndef NDEBUG
            if(is_debug_symbols_enabled) {
              std::cerr << "getFunctionSyms: inside if sym->flags, size=" << size
        	<< " section_vma=" << Address(section_vma) 
        	<< " begin_addr=" << Address(begin_addr)
        	<< std::endl;
            }
#endif

	    if (section_vma <= begin_addr && begin_addr > size + section_vma) {
#ifndef NDEBUG
                if(is_debug_symbols_enabled) {
                  std::cerr << "getFunctionSyms: BREAK in section_vma if test " 
	                    << std::endl;
                }
#endif
		break;
	    } 

	    Address real_begin(base + (uint64_t)begin_addr);
	    if (!range_in_this_obj.doesContain(real_begin)) {
#ifndef NDEBUG
                if(is_debug_symbols_enabled) {
                  std::cerr << "getFunctionSyms: CONTINUE in range_in_this_obj if test " 
	                    << " base =" << base
	                    << " begin_addr =" << Address(begin_addr)
	                    << " real_begin =" << real_begin
		            << " PC range_in_this_obj " << range_in_this_obj
	                    << std::endl;
                }
#endif
		continue;
	    }

	    // default to end address of section.  Will refine end_addr below.
	    bfd_vma end_addr = size + section_vma;

	    std::string symname = bfd_asymbol_name(sym);
// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
	      std::cerr << "getFunctionSyms: TESTING "
		<< symname << " at " << real_begin
		<< "," << Address(base + end_addr)
		<< " end_addr" << Address(end_addr)
		<< " in section " << Address(base + section_vma)
		<< " with size " <<  size
		<< " PC range_in_this_obj " << range_in_this_obj << std::endl;
	    }
#endif

	    // Now examine sortedsyms starting at next index for
	    // a valid function with text. Since sortedsyms is sorted
	    // on increasing addresses, the begin address of the next
	    // function will be used to compute the end address of
	    // the current function.
	    int next = i + 1;
	    bool notfound = true;
	    while (notfound && next < numsortedsyms) {
	      asymbol* nextsym = sortedsyms[next];
	      if ((nextsym->flags & BSF_FUNCTION) &&
		!bfd_is_und_section(nextsym->section)) {

		notfound = false;
		bfd_size_type nextsize = bfd_get_section_size(nextsym->section);
		bfd_vma nextsection_vma =
				bfd_get_section_vma (theBFD, nextsym->section);
		bfd_vma nextbegin_addr = bfd_asymbol_value(nextsym);
		bfd_vma next_end_addr =  bfd_asymbol_value(nextsym);

// DEBUG
#ifndef NDEBUG
	        if(is_debug_symbols_enabled) {
		    std::cerr << "getFunctionSyms: next symbol"
			<< " is " << bfd_asymbol_name(nextsym)
			<< " at " << Address(nextbegin_addr)
			<< ", " << Address(next_end_addr)
			<< std::endl;
		}
#endif

		if ( next+1 <= numsortedsyms &&
		    next_end_addr == begin_addr &&
		    (sym->flags & BSF_GLOBAL &&
		     sym->flags & BSF_EXPORT &&
		     !(sym->flags & BSF_WEAK)
		    ) ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " addr is == current symbol BSF_GLOBAL BSF_EXPORT "
			<< Address(next_end_addr)
			<< " current name " << bfd_asymbol_name(sym)
			<< std::endl;
		    }
#endif
	            asymbol*  nnsym = sortedsyms[next+1];
		    if ((nnsym->flags & BSF_FUNCTION) &&
			!bfd_is_und_section(nnsym->section)) {
			bfd_vma nn_addr = bfd_asymbol_value(nnsym);
			end_addr = nn_addr;
		    }
		}

		if (next_end_addr == begin_addr &&
		    !(next_end_addr < end_addr ) ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " addr is == current symbol "
			<< Address(next_end_addr)
			<< " current name " << bfd_asymbol_name(sym)
			<< std::endl;
		    }
#endif

		}

		if (next_end_addr < end_addr ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		       std::cerr << "getFunctionSyms: Next symbol is in this "
			 << "section. Use section next_end_addr "
			 << Address(next_end_addr) << std::endl;
		    }
#endif
		     end_addr = next_end_addr;
		} else {
// DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: "
			<< "INFO: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " at " << Address(next_end_addr)
			<< " is not in this"
			<< " section. Use section end_addr "
			<< Address(end_addr) << std::endl;
		    }
#endif
		}
		     
		if (nextsection_vma <= nextbegin_addr &&
		    nextbegin_addr > nextsize + nextsection_vma) {
		    std::cerr << "getFunctionSyms: WARNING: next sym name "
			<< bfd_asymbol_name(nextsym)
			<< " is OUTOFRANGE." << std::endl;
		}
	      } else {
// DEBUG
#ifndef NDEBUG
		if(is_debug_symbols_enabled) {
		  std::cerr << "getFunctionSyms: "
		    << bfd_asymbol_name(nextsym)
		    << " NOT nextsym->flags & BSF_FUNCTION && !undefined section"
		    << std::endl;
		}
#endif
		// skip these non-function entries.
		next++;
	      }
	    } //end while

	    if (section_vma <= end_addr &&  end_addr > size + section_vma) {
		std::cerr << "getFunctionSyms: WARNING SKIPPED SECTION "
		   << " symname " << symname
		   << " section_vma " << Address(section_vma)
		   << " section size " << Address(size)
		   << " begin_addr " << Address(begin_addr)
		   << " end_addr " << Address(end_addr)
		   << std::endl;
		continue;
	    }


	    // This is not quite exact but will work for our puposes.
	    // On x86 and x86_64, instructions a variable length and will be
	    // creater than one.  We know that sortedsyms is sorted on addresses
	    // and that the next symbol address is the begin address of the
	    // next function.  We really only need a range for the current
	    // function so we can just subtract 1 from the address of the next
	    // function and use that as the end address of the current function.
	    // VERIFY: On ia64, an instruction bundle is length 0x10.
	    // The last function's end address should be the end address of the section.
	    // The most accurate way to do this is to disassemble the function for x86, x86_64.
	    int ins_size = 1;

	    uint64_t f_end = end_addr;

	    // skip weak symbols
	    if (begin_addr >= f_end) {
// DEBUG
#ifndef NDEBUG
		if(is_debug_symbols_enabled) {
		    if (sym->flags & BSF_WEAK) {
		        std::cerr << "Skipping weak symbol " << symname
			    << " begin " << static_cast<Address>(begin_addr)
			    << " end " << static_cast<Address>(f_end)
			    << std::endl;
		    }
		    if (sym->flags & BSF_GLOBAL) {
		        std::cerr << symname << " is BSF_GLOBAL" << std::endl;
		    }
		    if (sym->flags & BSF_EXPORT) {
		        std::cerr << symname << " is BSF_EXPORT" << std::endl;
		    }
		    if (sym->flags & BSF_WARNING) {
		        std::cerr << symname << " is BSF_WARNING" << std::endl;
		    }
		    if (sym->flags & BSF_OBJECT) {
		        std::cerr << symname << " is BSF_OBJECT" << std::endl;
		    }
		    if (sym->flags & BSF_DYNAMIC) {
		        std::cerr << symname << " is BSF_DYNAMIC" << std::endl;
		    }
		}
#endif
		continue;
	    }

	    // The following loop adds any function found to to the functionvec.
	    // We restrict this to functions who are found to
	    // contain an address for the performance PC address buffer.
	    AddressRange frange(real_begin,
				Address(base + f_end));

#if !defined(USE_ALL_BFD_SYMBOLS)
	    for (unsigned int ii = 0; ii < addrvec.size(); ii++) {
		// To improve performance of this search,
		// see if one of the found functions already
		// has this address so we do not search remaining
		// functions in this dso for an address that
		// already has it's function found. i.e. as
		// each address in addrvec has it's enclosing function
		// resolved, remove it from addrvec. If and when
		// addrvec is empty we can terminate the search for
		// functions to add to our symboltable.

		Address cur_addr(addrvec[ii]);

		bool func_already_found = false;
	        for(FunctionsVec::iterator f = functionvec.begin();
					   f != functionvec.end(); ++f) {

        	    AddressRange range(f->getFuncBegin(), f->getFuncEnd());

#if 0
        	    if (f->getFuncName() == symname) {
			std::cerr << "getFunctionSyms: Address"
			<< cur_addr << " at addrvec[" << ii << "] "
			<< " function already in vec " << f->getFuncName()
			<< " in range " << range
			<< " this frange " << frange
			<< std::endl;
		    }
#endif

        	    if (range.doesContain(cur_addr)) {
// DEBUG
#ifndef NDEBUG
		        if(is_debug_symbols_enabled) {
			  std::cerr << "getFunctionSyms: Address "
			    << cur_addr
			    << " at addrvec[" << ii << "] "
			    << "already found in function "
			    << f->getFuncName() << " " << range << std::endl;
			}
#endif

			foundpcs++;
			// do not search for this address again.
			std::vector<uint64_t>::iterator toremove = 
					std::find(addrvec.begin(),
						  addrvec.end(),
						  addrvec[ii]);
			func_already_found = true;
			ii--;
			addrvec.erase(toremove);
			break;
		    }
		}

		if (func_already_found) {
		    continue;
		}

		if (frange.doesContain(cur_addr)) {
// DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
		        std::cerr << "getFunctionSyms: functionvec PUSH BACK "
			    << symname << " at " << frange
			    << " for pc " << cur_addr
			    << " ii " << ii << " of " << addrvec.size()
			    << std::endl;
		    }
#endif
		    functionvec.push_back( BFDFunction(symname,
							  frange.getBegin().getValue(),
							  frange.getEnd().getValue())
						);
		    foundpcs++;
		    // do not search for this address again.
		    std::vector<uint64_t>::iterator toremove = 
			std::find(addrvec.begin(), addrvec.end(), addrvec[ii]);
		    addrvec.erase(toremove);
		    ii--;
		    break;
		} else {
		}
	    } // end for addrvec
#else
	    functionvec.push_back( BFDFunction(symname,
					       frange.getBegin().getValue(),
					       frange.getEnd().getValue()) );
#endif
	}

	// If all addresses have been resolved, terminate search.
	if (addrvec.size() == 0 ) {
// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
	        std::cerr << "Done searching for functions at index "
		    << i << " out of " << numsortedsyms << " syms." << std::endl;
	    }
#endif
	    break;
	}
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: found " << foundpcs
	<< " functions with data after examining " << addrbuf->length
	<< " unique addresses" << std::endl;
    }
#endif
    return foundpcs;
}

int
BFDSymbols::getFunctionSyms (std::set<Address>& addresses, AddressRange& range_in_this_obj)
{

#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
	std::cerr << "BFDSymbols::getFunctionSyms ENTERED with range:" << range_in_this_obj << std::endl;
    }
#endif

    bfd_vma obj_load_addr = range_in_this_obj.getBegin().getValue();

    // No sampled addresses from this object so nothing to do. Just return 0.
    if (addresses.size() == 0 ) {
	return 0;
    }

    std::set<Address>::iterator ai;
    std::set<Address>::iterator ai_begin = addresses.equal_range(range_in_this_obj.getBegin()).first;
    std::set<Address>::iterator ai_end = addresses.equal_range(range_in_this_obj.getEnd()).second;

    /* We make a copy of syms to sort.  We don't want to sort syms
       because that will screw up any relocs.  */
    sortedsyms = (asymbol**)xmalloc (numsyms * sizeof (asymbol *));
    memcpy (sortedsyms, syms, numsyms * sizeof (asymbol *));

    numsortedsyms = remove_useless_symbols (sortedsyms, numsyms);

    /* Sort the symbols into section and symbol order.  */
    qsort (sortedsyms, numsortedsyms, sizeof (asymbol *), compare_symbols);

    // begining of text (entry).
    bfd_vma bfd_text_begin = bfd_get_start_address(theBFD);

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: Examining " << numsortedsyms
	<< " sortedsyms for functions with data"
	<< std::endl;

      std::cerr << "getFunctionSyms: start address for this bfd "
	<< Address(bfd_text_begin)
	<< ", load address for object is " << (Address)obj_load_addr
	<< std::endl;

      std::cerr << "getFunctionSyms: offset "
	<< (bfd_signed_vma ) ((bfd_signed_vma) bfd_text_begin - (bfd_signed_vma) obj_load_addr)
	<< std::endl;
    }
#endif

    // Compute offset object was loaded at.
    // Use load address from /proc/self/maps if offset is
    // less than 0.
    Address base = Address(0);
    if ( (Address(bfd_text_begin) - Address(obj_load_addr)) < 0 ) {
        base = Address(obj_load_addr);
    }

    obj_base = base.getValue();

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: using base address " << base << " for function offsets" << std::endl;
    }
#endif

    int foundpcs = 0;
    for (int i = 0; i < numsortedsyms; i++) {
	asymbol* sym = sortedsyms[i];
#ifndef NDEBUG
        if(is_debug_symbols_enabled) {
            std::cerr << "getFunctionSyms: before if sym->flags= " << sym->flags
                      << " BSF_FUNCTION= " << BSF_FUNCTION
                      << " (sym->flags & BSF_FUNCTION)= " << (sym->flags & BSF_FUNCTION)
                      << " sym->section= " << sym->section
                      << " !bfd_is_und_section(sym->section)= " << !bfd_is_und_section(sym->section)
      	              << std::endl;
          }
#endif
	if ((sym->flags & BSF_FUNCTION) && !bfd_is_und_section(sym->section)) {
	
	    bfd_size_type size = bfd_get_section_size(sym->section);
	    bfd_vma section_vma = bfd_get_section_vma (theBFD, sym->section);
	    bfd_vma begin_addr = bfd_asymbol_value(sym);
#ifndef NDEBUG
            if(is_debug_symbols_enabled) {
              std::cerr << "getFunctionSyms: inside if sym->flags, size=" << size
        	<< " section_vma=" << Address(section_vma) 
        	<< " begin_addr=" << Address(begin_addr)
        	<< std::endl;
            }
#endif

	    if (section_vma <= begin_addr && begin_addr > size + section_vma) {
#ifndef NDEBUG
                if(is_debug_symbols_enabled) {
                  std::cerr << "getFunctionSyms: BREAK in section_vma if test " 
	                    << std::endl;
                }
#endif
		break;
	    } 

	    Address real_begin(base + (uint64_t)begin_addr);
	    if (!range_in_this_obj.doesContain(real_begin)) {
#ifndef NDEBUG
                if(is_debug_symbols_enabled) {
                  std::cerr << "getFunctionSyms: CONTINUE in range_in_this_obj if test " 
	                    << " base =" << base
	                    << " begin_addr =" << Address(begin_addr)
	                    << " real_begin =" << real_begin
		            << " PC range_in_this_obj " << range_in_this_obj
	                    << std::endl;
                }
#endif
		continue;
	    }

	    // default to end address of section.  Will refine end_addr below.
	    bfd_vma end_addr = size + section_vma;

	    std::string symname = bfd_asymbol_name(sym);
// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
	      std::cerr << "getFunctionSyms: TESTING "
		<< symname << " at " << real_begin
		<< "," << Address(base + end_addr)
		<< " end_addr" << Address(end_addr)
		<< " in section " << Address(base + section_vma)
		<< " with size " <<  size
		<< " PC range_in_this_obj " << range_in_this_obj << std::endl;
	    }
#endif

	    // Now examine sortedsyms starting at next index for
	    // a valid function with text. Since sortedsyms is sorted
	    // on increasing addresses, the begin address of the next
	    // function will be used to compute the end address of
	    // the current function.
	    int next = i + 1;
	    bool notfound = true;
	    while (notfound && next < numsortedsyms) {
	      asymbol* nextsym = sortedsyms[next];
	      if ((nextsym->flags & BSF_FUNCTION) &&
		!bfd_is_und_section(nextsym->section)) {

		notfound = false;
		bfd_size_type nextsize = bfd_get_section_size(nextsym->section);
		bfd_vma nextsection_vma =
				bfd_get_section_vma (theBFD, nextsym->section);
		bfd_vma nextbegin_addr = bfd_asymbol_value(nextsym);
		bfd_vma next_end_addr =  bfd_asymbol_value(nextsym);

// DEBUG
#ifndef NDEBUG
	        if(is_debug_symbols_enabled) {
		    std::cerr << "getFunctionSyms: next symbol"
			<< " is " << bfd_asymbol_name(nextsym)
			<< " at " << Address(nextbegin_addr)
			<< ", " << Address(next_end_addr)
			<< std::endl;
		}
#endif

		if ( next+1 <= numsortedsyms &&
		    next_end_addr == begin_addr &&
		    (sym->flags & BSF_GLOBAL &&
		     sym->flags & BSF_EXPORT &&
		     !(sym->flags & BSF_WEAK)
		    ) ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " addr is == current symbol BSF_GLOBAL BSF_EXPORT "
			<< Address(next_end_addr)
			<< " current name " << bfd_asymbol_name(sym)
			<< std::endl;
		    }
#endif
	            asymbol*  nnsym = sortedsyms[next+1];
		    if ((nnsym->flags & BSF_FUNCTION) &&
			!bfd_is_und_section(nnsym->section)) {
			bfd_vma nn_addr = bfd_asymbol_value(nnsym);
			end_addr = nn_addr;
		    }
		}

		if (next_end_addr == begin_addr &&
		    !(next_end_addr < end_addr ) ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " addr is == current symbol "
			<< Address(next_end_addr)
			<< " current name " << bfd_asymbol_name(sym)
			<< std::endl;
		    }
#endif

		}

		if (next_end_addr < end_addr ) {
// DEBUG
#ifndef NDEBUG
	            if(is_debug_symbols_enabled) {
		       std::cerr << "getFunctionSyms: Next symbol is in this "
			 << "section. Use section next_end_addr "
			 << Address(next_end_addr) << std::endl;
		    }
#endif
		     end_addr = next_end_addr;
		} else {
// DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
		      std::cerr << "getFunctionSyms: "
			<< "INFO: Next symbol "
			<< bfd_asymbol_name(nextsym)
			<< " at " << Address(next_end_addr)
			<< " is not in this"
			<< " section. Use section end_addr "
			<< Address(end_addr) << std::endl;
		    }
#endif
		}
		     
		if (nextsection_vma <= nextbegin_addr &&
		    nextbegin_addr > nextsize + nextsection_vma) {
		    std::cerr << "getFunctionSyms: WARNING: next sym name "
			<< bfd_asymbol_name(nextsym)
			<< " is OUTOFRANGE." << std::endl;
		}
	      } else {
// DEBUG
#ifndef NDEBUG
		if(is_debug_symbols_enabled) {
		  std::cerr << "getFunctionSyms: "
		    << bfd_asymbol_name(nextsym)
		    << " NOT nextsym->flags & BSF_FUNCTION && !undefined section"
		    << std::endl;
		}
#endif
		// skip these non-function entries.
		next++;
	      }
	    } //end while

	    if (section_vma <= end_addr &&  end_addr > size + section_vma) {
		std::cerr << "getFunctionSyms: WARNING SKIPPED SECTION "
		   << " symname " << symname
		   << " section_vma " << Address(section_vma)
		   << " section size " << Address(size)
		   << " begin_addr " << Address(begin_addr)
		   << " end_addr " << Address(end_addr)
		   << std::endl;
		continue;
	    }


	    // This is not quite exact but will work for our puposes.
	    // On x86 and x86_64, instructions a variable length and will be
	    // creater than one.  We know that sortedsyms is sorted on addresses
	    // and that the next symbol address is the begin address of the
	    // next function.  We really only need a range for the current
	    // function so we can just subtract 1 from the address of the next
	    // function and use that as the end address of the current function.
	    // VERIFY: On ia64, an instruction bundle is length 0x10.
	    // The last function's end address should be the end address of the section.
	    // The most accurate way to do this is to disassemble the function for x86, x86_64.
	    int ins_size = 1;

	    uint64_t f_end = end_addr;

	    // skip weak symbols
	    if (begin_addr >= f_end) {
// DEBUG
#ifndef NDEBUG
		if(is_debug_symbols_enabled) {
		    if (sym->flags & BSF_WEAK) {
		        std::cerr << "Skipping weak symbol " << symname
			    << " begin " << static_cast<Address>(begin_addr)
			    << " end " << static_cast<Address>(f_end)
			    << std::endl;
		    }
		    if (sym->flags & BSF_GLOBAL) {
		        std::cerr << symname << " is BSF_GLOBAL" << std::endl;
		    }
		    if (sym->flags & BSF_EXPORT) {
		        std::cerr << symname << " is BSF_EXPORT" << std::endl;
		    }
		    if (sym->flags & BSF_WARNING) {
		        std::cerr << symname << " is BSF_WARNING" << std::endl;
		    }
		    if (sym->flags & BSF_OBJECT) {
		        std::cerr << symname << " is BSF_OBJECT" << std::endl;
		    }
		    if (sym->flags & BSF_DYNAMIC) {
		        std::cerr << symname << " is BSF_DYNAMIC" << std::endl;
		    }
		}
#endif
		continue;
	    }

	    // The following loop adds any function found to to the functionvec.
	    // We restrict this to functions who are found to
	    // contain an address for the performance PC address buffer.
	    AddressRange frange(real_begin,
				Address(base + f_end));

#if !defined(USE_ALL_BFD_SYMBOLS)
	    for (ai=ai_begin; ai!=ai_end; ++ai) {
		// To improve performance of this search,
		// see if one of the found functions already
		// has this address so we do not search remaining
		// functions in this dso for an address that
		// TODO: See SymTabAPISymbols.cxx for better way to do this.
		//       Likely loop on functionvec then on addresses in ai_begin thru ai_end.

		Address cur_addr(*ai);

		bool func_already_found = false;
	        for(FunctionsVec::iterator f = functionvec.begin();
					   f != functionvec.end(); ++f) {

        	    AddressRange range(f->getFuncBegin(), f->getFuncEnd());

        	    if (range.doesContain(cur_addr)) {
// DEBUG
#ifndef NDEBUG
		        if(is_debug_symbols_enabled) {
			  std::cerr << "getFunctionSyms: Address "
			    << cur_addr
			    << "already found in function "
			    << f->getFuncName() << " " << range << std::endl;
			}
#endif

			foundpcs++;
			// do not search for this address again.
			func_already_found = true;
			break;
		    }
		}

		if (func_already_found) {
		    continue;
		}

		if (frange.doesContain(cur_addr)) {
// DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
		        std::cerr << "getFunctionSyms: functionvec PUSH BACK "
			    << symname << " at " << frange
			    << " for pc " << cur_addr
			    << std::endl;
		    }
#endif
		    functionvec.push_back( BFDFunction(symname,
							  frange.getBegin().getValue(),
							  frange.getEnd().getValue())
						);
		    foundpcs++;
		    // do not search for this address again.
		    break;
		} else {
		}
	    } // end for ai addresses in range loop
#else
	    functionvec.push_back( BFDFunction(symname,
					       frange.getBegin().getValue(),
					       frange.getEnd().getValue()) );
#endif
	}
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getFunctionSyms: found " << foundpcs
	<< " functions with data after examining " << addresses.size()
	<< " unique addresses" << std::endl;
    }
#endif
    return foundpcs;
}

int BFDSymbols::initBFD (std::string filename)
{
    bfd_init();

#if 0
    /* binutils 2.23/2.24 use this definition */
    /* 2.28 replaces ... with va_list, but there is no */
    /* binutils version string to create a version specific */
    /* routine, so commenting out for now */
    // create an error handler for bfd.
    bfd_set_error_handler(bfdErrHandler);
#endif

    // open bfd for native target.
    theBFD = bfd_openr(filename.c_str(),NULL);

    // print error if no bfd could be opened.
    if (!theBFD) {
	std::cerr << "initBFD: Could not open " << filename << std::endl;
	return -1;
    }

    // is passed filename an object file?
    if (!bfd_check_format (theBFD, bfd_object)) {
	std::cerr << "initBFD: The file " << filename
		  << " is not an object file." << std::endl;
	return -1;
    }

    slurp_symtab(theBFD);
    init_done = 1;
    return 0;
}

// Callback for bfd_map_over_sections to find nearest line.
void
find_address_in_section (bfd *theBFD, asection *section,
                         void *data ATTRIBUTE_UNUSED)
{
    bfd_vma vma;
    bfd_size_type size;

    if (found) {
	return;
    }

    if ((bfd_get_section_flags (theBFD, section) & SEC_ALLOC) == 0) {
	return;
    }

    bfd_vma real_pc = pc;

    if (obj_base > 0) {
	real_pc = pc - obj_base;
    }

    vma = bfd_get_section_vma (theBFD, section);

    if (real_pc < vma) {
	return;
    }


    size = bfd_get_section_size(section);

    if (real_pc >= vma + size) {
// DEBUG
#if 0
	std::cerr << "find_address_in_section: RETURNS EARLY "
	   << " pc " << Address(pc)
	   << " >= vma + size " << Address(vma + size)
	   << " for vma " << Address(vma)
	   << " + size " << Address(size) << std::endl;
#endif
	return;
    }

    // DO NOT USE sorted symbols here.  The bfd_find_nearest_line
    // need the original unsorted symbols.
    const char *filename;
    const char *functionname;
    unsigned int line;
    found = bfd_find_nearest_line (theBFD, section, syms, real_pc - vma,
                                   &filename, &functionname, &line);
    if (!found) {
// DEBUG
#ifndef NDEBUG
	if(debug_symbols) {
	    std::cerr << "find_address_in_section: "
	    << " bfd_find_nearest_line FAILS FOR " << Address(pc) << std::endl;
	}
#endif
	return;
    }

    std::string tfunc = functionname ? functionname : "UNKNOWN FUNCTION";
    std::string tfile = filename ? filename : "";

// DEBUG
#ifndef NDEBUG
    if(debug_symbols) {
      std::cerr << "find_address_in_section: addr[" << Address(pc) << "]"
	<< " func[" << tfunc << "]"
	<< " file[" << tfile << "]"
	<< " line[" << line << "]"
	<< " vma section addr " << Address(vma)
	<< " section size " << size
	<< std::endl;
    }
#endif

    // Create a BFDStatement object for this pc and add it into
    // statementvec for later update to database.
    // DPM: do not add statement info if there is no source file
    // found. Typically tfile is empty and line is 0 in this case.
    if (!tfile.empty()) {
	BFDStatement datastatement(pc, tfile, line);
	statementvec.push_back(datastatement);
    }

    fflush(stdout);
}

Path BFDSymbols::getObjectFile(Path filename)
{
    if (filename.doesExist()) {
	return filename;
    }

    Path newpath;

    char delimitor = ':';
    Path lib64path = "/lib64:/usr/lib64";
    Path libpath = "/lib:/usr/lib";

    std::string::size_type start = 0;
    std::string::size_type end;
    while ((end = lib64path.find_first_of(delimitor,start))!=
						std::string::npos) {
	if (lib64path.doesExist()) {
	    libpath = lib64path;
	    break;
	}
	start = end+1;
    }

    std::string ld_library_path = libpath;
    if(getenv("LD_LIBRARY_PATH") != NULL) {
	ld_library_path += delimitor;
	ld_library_path += getenv("LD_LIBRARY_PATH");
    }

    if (!ld_library_path.empty()) {
	std::string::size_type start = 0;
	std::string::size_type end;
	while ((end = ld_library_path.find_first_of(delimitor,start))!=
							std::string::npos) {
	    newpath =  std::string(ld_library_path,start,end-start);
	    newpath += filename;
	    if(newpath.doesExist()) {
		break;
	    }
	    start = end+1;
	}
    }

    return newpath;
}



int
BFDSymbols::getBFDFunctionStatements(PCBuffer *addrbuf,
				     const LinkedObject& linkedobject,
				     SymbolTableMap& stmap)
{
    int rval = -1;
    init_done = 0;
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
	debug_symbols = true;
    }
#endif

    std::string filename = linkedobject.getPath();
    std::set<AddressRange> lorange = linkedobject.getAddressRange();

    Path objfilename = getObjectFile((Path)filename);

    int foundpath = 0;
    if (objfilename.doesExist()) {
	foundpath = 1;
	rval = initBFD(objfilename);;
    }

    if (!foundpath || rval < 0) {
	std::cerr << "getBFDFunctionStatements: Could not find a bfd for "
		  << filename << std::endl;
	return rval;
    }

    std::set<Address> function_begin_addresses;
    int addresses_found = 0;

    std::set<AddressRange>::iterator si;
    for(si = lorange.begin() ; si != lorange.end(); ++si) {
// VERBOSE
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "Processing linked object "
	        << filename << " with address range " << (*si) << std::endl;
	}
#endif

	std::vector<uint64_t> addrvec;

	for (unsigned ii = 0; ii < addrbuf->length; ii++) {
	    if ((*si).doesContain(static_cast<Address>(addrbuf->pc[ii]))) {
		addrvec.push_back(addrbuf->pc[ii]);
	    }
	}

	// DSO OFFSET. Pass address object was loaded at in victim process.
	// The file /proc/self/maps has this info.
	// Use section .text.  section->size - section->vma == offset.
	// nm -h gives this for .text:
	// Idx Name  Size      VMA               LMA               File off  Algn
	//   9 .text 0012fe68  00000000000720d0  00000000000720d0  000720d0  2**4
	//                   CONTENTS, ALLOC, LOAD, READONLY, CODE

	// Find only those functions whose address range contains a value
	// in the passed address buffer addrbuf.
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "Calling getFunctionSyms for "
	        << " (*si).getBegin().getValue()=" << Address((*si).getBegin().getValue())
	        << " (*si).getEnd().getValue()=" << Address((*si).getEnd().getValue())
                << std::endl;
	}
#endif

	int found_functions = getFunctionSyms(addrbuf,
					      (*si).getBegin().getValue(),
					      (*si).getEnd().getValue());

#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "After Calling getFunctionSyms for "
                << " found_functions=" << found_functions
                << std::endl;
	}
#endif
	// foreach address in addrvec, find the function, file, line number.
	// See find_address_in_section for details.
	// store functions begin and end address for functions with found
	// in the sampled address space.
	for (unsigned ii = 0; ii < addrvec.size(); ++ii) {
            int foundpc = 0;
            pc = addrvec[ii];
            Address cur_pc(addrvec[ii]);
            found = false;

	    for(FunctionsVec::iterator f = functionvec.begin();
				       f !=  functionvec.end(); ++f) {

		AddressRange range(f->getFuncBegin(),f->getFuncEnd());
#ifndef NDEBUG
		    if (is_debug_symbols_enabled) {
		      std::cerr << "getBFDFunctionStatements: LOOKING FOR FUNCTION for pc " << cur_pc
		        << " at " << ii << " range is: Function Begin=" << f->getFuncBegin()
		        << " Function End=" << f->getFuncEnd() << std::endl;
		    }
#endif

		if (range.doesContain(cur_pc)) {
// DEBUG
#ifndef NDEBUG
		    if (is_debug_symbols_enabled) {
		      std::cerr << "getBFDFunctionStatements: FOUND FUNCTION for pc " << cur_pc
		        << " at " << ii << " of " << addrbuf->length
		        << " for " << f->getFuncName() << std::endl;
		    }
#endif
		    foundpc++;
		    addresses_found++;

		    // Record the function begin addresses, This allows the cli and gui
		    // to focus on or display the first statement of a function.
		    // The function begin addresses will be processed later
		    // for statement info and added to our statements.
		    function_begin_addresses.insert(f->getFuncBegin());
		}
#ifndef NDEBUG
 	        if(is_debug_symbols_enabled) {
		    if (foundpc == 0) {
		        if(is_debug_symbols_enabled) {
		            std::cerr << "getBFDFunctionStatements: FAILED FUNCTION for pc " << cur_pc
		    	    << " at " << ii << " of " << addrbuf->length 
		            << " range is: Function Begin=" << f->getFuncBegin()
		            << " Function End=" << f->getFuncEnd() 
                            << std::endl;
		        }
		    }
	        }
#endif
	    }

	    if (foundpc > 1) {
		addresses_found--;
// DEBUG
#ifndef NDEBUG
		if (is_debug_symbols_enabled) {
	            std::cerr << "getBFDFunctionStatements: FOUND MULTIPLE " << foundpc
		    << " functions with pc " << cur_pc
	            << " at " << ii << " of " << addrvec.size()
	            << std::endl;
		}
#endif
	    }

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
		if (foundpc == 0) {
		    if(is_debug_symbols_enabled) {
		        std::cerr << "getBFDFunctionStatements: FAILED FUNCTION for pc " << cur_pc
		    	<< " at " << ii << " of " << addrbuf->length 
                        << std::endl;
		    }
		}
	    }
#endif

	    rval = addresses_found;
	    bfd_map_over_sections (theBFD, find_address_in_section, NULL);
	}
    }

    // Find any statements for the begin and end of functions that
    // contained a valid sample address.
    for(std::set<Address>::const_iterator fi = function_begin_addresses.begin();
					  fi != function_begin_addresses.end();
					  ++fi) {
	found = false;
	pc = (*fi).getValue();
        bfd_map_over_sections (theBFD, find_address_in_section, NULL);
    }

    if (syms) {
	free(syms);
	syms = NULL;
    }
    if (sortedsyms) {
	free(sortedsyms);
	sortedsyms = NULL;
    }

    bfd_close(theBFD);

// VERBOSE
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getBFDFunctionStatements: total addressess found "
	<< addresses_found << " out of buffer of " << addrbuf->length
	<< std::endl;
    }
#endif

    return rval;
}



int
BFDSymbols::getBFDFunctionStatements(std::set<Address>& addresses,
				     const LinkedObject& linkedobject,
				     SymbolTableMap& stmap)
{
    int rval = -1;
    init_done = 0;
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
	debug_symbols = true;
    }
#endif

    std::string filename = linkedobject.getPath();
    std::set<AddressRange> lorange = linkedobject.getAddressRange();

    Path objfilename = getObjectFile((Path)filename);

    int foundpath = 0;
    if (objfilename.doesExist()) {
	foundpath = 1;
	rval = initBFD(objfilename);;
    }

    if (!foundpath || rval < 0) {
	std::cerr << "getBFDFunctionStatements: Could not find a bfd for "
		  << filename << std::endl;
	return rval;
    }

    std::set<Address> function_begin_addresses;
    int addresses_found = 0;

    std::set<AddressRange>::iterator si;
    for(si = lorange.begin() ; si != lorange.end(); ++si) {
// VERBOSE
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "Processing linked object "
	        << filename << " with address range " << (*si) << std::endl;
	}
#endif

	// find the subset of addresses within the range of symtab si.
	std::set<Address>::iterator ai;
	std::set<Address>::iterator ai_begin = addresses.equal_range((*si).getBegin()).first;
	std::set<Address>::iterator ai_end = addresses.equal_range((*si).getEnd()).second;

	// DSO OFFSET. Pass address object was loaded at in victim process.
	// The file /proc/self/maps has this info.
	// Use section .text.  section->size - section->vma == offset.
	// nm -h gives this for .text:
	// Idx Name  Size      VMA               LMA               File off  Algn
	//   9 .text 0012fe68  00000000000720d0  00000000000720d0  000720d0  2**4
	//                   CONTENTS, ALLOC, LOAD, READONLY, CODE

	// Find only those functions whose address range contains a value
	// in the passed address buffer addrbuf.
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "Calling getFunctionSyms for *si range:" << *si << std::endl;
	}
#endif

	AddressRange r(*si);
	int found_functions = getFunctionSyms(addresses, r);

#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	    std::cerr << "After Calling getFunctionSyms for "
                << " found_functions=" << found_functions
                << std::endl;
	}
#endif
	// foreach address in addrvec, find the function, file, line number.
	// See find_address_in_section for details.
	// store functions begin and end address for functions with found
	// in the sampled address space.
	for (ai=ai_begin; ai!=ai_end; ++ai) {
            int foundpc = 0;
            pc = (*ai).getValue();
            Address cur_pc(*ai);
            found = false;

	    for(FunctionsVec::iterator f = functionvec.begin();
				       f !=  functionvec.end(); ++f) {

		AddressRange range(f->getFuncBegin(),f->getFuncEnd());
#ifndef NDEBUG
		    if (is_debug_symbols_enabled) {
		      std::cerr << "getBFDFunctionStatements: LOOKING FOR FUNCTION for pc " << cur_pc
		        << " range is: Function Begin=" << f->getFuncBegin()
		        << " Function End=" << f->getFuncEnd() << std::endl;
		    }
#endif

		if (range.doesContain(cur_pc)) {
// DEBUG
#ifndef NDEBUG
		    if (is_debug_symbols_enabled) {
		      std::cerr << "getBFDFunctionStatements: FOUND FUNCTION for pc " << cur_pc
		        << " for " << f->getFuncName() << std::endl;
		    }
#endif
		    foundpc++;
		    addresses_found++;

		    // Record the function begin addresses, This allows the cli and gui
		    // to focus on or display the first statement of a function.
		    // The function begin addresses will be processed later
		    // for statement info and added to our statements.
		    function_begin_addresses.insert(f->getFuncBegin());
		}
#ifndef NDEBUG
 	        if(is_debug_symbols_enabled) {
		    if (foundpc == 0) {
		        if(is_debug_symbols_enabled) {
		            std::cerr << "getBFDFunctionStatements: FAILED FUNCTION for pc " << cur_pc
		            << " range is: Function Begin=" << f->getFuncBegin()
		            << " Function End=" << f->getFuncEnd() 
                            << std::endl;
		        }
		    }
	        }
#endif
	    }

	    if (foundpc > 1) {
		addresses_found--;
// DEBUG
#ifndef NDEBUG
		if (is_debug_symbols_enabled) {
	            std::cerr << "getBFDFunctionStatements: FOUND MULTIPLE " << foundpc
		    << " functions with pc " << cur_pc
	            << std::endl;
		}
#endif
	    }

// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
		if (foundpc == 0) {
		    if(is_debug_symbols_enabled) {
		        std::cerr << "getBFDFunctionStatements: FAILED FUNCTION for pc " << cur_pc
                        << std::endl;
		    }
		}
	    }
#endif

	    rval = addresses_found;
	    bfd_map_over_sections (theBFD, find_address_in_section, NULL);
	}
    }

    // Find any statements for the begin and end of functions that
    // contained a valid sample address.
    for(std::set<Address>::const_iterator fi = function_begin_addresses.begin();
					  fi != function_begin_addresses.end();
					  ++fi) {
	found = false;
	pc = (*fi).getValue();
        bfd_map_over_sections (theBFD, find_address_in_section, NULL);
    }

    if (syms) {
	free(syms);
	syms = NULL;
    }
    if (sortedsyms) {
	free(sortedsyms);
	sortedsyms = NULL;
    }

    bfd_close(theBFD);

// VERBOSE
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
      std::cerr << "getBFDFunctionStatements: total addressess found "
	<< addresses_found << " out of buffer of " << addresses.size()
	<< std::endl;
    }
#endif

    return rval;
}



void
BFDSymbols::getSymbols(PCBuffer *addrbuf,
		       const LinkedObject& lo, SymbolTableMap& stmap)
{
// DEBUG
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
	std::cerr << "BFDSymbols::getSymbols: ENTERED.. addrbuf length:" << addrbuf->length << std::endl;
    }
#endif
    // LinkedObject::getAddressRange actually returns the
    // set of AddressSpaces found in the database where the
    // LinkedObject actually was loaded into memory.
    // The SymbolTableMap will only know about one of these
    // addressspaces, the one used to create it.
    std::set<AddressRange> lorange = lo.getAddressRange();
    AddressRange stmap_lorange;
    std::set<AddressRange>::iterator si;
    for(si = lorange.begin() ; si != lorange.end(); ++si) {

#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	     std::cerr  << "BFDSymbols::getSymbols: RESOLVING LO " << lo.getPath()
		 << ":" << *si << std::endl;
	}
#endif

	// find which addrssspace is used for the symboltablemap
	if (stmap.find(*si) != stmap.end()) {
	    stmap_lorange = *si;
// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
	        std::cerr << "BFDSymbols::getSymbols: " << " stmap RANGE " << *si << std::endl;
	    }
#endif
	}
    }

    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	AddressRange lrange = (*si);

// DEBUG
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	     std::cerr  << "BFDSymbols::getSymbols: RESOLVING LO " << lo.getPath()
		 << ":" << lrange << std::endl;
	}
#endif
	// addressrange lrange may not be the one in stmap.
	if (stmap.find(lrange) != stmap.end()) {
	    int addresses_found =
		getBFDFunctionStatements(addrbuf, lo, stmap);
	    
	    // Add Functions
	    // RESTRICT functions to only those with sampled addresses.
	    // We need to remember that some sampled addresses may come
	    // from an addressspace OTHER than the one used for the
	    // SymbolTableMap.  Need to compute an offset for those. somehow.

	    for(FunctionsVec::iterator f = functionvec.begin();
				       f != functionvec.end(); ++f) {

		AddressRange frange(f->getFuncBegin(),f->getFuncEnd());

		// See if the function range is inclosed by any addressspace
		// in the SymbolTableMap. If it is found in the addressrange
		// used to create the symboltable entry we can use it straight away.
		// Otherwise we need to find the addressspace the function was
		// sampled in and compute an offset to the stmap addressrange.
		if (stmap.find(frange) != stmap.end()) {
		    SymbolTable& symbol_table =
					stmap.find(frange)->second.first;
//DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
			std::cerr << "BFDSymbols::getSymbols: "
			    << "ADDING FUNCTION for " << f->getFuncName()
			    << " with range " << frange << std::endl;
		    }
#endif

		    symbol_table.addFunction(f->getFuncBegin(),
					     f->getFuncEnd(), f->getFuncName());

		} else {
		    std::set<AddressRange>::iterator li;
		    for(li = lorange.begin() ; li != lorange.end(); ++li) {
			if ((*li).doesContain(frange)) {

			    Address offset(stmap_lorange.getBegin() - (*li).getBegin());
//DEBUG
#ifndef NDEBUG
			    if(is_debug_symbols_enabled) {
			        std::cerr << "BFDSymbols::getSymbols: " << f->getFuncName()
				<< " with range " << frange
				<< " found in alternate " << (*li)
				<< " offset is " << offset
				<< std::endl;
			    }
#endif
			    // the real symboltable for this function.
		    	    SymbolTable& symbol_table =
					stmap.find(stmap_lorange)->second.first;

			    // compute new start and end to fit stmap_lorange.
			    Address start(f->getFuncBegin().getValue() +
					  offset.getValue());
			    Address end(f->getFuncEnd().getValue() +
					  offset.getValue());

			    // Now we can add this function
			    symbol_table.addFunction(start, end, f->getFuncName());
			}
		    }
		}
	    }

	    // Add Statements
	    // RESTRICT statements to only those with sampled addresses.
	    // NOTE: We may need to look at offsets like we do for functions.

	    for(StatementsVec::iterator objsyms = statementvec.begin() ;
					objsyms !=  statementvec.end() ;  ++objsyms) {

		AddressRange frange(objsyms->pc,objsyms->pc+1);
		if (stmap.find(frange) != stmap.end()) {
		    SymbolTable& symbol_table =
					stmap.find(frange)->second.first;
		    Address s_begin = objsyms->pc;
		    Address s_end = objsyms->pc + 1; // HACK
		    std::string path = objsyms->file_name;
		    unsigned int line = objsyms->lineno;
		    unsigned int col = 0;
//DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
			std::cerr << "BFDSymbols::getSymbols:"
			    << " ADDING STATEMENT for " << Address(objsyms->pc)
			    << " with path " << path << " line " << line << std::endl;
		    }
#endif

		    if (path.size() != 0) {
			symbol_table.addStatement(s_begin,s_end,path,line,col);
		    }
		} else {
		}
	    }
	}
    } // end for lorange
    functionvec.clear();
    statementvec.clear();
}

void
BFDSymbols::getSymbols(std::set<Address>& addresses,
		       const LinkedObject& lo, SymbolTableMap& stmap)
{
#ifndef NDEBUG
    if(is_debug_symbols_enabled) {
	std::cerr << "BFDSymbols::getSymbols: ENTERED.. addresses size:" << addresses.size() << std::endl;
    }
#endif

    // LinkedObject::getAddressRange actually returns the
    // set of AddressSpaces found in the database where the
    // LinkedObject actually was loaded into memory.
    // The SymbolTableMap will only know about one of these
    // addressspaces, the one used to create it.
    std::set<AddressRange> lorange = lo.getAddressRange();
    AddressRange stmap_lorange;
    std::set<AddressRange>::iterator si;
    for(si = lorange.begin() ; si != lorange.end(); ++si) {
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	     std::cerr  << "BFDSymbols::getSymbols: RESOLVING LO " << lo.getPath()
		 << ":" << *si << std::endl;
	}
#endif
	// find which addrssspace is used for the symboltablemap
	if (stmap.find(*si) != stmap.end()) {
	    stmap_lorange = *si;
// DEBUG
#ifndef NDEBUG
	    if(is_debug_symbols_enabled) {
	        std::cerr << "BFDSymbols::getSymbols: " << " stmap RANGE " << *si << std::endl;
	    }
#endif
	}
    }

    for(si = lorange.begin() ; si != lorange.end(); ++si) {
	AddressRange lrange = (*si);

// DEBUG
#ifndef NDEBUG
	if(is_debug_symbols_enabled) {
	     std::cerr  << "BFDSymbols::getSymbols: RESOLVING LO " << lo.getPath()
		 << ":" << lrange << std::endl;
	}
#endif
	// addressrange lrange may not be the one in stmap.
	if (stmap.find(lrange) != stmap.end()) {
	    int addresses_found =
		getBFDFunctionStatements(addresses, lo, stmap);
	    
	    // Add Functions
	    // RESTRICT functions to only those with sampled addresses.
	    // We need to remember that some sampled addresses may come
	    // from an addressspace OTHER than the one used for the
	    // SymbolTableMap.  Need to compute an offset for those. somehow.

	    for(FunctionsVec::iterator f = functionvec.begin();
				       f != functionvec.end(); ++f) {

		AddressRange frange(f->getFuncBegin(),f->getFuncEnd());

		// See if the function range is inclosed by any addressspace
		// in the SymbolTableMap. If it is found in the addressrange
		// used to create the symboltable entry we can use it straight away.
		// Otherwise we need to find the addressspace the function was
		// sampled in and compute an offset to the stmap addressrange.
		if (stmap.find(frange) != stmap.end()) {
		    SymbolTable& symbol_table =
					stmap.find(frange)->second.first;
//DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
			std::cerr << "BFDSymbols::getSymbols: "
			    << "ADDING FUNCTION for " << f->getFuncName()
			    << " with range " << frange << std::endl;
		    }
#endif

		    symbol_table.addFunction(f->getFuncBegin(),
					     f->getFuncEnd(), f->getFuncName());

		} else {
		    std::set<AddressRange>::iterator li;
		    for(li = lorange.begin() ; li != lorange.end(); ++li) {
			if ((*li).doesContain(frange)) {

			    Address offset(stmap_lorange.getBegin() - (*li).getBegin());
//DEBUG
#ifndef NDEBUG
			    if(is_debug_symbols_enabled) {
			        std::cerr << "BFDSymbols::getSymbols: " << f->getFuncName()
				<< " with range " << frange
				<< " found in alternate " << (*li)
				<< " offset is " << offset
				<< std::endl;
			    }
#endif
			    // the real symboltable for this function.
		    	    SymbolTable& symbol_table =
					stmap.find(stmap_lorange)->second.first;

			    // compute new start and end to fit stmap_lorange.
			    Address start(f->getFuncBegin().getValue() +
					  offset.getValue());
			    Address end(f->getFuncEnd().getValue() +
					  offset.getValue());

			    // Now we can add this function
			    symbol_table.addFunction(start, end, f->getFuncName());
			}
		    }
		}
	    }

	    // Add Statements
	    // RESTRICT statements to only those with sampled addresses.
	    // NOTE: We may need to look at offsets like we do for functions.

	    for(StatementsVec::iterator objsyms = statementvec.begin() ;
					objsyms !=  statementvec.end() ;  ++objsyms) {

		AddressRange frange(objsyms->pc,objsyms->pc+1);
		if (stmap.find(frange) != stmap.end()) {
		    SymbolTable& symbol_table =
					stmap.find(frange)->second.first;
		    Address s_begin = objsyms->pc;
		    Address s_end = objsyms->pc + 1; // HACK
		    std::string path = objsyms->file_name;
		    unsigned int line = objsyms->lineno;
		    unsigned int col = 0;
//DEBUG
#ifndef NDEBUG
		    if(is_debug_symbols_enabled) {
			std::cerr << "BFDSymbols::getSymbols:"
			    << " ADDING STATEMENT for " << Address(objsyms->pc)
			    << " with path " << path << " line " << line << std::endl;
		    }
#endif

		    if (path.size() != 0) {
			symbol_table.addStatement(s_begin,s_end,path,line,col);
		    }
		} else {
		}
	    }
	}
    } // end for lorange
    functionvec.clear();
    statementvec.clear();
}
