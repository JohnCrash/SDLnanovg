#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "basetype.h"

/*
 * createSphereTriangle 创建一个球
 * 方法是当n=1时构造一个等边的三角体，从中心到顶点的距离为r
 * n=2时，以三角体的一个三角面为底面，该三角的中心垂线和球面
 * 的交点为顶点作作3个三角形。n=3时继续做该操作。
 */
surface_t * createSphereTriangle(float r,int n);

// 释放表面
void deleteSurface(surface_t *surface);

#endif