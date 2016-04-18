#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "basetype.h"

/*
 * createSphereTriangle 创建一个球
 * 创建一个球体，中心位置在（0，0，0），u,v分别是经纬的密度
 */
surface_t * createSphere(float r,int u,int v);

/*
 * createBox 创建一个盒子
 * 盒子的长宽高由v指定
 */
surface_t * createBox(vec3_t v);

// 释放表面
void deleteSurface(surface_t *surface);

#endif