#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include "basetype.h"

/*
 * createSphereTriangle ����һ����
 * ����һ�����壬����λ���ڣ�0��0��0����u,v�ֱ��Ǿ�γ���ܶ�
 */
surface_t * createSphere(float r,int u,int v);

/*
 * createBox ����һ������
 * ���ӵĳ������vָ��
 */
surface_t * createBox(vec3_t v);

// �ͷű���
void deleteSurface(surface_t *surface);

#endif