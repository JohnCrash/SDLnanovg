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
 * ����ĺ���������vec3_t
 * vectorClear(a)				����������0
 * vectorNegate( a, b )		��a�ķ���ȡ������b (b = -a)
 * vectorSet( v, x, y, z )	��x,y,z����v��Ӧ�ķ���
 * vectorInverse( a )		��a�ĸ�����ȡ������a (a = -a)
 */
#define vectorClear( a )		( (a)[ 0 ] = (a)[ 1 ] = (a)[ 2 ] = 0 )
#define vectorNegate( a, b )	( (b)[ 0 ] = -(a)[ 0 ], (b)[ 1 ] = -(a)[ 1 ], (b)[ 2 ] = -(a)[ 2 ] )
#define vectorSet( v, x, y, z )	( (v)[ 0 ] = ( x ), (v)[ 1 ] = ( y ), (v)[ 2 ] = ( z ) )
#define vectorInverse( a )		( (a)[ 0 ] = (-a)[ 0 ], (a)[ 1 ] = (-a)[ 1 ], (a)[ 2 ] = (-a)[ 2 ] )

/*
 * DotProduct( x, y )					��x,y�ĵ�� (x��y)
 * vectorSubtract( a, b, c )			������ c = a - b
 * vectorAdd( a, b, c )					������ c = a + b
 * vectorCopy( a, b )					�������� b = a
 * vectorScale( v, s, o )				�������� o = s*v (v��������s�Ǳ���)
 * vectorMA( v, s, b, o )				���� o = v + s*b 
 *												(����v��b��������s�Ǳ�����v�൱��������㣬b�����ߵķ���s���뿪ԭ��ľ���)
 * vector_Interpolate( vtarget, v1, value, v2 )	������ֵ vtarget = v1*(1-value)+v2*value 
 *																(����v1,v2��������value������valueȡ0-1��vtarget��v1��v2֮��仯)
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
 * vectorCrossProduct(v1,v2,cross)			�����Ĳ�� (cross = v1 x v2)
 * normalize(v)										��λ��v
 * vectorInterpolate(v1,v2,t,dest)				������ֵ��dest = (1 - f)*v1 + f *v2 ����vector_InterpolateЧ��һ��)
 */
void vectorCrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
void normalize(vec3_t v);
void vectorInterpolate(const vec3_t v1, const vec3_t v2, float t, vec3_t dest);

typedef int triangle_t[3];

typedef struct surface_t{
	int	 v_count;			//������
	vec3_t *vertex;		//����
	vec3_t *normal;		//����ķ�����
	int t_count;			//������
	triangle_t *trians;	//����
} surface_t;

surface_t * allocSurface(int nvec,int ntrians);
void deleteSurface(surface_t *surface);

#endif