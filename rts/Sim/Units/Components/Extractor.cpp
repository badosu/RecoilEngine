/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "Extractor.h"

#include "Map/MetalMap.h"
#include "Map/ReadMap.h"
#include "Sim/Ecs/Registry.h"
#include "Sim/Misc/ExtractorHandler.h"
#include "Sim/Misc/QuadField.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/Scripts/UnitScript.h"
#include "Sim/Units/UnitHandler.h"
#include "Sim/Units/UnitDef.h"
#include "System/ContainerUtil.h"

#include "System/Misc/TracyDefs.h"

using namespace UnitComponents;

Extractor::Extractor(CUnit* unit, float extractionRange, float extractionDepth)
		: unit(unit), extractionRange(extractionRange), extractionDepth(extractionDepth) {}


void Extractor::PreInit(const UnitLoadParams& params)
{
	extractionRange = unit->unitDef->extractRange;
	extractionDepth = unit->unitDef->extractsMetal;
}

void Extractor::PostLoad(CUnit* myUnit)
{
	unit = myUnit;
}


/* resets the metalMap and notifies the neighbours */
void Extractor::ResetExtraction()
{
	RECOIL_DETAILED_TRACY_ZONE;
	unit->metalExtract = 0;
	unit->script->ExtractionRateChanged(unit->metalExtract);

	// undo the extraction-area
	for (auto si = metalAreaOfControl.begin(); si != metalAreaOfControl.end(); ++si) {
		metalMap.RemoveExtraction(si->x, si->z, si->extractionDepth);
	}

	metalAreaOfControl.clear();

	// tell the neighbours (if any) to take it over
	for (Extractor* ngb: neighbours) {
		ngb->RemoveNeighbour(this);
		ngb->ReCalculateMetalExtraction();
	}
	neighbours.clear();
}


/* determine if two extraction areas overlap */
bool Extractor::IsNeighbour(Extractor* other)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// circle vs. circle
	return (unit->pos.SqDistance2D(other->unit->pos) < Square(extractionRange + other->extractionRange));
}


void Extractor::FindNeighbours()
{
	RECOIL_DETAILED_TRACY_ZONE;
	QuadFieldQuery qfQuery;
	quadField.GetUnits(qfQuery, unit->pos, extractionRange + extractorHandler.maxExtractionRange);

	for (CUnit* u: *qfQuery.units) {
		if (u == unit)
			continue;

		auto *eb = extractorHandler.TryGetExtractor(u);
		if (eb == nullptr)
			continue;

		if (!IsNeighbour(eb))
			continue;

		AddNeighbour(eb);
	}
}


/* sets the range of extraction for this extractor, also finds overlapping neighbours. */
void Extractor::SetExtractionRangeAndDepth(float range, float depth)
{
	RECOIL_DETAILED_TRACY_ZONE;
	extractionRange = std::max(range, 0.001f);
	extractionDepth = std::max(depth, 0.0f);
	extractorHandler.UpdateMaxExtractionRange(extractionRange);

	// find any neighbouring extractors
	QuadFieldQuery qfQuery;
	quadField.GetUnits(qfQuery, unit->pos, extractionRange + extractorHandler.maxExtractionRange);

	for (CUnit* u: *qfQuery.units) {
		if (u == unit)
			continue;

		auto *eb = extractorHandler.TryGetExtractor(u);
		if (eb == nullptr)
			continue;

		if (!IsNeighbour(eb))
			continue;

		AddNeighbour(eb);
		eb->AddNeighbour(this);
	}

	if (!unit->activated) {
		assert(unit->metalExtract == 0); // when deactivated metalExtract should always be 0

		return;
	}

	// calculate this extractor's area of control and metalExtract amount
	unit->metalExtract = 0;

	const int xBegin = std::max(                   0, (int) ((unit->pos.x - extractionRange) / METAL_MAP_SQUARE_SIZE));
	const int xEnd   = std::min(mapDims.mapx / 2 - 1, (int) ((unit->pos.x + extractionRange) / METAL_MAP_SQUARE_SIZE));
	const int zBegin = std::max(                   0, (int) ((unit->pos.z - extractionRange) / METAL_MAP_SQUARE_SIZE));
	const int zEnd   = std::min(mapDims.mapy / 2 - 1, (int) ((unit->pos.z + extractionRange) / METAL_MAP_SQUARE_SIZE));

	metalAreaOfControl.reserve((xEnd - xBegin + 1) * (zEnd - zBegin + 1));

	// go through the whole (x, z)-square
	for (int x = xBegin; x <= xEnd; x++) {
		for (int z = zBegin; z <= zEnd; z++) {
			// center of metalsquare at (x, z)
			const float3 msqrPos((x + 0.5f) * METAL_MAP_SQUARE_SIZE, unit->pos.y,
													 (z + 0.5f) * METAL_MAP_SQUARE_SIZE);
			const float sqrCenterDistance = msqrPos.SqDistance2D(unit->pos);

			if (sqrCenterDistance < Square(extractionRange)) {
				MetalSquareOfControl msqr;
				msqr.x = x;
				msqr.z = z;
				// extraction is done in a cylinder of height <depth>
				msqr.extractionDepth = metalMap.RequestExtraction(x, z, depth);
				metalAreaOfControl.push_back(msqr);
				unit->metalExtract += msqr.extractionDepth * metalMap.GetMetalAmount(msqr.x, msqr.z);
			}
		}
	}

	// set the COB animation speed
	unit->script->ExtractionRateChanged(unit->metalExtract);
}


/* adds a neighbour for this extractor */
void Extractor::AddNeighbour(Extractor* neighbour)
{
	RECOIL_DETAILED_TRACY_ZONE;
	assert(neighbour != this);
	spring::VectorInsertUnique(neighbours, neighbour, true);
}


/* removes a neighbour for this extractor */
void Extractor::RemoveNeighbour(Extractor* neighbour)
{
	RECOIL_DETAILED_TRACY_ZONE;
	assert(neighbour != this);
	spring::VectorErase(neighbours, neighbour);
}


/* recalculate metalExtract for this extractor (eg. when a neighbour dies) */
void Extractor::ReCalculateMetalExtraction()
{
	RECOIL_DETAILED_TRACY_ZONE;
	unit->metalExtract = 0;

	for (MetalSquareOfControl& msqr: metalAreaOfControl) {
		metalMap.RemoveExtraction(msqr.x, msqr.z, msqr.extractionDepth);

		if (unit->activated) {
			// extraction is done in a cylinder
			msqr.extractionDepth = metalMap.RequestExtraction(msqr.x, msqr.z, extractionDepth);
			unit->metalExtract += (msqr.extractionDepth * metalMap.GetMetalAmount(msqr.x, msqr.z));
		}
	}

	// set the new rotation-speed
	unit->script->ExtractionRateChanged(unit->metalExtract);
}


void Extractor::Activate()
{
	/* Finds the amount of metal to extract and sets the rotationspeed when the extractor is built. */
	SetExtractionRangeAndDepth(extractionRange, extractionDepth);
}


void Extractor::Deactivate()
{
	ResetExtraction();
}

