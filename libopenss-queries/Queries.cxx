////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
 * Definition of the Queries namespace and related code.
 *
 */

#include <inttypes.h>
#include "SS_Configure.hxx"
#include "Queries.hxx"
#include "ToolAPI.hxx"


//#define DEBUG_Queries 1

using namespace OpenSpeedShop;



/**
 * Make a thread group from a thread.
 *
 * Constructs a thread group containing the one specified thread.
 *
 * @param thread    Thread from which to construct the thread group.
 * @return          Thread group containing that thread.
 */
Framework::ThreadGroup
Queries::MakeThreadGroup(const Framework::Thread& thread)
{
    Framework::ThreadGroup threads;
    threads.insert(thread);
    return threads;
}



/**
 * Strict weak ordering predicate evaluator for linked objects.
 *
 * Implements a strict weak ordering predicate for linked objects that works
 * properly even when the two linked objects are in different experiment
 * databases.
 *
 * @param lhs    Linked object on left hand side of comparison.
 * @param rhs    Linked object on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareLinkedObjects::operator()(
    const Framework::LinkedObject& lhs,
    const Framework::LinkedObject& rhs) const
{

#if DEBUG_Queries
    std::cerr << "LO: Queries::CompareLinkedObjects::operator(), return (lhs.getPath() < rhs.getPath())=" 
               << (lhs.getPath() < rhs.getPath()) << std::endl;
    std::cerr << "LO: Queries::CompareLinkedObjects::operator(), lhs.getPath()=" << lhs.getPath() << std::endl;
    std::cerr << "LO: Queries::CompareLinkedObjects::operator(), rhs.getPath()=" << rhs.getPath() << std::endl;
#endif

    return lhs.getPath() < rhs.getPath();
}



/**
 * Strict weak ordering predicate evaluator for functions.
 *
 * Implements a strict weak ordering predicate for functions that works
 * properly even when the two functions are in different experiment databases.
 *
 * @param lhs    Function on left hand side of comparison.
 * @param rhs    Function on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareFunctions::operator()(
    const Framework::Function& lhs,
    const Framework::Function& rhs,
    bool less_restrictive_comparisons) const
{

#if DEBUG_Queries
     std::cerr << "Queries::CompareFunctions::operator, lhs.getMangledName()=" << lhs.getMangledName() << "\n";
     std::cerr << "Queries::CompareFunctions::operator, rhs.getMangledName()=" << rhs.getMangledName() << "\n";
#endif
    
    if(lhs.getMangledName() < rhs.getMangledName()) {
#if DEBUG_Queries
        std::cerr << "MANGLED <: Queries::CompareFunctions::operator(), return TRUE, " 
                  << " lhs.getMangledName()=" << lhs.getMangledName() 
                  << " rhs.getMangledName()=" << rhs.getMangledName() << std::endl;
#endif
	return true;
    } else if (lhs.getMangledName() > rhs.getMangledName()) {
#if DEBUG_Queries
        std::cerr << "MANGLED >: Queries::CompareFunctions::operator(), return FALSE, " 
                  << " lhs.getMangledName()=" << lhs.getMangledName() 
                  << " rhs.getMangledName()=" << rhs.getMangledName() << std::endl;
#endif
	return false;
    }
    
    bool compareOnlyFunctionName = FALSE;
    if (less_restrictive_comparisons) {
       compareOnlyFunctionName = TRUE;
    }

#if DEBUG_Queries
     std::cerr << "Queries::CompareFunctions::operator, FALL THROUGH compareOnlyFunctionName=" <<  compareOnlyFunctionName << "\n";
     std::cerr << "Queries::CompareFunctions::operator, FALL THROUGH lhs.getMangledName()=" << lhs.getMangledName() << "\n";
     std::cerr << "Queries::CompareFunctions::operator, FALL THROUGH rhs.getMangledName()=" << rhs.getMangledName() << "\n";
#endif


    if (compareOnlyFunctionName) {
#if DEBUG_Queries
       std::cerr << "Queries::CompareFunctions::operator, FALL THROUGH RETURN false, compareOnlyFunctionName=" <<  compareOnlyFunctionName << "\n";
#endif
       return false;
    } else {

#if DEBUG_Queries
       std::cerr << "Queries::CompareFunctions::operator, FALL THROUGH call getLinkedObject for each side and compare, compareOnlyFunctionName=" <<  compareOnlyFunctionName << "\n";
#endif

       return lhs.getLinkedObject().getPath() < rhs.getLinkedObject().getPath();
    } 
}


/**
 * Strict weak ordering predicate evaluator for statements.
 *
 * Implements a strict weak ordering predicate for statements that works
 * properly even when the two statements are in different experiment databases.
 *
 * @param lhs    Statement on left hand side of comparison.
 * @param rhs    Statement on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareStatements::operator()(
    const Framework::Statement& lhs,
    const Framework::Statement& rhs,
    bool less_restrictive_comparisons) const
{
    bool compareOnlyStmtLine = FALSE;
    if (less_restrictive_comparisons) {
       compareOnlyStmtLine = TRUE;
    }

#if DEBUG_Queries
    std::cerr << "Queries::CompareStatements::operator(), lhs.getPath()=" << lhs.getPath() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), lhs.getLine()=" << lhs.getLine() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), lhs.getColumn()=" << lhs.getColumn() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), lhs.getLinkedObject().getPath()=" << lhs.getLinkedObject().getPath() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), rhs.getPath()=" << rhs.getPath() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), rhs.getLine()=" << rhs.getLine() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), rhs.getColumn()=" << rhs.getColumn() << std::endl;
    std::cerr << "Queries::CompareStatements::operator(), rhs.getLinkedObject().getPath()=" << rhs.getLinkedObject().getPath() << std::endl;
#endif

    if(lhs.getLine() < rhs.getLine())
	return true;
    else if(lhs.getLine() > rhs.getLine())
	return false;
    else if(lhs.getColumn() < rhs.getColumn())
	return true;
    else if(lhs.getColumn() > rhs.getColumn())
	return false;

    if (lhs.getLine() != 0 && rhs.getLine() != 0 && compareOnlyStmtLine) {
#if DEBUG_Queries
       std::cerr << "Queries::CompareStatements::operator, FALL THROUGH RETURN false, compareOnlyStmtLine=" <<  compareOnlyStmtLine << "\n";
#endif
       return false;
    } 

    if(lhs.getPath() < rhs.getPath())
	return true;
    else if(lhs.getPath() > rhs.getPath())
	return false;
    return lhs.getLinkedObject().getPath() < rhs.getLinkedObject().getPath();
}


/**
 * Strict weak ordering predicate evaluator for loops.
 *
 * Implements a strict weak ordering predicate for loops that works
 * properly even when the two loops are in different experiment databases.
 *
 * @param lhs    Loop on left hand side of comparison.
 * @param rhs    Loop on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareLoops::operator()(
    const Framework::Loop& lhs,
    const Framework::Loop& rhs,
    bool less_restrictive_comparisons) const
{
    bool compareOnlyLoopLine = FALSE;
    if (less_restrictive_comparisons) {
       compareOnlyLoopLine = TRUE;
    }

#if DEBUG_Queries
    std::cerr << "LOOP: Queries::CompareLoops::operator()"  << std::endl;
#endif

    std::set<Framework::Statement> lhs_stmt = lhs.getDefinitions();
    std::set<Framework::Statement> rhs_stmt = rhs.getDefinitions();

    if(lhs_stmt == rhs_stmt) {
#if DEBUG_Queries
        std::cerr << "LOOP: Queries::CompareLoops::operator(), return true"  << std::endl;
#endif
	return true;
    } else {
#if DEBUG_Queries
        std::cerr << "LOOP: Queries::CompareLoops::operator(), return false"  << std::endl;
#endif
	return false;
    }

}

