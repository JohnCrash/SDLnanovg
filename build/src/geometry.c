#include "geometry.h"

surface_t * createSphereTriangle(float r, int n)
{
	int vertex_count;
	
	vertex_count = n * 4;
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