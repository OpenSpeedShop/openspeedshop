//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



#ifndef GGGRID_H
#define GGGRID_H


#include <ggStaticArray.h>
#include <ggBox3.h>
#include <ggRay3.h>
#include <ggConstants.h>
#include <assert.h>


// **********************************************************************
// * The grid class provides an spatial subdivision algorithm so that
// * a ray tracer can efficiently render a scene or a portion of a scene.
// * Storage is provided by ggStaticArray which is a linear array structure.
// * The friend class ggGridIterator sets up a performs the grid traversal
// * of a ray in the most efficient manner that we have found.
// **********************************************************************
template <class T>
class ggGrid {

public:

  ggGrid(int xCells,   // # of cells in the x direction
	 int yCells,	// # of cells in the y direction
	 int zCells,	// # of cells in the z direction
	 ggPoint3 min, // min point of grid
	 ggPoint3 max,	// max point of grid
	 T iValue):	// value with which to initialize grid cells
      grid(xCells, yCells, zCells) // call constructor for grid storage	
  {
    nx = xCells; ny = yCells; nz = zCells; // store dimensions
    nCells = nx*ny*nz;
    nyTimesNz = ny*nz;
    gridBox.Set(min,max);
    xDimension = (gridBox.max().x() - gridBox.min().x())/double(nx);
    yDimension = (gridBox.max().y() - gridBox.min().y())/double(ny);
    zDimension = (gridBox.max().z() - gridBox.min().z())/double(nz);
    initialize(iValue); // initialize the storage with iValue
  }

  ggGrid() : grid()
    {
      nx = ny = nz = 0;
      nCells = nx*ny*nz;
      nyTimesNz = ny*nz;
      gridBox.Set(ggPoint3(0,0,0), ggPoint3(0,0,0));
      xDimension = yDimension = zDimension = 0;
    }

  void Reset(int xCells,   // # of cells in the x direction
		 int yCells,	// # of cells in the y direction
		 int zCells,	// # of cells in the z direction
		 ggPoint3 min, // min point of grid
		 ggPoint3 max,	// max point of grid
		 T iValue)  // value with which to initialize grid cells
    {
      grid.Resize(xCells,yCells,zCells);
      nx = xCells; ny = yCells; nz = zCells; // store dimensions
      nCells = nx*ny*nz;
      nyTimesNz = ny*nz;
      gridBox.Set(min,max);
      xDimension = (gridBox.max().x() - gridBox.min().x())/double(nx);
      yDimension = (gridBox.max().y() - gridBox.min().y())/double(ny);
      zDimension = (gridBox.max().z() - gridBox.min().z())/double(nz);
      initialize(iValue); // initialize the storage with iValue
    }

