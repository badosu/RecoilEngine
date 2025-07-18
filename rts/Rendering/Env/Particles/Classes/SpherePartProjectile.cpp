/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "SpherePartProjectile.h"

#include "Rendering/GlobalRendering.h"
#include "Rendering/Env/Particles/ProjectileDrawer.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Projectiles/ExpGenSpawnableMemberInfo.h"
#include "Sim/Projectiles/ProjectileMemPool.h"
#include "System/SpringMath.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CSpherePartProjectile, CProjectile, )

CR_REG_METADATA(CSpherePartProjectile, (
	CR_MEMBER(centerPos),
	CR_MEMBER(vectors),
	CR_MEMBER(color),
	CR_MEMBER(sphereSize),
	CR_MEMBER(expansionSpeed),
	CR_MEMBER(xbase),
	CR_MEMBER(ybase),
	CR_MEMBER(baseAlpha),
	CR_MEMBER(age),
	CR_MEMBER(ttl),
	CR_MEMBER(texx),
	CR_MEMBER(texy)
))


CSpherePartProjectile::CSpherePartProjectile(
	const CUnit* owner,
	const float3& centerPos,
	int xpart,
	int ypart,
	float expansionSpeed,
	float alpha,
	int ttl,
	const float3& color
):
	CProjectile(centerPos, ZeroVector, owner, false, false, false),
	centerPos(centerPos),
	color(color),
	sphereSize(expansionSpeed),
	expansionSpeed(expansionSpeed),
	xbase(xpart),
	ybase(ypart),
	baseAlpha(alpha),
	age(0),
	ttl(ttl)
{
	deleteMe = false;
	checkCol = false;

	for(int y = 0; y < 5; ++y) {
		const float yp = (y + ypart) / 16.0f*math::PI - math::HALFPI;
		for (int x = 0; x < 5; ++x) {
			float xp = (x + xpart) / 32.0f*math::TWOPI;
			vectors[y*5 + x] = float3(std::sin(xp)*std::cos(yp), std::sin(yp), std::cos(xp)*std::cos(yp));
		}
	}
	pos = centerPos+vectors[12] * sphereSize;

	drawRadius = 60;

	texx = projectileDrawer->sphereparttex->xstart + (projectileDrawer->sphereparttex->xend - projectileDrawer->sphereparttex->xstart) * 0.5f;
	texy = projectileDrawer->sphereparttex->ystart + (projectileDrawer->sphereparttex->yend - projectileDrawer->sphereparttex->ystart) * 0.5f;
}


void CSpherePartProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	deleteMe |= ((age += 1) >= ttl);
	sphereSize += expansionSpeed;
	pos = centerPos + vectors[12] * sphereSize;
}

void CSpherePartProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	unsigned char col0[4];
	unsigned char col1[4];

	const float interSize = sphereSize + expansionSpeed * globalRendering->timeOffset;

	const auto& pageNum = projectileDrawer->sphereparttex->pageNum;

	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			float alpha =
				baseAlpha *
				(1.0f - std::min(1.0f, float(age + globalRendering->timeOffset) / (float) ttl)) *
				(1.0f - std::fabs(y + ybase - 8.0f) / 8.0f * 1.0f);

			col0[0] = (unsigned char) (color.x * 255.0f * alpha);
			col0[1] = (unsigned char) (color.y * 255.0f * alpha);
			col0[2] = (unsigned char) (color.z * 255.0f * alpha);
			col0[3] = ((unsigned char) (40 * alpha)) + 1;

			alpha = baseAlpha * (1.0f - std::min(1.0f, (float)(age + globalRendering->timeOffset) / (float)ttl)) * (1 - std::fabs(y + 1 + ybase - 8.0f) / 8.0f * 1.0f);

			col1[0] = (unsigned char)(color.x * 255.0f * alpha);
			col1[1] = (unsigned char)(color.y * 255.0f * alpha);
			col1[2] = (unsigned char)(color.z * 255.0f * alpha);
			col1[3] = ((unsigned char)(40 * alpha)) + 1;

			AddEffectsQuad<0>(
				pageNum,
				{ centerPos + vectors[y*5 + x    ]     * interSize, texx, texy, col0 },
				{ centerPos + vectors[y*5 + x + 1]     * interSize, texx, texy, col0 },
				{ centerPos+vectors[(y + 1)*5 + x + 1] * interSize, texx, texy, col1 },
				{ centerPos+vectors[(y + 1)*5 + x    ] * interSize, texx, texy, col1 }
			);
		}
	}
}

int CSpherePartProjectile::GetProjectilesCount() const
{
	return 4 * 4;
}

void CSpherePartProjectile::CreateSphere(const CUnit* owner, int ttl, float alpha, float expansionSpeed, float3 pos, float3 color)
{
	RECOIL_DETAILED_TRACY_ZONE;
	for (int y = 0; y < 16; y += 4) {
		for (int x = 0; x < 32; x += 4) {
			projMemPool.alloc<CSpherePartProjectile>(owner, pos, x, y, expansionSpeed, alpha, ttl, color);
		}
	}
}





CSpherePartSpawner::CSpherePartSpawner()
	: alpha(0.0f)
	, ttl(0)
	, expansionSpeed(0.0f)
	, color(ZeroVector)
{
}


CR_BIND_DERIVED(CSpherePartSpawner, CProjectile, )

CR_REG_METADATA(CSpherePartSpawner,
(
	CR_MEMBER_BEGINFLAG(CM_Config),
		CR_MEMBER(alpha),
		CR_MEMBER(ttl),
		CR_MEMBER(expansionSpeed),
		CR_MEMBER(color),
	CR_MEMBER_ENDFLAG(CM_Config)
))

void CSpherePartSpawner::Init(const CUnit* owner, const float3& offset)
{
	RECOIL_DETAILED_TRACY_ZONE;
	CProjectile::Init(owner, offset);
	deleteMe = true;
	CSpherePartProjectile::CreateSphere(owner, ttl, alpha, expansionSpeed, pos, color);
}

int CSpherePartSpawner::GetProjectilesCount() const
{
	return 0;
}


bool CSpherePartSpawner::GetMemberInfo(SExpGenSpawnableMemberInfo& memberInfo)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (CProjectile::GetMemberInfo(memberInfo))
		return true;

	CHECK_MEMBER_INFO_FLOAT (CSpherePartSpawner, alpha         );
	CHECK_MEMBER_INFO_FLOAT (CSpherePartSpawner, expansionSpeed);
	CHECK_MEMBER_INFO_INT   (CSpherePartSpawner, ttl           );
	CHECK_MEMBER_INFO_FLOAT3(CSpherePartSpawner, color         );

	return false;
}
