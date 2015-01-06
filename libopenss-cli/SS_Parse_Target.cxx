/* //////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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
////////////////////////////////////////////////////////////////////////////// */

/** @file
 *
 * Storage for the parse results of a single given host/file/cluster
 * rank/pid/thread Target.
 *
 */


// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"

#include <vector>
#include <iostream>

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

/**
 * Constructor: ParseTarget::ParseTarget()
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
ParseTarget::
ParseTarget() :
    dm_used(false)
{
    return ;
}
 
/**
 * Method: ParseTarget::pushHostPoint(char * name)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostPoint(const char * name)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(name);
    dm_host_list.push_back(range);
    return ;
}
 
/**
 * Method: ParseTarget::pushHostPoint(int num)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostPoint(int64_t num)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(num);
    dm_host_list.push_back(range);
    return ;
}
 
/**
 * Method: ParseTarget::pushHostRange(char * name, int num)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostRange(const char * name, int64_t num)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(name,num);
    dm_host_list.push_back(range);
    return ;
}
 
/**
 * Method: ParseTarget::pushHostRange(char * name1, char * name2)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostRange(const char * name1, const char * name2)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(name1,name2);
    dm_host_list.push_back(range);
    return ;
}
 
/**
 * Method: ParseTarget::pushHostRange(int num, char * name)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostRange(int64_t num, const char * name)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(num,name);
    dm_host_list.push_back(range);
    return ;
}
 
/**
 * Method: ParseTarget::pushHostRange( int num1, int num2)
 * 
 *     
 * @return  .
 *
 * @todo    Error handling.
 *
 */
void
ParseTarget::
pushHostRange( int64_t num1, int64_t num2)
{
    this->dm_used = true;   // All insertions need to do this currently
    ParseRange range(num1,num2);
    dm_host_list.push_back(range);
    return ;
}
 