  ggBoolean bounds(ggBox3& oBox, // bounding box for thing to be inserted
	 int& iMin, int& iMax,	// min and max x indices
	 int& jMin, int& jMax,	// min and max y indices
	 int& kMin, int& kMax)	// min and max z indices
  {
#ifdef GGSAFE
    assert(nx != 0 && ny != 0 && nz !=0);
#endif    
     ggBox3 box = oBox;

     // ******************************************************
     // * if the box lies totally outside of the Grid then the
     // * box intersects no grid cells.  Return false
     // ******************************************************
     if((box.max().x() < gridBox.min().x()) ||
	(box.min().x() > gridBox.max().x()) ||
	(box.max().y() < gridBox.min().y()) ||
	(box.min().y() > gridBox.max().y()) ||
	(box.max().z() < gridBox.min().z()) ||
	(box.min().z() > gridBox.max().z()))
       { return ggFalse;}
  
     // ******************************************************
     // * if the bounding box of an object is not 3-D (ie when
     // * a triangle lies parallel to a X plane) make it 3-D
     // ******************************************************
     if(box.min().x() == box.max().x())
       box.Set(box.min() - ggVector3(ggEpsilon,0,0),
	       box.max() + ggVector3(ggEpsilon,0,0));
     if(box.min().y() == box.max().y())
       box.Set(box.min() - ggVector3(0,ggEpsilon,0),
	       box.max() + ggVector3(0,ggEpsilon,0));
     if(box.min().z() == box.max().z())
       box.Set(box.min() - ggVector3(0,0,ggEpsilon),
	       box.max() + ggVector3(0,0,ggEpsilon));
     
     // ******************************************************
     // * In the case that we have to nest grids inside
     // * gridcells we need to take care of the case when the 
     // * object is larger than the nested grid.  Thus the use
     // * of ggMin and ggMax
     // ******************************************************

     // *********** Which cell contains box.min() ? **********
     iMin = ggMax(int((box.min().x() - gridBox.min().x())/xDimension),0);
     jMin = ggMax(int((box.min().y() - gridBox.min().y())/yDimension),0);
     kMin = ggMax(int((box.min().z() - gridBox.min().z())/zDimension),0);

     // just in case there are precision problems at the top
     if (iMin >= nx) iMin = nx-1;
     if (jMin >= ny) jMin = ny-1;
     if (kMin >= nz) kMin = nz-1;

     // *********** Which cell contains box.max() ? **********
     iMax = ggMin(int((box.max().x()-gridBox.min().x())/xDimension),nx-1);
     jMax = ggMin(int((box.max().y()-gridBox.min().y())/yDimension),ny-1);
     kMax = ggMin(int((box.max().z()-gridBox.min().z())/zDimension),nz-1);

     return ggTrue;
  }
  // 1-d arrays

  void setCellBox(int i, int j, int k, ggBox3& box) const
   {
     ggPoint3 boxMin(gridBox.min().x() + i * xDimension,
		     gridBox.min().y() + j * yDimension,
		     gridBox.min().z() + k * zDimension);
     ggPoint3 boxMax(gridBox.min().x() + (i + 1) * xDimension,
		     gridBox.min().y() + (j + 1) * yDimension,
		     gridBox.min().z() + (k + 1) * zDimension);
     box.Set(boxMin,boxMax);
   }

  void cells(int& x, int& y, int& z) const { x = nx; y = ny; z=nz ;}

  T operator[] (int i) const { return grid[i]; }
  T& operator[] (int i)      { return grid[i]; }
  T operator() (int i) const { return grid(i); }
  T& operator() (int i)      { return grid(i); }
  // 3-d arrays
  T operator() (int i, int j, int k) const { return grid(i,j,k); }
  T& operator() (int i, int j, int k)      { return grid(i,j,k); }

// Should be protected, but bug in NCC.  -- Ken 5/3/94
public:
  ggBox3  gridBox;
  double xDimension, yDimension, zDimension;
  int nx,ny,nz;
  int nCells;  //  nx ny nz
  int nyTimesNz;  //  ny nz
  ggStaticArray<T>  grid;

  void initialize(T initialValue) // ititialize grid
  {
    // *** Set all cell values equal to initial Value ***
    for(int i = 0; i < nx; i++)
      for(int j = 0; j < ny; j++)
	for(int k = 0; k < nz; k++)
	  grid(i,j,k) = initialValue;
  }

};

