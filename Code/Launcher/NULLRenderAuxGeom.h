/**
 * @file
 * @brief RenderAuxGeom for NULL renderer.
 */

#pragma once

// CryEngine headers
#include "Cry_Math.h"
#include "Cry_Geo.h"
#include "Cry_Color.h"
#include "IRenderAuxGeom.h"

/**
 * @brief Empty IRenderAuxGeom implementation used as replacement of the existing RenderAuxGeom in CryRenderNULL.
 * It's here only to prevent server crash because some IRenderAuxGeom calls are done even on dedicated server.
 */
class CNULLRenderAuxGeom : public IRenderAuxGeom
{
	SAuxGeomRenderFlags m_flags;

public:
	CNULLRenderAuxGeom()
	: m_flags()
	{
	}

	void SetRenderFlags( const SAuxGeomRenderFlags & renderFlags ) override;
	SAuxGeomRenderFlags GetRenderFlags() override;

	void DrawPoint( const Vec3 & v, const ColorB & col, uint8 size = 1 ) override;

	void DrawPoints( const Vec3 *v, uint32 numPoints, const ColorB & col, uint8 size = 1 ) override;
	void DrawPoints( const Vec3 *v, uint32 numPoints, const ColorB *col, uint8 size = 1 ) override;

	void DrawLine( const Vec3 & v0, const ColorB & colV0,
	               const Vec3 & v1, const ColorB & colV1, float thickness = 1.0f ) override;

	void DrawLines( const Vec3 *v, uint32 numPoints, const ColorB & col, float thickness = 1.0f ) override;
	void DrawLines( const Vec3 *v, uint32 numPoints, const ColorB *col, float thickness = 1.0f ) override;
	void DrawLines( const Vec3 *v, uint32 numPoints, const uint16 *ind,
	                uint32 numIndices, const ColorB & col, float thickness = 1.0f ) override;
	void DrawLines( const Vec3 *v, uint32 numPoints, const uint16 *ind,
	                uint32 numIndices, const ColorB *col, float thickness = 1.0f ) override;

	void DrawPolyline( const Vec3 *v, uint32 numPoints, bool closed, const ColorB & col, float thickness = 1.0f ) override;
	void DrawPolyline( const Vec3 *v, uint32 numPoints, bool closed, const ColorB *col, float thickness = 1.0f ) override;

	void DrawTriangle( const Vec3 & v0, const ColorB & colV0,
	                   const Vec3 & v1, const ColorB & colV1,
	                   const Vec3 & v2, const ColorB & colV2 ) override;

	void DrawTriangles( const Vec3 *v, uint32 numPoints, const ColorB & col ) override;
	void DrawTriangles( const Vec3 *v, uint32 numPoints, const ColorB *col ) override;
	void DrawTriangles( const Vec3 *v, uint32 numPoints, const uint16 *ind, uint32 numIndices,
	                    const ColorB & col ) override;
	void DrawTriangles( const Vec3 *v, uint32 numPoints, const uint16 *ind, uint32 numIndices,
	                    const ColorB *col ) override;

	void DrawAABB( const AABB & aabb, bool bSolid,
	               const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle ) override;
	void DrawAABB( const AABB & aabb, const Matrix34 & matWorld, bool bSolid,
	               const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle ) override;

	void DrawOBB( const OBB & obb, const Vec3 & pos, bool bSolid,
	              const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle ) override;
	void DrawOBB( const OBB & obb, const Matrix34 & matWorld, bool bSolid,
	              const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle ) override;

	void DrawSphere( const Vec3 & pos, float radius, const ColorB & col, bool drawShaded = true ) override;

	void DrawCone( const Vec3 & pos, const Vec3 & dir, float radius, float height,
	               const ColorB & col, bool drawShaded = true ) override;

	void DrawCylinder( const Vec3 & pos, const Vec3 & dir, float radius, float height,
	                   const ColorB & col, bool drawShaded = true ) override;

	void DrawBone( const QuatT & rParent, const QuatT & rBone, ColorB col ) override;
	void DrawBone( const Matrix34 & rParent, const Matrix34 & rBone, ColorB col ) override;

	void Flush( bool discardGeometry = true ) override;
};

