#ifndef __BASETYPE_H__
#define __BASETYPE_H__

/* Vectors */
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];
typedef short vec3short_t[3];
typedef short vec2short_t[2];

typedef struct color_t{
	union {
		float rgba[4];
		struct {
			float r, g, b, a;
		};
	};
} color_t;

/*
 * 下面的宏用来操作vec3_t
 * vectorClear(a)				将分量都置0
 * vectorNegate( a, b )		将a的分量取负赋予b (b = -a)
 * vectorSet( v, x, y, z )	将x,y,z赋予v对应的分量
 * vectorInverse( a )		将a的各分量取负赋予a (a = -a)
 */
#define vectorClear( a )		( (a)[ 0 ] = (a)[ 1 ] = (a)[ 2 ] = 0 )
#define vectorNegate( a, b )	( (b)[ 0 ] = -(a)[ 0 ], (b)[ 1 ] = -(a)[ 1 ], (b)[ 2 ] = -(a)[ 2 ] )
#define vectorSet( v, x, y, z )	( (v)[ 0 ] = ( x ), (v)[ 1 ] = ( y ), (v)[ 2 ] = ( z ) )
#define vectorInverse( a )		( (a)[ 0 ] = (-a)[ 0 ], (a)[ 1 ] = (-a)[ 1 ], (a)[ 2 ] = (-a)[ 2 ] )

/*
 * DotProduct( x, y )					求x,y的点积 (x·y)
 * vectorSubtract( a, b, c )			向量差 c = a - b
 * vectorAdd( a, b, c )					向量加 c = a + b
 * vectorCopy( a, b )					向量复制 b = a
 * vectorScale( v, s, o )				向量缩放 o = s*v (v是向量，s是标量)
 * vectorMA( v, s, b, o )				射线 o = v + s*b 
 *												(其中v，b是向量，s是标量。v相当于射线起点，b是射线的方向，s是离开原点的距离)
 * vector_Interpolate( vtarget, v1, value, v2 )	向量插值 vtarget = v1*(1-value)+v2*value 
 *																(其中v1,v2是向量，value标量，value取0-1，vtarget在v1和v2之间变化)
 */
#define DotProduct( x, y )			( (x)[ 0 ] * (y)[ 0 ] + (x)[ 1 ] * (y)[ 1 ] + (x)[ 2 ] * (y)[ 2 ] )
#define vectorSubtract( a, b, c )	( (c)[ 0 ] = (a)[ 0 ] - (b)[ 0 ], (c)[ 1 ] = (a)[ 1 ] - (b)[ 1 ], (c)[ 2 ] = (a)[ 2 ] - (b)[ 2 ] )
#define vector2Subtract( a, b, c )	( (c)[ 0 ] = (a)[ 0 ] - (b)[ 0 ], (c)[ 1 ] = (a)[ 1 ] - (b)[ 1 ])
#define vectorAdd( a, b, c )		( (c)[ 0 ] = (a)[ 0 ] + (b)[ 0 ], (c)[ 1 ] = (a)[ 1 ] + (b)[ 1 ], (c)[ 2 ] = (a)[ 2 ] + (b)[ 2 ] )
#define vector2Add( a, b, c )	( (c)[ 0 ] = (a)[ 0 ] + (b)[ 0 ], (c)[ 1 ] = (a)[ 1 ] + (b)[ 1 ])
#define vectorCopy( a, b )			( (b)[ 0 ] = (a)[ 0 ], (b)[ 1 ] = (a)[ 1 ], (b)[ 2 ] = (a)[ 2 ] )
#define vector2Copy( a, b )			( (b)[ 0 ] = (a)[ 0 ], (b)[ 1 ] = (a)[ 1 ] )
#define	vectorScale( v, s, o )		( (o)[ 0 ] = (v)[ 0 ] * (s),(o)[ 1 ] = (v)[ 1 ] * (s), (o)[ 2 ] = (v)[ 2 ] * (s) )
#define	vectorMA( v, s, b, o )		( (o)[ 0 ] = (v)[ 0 ] + (b)[ 0 ]*(s),(o)[ 1 ] = (v)[ 1 ] + (b)[ 1 ] * (s),(o)[ 2 ] = (v)[ 2 ] + (b)[ 2 ] * (s) )

#define vector_Interpolate( vtarget, v1, value, v2 ) ( (vtarget)[0] = (v1)[0] * (1 - (value)) + (v2)[0] * (value),   (vtarget)[1] = (v1)[1] * (1 - (value)) + (v2)[1] * (value)  , (vtarget)[2] = (v1)[2] * (1 - (value)) + (v2)[2] * (value)  )

/*
 * vectorCrossProduct(v1,v2,cross)			向量的叉积 (cross = v1 x v2)
 * normalize(v)										单位化v
 * vectorInterpolate(v1,v2,t,dest)				向量插值（dest = (1 - f)*v1 + f *v2 ，和vector_Interpolate效果一样)
 */
void vectorCrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
void normalize(vec3_t v);
void vectorInterpolate(const vec3_t v1, const vec3_t v2, float t, vec3_t dest);

typedef int triangle_t[3];

typedef struct surface_t{
	int	 v_count;			//顶点数
	vec3_t *vertex;		//顶点
	vec3_t *normal;		//顶点的法向量
	int t_count;			//三角数
	triangle_t *trians;	//三角
} surface_t;

surface_t * allocSurface(int nvec,int ntrians);
void deleteSurface(surface_t *surface);

#endif