// **********************************************************************
// * The ggGridIterator class is a friend of ggGrid.  This class computes
// * the incremental values needed to efficiently traverse the given grid.
// * The class provides a member function Next() that returns the contents 
// * of a particular grid cell and the min and max parameters for a ray
// * passing through that cell.
// **********************************************************************
template <class T>
class ggGridIterator{
public:
  ggGridIterator(const ggRay3& r, const ggGrid<T>& grid, double tstart)
  {
    double t1, t2;
    ggBox3 cellBox;
#ifdef GGSAFE
    // We cannot traverse a grid without dimension
    assert(grid.nx != 0 && grid.ny != 0 && grid.nz != 0);
#endif    
    iGrid = &grid;

    ggPoint3 startPoint = r.pointAtParameter(tstart);
    
    // ******************************************************
    // *  Does the ray start inside the grid or does
    // *  it hit the grid?  Where are the nearest
    // *  x,y, and z planes?
    // ******************************************************

    notDone = ggTrue;
    if((iGrid->gridBox).surroundsPoint(startPoint))
	    t1 = tstart;
    else if((iGrid->gridBox).hitByRay(r,t1,t2))
      {
	if(t2 < tstart)
	  notDone = ggFalse;
      }
    else notDone = ggFalse;

    if(notDone)
      {  
	  tCellMax = t1;
	  ComputeCellAndTraversal(r, t1);
	  iGrid->setCellBox(i,j,k,cellBox);
	  ComputeDistanceToPlanes(r, cellBox);

          // *****  establish t value for next plane  *****
	  txNext = tx + dtx;
	  tyNext = ty + dty;
	  tzNext = tz + dtz;
	  
	  SetAddressingVariables();
	}
  }

  ggBoolean Next(T& object)
  {
    double tMin, tMax;
    return Next(object,tMin,tMax);
  }

  ggBoolean Next(T& object, double& tMin, double& tMax)
  {
    if(!notDone) { return ggFalse;}
    else
      {
	tCellMin = tCellMax;


	// to stop wierd seg faults because we are going out of range
        // remove it this warning doesn't come up for a few months pete 6/18/94

	if (address < 0 || address >= iGrid->nCells) {
               cerr << "Warning: address out of range in Next: " << address << "\n";
               cerr << "i j k = " << i << " " << j << " " << k << "\n";
               return ggFalse;
        }

	object = (*iGrid)[address];
	if(txNext < tyNext && txNext < tzNext)
	  {
	    tCellMax = txNext;
	    txNext += dtx;
	    address += xAddressIncrement;
	    i+= iIncrement;
	    if (i == iStop) notDone = ggFalse;
	  }
	else if(tyNext < tzNext)
	  {
	    tCellMax = tyNext;
	    tyNext += dty;
	    address += yAddressIncrement;
	    j += jIncrement;
	    if (j == jStop) notDone = ggFalse;
	  }
	else
	  {
	    tCellMax = tzNext;
	    tzNext += dtz;
	    address += zAddressIncrement;
	    k += kIncrement;
	    if (k == kStop) notDone = ggFalse;
	  }
	tMin = tCellMin; tMax = tCellMax;
	return ggTrue;
      }
  }
  
protected:
   int i, j, k; // The current cell indices.
   int iIncrement, jIncrement, kIncrement;  // How to increment indices.
   int iStop, jStop, kStop; // Cannot increment any more.
   double tx, ty, tz;  // Parameters for ray/plane intersections.
   double dtx, dty, dtz;  // Distance between planes in parameter space.
   double txNext, tyNext, tzNext; // Parameters for next ray/plane intersection
   double tCellMin, tCellMax; // minimum and maximum parameters for a cell
   ggBoolean notDone; // Loop flag for iterator.
   const ggGrid<T> *iGrid;  // Pointer to ggGrid.
  
   int address; // Used to efficiently index the storage.
   int xAddressIncrement, yAddressIncrement, zAddressIncrement; // see below
   // *******************************************************************
   // * If we call grid(i,j,k) the staticArray code does 2 multiplies
   // * and 2 additions to compute the address of the cell in the array.
   // * If we initially calculate an address for the array and the proper
   // * address increments corresponding to each change in i, j, and k we 
   // * can then call grid(address) which requires no operations. Considering
   // * that for each cell that a ray moves through we must increment some-
   // * thing the combination of i++, grid(i,j,k) requires 5 integer
   // * operations, the combination of address += xAddressIncrement,
   // * grid[address] requires only 1 integer operation.
   // ********************************************************************
   
