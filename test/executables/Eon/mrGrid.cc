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



#include <assert.h>
#include <ggMacros.h>
#include <mrGrid.h>


mrGrid::mrGrid(
           double time1,         // low end of time range
           double time2,         // low end of time range
           mrSurfaceList* l,            // list of RTObjects
           ggPoint3 min, // minimum point of bounding box
           ggPoint3 max, // maximum point of bounding box
           double m,
           int perCell,   // maximum number of objects per cell
           int maxDepth)  // number of nests possible

{

  int i,j,k;
  mrSurface *obj_ptr;
  ggBox3 box;
  double xMeasure, yMeasure, zMeasure;
  double cellDimension;
  int nx,ny,nz;

  surfaceCounter = 0;
  
  multiplier = m;
  
  list = l;   // ** make list point to the given list **
  rayCounter = 0;
  // *** Get the Bounding box for the list ***
  list->boundingBox(time1,time2,box);
   min -= ggVector3(ggEpsilon, ggEpsilon, ggEpsilon);
   max += ggVector3(2*ggEpsilon, 3*ggEpsilon, 4*ggEpsilon);

  // *** What are the x, y, and z ranges for list ***
  xMeasure = max.x() - min.x();
  yMeasure = max.y() - min.y();
  zMeasure = max.z() - min.z();
 
  // *** All cells will be cubes.  Establish the length of a side ***
  cellDimension = (1.0 / multiplier)*pow((xMeasure * yMeasure * zMeasure) /
                          double(list->length()), 0.33333333);

 
   // *** How many x, y, and z planes will there be? ***
  cerr << "Grid measure is " << xMeasure << " by " << yMeasure << " by "
    << zMeasure << "\n";
  cerr << "cell dimension is " << cellDimension << "\n";
  nx = int(xMeasure/cellDimension + 1);
  ny = int(yMeasure/cellDimension + 1);
  nz = int(zMeasure/cellDimension + 1);

  cerr << "Creating grid for list of length " << list->length() << endl;
  cerr<<"Grid size = "<<nx<<" by "<<ny<<" by "<<nz<<endl;

  // ********************************************************
  // * Now our cells force an enlargement of the bounding box.
  // * Expand it in all directions.
  // ********************************************************
  min = ggPoint3(min.x()-(nx*cellDimension - xMeasure)/2.0,
                       min.y()-(ny*cellDimension - yMeasure)/2.0,
                       min.z()-(nz*cellDimension - zMeasure)/2.0);
  max = (ggPoint3(min.x() + nx*cellDimension,
                        min.y() + ny*cellDimension,
                        min.z() + nz*cellDimension));

  // *** Now set the bounding box for the Grid ***
  bbox.Set(min, max);

  // **************** Create Grid *************************
  grid.Reset(nx,ny,nz,bbox.min(),bbox.max(),0);

  // ******* Insert objects into their proper cells *******
  for (int o = 0; o < list->length(); o++) {
      obj_ptr = (*list)[o];
      insert(time1,time2,obj_ptr);
   }
 // ******************************************************
  // If the list length for a cell equals 1 then just point
  // to that one object.  If the list length is greater than
  // the maximum number of objects allowed per cell then
  // create and nest a new grid.
  // ******************************************************
  mrSurfaceList *lTemp;
  for(i = 0; i < nx; i++) {
      for(j = 0; j < ny; j++) {
        for(k = 0; k < nz; k++) {
            if(grid(i, j, k) != 0) {
                if(((mrSurfaceList *)grid(i, j, k))->length() == 1) {
                      lTemp = (mrSurfaceList *)grid(i, j, k);
                      grid(i, j, k) = (*((mrSurfaceList *)grid(i, j, k)))[0];
                      delete lTemp;
                }
                else
                  if(((mrSurfaceList *)grid(i, j, k))->length() > perCell
                     && maxDepth != 0) {
                      lTemp = (mrSurfaceList *)grid(i, j, k);
                      grid.setCellBox(i,j,k,box);
                      grid(i, j, k) = new mrGrid(time1,time2,
                                                 lTemp,
                                                 box.min(),
                                                 box.max(),
                                                 multiplier,
                                                 perCell,
                                                 maxDepth - 1);
                  }
              }
          }
      }
   }

   cerr << "Total occupancy = " << surfaceCounter << "\n";

}


