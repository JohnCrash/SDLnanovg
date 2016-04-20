#include "geometry.h"

surface_t * createSphereTriangle(float r, int u, int v)
{
	return 0;
}

surface_t * createBox(vec3_t v)
{
	surface_t * psur;

	psur = allocSurface(8, 12);
	/*
	 *
	 */
	short s[] = {1,-1,-1,1};
	short t[] = {1,1,-1,-1};
	for (int i = 0; i < 8; i++){
		vectorSet(psur->vertex[i], s[i%4]*v[0], t[i%4]*v[1], (i<4?1:-1)*v[2]);
		vectorCopy(psur->vertex[i], psur->normal[i]);
		normalize(psur->normal[i]);
	}
	return psur;
}

surface_t * allocSurface(int nvec, int ntrians)
{
	surface_t * psur;
	psur = malloc(sizeof(surface_t));
	psur->v_count = sizeof(vec3_t)* nvec;
	psur->vertex = malloc(psur->v_count);
	psur->normal = malloc(psur->v_count);
	psur->t_count = sizeof(vec3short_t)* ntrians;
	psur->trians = malloc(psur->t_count);
	return psur;
}

void deleteSurface(surface_t *surface)
{
	if (surface){
		free(surface->normal);
		free(surface->vertex);
		free(surface->trians);
	}
	free(surface);
}