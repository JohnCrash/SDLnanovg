#include "bitree.h"
#include <stdio.h>
#include <stdlib.h>

/**
* \addtogroup bitree bitree bitree
* \brief ���ַ��Կռ���зָ����
*
* 
* @{
*/
static int pow2table[] = {
	1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8,
	1<<9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15, 1 << 16, 
	1<<17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24,
	1<<25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1 << 30, 1 << 31,
};

#define PARENT_VALUE(v) v
#define LEFT_VALUE(v) v
#define RIGHT_VALUE(v) v

static int bi_level(int v)
{
	int i = 0;
	if (v & 1)return i;
	while (!(v>>=1&1))
		i++;
	return i;
}

bitree * bitree_create(int v)
{
	bitree* pi = (bitree*)malloc(sizeof(bitree));
	if (pi){
		memset(pi, 0, sizeof(bitree));
		pi->value = v;
	}
	return pi;
}

static void bitree_left_delete(bitree * pbt)
{
}

/**
 * \brief ɾ���������ӵ�ȫ���ڵ�
 * \param pbt �����ڵ����
 */
void bitree_delete(bitree * pbt)
{
	bitree * it = pbt;
	bitree * l = it;
	while (it){
		l = it;
		it = it->l;
	}
	bitree_left_delete(l);
}

bitree * bitree_add(bitree * pbt, int v)
{
	bitree * it = pbt;
	bitree * last = it;
	while (it){
		if (v > (it->value >> 1)){
			if (it->p)
				it = it->p;
			else{
				int pv = PARENT_VALUE(it->value);
				it->p = bitree_create(pv);
				if (it->value == LEFT_VALUE(pv))
					it->p->l = it;
				else
					it->p->r = it;
				it = it->p;
			}
		}
		else break;
	}
	while (it){
		if (it->value > v){
			if (!it->l)
				it->l = bitree_create(LEFT_VALUE(it->value));
			it = it->l;
		}
		else if (it->value < v){
			if (!it->r)
				it->r = bitree_create(RIGHT_VALUE(it->value));
			it = it->r;
		}
		else
			return it;
	}
	return it;
}

/**
 * \brief ���ظ���ֵ�����ڵ�
 * \param pbt �������
 * \param v Ҫ����������ֵ
 */
bitree * bitree_at(bitree * pbt, int v)
{
	bitree * it = pbt;

	while( it && v>(pbt->value >> 1)){
		it = it->p;
	}
	while (it){
		if (it->value > v)
			it = it->l;
		else if (it->value < v)
			it = it->r;
		else
			return it;
	}
	return it;
}

/**
 * \brief ���ظ��ڵ�
 * \param pbt �������
 */
bitree * bitree_root(bitree * pbt)
{
	bitree * it = pbt;
	bitree * root = it;
	while (it){
		root = it;
		it = it->p;
	}
	return root;
}

/**
 * \brief ��������ָ����Χ
 * \param pbt ���
 * \param v0 ��ʼ��Χֵ
 * \param v1 ������Χֵ
 * \param cb �����ص��������ҵ�����Ҫ���ֵ�͵��øûص�
 */
void bitree_range(bitree * pbt, int v0, int v1, void(*cb)(bitree *it))
{
	
}

/**
 * @}
 */