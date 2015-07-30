#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "basetype.h"

/*
 * createSphereTriangle ����һ����
 * �����ǵ�n=1ʱ����һ���ȱߵ������壬�����ĵ�����ľ���Ϊr
 * n=2ʱ�����������һ��������Ϊ���棬�����ǵ����Ĵ��ߺ�����
 * �Ľ���Ϊ��������3�������Ρ�n=3ʱ�������ò�����
 */
surface_t * createSphereTriangle(float r,int n);

// �ͷű���
void deleteSurface(surface_t *surface);

#endif