#ifndef _BITREE_H_
#define _BITREE_H_

typedef struct bitree_t{
	struct bitree_t * p;
	struct bitree_t * l;
	struct bitree_t * r;
	int value;
	void * obj;
} bitree;

bitree * bitree_create();
void bitree_delete(bitree * pbt);
bitree * bitree_add(bitree * pbt, int v);
bitree * bitree_at(bitree * pbt, int v);
bitree * bitree_root(bitree * pbt);
int bitree_ischild(bitree * pbt,int v);
void bitree_range(bitree * pbt, int v0, int v1, void(*cb)(bitree *it));

int pow2table[];
void bi_decompose(int v, int *m, int *n);
void bi_range(int v, int *minx, int *maxx);

/*
 * 将大于0和小于0的连接到0节点上,这样将整个int全部整合在一个棵树上
 */
#define BI_PARENT_VALUE(v,m,n) (m >= 30 ? 0:(n % 2 ? v - pow2table[m] : v + pow2table[m]))
#define BI_LEFT_VALUE(v,m) (m == 31 ? pow2table[31] : v - pow2table[m - 1])
#define BI_RIGHT_VALUE(v,m) (m == 31 ? -pow2table[31] : v + pow2table[m - 1])
#define BI_VALUE(m,n) (m == 31 ?0:pow2table[m]*(2 * n + 1))

#endif