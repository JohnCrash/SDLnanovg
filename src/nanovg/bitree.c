#include "bitree.h"
#include <stdio.h>
#include <stdlib.h>

/**
* \addtogroup bitree bitree bitree
* \brief 二分法对空间进行分割查找
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

int bi_level(int v)
{
	int i = 0;
	if (v & 1 || v == 0)return i;
	i++;
	while (!((v >>= 1) & 1))
		i++;
	return i;
}

#define PARENT_VALUE(v) v
#define LOW_VALUE(v) v
#define LEFT_VALUE(v) (v - pow2table[bi_level(v) - 1])
#define RIGHT_VALUE(v) (v + pow2table[bi_level(v) - 1])

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
 * \brief 删除树相连接的全部节点
 * \param pbt 搜索节点起点
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
 * \brief 如果v是pbt的一个子节点返回1，否则0.
 * \param pbt 二分树节点
 * \param v	一个整数值
 */
int bitree_ischild(bitree * pbt, int v)
{
	return (v < PARENT_VALUE(pbt->value)) && (v > LOW_VALUE(pbt->value));
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
	while (it && bitree_ischild(it,v)){
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
	while (it && bitree_ischild(it, v)){
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