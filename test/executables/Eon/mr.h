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



#include <mrBox.h>
#include <mrDiffuseAreaSphereLuminaire.h>
#include <mrDiffuseAreaTriangleLuminaire.h>
#include <mrDiffuseAreaXYRectangleLuminaire.h>
#include <mrDiffuseAreaXZRectangleLuminaire.h>
#include <mrDiffuseAreaYZRectangleLuminaire.h>
#include <mrDiffuseAreaZCylinderLuminaire.h>
#include <mrDiffuseCosineSphereLuminaire.h>
#include <mrDiffuseCosineZCylinderLuminaire.h>
#include <mrDiffuseSolidAngleSphereLuminaire.h>
#include <mrDiffuseVisibleAreaZCylinderLuminaire.h>
#include <mrEmitter.h>
#include <mrFastTriangle.h>
#include <mrGrid.h>
#include <mrImposter.h>
#include <mrInstance.h>
#include <mrLinkedObjects.h>
#include <mrMaterial.h>
#include <mrSpotAreaXYDiskLuminaire.h>
#include <mrPhongAreaTriangleLuminaire.h>
#include <mrPhongAreaXYRectangleLuminaire.h>
#include <mrPhongAreaXZRectangleLuminaire.h>
#include <mrPhongAreaYZRectangleLuminaire.h>
#include <mrPolygon.h>
#include <mrShellLuminaire.h>
#include <mrSolidTexture.h>
#include <mrSphere.h>
#include <mrSurface.h>
#include <mrSurfaceList.h>
#include <mrSurfaceTexture.h>
#include <mrTriangle.h>
#include <mrXCylinder.h>
#include <mrXEllipticalCylinder.h>
#include <mrXYDisk.h>
#include <mrXYRectangle.h>
#include <mrXZDisk.h>
#include <mrXZRectangle.h>
#include <mrYCylinder.h>
#include <mrYEllipticalCylinder.h>
#include <mrYZDisk.h>
#include <mrYZRectangle.h>
#include <mrZCylinder.h>
#include <mrZEllipticalCylinder.h>
#include <mrCamera.h>
#include <mrCookPixelRenderer.h>
#include <mrKajiyaPixelRenderer.h>