   ggGridIterator() {} // Cannot construct an iterator without ray and grid.
	    
  
   void SetAddressingVariables() 
     {
       if (iIncrement == 1) {
	 iStop = iGrid->nx;
	 xAddressIncrement = iGrid->nyTimesNz;
       }
       else {
	 iStop = -1;
	 xAddressIncrement = -iGrid->nyTimesNz;
       }
       if (jIncrement == 1) { 
	 jStop = iGrid->ny;
	 yAddressIncrement = iGrid->nz;
       }
       else {
	 jStop = -1;
	 yAddressIncrement = -iGrid->nz;
       }
       if (kIncrement == 1) {
	 kStop = iGrid->nz;
	 zAddressIncrement = 1;
       }
       else {
	 kStop = -1;
	 zAddressIncrement = -1;
       }

       address = (i*iGrid->ny + j)*iGrid->nz +k;
     }
       

  void ComputeDistanceToPlanes(
		const ggRay3& r,       // ray being sent
                const ggBox3& cellbox) // first cell of intersection
  {
  
    double big = 1e10;
    double o, d;      //  origin component and inverse direction component
    double temp;

    o = r.origin().x();

    temp = r.direction().x();
    if ( temp != 0.0 ) {
      d = 1.0/temp;
    } else {
      d = big;
    }

    if(iIncrement < 0)
      {
	tx = (cellbox.max().x() - o) * d;
	dtx = -iGrid->xDimension * d;
      }
    else if(iIncrement > 0)
      {
	tx = (cellbox.min().x() - o) * d;
	dtx = iGrid->xDimension * d;
      }
    else
      {
	tx = big;
	dtx = big;
      }

    o = r.origin().y();
    temp = r.direction().y();
    if ( temp != 0.0 ) {
      d = 1.0/temp;
    } else {
      d = big;
    }
    if(jIncrement < 0)
      {
	ty = (cellbox.max().y() - o) * d;
	dty = -iGrid->yDimension * d;
      }
    else if(jIncrement > 0)
      {
	ty = (cellbox.min().y() - o) * d;
	dty = iGrid->yDimension * d;
      }
    else
      {
	ty = big;
	dty = big;
      }

    o = r.origin().z();
    temp = r.direction().z();
    if ( temp != 0.0 ) {
      d = 1.0/temp;
    } else {
      d = big;
    }
    if(kIncrement < 0)
      {
	tz = (cellbox.max().z() - o) * d;
	dtz = -iGrid->zDimension * d;
      }
    else if(kIncrement > 0)
      {
	tz = (cellbox.min().z() - o) * d;
	dtz = iGrid->zDimension * d;
      }
    else
      {
	tz = big;
	dtz = big;
      }
  }

		
  void ComputeCellAndTraversal(
	        const ggRay3& r,  // ray being sent.
	        double t)          // parameter at which ray intersects box
     {
     ggPoint3 p;
     p = r.pointAtParameter(t);
     double d;

     // ****  Cell indices **************
     i = int((p.x() - iGrid->gridBox.min().x())/iGrid->xDimension);
     j = int((p.y() - iGrid->gridBox.min().y())/iGrid->yDimension);
     k = int((p.z() - iGrid->gridBox.min().z())/iGrid->zDimension);

     // ******************************************************************
     // * If the ray hits a maximum plane then we must decrement the indices
     // ******************************************************************
     if(i >= iGrid->nx) i = iGrid->nx - 1;
     if(j >= iGrid->ny) j = iGrid->ny - 1;
     if(k >= iGrid->nz) k = iGrid->nz - 1;
  
     // ****  determine the increment for i,j,k through grid *******
     d = r.direction().x();
     if(d > ggEpsilon) iIncrement = 1;
     else if(d < -ggEpsilon) iIncrement = -1;
     else iIncrement = 0;
     
     d = r.direction().y();
     if(d > ggEpsilon) jIncrement = 1;
     else if(d < -ggEpsilon) jIncrement = -1;
     else jIncrement = 0;
     
     d = r.direction().z();
     if(d > ggEpsilon) kIncrement = 1;
     else if(d < -ggEpsilon) kIncrement = -1;
     else kIncrement = 0;
   }


};

#endif
