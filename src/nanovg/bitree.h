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

#endif