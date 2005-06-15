################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

bin_PROGRAMS = eon

eon_CXXFLAGS = \
	-g -gdwarf-2 -DSPEC_CPU2000_LP64 -DHAS_ERRLIST -DFMAX_IS_DOUBLE  -I. -DNDEBUG 

eon_LDADD = \
	-ldl

eon_SOURCES = \
	eon.cc eonImageCalculator.cc ggBoardFloorSolidTexture.cc ggBox2.cc ggBox3.cc ggBoxPixelFilter.cc ggBRDF.cc \
	ggCamera.cc ggConductorMaterial.cc ggCoverageSolidTexture.cc \
	ggDielectricMaterial.cc ggDiffuseBRDF.cc ggDiffuseMaterial.cc ggEdgeDiscrepancy.cc ggErr.cc ggFineSpectrum.cc \
	ggFormat.cc ggFrame2.cc ggFrame3.cc ggFrameInterpolation.cc ggGamma.cc ggGeometry.cc ggGrayPixel_x.cc \
	ggHAffineMatrix3.cc ggHBoxMatrix3.cc ggHMatrix3.cc ggHPerspectiveMatrix3.cc ggHPoint3.cc ggHReflectionMatrix3.cc ggHRigidBodyMatrix3.cc \
	ggHRotationMatrix3.cc ggHScaleMatrix3.cc ggHShearMatrix3.cc ggHTranslationMatrix3.cc ggJitterSample1.cc \
	ggJitterSample2.cc ggMaterial.cc \
	ggNA.cc ggNRooksSample2.cc ggOakSolidTexture.cc ggONB2.cc ggONB3.cc ggOptics.cc ggPathDielectricMaterial.cc \
	ggPhongBRDF.cc ggPinholeCamera.cc ggPixelFilter.cc ggPlane.cc ggPoint2.cc ggPoint3.cc \
	ggPolishedBRDF.cc ggPolishedMaterial.cc ggPolygon.cc ggQuaternion.cc ggRandomSample2.cc ggRandomSample3.cc \
	ggRasterSurfaceTexture.cc ggRay2.cc ggRay3.cc ggRGBE.cc \
	ggRGBFPixel.cc ggRGBPixel_x.cc ggRotatingPinholeCamera.cc ggSolidNoise2.cc ggSolidNoise3.cc ggSolidTexture.cc ggSpectrum.cc \
	ggSpecularMaterial.cc ggSphere.cc ggSpline.cc ggString.cc ggThinLensCamera.cc  \
	ggTrianglePixelFilter.cc ggVector2.cc ggVector3.cc mrBox.cc mrBruteForcePixelRenderer.cc mrCamera.cc mrCoarsePixelRenderer.cc \
	mrCookPixelRenderer.cc mrDiffuseAreaSphereLuminaire.cc mrDiffuseAreaTriangleLuminaire.cc mrDiffuseAreaXYRectangleLuminaire.cc mrDiffuseAreaXZRectangleLuminaire.cc \
	mrDiffuseAreaYZRectangleLuminaire.cc mrDiffuseAreaZCylinderLuminaire.cc mrDiffuseCosineSphereLuminaire.cc mrDiffuseCosineZCylinderLuminaire.cc \
	mrDiffuseSolidAngleSphereLuminaire.cc mrDiffuseVisibleAreaZCylinderLuminaire.cc mrEmitter.cc mrFastTriangle.cc \
	mrGrid.cc mrImposter.cc mrIndirectPixelRenderer.cc mrInstance.cc mrKajiyaPixelRenderer.cc mrLinkedObjects.cc \
	mrMaterial.cc mrObjectRecord.cc mrPhongAreaTriangleLuminaire.cc mrPhongAreaXYRectangleLuminaire.cc mrPhongAreaXZRectangleLuminaire.cc \
	mrPhongAreaYZRectangleLuminaire.cc mrPolygon.cc mrRushmeierPixelRenderer.cc mrScene.cc mrShellLuminaire.cc mrSolidTexture.cc \
	mrSphere.cc mrSpotAreaXYDiskLuminaire.cc mrSurface.cc mrSurfaceList.cc mrSurfaceTexture.cc mrTriangle.cc mrXCylinder.cc \
	mrXEllipticalCylinder.cc mrXYDisk.cc mrXYRectangle.cc mrXZDisk.cc mrXZRectangle.cc mrYCylinder.cc mrYEllipticalCylinder.cc \
	mrYZDisk.cc mrYZRectangle.cc mrZCylinder.cc mrZEllipticalCylinder.cc myrand.cc 

