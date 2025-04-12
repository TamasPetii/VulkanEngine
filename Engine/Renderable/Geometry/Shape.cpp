#include "Shape.h"

void Shape::Initialize()
{
	PopulateSurfacePoints();
	GenerateVertices();
	GenerateIndices();
	GenerateTangents();
	GenerateBoundingVolume();
	UploadToGpu();
}

void Shape::GenerateTangents()
{
}
