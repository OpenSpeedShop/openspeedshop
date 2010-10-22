################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute All Rights Reserved.
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

noinst_PROGRAMS = eon

CXXFLAGS = \
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

EXTRA_DIST = \
	assert-local.h bsd_time.h eonImageCalculator.h ggAll.h \
	ggBoardFloorSolidTexture.h ggBoolean.h ggBox2.h ggBox3.h \
	ggBoxPixelFilter.h ggBRDF.h ggBST.h ggCamera.h ggConductorMaterial.h \
	ggConstantMaterial.h ggConstants.h ggCookSample3.h ggCoverageSolidTexture.h \
	ggDictionary.h ggDielectricMaterial.h ggDiffuseBRDF.h ggDiffuseMaterial.h \
	ggEdgeDiscrepancy.h ggErr.h ggFineSpectrum.h ggFormat.h ggFPoint3.h \
	ggFrame2.h ggFrame3.h ggFrameInterpolation.h ggGamma.h ggGE.h ggGeometry.h \
	ggGrayPixel.h ggGrid.h gg.h ggHAffineMatrix3.h ggHBoxMatrix3.h ggHMatrix3.h \
	ggHPerspectiveMatrix3.h ggHPoint3.h ggHReflectionMatrix3.h ggHRigidBodyMatrix3.h \
	ggHRotationMatrix3.h ggHScaleMatrix3.h ggHShearMatrix3.h ggHTranslationMatrix3.h \
	ggIO.h ggIOhdr.h ggJitterSample1.h ggJitterSample2.h ggJitterSample3.h \
	ggJRSample2.h ggJRSample3.h ggList.h ggMacros.h ggMaterial.h ggMetamer.h \
	ggMultiJitterSample2.h ggMultiJitterSample3.h ggNA.h ggNRooksSample2.h \
	ggNRooksSample3.h ggOakSolidTexture.h ggONB2.h ggONB3.h ggOptics.h \
	ggPathDielectricMaterial.h ggPermute.h ggPhongBRDF.h ggPhotometer.h \
	ggPinholeCamera.h ggPixelFilter.h ggPlane.h ggPoint2.h ggPoint3.h \
	ggPoissonDiskSample2.h ggPoissonSphereSample3.h ggPolishedBRDF.h \
	ggPolishedMaterial.h ggPolygon.h ggQuaternion.h ggRandomSample2.h \
	ggRandomSample3.h ggRange.h ggRanNum.h ggRaster.h ggRasterSurfaceTexture.h \
	ggRay2.h ggRay3.h ggRectangle.h ggRegularSample2.h ggRegularSample3.h \
	ggRGBE.h ggRGBFPixel.h ggRGBPixel.h ggRotatingPinholeCamera.h ggSample1.h \
	ggSample2.h ggSample3.h ggSolidNoise2.h ggSolidNoise3.h ggSolidTexture.h \
	ggSpectrum.h ggSpecularMaterial.h ggSphere.h ggSpline.h ggStaticArray.h \
	ggString.h ggThinLensCamera.h ggTimer.h ggTrain.h ggTriangle.h \
	ggTrianglePixelFilter.h ggVector2.h ggVector3.h kai.h mrBox.h \
	mrBruteForcePixelRenderer.h mrCamera.h mrChiuPixelRenderer.h mrCoarsePixelRenderer.h \
	mrCookPixelRenderer.h mrDiffuseAreaSphereLuminaire.h mrDiffuseAreaTriangleLuminaire.h \
	mrDiffuseAreaXYRectangleLuminaire.h mrDiffuseAreaXZRectangleLuminaire.h \
	mrDiffuseAreaYZRectangleLuminaire.h mrDiffuseAreaZCylinderLuminaire.h \
	mrDiffuseCosineSphereLuminaire.h mrDiffuseCosineZCylinderLuminaire.h \
	mrDiffuseSolidAngleSphereLuminaire.h mrDiffuseVisibleAreaZCylinderLuminaire.h \
	mrEmitter.h mrFastTriangle.h mrGrid.h mr.h mrImposter.h mrIndirectPixelRenderer.h \
	mrInstance.h mrKajiyaPixelRenderer.h mrLinkedObjects.h mrMaterial.h \
	mrObjectRecord.h mrPhongAreaTriangleLuminaire.h mrPhongAreaXYRectangleLuminaire.h \
	mrPhongAreaXZRectangleLuminaire.h mrPhongAreaYZRectangleLuminaire.h mrPixelRenderer.h \
	mrPolygon.h mrRushmeierPixelRenderer.h mrScene.h mrShellLuminaire.h mrSolidTexture.h \
	mrSphere.h mrSpotAreaXYDiskLuminaire.h mrSurface.h mrSurfaceList.h mrSurfaceTexture.h \
	mrTriangle.h mrXCylinder.h mrXEllipticalCylinder.h mrXYDisk.h \
	mrXYRectangle.h mrXZDisk.h mrXZRectangle.h mrYCylinder.h mrYEllipticalCylinder.h \
	mrYZDisk.h mrYZRectangle.h mrZCylinder.h mrZEllipticalCylinder.h myrand.h \
	ggBST.cc ggConstantMaterial.cc ggCookSample3.cc ggIO.cc \
	ggJitterSample3.cc ggJRSample2.cc ggJRSample3.cc ggMultiJitterSample2.cc \
	ggMultiJitterSample3.cc ggNRooksSample3.cc ggPermute.cc ggPoissonDiskSample2.cc \
	ggPoissonSphereSample3.cc ggRange.cc ggRanNum.cc ggRaster.cc ggRegularSample2.cc \
	ggRegularSample3.cc ggRGBPixel.cc ggStaticArray.cc ggTimer.cc ggTrain.cc mrChiuPixelRenderer.cc

