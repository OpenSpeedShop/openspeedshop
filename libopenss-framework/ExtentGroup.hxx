////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration of the ExtentGroup class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExtentGroup_
#define _OpenSpeedShop_Framework_ExtentGroup_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Extent.hxx"

#include <set>
#include <vector>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Arbitrary group of extents.
     *
     * Container holding an arbitrary group of extents. No specific relationship
     * is implied between the extents within a given extent group, although such
     * groups are primarily used for representing when/where a DSO, compilation
     * unit, function, etc. was located within a thread.
     *
     * The container also places each extent into a two-dimensional Kd-tree,
     * allowing searches for partially overlapping extents in logarithmic time.
     * Such high-speed searches were crucial for accelerating the mapping of
     * performance data to statements, functions, etc.
     *
     * @sa    http://en.wikipedia.org/wiki/Kd-tree
     *
     * @ingroup Utility
     */
    class ExtentGroup :
	public std::vector<Extent>
    {
	
    public:

	Extent getBounds() const;
	std::set<size_type> getIntersectionWith(const Extent&) const;
	std::set<size_type> getIntersectionWith(const ExtentGroup&) const;
	
    private:

	/**
	 * Kd-tree node.
	 *
	 * Structure for a single node in the Kd-tree. Contains a pointer to
	 * the node's children, a description of which extents are enclosed by
	 * the node, and the bounds of those enclosed extents.
	 */
	struct Node
	{
	    
	    /** Index of our left child (right child is "dm_children + 1"). */
	    std::vector<Node>::difference_type dm_children;
	    
	    /** Index of the first extent enclosed by this node. */
	    std::vector<ExtentGroup::size_type>::size_type dm_first;

	    /** Index of the last extent enclosed by this node. */
	    std::vector<ExtentGroup::size_type>::size_type dm_last;
	    
	    /** Bounds of the extents enclosed by this node. */
	    Extent dm_bounds;
	    
	    /** Default constructor. */
	    Node() :
		dm_children(0),
		dm_first(0),
		dm_last(0),
		dm_bounds()
	    {
	    }

	    /** Constructor from fields. */
	    Node(const std::vector<Node>::size_type& children,
		 const std::vector<ExtentGroup::size_type>::size_type& first,
		 const std::vector<ExtentGroup::size_type>::size_type& last,
		 const Extent& bounds) :
		dm_children(children),
		dm_first(first),
		dm_last(last),
		dm_bounds(bounds)
	    {
	    }

	};    

	/** Index of next available node in the tree. */
	mutable std::vector<Node>::size_type dm_next;
	
	/** Nodes of the tree. */
	mutable std::vector<Node> dm_tree;
	
	/** Map used to partition extents without actually moving them. */
	mutable std::vector<ExtentGroup::size_type> dm_map;
	
	void initializeTree() const;
	void buildChildren(const size_type&) const;
	
    };
    
} }



#endif
