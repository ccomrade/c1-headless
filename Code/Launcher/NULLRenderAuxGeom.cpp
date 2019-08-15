/**
 * @file
 * @brief Implementation of RenderAuxGeom for NULL renderer.
 */

// Launcher headers
#include "NULLRenderAuxGeom.h"

void CNULLRenderAuxGeom::SetRenderFlags( const SAuxGeomRenderFlags & renderFlags )
{
}

SAuxGeomRenderFlags CNULLRenderAuxGeom::GetRenderFlags()
{
	return m_flags;
}

void CNULLRenderAuxGeom::DrawPoint( const Vec3 & v, const ColorB & col, uint8 size )
{
}

void CNULLRenderAuxGeom::DrawPoints( const Vec3 *v, uint32 numPoints, const ColorB & col, uint8 size )
{
}

void CNULLRenderAuxGeom::DrawPoints( const Vec3 *v, uint32 numPoints, const ColorB *col, uint8 size )
{
}

void CNULLRenderAuxGeom::DrawLine( const Vec3 & v0, const ColorB & colV0,
                                   const Vec3 & v1, const ColorB & colV1, float thickness )
{
}

void CNULLRenderAuxGeom::DrawLines( const Vec3 *v, uint32 numPoints, const ColorB & col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawLines( const Vec3 *v, uint32 numPoints, const ColorB *col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawLines( const Vec3 *v, uint32 numPoints, const uint16 *ind,
                                    uint32 numIndices, const ColorB & col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawLines( const Vec3 *v, uint32 numPoints, const uint16 *ind,
                                    uint32 numIndices, const ColorB *col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawPolyline( const Vec3 *v, uint32 numPoints, bool closed, const ColorB & col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawPolyline( const Vec3 *v, uint32 numPoints, bool closed, const ColorB *col, float thickness )
{
}

void CNULLRenderAuxGeom::DrawTriangle( const Vec3 & v0, const ColorB & colV0,
                                       const Vec3 & v1, const ColorB & colV1,
                                       const Vec3 & v2, const ColorB & colV2 )
{
}

void CNULLRenderAuxGeom::DrawTriangles( const Vec3 *v, uint32 numPoints, const ColorB & col )
{
}

void CNULLRenderAuxGeom::DrawTriangles( const Vec3 *v, uint32 numPoints, const ColorB *col )
{
}

void CNULLRenderAuxGeom::DrawTriangles( const Vec3 *v, uint32 numPoints, const uint16 *ind, uint32 numIndices,
                                        const ColorB & col )
{
}

void CNULLRenderAuxGeom::DrawTriangles( const Vec3 *v, uint32 numPoints, const uint16 *ind, uint32 numIndices,
                                        const ColorB *col )
{
}

void CNULLRenderAuxGeom::DrawAABB( const AABB & aabb, bool bSolid,
                                   const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle )
{
}

void CNULLRenderAuxGeom::DrawAABB( const AABB & aabb, const Matrix34 & matWorld, bool bSolid,
                                   const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle )
{
}

void CNULLRenderAuxGeom::DrawOBB( const OBB & obb, const Vec3 & pos, bool bSolid,
                                  const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle )
{
}

void CNULLRenderAuxGeom::DrawOBB( const OBB & obb, const Matrix34 & matWorld, bool bSolid,
                                  const ColorB & col, const EBoundingBoxDrawStyle & bbDrawStyle )
{
}

void CNULLRenderAuxGeom::DrawSphere( const Vec3 & pos, float radius, const ColorB & col, bool drawShaded )
{
}

void CNULLRenderAuxGeom::DrawCone( const Vec3 & pos, const Vec3 & dir, float radius, float height,
                                   const ColorB & col, bool drawShaded )
{
}

void CNULLRenderAuxGeom::DrawCylinder( const Vec3 & pos, const Vec3 & dir, float radius, float height,
                                       const ColorB & col, bool drawShaded )
{
}

void CNULLRenderAuxGeom::DrawBone( const QuatT & rParent, const QuatT & rBone, ColorB col )
{
}

void CNULLRenderAuxGeom::DrawBone( const Matrix34 & rParent, const Matrix34 & rBone, ColorB col )
{
}

void CNULLRenderAuxGeom::Flush( bool discardGeometry )
{
}

