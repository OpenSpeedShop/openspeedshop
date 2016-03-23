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
 * Definition of the ExtentGroup class.
 *
 */

#include "ExtentGroup.hxx"

#include <deque>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <algorithm>

using namespace OpenSpeedShop::Framework;



namespace {

    /**
     * Strict weak ordering predicate for extents.
     *
     * Defines a strict weak ordering predicate for extents in an extent group
     * in order to partition that group using std::nth_element(). Compares the
     * center points of either the time interval or address range for the given
     * extents.
     */
    class CompareExtents : 
	public std::binary_function<const ExtentGroup::size_type&,
				    const ExtentGroup::size_type&,
				    bool>
    {
	
    public:

	/** Constructor from extent group and partitioning direction. */
	CompareExtents(const ExtentGroup& group, const bool& by_time) :
	    dm_group(group),
	    dm_by_time(by_time)
	{
	}
	
	/** Evaluator for this predicate. */
	bool operator()(const ExtentGroup::size_type& lhs,
			const ExtentGroup::size_type& rhs) const
	{
	    // Are we partitioning by time interval?
	    if(dm_by_time)
		return
		    ((dm_group[lhs].getTimeInterval().getBegin().getValue() +
		     dm_group[lhs].getTimeInterval().getEnd().getValue()) / 2)
		    <
		    ((dm_group[rhs].getTimeInterval().getBegin().getValue() +
		      dm_group[rhs].getTimeInterval().getEnd().getValue()) / 2);
	    
	    // Otherwise we are partitioning by address range
	    else
		return
		    ((dm_group[lhs].getAddressRange().getBegin().getValue() +
		      dm_group[lhs].getAddressRange().getEnd().getValue()) / 2)
		    <
		    ((dm_group[rhs].getAddressRange().getBegin().getValue() +
		      dm_group[rhs].getAddressRange().getEnd().getValue()) / 2);
	}
	
    private:

	/** Extent group which is being partitioned. */
	const ExtentGroup& dm_group;
	
	/** Flag indicating if paritioning is being done by time interval. */
	bool dm_by_time;
	
    };
    
}



/**
 * Get our bounds.
 *
 * Returns the bounds of all extents in this group.
 *
 * @return    Bounds of all extents in this group.
 */
Extent ExtentGroup::getBounds() const
{
    // Handle special case of an empty group
    if(empty())
	return Extent();

    // Initialize the tree (when necessary)
    initializeTree();
    
    // Return the bounds from the tree's root node (which contains all extents)
    return dm_tree[0].dm_bounds;
}



/**
 * Get our intersection with an extent.
 *
 * Returns those extents in this group that intersect the specified extent. The
 * results are returned as a set of indicies into this group. An empty set is
 * returned if the specified extent does not interesect with any of the extents
 * in this group.
 *
 * @param extent    Extent with which to intersect.
 * @return          Extents that intersect with this extent.
 */
std::set<ExtentGroup::size_type> 
ExtentGroup::getIntersectionWith(const Extent& extent) const
{
    std::set<size_type> intersection;

    // Handle special case of an empty group
    if(empty())
	return std::set<size_type>();

    // Initialize the tree (when necessary)
    initializeTree();
    
    // Initialize the traversal stack with the tree's root node
    std::deque<std::vector<Node>::size_type> traversal;
    traversal.push_back(0);
    
    // Search the tree until the traversal stack is empty
    while(!traversal.empty()) {
	
	// Grab the top node from the traversal stack
	std::vector<Node>::size_type node = traversal.front();
	traversal.pop_front();
	
	// Ignore this node and its children if it doesn't intersect the extent
	if(!extent.doesIntersect(dm_tree[node].dm_bounds))
	    continue;
	
	// Build this node's children (if necessary)
	if(dm_tree[node].dm_children == 0)
	    buildChildren(node);
	
	// Push our children on the traversal stack if we're an internal node
	if(dm_tree[node].dm_children > 0) {
	    traversal.push_back(dm_tree[node].dm_children);
	    traversal.push_back(dm_tree[node].dm_children + 1);	    
	}
	
	// Add ourselves to the intersection results if we're a leaf node
	else if(dm_tree[node].dm_children < 0)
	    intersection.insert(-(dm_tree[node].dm_children + 1));
	
    }
    
    // Return the intersection to the caller
    return intersection;
}



