#include "mesh.h"
#include "../constants/constants.h"

Mesh::Mesh() {
	vertexCount=0;
	indexCount=0;
	vertices=NULL;
	normals=NULL;
	texcoords=NULL;
	materialids = NULL;
	indices=NULL;
}

Mesh::Mesh(const Mesh& rhs) {

}

Mesh::~Mesh() {
	if (vertices) delete[] vertices;
	if (normals) delete[] normals;
	if (texcoords) delete[] texcoords;
	if (materialids) delete[] materialids;
	if (indices) delete[] indices;
	vertices = NULL;
	normals = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
}