#eonstatic-pcsamp: eon
#	monitor-link -v -i /home/dpm/oss-demo/lib64/offline_monitor.o \
#		-i /home/dpm/oss-demo/lib64/openspeedshop/libpcsamp-offline.a \
#		g++ -o eonstatic-pcsamp ${eon_OBJECTS} -static -lpthread -lrt -lgcc -ldl -lstdc++

eonstatic-pcsamp: eon
	monitor-link -v \
		-i ../../../libopenss-runtime/offline/.libs/libopenss_runtime_offline_la-offline_monitor.o \
		-i ../../../plugins/collectors/pcsamp/.libs/libpcsamp-offline.a \
		g++ -o eonstatic-pcsamp ${eon_OBJECTS} -static -lpthread -lrt -lgcc -ldl -lstdc++

eonstatic-hwc: eon
	monitor-link -v \
		-i ../../../libopenss-runtime/offline/.libs/libopenss_runtime_offline_la-offline_monitor.o \
		-i ../../../plugins/collectors/hwc/.libs/libhwc-offline.a \
		g++ -o eonstatic-hwc ${eon_OBJECTS} -static -lpthread -lrt -ldl -lpapi

eonstatic-usertime: eon
	monitor-link -v \
		-i ../../../libopenss-runtime/offline/.libs/libopenss_runtime_offline_la-offline_monitor.o \
		-i ../../../plugins/collectors/usertime/.libs/libusertime-offline.a \
		g++ -o eonstatic-usertime ${eon_OBJECTS} -Wl,--allow-multiple-definition --static -lpthread -lrt -ldl -lunwind

eonstatic-hwctime: eon
	monitor-link -v \
		-i ../../../libopenss-runtime/offline/.libs/libopenss_runtime_offline_la-offline_monitor.o \
		-i ../../../plugins/collectors/hwctime/.libs/libhwctime-offline.a \
		g++ -o eonstatic-hwctime ${eon_OBJECTS} -Wl,--allow-multiple-definition -static -lpapi -lpthread -lrt -ldl -lunwind