/**
 * Get our intersection with an extent group.
 *
 * Returns those extents in this group that intersect the specified extent
 * group. The results are returned as a set of indicies into this group. An
 * empty set is returned if the specified extent group does not intersect
 * with any of the extents in this group.
 *
 * @param extents    Extent group with which to intersect.
 * @return           Extents that intersect with this extent group.
 */
std::set<ExtentGroup::size_type> 
ExtentGroup::getIntersectionWith(const ExtentGroup& extents) const
{
    std::set<size_type> intersection;

    // Iterate over each extent in the group with which to intersect
    for(ExtentGroup::const_iterator 
	    i = extents.begin(); i != extents.end(); ++i) {

	// Get our intersection with this extent and accumulate the results
	std::set<size_type> subset = getIntersectionWith(*i);
	intersection.insert(subset.begin(), subset.end());

    }

    // Return the intersection to the caller
    return intersection;
}



/**
 * Initialize the Kd-tree.
 *
 * Initializes the Kd-tree by initialize the tree's structures and creating
 * the root node. This is only done if the tree wasn't previously initialized,
 * of if extents have been added/removed from this group (thus requiring a
 * complete rebuild of the tree).
 */
void ExtentGroup::initializeTree() const
{
    // Is the tree already initialized?
    if((dm_map.size() > 0) && (dm_map.size() == size()))
	return;

    // Size and initialize the tree structures
    dm_next = 0;
    dm_tree.resize(2 * size(), Node());
    dm_map.resize(size());
    
    // Create the initial partitioning map
    for(std::vector<ExtentGroup::size_type>::size_type
	    i = 0; i < dm_map.size(); ++i)
	dm_map[i] = i;
    
    // Create the tree's root node
    Extent bounds;
    for(size_type i = 0; i < size(); ++i)
	bounds |= (*this)[i];
    dm_tree[dm_next++] = Node(0, 0, size() - 1, bounds);
}



/**
 * Build the children of a node.
 *
 * Constructs the two children of the specified, existing, tree node. This node
 * is made into a leaf node if it only contains a single extent. Otherwise all
 * the extents enclosed by this node are partitioned into two equal sized sets
 * such that each set lies entirely on one side of a plane through the extent
 * space. Each of these sets then become one of the children.
 *
 * @param node    Node for whom children should be built
 */
void ExtentGroup::buildChildren(const size_type& node) const
{
    // Make the current node a leaf node if it contains a single extent
    if(dm_tree[node].dm_first == dm_tree[node].dm_last) {
	dm_tree[node].dm_children = -(dm_map[dm_tree[node].dm_first] + 1);
	dm_tree[node].dm_bounds = (*this)[dm_map[dm_tree[node].dm_first]];
	return;
    }

    // Calculate the location of the partitioning pivot (the middle extent)
    std::vector<ExtentGroup::size_type>::size_type nth =
	(dm_tree[node].dm_first + dm_tree[node].dm_last) / 2;
    
    //
    // Decide if the extents enclosed by this node should be partitioned by
    // their time interval or their address range. Always select the widest of
    // the two dimensions.
    //
    bool by_time = 
	dm_tree[node].dm_bounds.getTimeInterval().getWidth() >
	dm_tree[node].dm_bounds.getAddressRange().getWidth();
    
    //
    // Partition the extents enclosed by this node in either the time interval
    // or address range dimension. After partitioning, the first half of the
    // extents will all lie on one side of a plane dividing the extents in the
    // given dimension, and the second half will all lie on the other side.
    // These two partitions then become the left and right child of this node.
    //

    std::nth_element(dm_map.begin() + dm_tree[node].dm_first,
		     dm_map.begin() + nth,
		     dm_map.begin() + dm_tree[node].dm_last,
		     CompareExtents(*this, by_time));
    
    // Give the parent a reference to its two chilldren
    dm_tree[node].dm_children = dm_next;
    
    // Add the left child to the tree
    Extent left_bounds;
    for(std::vector<ExtentGroup::size_type>::size_type 
	    i = dm_tree[node].dm_first; i <= nth; ++i)
	left_bounds |= (*this)[dm_map[i]];
    dm_tree[dm_next++] = Node(0, dm_tree[node].dm_first, nth, left_bounds);
    
    // Add the right child to the tree
    Extent right_bounds;
    for(std::vector<ExtentGroup::size_type>::size_type 
	    i = nth + 1; i <= dm_tree[node].dm_last; ++i)
	right_bounds |= (*this)[dm_map[i]]; 
    dm_tree[dm_next++] = Node(0, nth + 1, dm_tree[node].dm_last, right_bounds);
}
