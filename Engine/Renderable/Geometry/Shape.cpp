#include "Shape.h"

void Shape::Initialize()
{
	GenerateSurfacePoints();
	GenerateVertices();
	GenerateIndices();
	GenerateTangents();
	UploadToGpu();
}

void Shape::GenerateTangents()
{
}
