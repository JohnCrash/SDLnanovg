#include "bitree.h"
#include <stdio.h>
#include <stdlib.h>

/**
* \addtogroup bitree bitree bitree
* \brief ���ַ��Կռ���зָ����
*
* ���κ�һ����v��ʾΪ���½ṹ,v = 2^m*(2n+1)������m=0,1,2,3...,n=0,1,2,3
* ��ô���n = 0,2,4...��ôv����֦�����n = 1,3,5...��ôv����֦��
* ֵv������֦��v-2^(m-1)��v������֦��v+2^(m-1)��v�ĸ��ڵ�ֵ��v+-2^m���v����ֵ��+����ֵ��-
* @{
*/
int pow2table[] = {
	1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8,
	1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15, 1 << 16,
	1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24,
	1 << 25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1 << 30, -(1 << 30)
};

/**
 * \breif ��һ��ֵ�����ַֽ⣬v = 2^m(2n+1) ,m=0,1,2..,n=0,1,2..
 * \param m ����mֵ
 * \param n ����nֵ
 */
void bi_decompose(int v, int *m, int *n)
{
	int i = 0;
	if (v == 0){
		*m = 31;
		*n = 0;
		return;
	}
	while (!(v & 1)){
		v >>= 1;
		i++;
	}
	*m = i;
	*n = (v - 1) >> 1;
}

/**
 * \breif ���ַ�������ֵ��Ӧ�Ľڵ�ķ�Χ
 * \param minx ������Сֵ
 * \param maxx �������ֵ
 */
void bi_range(int v, int *minx, int *maxx)
{
	int m, n, pm, pn;
	if (v == 0){
		*minx = -(1 << 31 - 1);
		*maxx = (1 << 31 - 1);
	}
	else{
		bi_decompose(v, &m, &n);
		*minx = pow2table[m + 1] * n + 1;
		*maxx = pow2table[m + 1] * (n + 1) - 1;
	}
}

/**
 * \brief ����һ�����ڵ㣬�ýڵ��ֵΪv
 * \return �ɹ��������ڵ㣬ʧ�ܷ���NULL
 */
bitree * bitree_create(int v)
{
	bitree* pi = (bitree*)malloc(sizeof(bitree));
	if (pi){
		memset(pi, 0, sizeof(bitree));
		pi->value = v;
	}
	return pi;
}

/**
 * \brief ɾ���������ӵ�ȫ���ڵ�
 * \param pbt �����ڵ����
 */
void bitree_delete(bitree * pbt)
{
}

/**
 * \brief �����ϼ���һ���ڵ㣬�����ýڵ����ӵ�֦����
 * \param pbt ��֦���ϵ�����һ���ڵ�
 * \param v Ҫ����Ľڵ�ֵ������Ѿ����ھ�ֱ�ӷ��ظýڵ�
 */
bitree * bitree_add(bitree * pbt, int v)
{
	int m, n;
	bitree * it = pbt;
	while (it){
		if (bitree_ischild(it, v))
			break;
		else if (!it->p){
			bi_decompose(it->value, &m, &n);
			it->p = bitree_create(BI_PARENT_VALUE(it->value, m,n));
			if (n % 2)
				it->p->r = it;
			else
				it->p->l = it;
		}
		it = it->p;
	}
	/*
	 * ���²���û�о���ӽڵ�
	 */
	while (it){
		if (it->value > v){
			if (!it->l){	
				bi_decompose(it->value, &m, &n);
				it->l = bitree_create(BI_LEFT_VALUE(it->value,m));
				it->l->p = it;
			}
			it = it->l;
		}
		else if (it->value < v){
			if (!it->r){
				int m, n;
				bi_decompose(it->value, &m, &n);
				it->r = bitree_create(BI_RIGHT_VALUE(it->value,m));
				it->r->p = it;
			}
			it = it->r;
		}
		else
			return it;
	}
	return it;
}

/**
 * \brief ���v��pbt��һ���ӽڵ㷵��1������0.
 * \param pbt �������ڵ�
 * \param v	һ������ֵ
 */
int bitree_ischild(bitree * pbt, int v)
{
	int minx, maxx;
	bi_range(pbt->value, &minx, &maxx);
	return v >= minx && v <= maxx;
}

/**
 * \brief ���ظ���ֵ�����ڵ�
 * \param pbt �������
 * \param v Ҫ����������ֵ
 */
bitree * bitree_at(bitree * pbt, int v)
{
	bitree * it = pbt;

	/* 
	 * �������������ҵ���һ������v�Ľڵ�
	 */
	while (it && !bitree_ischild(it,v)){
		it = it->p;
	}
	/*
	 * ���������v�Ľڵ�����������С��value��������������value��������
	 */
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

/*
 * �ҵ�һ����ӽ�vֵ�Ľڵ�
 */
static bitree * bitree_near(bitree * pbt, int v)
{
	bitree * it = pbt;
	bitree * near;
	/*
	 * �������������ҵ���һ������v�Ľڵ�
	 */
	while (it && !bitree_ischild(it, v)){
		it = it->p;
	}
	/*
	 * ���������v�Ľڵ�����������С��value��������������value��������
	 */
	near = it;
	while (it){
		near = it;
		if (it->value > v)
			it = it->l;
		else if (it->value < v)
			it = it->r;
		else
			return it;
	}
	return near;
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
	bitree * it = pbt;

	return it;
}

/**
 * @}
 */