// ****************  Actual insertion is done here *******
void mrGrid::insert(double time1, double time2, mrSurface *obj_ptr)
{

  int iMin = 0,jMin = 0,kMin = 0,iMax = 0,jMax = 0,kMax = 0;
  int nx = 0, ny = 0, nz = 0;

  grid.cells(nx,ny,nz);

  ggBox3 box;

  // ***** Insert object into the appropriate cells *******

  ggBox3 cellBox;
  obj_ptr->boundingBox(time1,time2,box);
  if(grid.bounds(box, iMin, iMax, jMin, jMax, kMin, kMax))
    {
      for(int i = iMin; i <= iMax; i++)
        for(int j = jMin; j <= jMax; j++)
          for(int k = kMin; k <= kMax; k++)
            {
              assert(i >= 0 && j >= 0 && k >= 0);
              assert(i < nx && j <= ny && k <= nz);

              grid.setCellBox(i, j, k, cellBox);
              if (obj_ptr->overlapsBox(time1, time2, cellBox)) {
                 surfaceCounter++;
                 if(grid(i, j, k) == 0) {
                     grid(i, j, k) = new mrSurfaceList();
                     ((mrSurfaceList *)grid(i, j, k))->Add(obj_ptr);
                  }
                  else 
                     ((mrSurfaceList *)grid(i, j, k))->Add(obj_ptr);
               }
            }
     }
}


      
ggBoolean mrGrid::selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& Nx, // unit vector at x
             const ggPoint2& uv,  // input coordinate
             const double time,           // time of query
             ggPoint3& on_light,  // point corresponding to uv
             double& prob)                // probability of selecting on_light
             const
	       {
  return list->selectVisiblePoint(x,Nx,uv,time,on_light,prob);
}

/*
ggBoolean mrGrid::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // direction x is viewed from
             const double time,             // time of query
             ggSpectrum& L)               // estimate
             const {
  return list->approximateDirectRadiance(x,N,time,L);
}
*/


ggBoolean mrGrid::viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,        // time ray is sent
             double tmin,        // minimum hit parameter to be searched for
             double tmax,        // maximum hit parameter to be searched for
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{
  double tCellMin = 0;                       // min ray parameter for cell.
  double tCellMax = 0;                       // max ray parameter for cell.
  mrSurface *oPtr = 0;
 

  // ******************************************************
  // *  We must traverse the Grid to see if we hit anything.
  // *  To do this we create a ggGridIterator which will
  // *  set up a traversal of the grid.
  // ******************************************************
    
  ggGridIterator<mrSurface *>  iterator(r, grid,tmin);
  
  
  // ******************************************************
  // * Loop until either we hit an object, t > tmax, or
  // * (o + tv) is outside the grid.
  // ******************************************************
  
  while(iterator.Next(oPtr,tCellMin, tCellMax))
    {
      if(oPtr)
	{
	  tCellMax = ggMin(tmax, tCellMax);
	  tCellMin = ggMax(tCellMin, tmin);
        // ** Check the objects in the list for intersection **
	  if(oPtr->viewingHit(r,time,tCellMin,tCellMax,VHR,MR))
	      return ggTrue;
	}
    }

  return ggFalse;
}
      
ggBoolean mrGrid::shadowHit(        
             const ggRay3& r,    // ray being sent
             double time,        // time ray is sent
             double tmin,        // minimum hit parameter to be searched for
             double tmax,        // maximum hit parameter to be searched for
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const
{

  double tCellMax = 0;                       // max ray parameter for cell.
  double tCellMin = 0;                       // max ray parameter for cell.
  mrSurface *oPtr = 0;
 
  // ******************************************************
  // *  We must traverse the Grid to see if we hit anything.
  // *  To do this we create a ggGridIterator which will
  // *  set up a traversal of the grid.
  // ******************************************************
    
  ggGridIterator<mrSurface *>  iterator(r, grid,tmin);

  
  // ******************************************************
  // * Loop until either we hit an object, t > tmax, or
  // * (o + tv) is outside the grid.
  // ******************************************************
  
  while(iterator.Next(oPtr,tCellMin, tCellMax))
    {
      if(oPtr)
	{ 
	  tCellMax = ggMin(tmax, tCellMax);
	  tCellMin = ggMax(tCellMin, tmin);
        // ** Check the objects in the list for intersection **
	  if(oPtr->shadowHit(r,time,tCellMin,tCellMax,t,N,emits,E)) 
	      return ggTrue;
	}
    }
  return ggFalse;
}

  
      

ggBoolean mrGrid::print(ostream& s) const
{
  int i,j,k,nx = 0,ny = 0, nz = 0;
  
  grid.cells(nx,ny,nz);
      s<< "Grid size = "<<nx<<" by "<<ny<<" by "<<nz<<endl;
      for(i = 0; i < nx; i++)
	for(j = 0; j < ny; j++)
	  for(k = 0; k < nz; k++)
	    {
	      if(grid(i, j, k) != 0)
		{
		  s << "cell["<<i<<"]["<<j<<"]["<<k<<"] = ";
		  (grid(i, j, k))->print(s);
		  s << endl;
		}
	    }
      return ggTrue;
}

ggBoolean mrGrid::boundingBox(double ,    // low end of time range
	                     double ,    // low end of time range
	                     ggBox3& box)
	                     const			  
{
  // return list->boundingBox(time1, time2, box);
     box = bbox;
     return ggTrue;
}
