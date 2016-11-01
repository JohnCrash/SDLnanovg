#include "bitree.h"
#include <stdio.h>
#include <stdlib.h>

/**
* \addtogroup bitree bitree bitree
* \brief 二分法对空间进行分割查找
*
* 将任何一个数v表示为如下结构,v = 2^m*(2n+1)，其中m=0,1,2,3...,n=0,1,2,3
* 那么如果n = 0,2,4...那么v是左枝，如果n = 1,3,5...那么v是右枝。
* 值v的左子枝是v-2^(m-1)，v的右子枝是v+2^(m-1)。v的父节点值是v+-2^m如果v是左值就+，右值就-
* @{
*/
int pow2table[] = {
	1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7, 1 << 8,
	1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15, 1 << 16,
	1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21, 1 << 22, 1 << 23, 1 << 24,
	1 << 25, 1 << 26, 1 << 27, 1 << 28, 1 << 29, 1 << 30, -(1 << 30)
};

/**
 * \breif 对一个值做二分分解，v = 2^m(2n+1) ,m=0,1,2..,n=0,1,2..
 * \param m 返回m值
 * \param n 返回n值
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
 * \breif 二分法，返回值对应的节点的范围
 * \param minx 返回最小值
 * \param maxx 返回最大值
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
 * \brief 创建一个树节点，该节点的值为v
 * \return 成功返回树节点，失败返回NULL
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
 * \brief 删除树相连接的全部节点
 * \param pbt 搜索节点起点
 */
void bitree_delete(bitree * pbt)
{
}

/**
 * \brief 向树上加入一个节点，并将该节点连接到枝干上
 * \param pbt 主枝干上的任意一个节点
 * \param v 要加入的节点值，如果已经存在就直接返回该节点
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
	 * 向下查找没有就添加节点
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
 * \brief 如果v是pbt的一个子节点返回1，否则0.
 * \param pbt 二分树节点
 * \param v	一个整数值
 */
int bitree_ischild(bitree * pbt, int v)
{
	int minx, maxx;
	bi_range(pbt->value, &minx, &maxx);
	return v >= minx && v <= maxx;
}

/**
 * \brief 返回给定值的树节点
 * \param pbt 搜索起点
 * \param v 要搜索的整数值
 */
bitree * bitree_at(bitree * pbt, int v)
{
	bitree * it = pbt;

	/* 
	 * 先向上搜索，找到第一个包括v的节点
	 */
	while (it && !bitree_ischild(it,v)){
		it = it->p;
	}
	/*
	 * 从这个包括v的节点向下搜索，小于value向左搜索，大于value向右搜索
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
 * \brief 返回跟节点
 * \param pbt 搜索起点
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
 * 找到一个最接近v值的节点
 */
static bitree * bitree_near(bitree * pbt, int v)
{
	bitree * it = pbt;
	bitree * near;
	/*
	 * 先向上搜索，找到第一个包括v的节点
	 */
	while (it && !bitree_ischild(it, v)){
		it = it->p;
	}
	/*
	 * 从这个包括v的节点向下搜索，小于value向左搜索，大于value向右搜索
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
 * \brief 遍历树的指定范围
 * \param pbt 起点
 * \param v0 起始范围值
 * \param v1 结束范围值
 * \param cb 遍历回调掉，当找到符合要求的值就调用该回调
 */
void bitree_range(bitree * pbt, int v0, int v1, void(*cb)(bitree *it))
{
	bitree * it = pbt;

	return it;
}

/**
 * @}
 */