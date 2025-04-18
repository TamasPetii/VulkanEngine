#include "Shape.h"

void Shape::Initialize()
{
	PopulateSurfacePoints();
	GenerateVertices();
	GenerateIndices();
	GenerateTangents();
	GenerateBoundingVolumeFromSurfacePoints();
	UploadToGpu();
}

void Shape::GenerateTangents()
{
}
