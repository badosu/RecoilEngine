/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "SmokeProjectile.h"
#include "WreckProjectile.h"
#include "Game/Camera.h"
#include "Map/Ground.h"
#include "Rendering/Colors.h"
#include "Rendering/Env/Particles/ProjectileDrawer.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/ProjectileMemPool.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CWreckProjectile, CProjectile, )
CR_REG_METADATA(CWreckProjectile, )


CWreckProjectile::CWreckProjectile(CUnit* owner, float3 pos, float3 speed, float temperature)
	: CProjectile(pos, speed, owner, false, false, false)
{
	checkCol = false;
	drawRadius = 2.0f;
}

void CWreckProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	speed.y += mygravity;
	speed.x *= 0.994f;
	speed.z *= 0.994f;

	if (speed.y > 0.0f)
		speed.y *= 0.998f;

	pos += speed;

	{
		std::scoped_lock<spring::mutex> lock(mut);
		if (!(gs->frameNum & (projectileHandler.GetParticleSaturation() < 0.5f ? 1 : 3))) {
			CSmokeProjectile* hp = projMemPool.alloc<CSmokeProjectile>(owner(), pos, ZeroVector, 50, 4, 0.3f, 0.5f);
			hp->size += 0.1f;
		}
	}
	deleteMe |= (pos.y + 0.3f < CGround::GetApproximateHeight(pos.x, pos.z));
}

void CWreckProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	unsigned char col[4];
	col[0] = (unsigned char) (0.15f * 200);
	col[1] = (unsigned char) (0.1f  * 200);
	col[2] = (unsigned char) (0.05f * 200);
	col[3] = 200;

	const auto* wt = projectileDrawer->wrecktex;
	AddEffectsQuad<0>(
		wt->pageNum,
		{ drawPos - camera->GetRight() * drawRadius - camera->GetUp() * drawRadius, wt->xstart, wt->ystart, col },
		{ drawPos + camera->GetRight() * drawRadius - camera->GetUp() * drawRadius, wt->xend,   wt->ystart, col },
		{ drawPos + camera->GetRight() * drawRadius + camera->GetUp() * drawRadius, wt->xend,   wt->yend,   col },
		{ drawPos - camera->GetRight() * drawRadius + camera->GetUp() * drawRadius, wt->xstart, wt->yend,   col }
	);
}

void CWreckProjectile::DrawOnMinimap() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	AddMiniMapVertices({ pos        , color4::redA }, { pos + speed, color4::redA });
}

int CWreckProjectile::GetProjectilesCount() const
{
	return 1;
}
