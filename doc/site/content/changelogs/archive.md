+++
title = 'Archive'
date = 2025-05-10T01:02:58-07:00
draft = false
+++

## Historical changelogs

Below is a series of changelogs that have been appended to since the first releases of Spring in the mid-2000s, up until Recoil started using dedicated changelog posts. They've been kicking around in a text file for a while, so it seems appropriate to put them in their own post.

> [!NOTE]
> Changelogs are available at [changelogs]({{% ref "changelogs" %}}).
> If you're looking at migrating from upstream Spring 105.0, there is a guide at [migrating-from-spring]({{% ref "migrating-from-spring" %}}),

Spring changelog
(since 85.0 a "!" prefix indicates backward compatibility broke)
(numbers in brackets normally mean the mantis ticket ID)

### BAR105 NEXT (since release 2511)

Lua:

- add `Spring.GetUnitPhysicalState(number unitID) -> Gets the unit's PhysicalState bitmask.
  {{< pull 1528 >}}
- add `Spring.RebuildSmoothMesh` to force an immediate build of the complete Air Smooth Mesh.
  {{< pull 1342 >}}
- Fix #1523, `Spring.UnitArrivedAtGoal` stack issue with warning spam.
  {{< pull 1546 >}}
- DrawWorldPreParticles is now has parameters describing the particular phase it is called within a draw frame.

Misc:

- Fixed broken test. {{< pull 1545 >}}
- New `OverheadMinZoomDistance` and `CamSpringMinZoomDistance` ConfigFloats to set minimum camera zoom distances for their respective cameras.

Pathing:

- HAPFS uses the raw move found in the ground movement code, rather than its on version, which addresses several
  issues. {{< pull 1402 >}}
- HAPFS uses more detailed checks on unit movement to better follow changes in the submerged state to give paths
  that will avoid units getting stuck on underwater obstacles. {{< pull 1402 >}}
- `movement.allowDirectionalPathing` when enabled now allows HAPFS to use directionality in its pathing algorithm.
  {{< pull 1402 >}}
- `movement.preferShortestPath` when enabled allows QTPFS to use the shortest path, rather than the fastest path.
  {{< pull 1402 >}}

Sim:

- Added: `system.smoothMeshResDivider` and `system.smoothMeshSmoothRadius` to control the behaviour of the Air Smooth
  Mesh. {{< pull 1341 >}}
- Added: `float separationDistance` to unitDef, so that units try to keep separationDistance elmos away from each
  other. This avoids having to increase the unit's movedef size, which has other additional consequences.
  {{< pull 1358 >}}
- Terraform activities are now done at slow update rate, rather than every frame. This is back to original behaviour.
  An extra step is added at the end of the job to guarantee that the last of the terrain changes are processed and not
  missed. {{< pull 1360 >}}
- `CGroundMoveType::ChangeHeading` now avoid updating the unit if no rotation is necessary.
  {{< pull 1382 >}}

UI:

- SDL updates in preparation for RmlUI. {{< pull 1419 >}}

### Since release 1544

Lua:

- add `Game.footprintScale`, the footprint multiplier compared to defs
- add `Game.buildSquareSize`, the building alignment grid size
- removed `Game.allowTeamColors` (was deprecated and always `true`)
- `Spring.SetActiveCommand()` or passing explicit `nil` now cancels the command.
  The previous method to pass `-1` still works.
- add `Spring.GetUnitSeismicSignature(unitID) -> number seismicSignature`
- add `Spring.SetUnitSeismicSignature(unitID, seismicSignature) -> nil`
- add `Spring.GetUnitWorkerTask(unitID) -> cmdID, targetID`. Returns a worker's current
  task, represented via commandID. Note that it differs from `Spring.GetUnitCurrentCommand`
  in that a unit may be working without having the relevant order in front of the queue,
  for example via Guard, or alternatively, it may not be working despite having it (for
  example it's not yet in range). It also resolves Repair vs Build commands.
- allow shallow recursion in `Spring.TransferUnit`
- add `Spring.SelectUnit(unitID, bool append = false) -> nil`, single-unit
  version of SelectUnit{Array,Map} that doesn't require a table
- add `Spring.DeselectUnit(unitID) -> nil`, deselects a unit
- add `Spring.SetUnitShieldRechargeDelay(unitID, [weaponNum], [seconds]) -> nil`. Resets a unit's
  shield regeneration delay. The weapon number is optional if the unit has a single shield. The
  timer value is also optional, if you leave it nil it will emulate a weapon hit. Note that a
  weapon hit (both via nil here, and "real" hits) will never decrease the remaining timer, though
  it can increase it. An explicit numerical value always sets the timer to that many seconds.
- add `wupget:Unit{Entered,Left}Underwater(unitID, unitDefID, teamID)`. Works very similar
  to the existing Water callins, but EnteredWater applies when the unit dips its toes in water
  while EnteredUnderwater applies when the unit is completely submerged.
- add `Spring.{Set,Get}PlayerRulesParam(s)`, similar to other rules params. There's currently
  two visibility levels, public and private. A notable difference is that the private level is
  only visible to that player, not his allyteam, and not even his (comsharing) team; this is
  partially for technical reasons and can be changed if need be. Synced and specs see everything.
- `Spring.MapArchive` and `Spring.UnmapArchive` have been temporarily removed due to sync safety.
  They will come back at some point, but there is no timeline yet. Use `Spring.UseArchive` meanwhile.
- removed `snext`, `spairs` and `sipairs`. These functions can be replaced by the regular `next`,
  `pairs`, and `ipairs` respectively with no change in behaviour.
- add `Spring.GetModelRootPiece(modelName)` -> number pieceID, returns the root piece of a model.
  Similarly `Spring.Get{Unit,Feature}RootPiece(thingID)` returns it for a live unit or feature.
- add `Spring.GetUnitIsBeingBuilt(unitID) -> bool beingBuilt, number buildProgress`. Note that this
  doesn't bring new capability because `buildProgress` was already available from `GetUnitHealth`,
  and `beingBuilt` from `GetUnitIsStunned`, but as you can see it wasn't terribly convenient/intuitive.
- rules params now support the boolean type. Skirmish AI and the rules param selection filter can
  read them via existing numerical interface by using 0 and 1.
- add `Spring.GetUnitEffectiveBuildRange(builderID[, buildeeDefID])` -> number. Returns the effective
  build range for building given target, counted from the center of the prospective target's center.
  Returns just the build range if buildee is nil. Useful for setting move goals manually.
- add `Script.DelayByFrames(frameDelay, function, args...)`. Runs `function(args...)` after a delay
  of the specified number of frames (at least 1). Multiple functions can be queued onto the same frame
  and run in the order they were added, just before that frame's GameFrame call-in.
- add a new 4th boolean parameter to `VFS.DirList` and `VFS.SubDirs`, controls whether to also return
  matches in subdirs, recursively.
- `gadget:AllowUnitCreation` now accepts a second return value that controls whether to drop the order.
  If set to false, the builder or factory will keep retrying. Defaults to true (drops order; current behavior).
- add `Spring.SetAllyTeamStartBox(allyTeamID, xMin, zMin, xMax, zMax) -> nil`, allowing to set
  start box (in elmos) for an ally team.
- add `Spring.GetFacingFromHeading(number heading) -> number facing` for unit conversion.
  Add `Spring.GetHeadingFromFacing(number facing) -> number heading`, ditto.
- add `Spring.SetUnitPhysicalStateBit(number unitID, number stateBit) -> Set a unit's PhysicalState.
- Lua C Macros (cast and registry) prefixed lua\_ to fix compilation issues.
- Protect Spring.GetGroupUnitsCount from crashing when given an invalid group id.
- Add new variadic variants for some Lua unit script functions: MultiTurn, MultiMove, MultiSpin, etc.

Game Setup:

- removed `mapSeed` parameter and `new_map_x`, `new_map_y` map generation options. Map is now generated
  blank via 'initBlank' parameter (either 1 or 0) and specifying `blank_map_x`, `blank_map_y`,
  `blank_map_height` map options.
- Emit a warning instead of immediately failing when MapParser does not find startpos defined in
  the map config for all teamIDs present in start script. This gives a chance to the game to remedy
  the situation and handle startpos itself. In the worst case, teams without a startpos will spawn
  in the corner -- giving an obvious hint that something is amiss and the map is unable to support
  this number of players by itself without assistance from the game.
- Prevent StartPosSelecter from trying to process mouse clicks while the match is still loading. This
  was able to cause the game to crash on the loading screens.

Misc:

- Add `/debugvisibility` command for debugging the visible quadfield quads
- add `system.allowEnginePlayerlist` modrule, defaults to true. If false,
  the built-in `/info` playerlist won't display. Use for anonymous modes
  in conjunction with `Spring.GetPlayerInfo` poisoning.
- Add new `/remove` cheat-only command, it removes selected units similar to
  `/destroy` except no death script is run (so no explosion nor wreckage).
- Units no longer clear command queue when shared. This can be implemented gameside with UnitGiven callin.
- Fix game save/load issue {{< issue 1425 >}}
- Fix rotating the camera with the middle mouse stopped unit tracking. {{< issue "626" >}}
- Fix when a client that takes too long to connect can potentially trigger a desync.
  {{< issue 1406 >}}
- Prevent logging general log information before the game engine has started. {{< issue "683" >}}
- New modinfo setting to skip issuing newly created units a move command off of the factory pad: insertBuiltUnitMoveCommand.

Sim:

- Improved performance of long-range path finding requests (TKPFS)
- Path data updates faster in response to map changes and can increase the rate dynamically as the
  number of map changes becomes larger (TKPFS)
- Path data update scaling rate can be adjusted by the new modrule
  `system.pathFinderUpdateRateScale` (default: 1.0) (TKPFS)
- Path data updates per sim frame loading is better balanced to reduce impact on jitter caused by
  sim frame time variations. (TKPFS)
- Reduced number of pathing blocks updated on map changes. (TKPFS)
- Improve performance of path data updates by only updating the pathing block-to-block links that
  are impacted by map changes (previously all links were updated.) (TKPFS)
- Engine-driven formation move commands now try to match units to nearest appropriate move targets
  so as to have units adopt formations without having to run across each other.
- edgeEffectiveness can now be 1 (previously capped at 0.999)
- armor multiplier (aka damageModifier) can now be 0 (previously capped at 0.0001)
- damage in unit defs can now be 0 (previously capped at 0.0001)
- damage and armor can also be negative again (like in 0.76b1, so that the target is healed), but keep in mind weapons
  will still always target enemies and never allies, so avoid using it outside of manually-triggered contexts
- nanoturret (immobile builder) build-range now only needs to reach the edge of the buildee's radius
  instead of its center. Mobile builders already worked this way.
- Renamed TKPFS to HAPFS. It is HAPFS with modifications and is ready now to take over. It was
  kept separate initially in case major issues were found, but it is now suitably stable to
  become the official version of HAPFS (as it was always intended.)
- Major rework of QTPFS, so many changes that this diserves its own section (see Sim::QTPFS)
- Added mod option, pfRepathDelayInFrames, which tells GroundMoveType how many frames at least
  must pass between checks for whether a unit is making enough progress to it's current waypoint
  or whether a new path should be requested. Defaults to 60 frames (2 seconds.) Adjust to find the
  right balance between how quickly units can get unstuck and how much CPU power is used. Smaller
  intervals increase the chance of slow units triggering unnecessary re-pathing requests, but
  reduces the chance that players may believe a unit is getting stuck and is not handling itself
  well.
- Added modrule, `system.pfRepathMaxRateInFrames`, which tells GroundMoveType the minimum amount of
  frames that must pass before a unit is allowed to request a new path. By default, it is 150
  (5 seconds.) This is mostly for rate limiting and prevent excessive CPU wastage, because
  processing path requests are not cheap.
- Fixed: units could sometimes get stuck as they spin/circle around a waypoint when a waypoint is
  too close to some small/tiny obstacles.
- Fixed: units could sometimes consume waypoints that were well ahead of their current position,
  which increased processing overhead and reduced their ability to navigate obstacles.
- Fixed: units were often setting their current and next waypoints to the same waypoint.
- Units repath when they are starting to struggle to make progression on their path. Previously
  they would request a new path upon touching blocking terrain or a building, which may not be
  necessary and caused a not insignificant impact on CPU usage.
- Reduced the amount of corner-cutting units try to do when following a path. This should reduce
  the difficulty some unit types experience when trying to move through narrow corridors, or
  between buildings in bases.
- Reduced the difficulty units face when trying to rejoin a path after being routed away due to a
  collision with a building.
- removed modrules: `system.pfForce[Update]SingleThreaded`. MT has shown itself stable.
- removed modrule: `system.pathFinderUpdateRate`. This modrule isn't used any more; instead, a
  different modrule serves a similar purpose, but uses different values, see: `pfUpdateRateScale`.
- Added modrule, `system.pfRawMoveSpeedThreshold`, which tells GroundMoveType and QTPFS what
  minimum speed mod on a given map square to block a raw move trace. By default the value is 0.0,
  which means only terrain, features, and structures that block movement should be considered.
  Raising the value will mean that raw move check will fail on technically pathable map squares,
  which means if unit is asked to cross it, it will resort to using normal path finding, which
  will likely try to path around those map squares. Think lava, it is very slow to move through,
  (and it hurts), you don't want units trying to path through lava unless they are trying to get
  out. This feature is considered experimental.
- Change the default behaviour of builders to not build on top of a blocking unit if they can't
  immediately do something about it.
- Synced build checks now ensure nearby units have accurate ground block information declared to
  avoid fast units running into an otherwise clear area and getting trapped inside building.
- Ground moving units can't step into the inside of static objects.
- Ground units cannot push each other into the inside of static objects.
- Collision forces applied to ground units will allow them to slide along edges of terrain and
  structures to ensure that units can be moved when pushed against these blockers. If they don't
  move then the pushing unit can get stuck between them and an adjacent blocker.
- Fixed: pushResistant units trigger pathing updates on start/stop moving. There's no point doing
  so while moving because the ground block type gets ignored.
- Units will now turn to their desired direction every frame, rather than on every
  modInfo.groundUnitCollisionAvoidanceUpdateRate frames.
- Units given new paths will now switch to a new non-temp waypoint in 1 frame rather than 2.
  Units have a next and current waypoint, so if next is non-temp, but current is then it will make
  current <- next, and grab the next waypoint.
- Units will skid if hit with impulses sufficiently large in the direction opposite their
  movement vector. Previously units would only skid on large impulses that hit their sides.
- New rollingResistanceCoefficient unitdef that is used to reduce a unit's speed when exceeding their maximumSpeed.
- New groundFrictionCoefficient unitdef that is used to reduce a unit's speed when skidding.
- New atmosphericDragCoefficient unitdef that reduces a unit's speed when skidding and exceeding speed maximum.
- Assumes that a unit's default buildeeRadius will be the model's radius when figuring out the
  correct distance at which to start a new unit construction. Previously it was assumed 0.
- unit defs have a new parameter, buildeeBuildRadius, which overrides the engine default assumption
  about the radius of an unit to use for build commands.
- Units and features now have a field, buildeeRadius, which defaults to the unit's model's radius,
  (but for units can be overridden by their buildeeBuildRadius unitDef parameter.) This field is used
  instead of radius for build, capture, repair, reclaim, guard and resurrect commands.
- New system added to allow units near to a recently created wreck (or feature) to immediately check
  for being stuck. The standard fall-back checks for catching stuck units take too long when units
  are getting stuck in wrecks in the thick of battle.
- Added [S|G]etUnitBuildeeRadius to allow game devs to configure the radius used in build-type
  commands when a unit is the target of such commands.
- Moving system will not try to get to goal if the unit has reached the last waypoint, and the pathing
  system reports that the path is incomplete. It will fail immediately rather than keep trying for A
  while before failing later.
- Units will consider going to next path waypoint if they collide with a unit standing on their waypoint.
- Fix issue where units can get stuck walking into each other if one of their paths is changed as they reached their
  current move goal. {{<issue 1469>}}
- Unit target selection no longer uses a random element. {{< pull 1453 >}}
- Fix collision momentum was being maintained on a unit when it gets turned back into a nanoframe.
  {{< issue 1413 >}}
- Fix when mutltiple builders are tasked with building something floating on the surface of the water and get there at
  different frames, it can trigger multiple versions of the same unit are started on the same location.
  {{< pull 1441 >}}
- Fixed the bugger-off checks may fail to get rid of units if they stand on the edge of an area of a new long building.
  {{< issue 1428 >}}
- Fixed: units can sometimes get left behind at a "gather and wait" point.
  {{< issue 2799 >}}
- Fixed: provide a UnitArrivedAtGoal event so that the game can tell that the engine believes the move is complete to
  help avoid custom move code breakage. {{< issue 1459 >}}
- Fixed: burst-fire weapons could hit new targets outside their intended aim angles mid-burst.
  {{< pull 1470 >}}
- Fixed: units pathing using HAPFS could can caught in an infinite looping cycle, never reaching their move goal.
  {{< pull 1444 >}}

Sim::QTPFS

- Debug Drawer now draws into the minimap, showing the map damage updates waiting to be processed.
  The more intense the colour, the more layers (MoveTypes) that still need to process the change.
- The update system now uses a dirty quad table, so multiple updates in the same area won't
  trigger unnecessary path data updates.
- The update system has been rebalanced to keep up with map updates while minimizing the cost of
  doing so. It scales up work if the number of updates starts to get larger.
- The implementation is now hybrid Quad-Tree. Instead of one root-node, there are now multiple
  root nodes in a uniform grid across the map. This is primarily to ensure that all quad sub
  divide into powers of 2, which affords a series of assumptions that allow for optimizations.
- This multi-root approach also fixes a problem with the original QTPFS where long maps would
  subdivide into long and sub-optimal rectangler shapes - like a 1x16 squares for example.
- The update system is now multi-threaded.
- Pathing requests are now fully multi-threaded. Previously the requests were single threaded,
  but in a separate thread from the main one.
- If a pathing request goal is an unaccessible quad, then the system will try to find the nearest
  accessible node to search instead.
- The node-related data structures have been reworked to require significantly less memory than
  before. Also, excess data links have been removed.
- QTPFS could group related pathing requests made in the same frame to reduce load. Now, all
  pathing requests can be grouped with any active pathing request. So a request result processed
  in a previous frame can be used as the answer for one in the current frame if they are
  travelling from/to the same quads.
- Support for unsynced pathing requests added.
- No longer saves/loads pathing data to/from disk: with the performance improvements it is faster
  to recalculate the pathing data on load than to use data saved to disk.
- ECS-based system added to monitor changes in maxRelSpeedMod during match, whereas before it was
  found at the start and then never updated.
- Use Bi-directional searching to half the search time for long distance pathing requests. Note,
  this is a lazy bi-directional search - it is fast rather than trying to find the perfect link
  between the forward and reverse searches.
- If the reverse search fails, then a limit is now set on the forward search to cut down the time
  spent searching for the nearest route.
- Searches are not only able to perform full-path sharing (i.e. same target/src quads), but also
  partial-path sharing. This means when the start/end quads are near another searches star/end
  quads, the path can be used to cut down search times by trying to link up to an existing route.
- QTPFS will automatically attempt to repath incomplete paths as progress is made to try and
  improve the pathing result and get the unit closer to the goal.
- New modrule, `system.qtRefreshPathMinDist`, can be used to configure the minimum size a path
  has to be in order to be eligible for an automatic incomplete path repath.
- New modrule. `system.qtMaxNodesSearched`, can be used to limit the number of nodes searched.
- New modrule, `system.qtMaxNodesSearchedRelativeToMapOpenNodes`, can be used to limit the number
  of nodes searched relative to the number of pathable quads on the map.

System:

- Simulation can be configured to use up to ~100% CPU time to catch up.
- Set default min sim speed for commands to 0.1 (previous limit was 0.3.)
- EnTT has been added to provide ECS capabilities. Patterns for usage to follow.

UI:

- Increase the rate at which the traversability view map is updated by x4

Unit def files and `UnitDefs` unification:

The following unit def keys now accept the same spelling as the ones exposed via UnitDefs.
The old spelling still works (old → new).

- metalUse → metalUpkeep
- energyUse → energyUpkeep
- buildCostMetal → metalCost
- buildCostEnergy → energyCost
- unitRestricted → maxThisUnit
- name → humanName

These two also accept a new spelling, and both the old and new spellings are in elmo/frame.
However, migrate to the new spellings ASAP because the original spellings will be changed
to use elmo/s sometime in the future.
! acceleration → maxAcc
! brakeRate → maxDec

The following unit def keys now accept a spelling and measurement unit
as the one exposed via UnitDefs (old → new). The old spelling still works,
and is still in the old unit.

- maxVelocity (elmo/frame) → speed (elmo/second)
- maxReverseVelocity (elmo/frame) → rSpeed (elmo/second)

The following UnitDefs keys now accept the same spelling as the ones
accepted for unit def files. The old spelling still works (old → new).

- tooltip → description
- wreckName → corpse
- buildpicname → buildPic
- canSelfD → canSelfDestruct
- selfDCountdown → selfDestructCountdown
- losRadius → sightDistance
- airLosRadius → airSightDistance
- radarRadius → radarDistance
- jammerRadius → radarDistanceJam
- sonarRadius → sonarDistance
- sonarJamRadius → sonarDistanceJam
- seismicRadius → seismicDistance
- kamikazeDist → kamikazeDistance
- targfac → isTargetingUpgrade

The following keys receive a new unified spelling for both unit defs
and UnitDefs (unit def, UnitDefs → unified). Old spellings still work.

- losEmitHeight, losHeight → sightEmitHeight
- cruiseAlt, wantedHeight → cruiseAltitude

Added the missing 'radarEmitHeight' to UnitDefs.
The unit def file key was also already 'radarEmitHeight'.

Changes to unit def validity checks:

- negative values for health, (reverse) speed, and metal/energy/buildtime
  now cause the unit def to be rejected, previously each was clamped to 0.1
- negative values for acceleration and brake rate now cause the unit def to
  be rejected, previously the absolute value was taken
- values (0; 0.1) now allowed for health and buildtime (0 still prohibited)
- values [0; 0.1) now allowed for metal cost (0 now allowed)
- undefined metal cost now defaults to 0 instead of 1
- undefined health and buildtime now each default to 100 instead of 0.1

All deprecated UnitDefs keys have been removed, listed below:

- techLevel
- harvestStorage
- extractSquare
- minx
- canHover
- drag
- isAirBase
- cloakTimeout
- miny
- minz
- maxx
- maxy
- maxz
- midx
- midy
- midz

Fixes:

- Fix direct hits doing X damage sometimes failing to kill an X health target
- Fix a pathfinding bug where the unit may try to force it's way through obstacles rather than
  walk around them.
- Fixed issue where Unit Weapon aiming angle checks would recalculate cos(20) every call - it is
  now a constant expression.
- Fixed TestMoveSquareRange() so that it only runs the required tests.
- Fixed issue where units standing still would recalculate their facing direction every frame even
  though they are not moving.
- Fix some Lua interfaces being able to set edgeEffectiveness above 1
- Fix issues where the ground and other visibility features would not render or return expected
  features when looking at certain oblique angles
- Fix using Set<Any>RulesParam with an incorrect type leaving that param set with a value of 0.
- Fix COB SetMaxReloadTime receiving a value 10% smaller than it should
- Fix builders not placing nanoframes from their maximum range

### BAR105 1544

BREAKING CHANGES SINCE 105.0 & HOW TO GET BACK OLD BEHAVIOURS:
! Add movetilt and movereset for previously hardcoded ALT and CTRL camera bindings.
To get back old behaviour, make sure to rebind them (remember "Any+", as in "Any+Alt")

! Split moverotate from movereset binding camera modifier. This is used in the Spring camera
for free rotation. To get back old behaviour, bind it to ALT.

! Resurrecting units no longer overrides their health to 5%. To get back the old behaviour:

-- if migrating in steps to 1544
function gadget:UnitCreated(unitID, unitDefID, teamID, builderID)
if builderID and (Spring.GetUnitCurrentCommand(builderID) == CMD.RESURRECT or Spring.GetUnitCurrentCommand(builderID) == CMD.GUARD) then
Spring.SetUnitHealth(unitID, Spring.GetUnitHealth(unitID) \* 0.05)
end
end

-- if migrating to a newer engine with `Spring.GetUnitWorkerTask`
function gadget:UnitCreated(unitID, unitDefID, teamID, builderID)
if builderID and Spring.GetUnitWorkerTask(builderID) == CMD.RESURRECT then
Spring.SetUnitHealth(unitID, Spring.GetUnitHealth(unitID) \* 0.05)
end
end

! onlyLocal for all three Spring.Marker functions (Point, Line, Erase) no longer accepts
numerical 0 as false (now follows regular Lua language rules where 0 is true). To get
back old behaviour, change `0` to `false`.

! Spring.GetConfigInt, Spring.GetConfigFloat and Spring.GetConfigString now accept nil
as the second argument (what to return by default if not set). Previously it was treated
as 0 (Int and Float) or "" (String). To get back previous behaviour:

local originalGetConfigInt = Spring.GetConfigInt
Spring.GetConfigInt = function(key, def)
return originalGetConfigInt(key, def) or 0
end

! paletted image files are no longer accepted. Convert your images not to be paletted.

! hovercraft and ships brought out of water no longer forced to be upright.
To get back previous behaviour, put the `upright = true` tag in all unit defs
whose move def is of the hovercraft or ship type.

! The return value from the UnitUnitCollision callin is now ignored
and there is only one event for each collision.
There is no way to get back the old behaviour for now,
but if someone needs it it could be arranged.

! Rendering fonts now obeys GL color state. This means that sometimes text
will not be the same color as previously. To get back previous behaviour,
you might need to add `gl.Color` in front of `gl.Text` calls.

! removed the following constants:
Platform.glSupport16bitDepthBuffer
Platform.glSupport24bitDepthBuffer
Platform.glSupport32bitDepthBuffer

To get back previous behaviour, replace with
Platform.glSupportDepthBufferBitDepth >= 16 -- or 24, or 32, respectively

! removed LOD rendering (2D unit billboards when zoomed out far), including
the `/distdraw` command and the `UnitLodDist` springsettings entry

! units with `useFootPrintCollisionVolume` but no `collisionVolumeScales` set
will now use the footprint volume (previously mistakenly used the model's sphere).
To keep the old hitvolume, set `useFootPrintCollisionVolume` to false for units
with no `collisionVolumeScales`. Assuming you apply `lowerkeys` to unit defs,
this can also be achieved by putting the following in unit defs post-processing:

for unitDefID, unitDef in pairs(UnitDefs) do
if not unitDef.collisionvolumescales then
unitDef.usefootprintcollisionvolume = nil
end
end

! removed the `AdvSky` springsetting and the `/dynamicsky` command,
which made clouds move across the sky. You cannot easily get back
previous behaviour, though you can probably achieve something similar
by rendering moving clouds yourself.

! tree feature defs `treetype0` through `treetype16` are now provided by the basecontent archive
instead of the engine. No known games ship their own basecontent and they would know what to do if so.

! the `firestarter` weapon tag no longer capped at 10000 in defs (which
becomes 100 in Lua WeaponDefs after rescale), now uncapped. To get back
previous behaviour, put the following in weapon defs post-processing:

for weaponDefID, weaponDef in pairs(WeaponDefs) do
if weaponDef.firestarter then
weaponDef.firestarter = math.min(weaponDef.firestarter, 10000)
end
end

! the tables returned by Spring.GetSelectedUnitsSorted and
Spring.GetSelectedUnitsCounts no longer have an additional `n` key with
corresponding value containing the number of unitdefs.

    Instead use the second return value, e.g.:

        local counts, unitDefsCount = Spring.GetSelectedUnitsCounts()

Lua:

- add `Spring.GiveOrderArrayToUnit(unitID, orderArray)`, a single-unitID version of `Spring.GiveOrderArrayToUnitArray`.
- add 5th and 6th params to Spring.MarkerErasePosition, onlyLocal and playerID.
  Makes the erase happen locally, optionally as if done by given player,
  same as the existing interface for MarkerAddPoint and MarkerAddLine.
  NOTE: 4th arg is currently unused!
- Fix moveslow and movefast camera bindings not being respected in favor of
  hardcoded ones
- Add `rotX, rotY, rotZ = Spring.GetCameraRotation` and `(0..2*PI) = Spring.GetMiniMapRotation()`
- Add `Game.metalMapSquareSize`, square size for metalmap functions (note, not the same as regular `squareSize`)
- Add Spring.GetPressedScans, for new returns output similar to
  GetPressedKeys but with scancodes
- Add minx, maxy, maxx, miny = Spring.GetSelectionBox() returning coordinates
  for current selection box. Returns nil if no valid selection active.
- Add `damage.debris` modrule, controls damage done by exploding pieces (default 50)
- Add Spring.GetKeyFromScanSymbol(scanSymbol) that receives a scancode and
  returns the users correspondent key in the current keyboard layout
- allow empty argument for Spring.GetKeyBindings to return all keybindings
- add modrule `reclaim.unitDrainHealth`, bool on whether reclaiming units drains health.
  To be used with the revert-to-nanoframe method (since the 'classic' method relies on health drain).
- add Spring.AddUnitExperience(unitID, delta_xp). Can subtract, but the result cannot be negative.
- change widget, gadget and action handlers to support scancodes
- Added Lua SyncedControl callins to modify the original/base heightmap (i.e. the values that the
  restore command will aim for.)
  - Spring.AdjustOriginalHeightMap
  - Spring.LevelOriginalHeightMap
  - Spring.RevertOriginalHeightMap
  - Spring.SetOriginalHeightMapFunc
  - Spring.AddOriginalHeightMap
  - Spring.SetOriginalHeightMap
- Added Spring.GetRenderFeaturesDrawFlagChanged to report all features whose draw flags have
  changed since the last sim frame.
- Added Spring.GetRenderUnitsDrawFlagChanged to report all units whose draw flags have changed
  since the last sim frame.
- Added Spring.KeyMapChanged callin for when the user switches keyboard
  layouts, for example switching input method language.
- Added Spring.GetTeamAllyTeamID(), returns the team's allyTeamID.
  Same as the 4th arg from GetTeamInfo, just more idiomatic.
- Added Spring.GetProjectileAllyTeamID(), ditto.

Maps:

- New bumpwater params, most of these were just hard-coded values:
  - waveOffsetFactor (0.0)
    - waveLength (0.15)
    - waveFoamDistortion (0.05)
    - waveFoamIntensity (0.5)
    - causticsResolution (75.0)
    - causticsStrength (0.08)

Map:

- The original/base height map is now editable, so the height map as read from the map file is
  held separately. Both the map file height map and the original/base height map are saved and
  loaded with save/load feature.

Misc:

- the `useFootPrintCollisionVolume` unit def tag now takes precedence over the default sphere
  that appears when `collisionVolumeScales` are zero. This means that units no longer have to
  define a bogus custom colvol just to prevent the default sphere.
- don't warn for archive checksum mismatch if server's checksum is zero
- 3D and Cubemap .dds textures supported in Lua
- deprecate DISALLOW_RELEASE_BINDINGS and Up+ modifier for keysets, a
  warning is displayed if attempted to bind. No change in behavior
- /keysave and /keyprint now preserves the binding order, providing a better output
- add support for f16-f24 key and scancodes; keycode support requires SDL1
  keycode deprecation
- add configurability of movefast and moveslow speed via added config values
  (CameraMoveFastMult, CameraMoveSlowMult) and scaling factors for specific
  cameras (CamSpringFastScaleMouseMove, CamSpringFastScaleMousewheelMove,
  CamOverheadFastScale).
- Added spring settings boolean flag DumpGameStateOnDesync. When set to true on the server, the
  server will request all clients to produce a local copy of the current game state (effectively
  calling `/dumpstate.`)

Sim:

- Added a new 'b' designator for yardmaps to declare an area that is buildable, but is not
  walkable. This allows for the current method of upgradable buildings to create a locking
  pattern that won't break pathing.
- Reserved MaxParticles \*2 for unsynced projectile-container vector to avoid a re-alloc during
  the multi-threaded section. To avoid a race condition while processing unsynced particles.
- Smooth Height Mesh tracks damage to height map and updates itself in 64m^2 squares
- Smooth Height Mesh switched from 2-pass Gaussian Blur to a single, kernelless Linear pass to
  reduce computational needs of the mesh to minimize impact of recaclulating mesh running sim
  frames
- Smooth Height Mesh uses SSE to optimize local maxima calculations.
- Smooth Height Mesh no longer multi-threads the horizontal and vertical passes because single
  thread is faster during sim frame (the work load has been reduced below multi-thread benefit
  threshold)
- Smooth Height Mesh can be disabled by setting mod rule "enableSmoothMesh" = 0 (enabled by
  default)
- Record the previous draw flag of units/features to support incremental rendering queries
- Multi-threaded GroundMoveType collision avoidance
- Multi-threaded GroundMoveType heading and acceleration planning
- GroundMoveType checks whether waypoints have changed before updating synced waypoint vars.
  This avoids unnecessary expensive checksum updates.
- Limited max push velocity from ground/sea unit collisions to avoid heavy slow units being pushed
  quickly by several small and fast units.
- Unit collision avoidance steering update rate is now configurable via new modrule
  `movement.groundUnitCollisionAvoidanceUpdateRate`. Default value is 3 for cycling through all
  units over 3 sim frames.
- Added `system.pfForceSingleThreaded` to force pathing requests to be sent single threaded.
- Added `system.pfForceUpdateSingleThreaded` to force pathing vertex updates to be processed single
  threaded.
- Added `movement.forceCollisionsSingleThreaded` to force collisions system to use only a single
  thread.
- Added `movement.forceCollisionAvoidanceSingleThreaded` to force collision avoidance system to use
  only a single thread.

System:

- Improved spinlocks by reducing their impact on the CPU, changed implementation from a
  test-and-set (TAS) to test and test-and-set (TTAS) to reduce cache coherency traffic on the
  processor
- Correct thread assignment for Intel 12th gen processors (Alder Lake)
- Windows Vista is now longer supported, Windows 7 is now the minimum required Windows OS
- Support added for Intel Alder Lake (12th gen core) CPUs
- Updated logging around CPU affinity to provide better information.

UI:

- Add Buildoptions, Cloak, Cloaked, Resurrect and Stealth selection filters. See
  `doc/SelectionKeys.md' for further details
- KeyPress and KeyRelease callins receive an additional actions
  7th argument
- Allow minimap to receive MouseWheelPress events, controlled by
  MiniMapMouseWheel = [0]|1
- Accept keyreload and keyload commands in uikeys, accept arguments for
  keyload, keyreload and keysave for the filename. Add keydefaults command
- Add `group unset` action to unassign any groups from selected units
- Allow drawlabel events inside minimap and add MiniMapCanDraw to enable
  drawing lines via cursor over minimap
- Minimap can flip if camera rotation is between 90 and 270 degrees. Can be
  turned on via spring setting `MiniMapCanFlip = [0]|1`
- KeyPress and KeyRelease callins receive an additional scanCode
  6th argument
- sc\_<k> keysets are introduced for scancodes and handled in conjunction
  with keycode bindings preserving the order on which the actions where
  bound. Can also be bound via `sc_0x<hexvalue>`
- Extend GetKeyBindings to receive an additional keyset string to retrieve scancode actions
- Add GetScanSymbol(int scanCode)

Fixes:

- Fix piece projectiles (flying parts from dead units) not colliding with units
- Fix KeyChains not being respected for GuiHandler actions and allow games to
  handle them with new actions argument on KeyPress/KeyRelease
- openAL-soft/SDL audio bugfix: finally making dynamic audio device changes,
  i.e. unplug/replug USB headset or switch between builtin soundcard to
  monitor soundcard (HDMI), seamlessly possible without requiring a program restart
- Fix keysets bound to duplicate actions when the action line contains
  discrepant comment sections
- Fix name retrieval for keycodes always returning hex value, this
  affected the `label` parameter of `KeyPress` and `KeyRelease` callins
  as well as the output of /keydebug
- Fix `GetKeyBindings` returning incorrect actions for bound keychains,
  e.g. if actions bound to m,n and b,n both would be
  returned on `GetKeyBindings('n')` regardless of the previously pressed key
- Fix server pause that happens when a spectator or player leaves the match
- Fix issue where desyncs could not be detected before the 300th sim frame (10 seconds)
- Fix corner case issue where units could start moving on their own if they haven't moved yet and
  collide with something.
- Fix unit movement performance issue where pathfind results return an effective raw move result
  over a long distance.

-- 105.0 --------------------------------------------------------
Sim:

- allow resurrecting indestructable features
- consider partially reclaimed wrecks nonfresh for area-resurrection commands
  ! remove undocumented BeamLaser range modifier (provided 30% extra when fired by mobile units)
  ! remove legacy (COB, though also affecting Lua) hack allowing units with onlyForward weapons to fire regardless of AimWeapon status
  ! remove CMD_SET_WANTED_MAX_SPEED
- allow CEG trails for crashing aircraft (engine will randomly select from any generators listed in sfxTypes.crashExplosionGenerators)
- DGun weapontype: allow leading (leadLimit for DGun defaults to 0 instead of -1 for back-compatibility)
- Rifle weapontype: obey collision flags
- change default ownerExpAccWeight to 0 for all weapon-types
- remove salvoError multiplier hack for positional and out-of-los targets
- add new UnitDef tag "stopToAttack"
- defs: accept "health" in addition to "maxdamage" (units) or "damage" (features)

Lua:

- add math.tau
- add sensors.decloakRequiresLineOfSight modrule. If true, sight radius works as a cap for decloak radius
  (for example a unit with decloak radius 200 will not be decloaked with a 100 LoS unit in the 100-200 range,
  and units with 0 LoS can't decloak others at all regardless of decloak range). Defaults to false.
- add Platform.osVersion; complements Platform.osName
- add Platform.hwConfig
- Script.IsEngineMinVersion now available in all Lua parsing contexts,
  most importantly in `defs.lua`
  ! change {Allow,Unit}Command callin parameters
  AllowCommand(..., cmdTag, fromSynced) --> AllowCommand(..., cmdTag, playerID, fromSynced, fromLua)
  UnitCommand(..., cmdTag ) --> UnitCommand(..., cmdTag, playerID, fromSynced, fromLua)
  ! remove Game.mapHumanName
  ! remove UnitDefs[i].moveDef.{family,type}
- add UnitDefs[i].moveDef.isSubmarine
- LuaUI now has access to Spring.SetUnitNo{Draw,Select,Minimap} and SetFeatureNoDraw
- let Spring.SelectUnitArray select enemy units with godmode enabled
- let Unit\*Collision callins skip engine collision handling if true is returned (from any synced gadget)
- call gadgetHandler:Explosion for unsynced gadgets (but discard the return value)
- allow specifying source by position but target by ID for Spring.GetUnitWeaponHaveFreeLineOfFire
- allow setting terrain-type hardness and name via Spring.SetTerrainTypeData
  ! return the terrain-type index from Spring.GetGroundInfo and Spring.GetTerrainTypeData
  these functions now push [number index, string name, ...] rather than [string name, ...]
  onto the stack
  ! change DefaultCommand call order
  previously widgets were called first, and if they returned anything, gadgets weren't called at all
  now gadgets are called first, then widgets are always called and can override
- add a 3rd param to DefaultCommand callin: the current default command
  works with the above: gadgets receive the default engine cmd,
  widgets then receive what gadgets returned (possibly engine default)
- add Spring.ClosestBuildPos(teamID, unitdefID, worldx,worldy,worldz, searchRadius, minDistance, buildFacing) -> buildx,buildy,buildz to LuaSyncedRead
- add Spring.GetGlobalLos(allyTeamID) -> bool to LuaSyncedRead
- add Spring.IsNoCostEnabled() -> bool to LuaSyncedRead
- add Spring.SetWind(number minStrength, number maxStrength)
- add Spring.SetTidal(number strength)
- add Spring.GetTidal
- add Spring.GetGameState() -> bool, bool, bool, bool to LuaUnsyncedRead
  1.  finished loading
  2.  loaded from a save
  3.  locally paused (i.e. after entering /pause)
  4.  lagging wrt expected simframe time
- add Spring.SetUnitUseWeapons(unitID, bool force, bool block), only one of the bools should be true
  setting force to true means the unit can fire even when being dead, stunned, a nanoframe, or in build stance
  setting block to true means the unit can't fire at all (unless "force" which takes priority)
- add Spring.Set{Unit,Feature}PieceVisible(unit/featureID, pieceID, bool visible)
- add Spring.{Unit,Feature}Rendering.SetProjectileLuaDraw
- add Spring.{Unit,Feature}Rendering.Set{Deferred,Forward}MaterialUniform
- add Spring.{Unit,Feature}Rendering.Clear{Deferred,Forward}MaterialUniform
- add Spring.AddObjectDecal
  ! rename Spring.RemoveBuildingDecal to Spring.RemoveObjectDecal
- add Spring.GetLuaMemUsage to LuaUnsyncedRead and LuaMenu
  returns the number of (kilo-)bytes used and (kilo-)allocations performed
  by the calling Lua state individually, as well as by all states globally
- add Spring.GetVidMemUsage to LuaUnsyncedRead
- add Spring.Ping callout and corresponding Pong callin
- add Spring.GetMapStartPositions callout
- add gl.GetViewRange callout
- add DrawMaterial callin
- add `VFS.GAME` as a synonym to `VFS.MOD`
- support local-space tracking for dynamic lights added via Spring.Add{Map,Model}Light
  the light-definition table should contain a 'localSpace = true' entry to enable this
- Add Spring.SetVideoCapturingTimeOffset() for > 30fps video capturing
- default `defs.lua` now reads a new file, `defs_post.lua`, which
  currently filters out movedefs unused by any unitdef (previously
  this was done by the engine). You can prevent this filter by adding
  the `dont_remove` field to such movedefs. This is useful to make
  them available in `Spring.MoveCtrl.SetMoveDef` but beware the perf cost.
- new TextEditing(utf8, start, length) callin, which holds the IME editing composition (https://wiki.libsdl.org/Tutorials-TextInput)
- new SDLSetTextInputRect(x, y, w, h), SDLStartTextInput() and SDLStopTextInput() functions for controlling IME input from Lua
- Add AllowUnitTransport callin. Called when the engine tests whether unit A can transport unit B. Returning true will let
  the engine decide, returning false will force the engine to disallow transportation (for this specific call).
- add AllowUnitTransport{Load,Unload} callins for controlling whether unit A can start {un}loading unit B
- add AllowUnit{Cloak,Decloak} callins for controlling whether a unit can become {de}cloaked
- new unit rules param access level, 'typed': between 'inlos' and 'inradar', applicable when the unit has once been in LoS
  and continuously in radar since (same rule as `GetUnitLosState(x).typed` and typed engine icons)
- the GiveOrder\* family of callouts can now accept a number for command params,
  equivalent to providing an array with one member (use it to save on arrays)
- MarkerAdd{Point,Line} now have an extra optional argument (after localOnly: 6th for point, 8th for line):
  the playerID of the player who should appear as the author of the drawing (if missing, defaults to the local player).
  Only has an effect when localOnly is true.
- add `paralyze.paralyzeDeclineRate` to modinfo, controls how many seconds it takes for EMP to decline by 100%%
- expose `Game.paralyzeDeclineRate` and `Game.paralyzeOnMaxHealth`
- Added tesselation shaders support to gl.CreateShader. New stages have "tcs" and "tes" names and defined similarly to existing
  "fragment", "vertex", "compute" stages. Also added gl.SetTesselationShaderParameter to define tesselation parameters from OpenGL
  host.
- Added support for Subroutine Uniform (gl.UniformSubroutine, gl.GetSubroutineIndex)
- Added a few OpenGL constants:
  1.  shader types GL.VERTEX_SHADER, GL.TESS_CONTROL_SHADER, GL.TESS_EVALUATION_SHADER, GL.GEOMETRY_SHADER_EXT, GL.FRAGMENT_SHADER.
      They are mostly useful as one of the arguments to the new Lua gl.UniformSubroutine and gl.GetSubroutineIndex callouts.
  2.  geometry shader parameter types GL.GEOMETRY_INPUT_TYPE_EXT, GL.GEOMETRY_OUTPUT_TYPE_EXT, GL.GEOMETRY_VERTICES_OUT_EXT.
      These are used in SetGeometryShaderParameter callout (formerly known as SetShaderParameter)
  3.  tesselation shader parameter types GL.PATCH_VERTICES, GL.PATCH_DEFAULT_OUTER_LEVEL, GL.PATCH_DEFAULT_INNER_LEVEL.
      The first is mandatory for tesselation to work, the other two are optional and can be used on the OpenGL host side instead of
      writing the tesselation control shader.
  4.  New vertex primitive type GL.PATCHES
      ! Renamed SetShaderParameter to SetGeometryShaderParameter to avoid confusion of what kind shader parameters belongs to.
- Added VFS.GetLoadedArchives() -> { string archiveName1, string archiveName2 ... }
- Added VFS.GetArchivePath(string archiveName1) -> string archivePathOnDisk
- Added VFS.GetFileAbsolutePath(string vfsFilePath) -> string filePathOnDisk
- Added VFS.GetArchiveContainingFile(string vfsFilePath) -> string archiveName
  ! VFS.MapArchive, VFS.UnmapArchive and VFS.UseArchive now take archiveName instead of fileName as the first parameter.
- add a 6th return value (feature->reclaimTime) to GetFeatureResources.
- Add `Spring.UnitFinishCommand(unitID)`, finishes current command. Unlike CMD.REMOVE it works with Repeat/UnitCmdDone
  and unlike CommandFallback it works on builtin engine commands.
- Allow spawning of any CEG from any LUS:
  EmitSfx(p, "cegTag") param will emit the CEG with tag="cegTag"
  EmitSfx(p, SFX.GLOBAL | cegID) will emit the CEG with id=cegID
  Added Spring.GetCEGID("cegTag") to get CEG id from tag
- Added Spring.GetUnitCurrentCommand(unitID [, cmdIndex]) -> [cmdID, cmdOpts (coded), cmdTag[, cmdParam1[, cmdParam2 ... ]]]
  Returns the index-th command in the unit's queue (or nil). Note: cmdOpts are represented by a single number
  (usually a table otherwise), and params are returned on the stack as well. This function allocates no tables.
  ! make Spring.Get{Unit,Factory}Commands always expect a second argument
  ! Spring.GetTeamInfo: switched the last two return values around (incomeMultiplier is now 7th, customKeys is now 8th)
- Added a second boolean parameter to Spring.Get{Player,Team}Info, if it is false the function will not return
  the (now) last return value, the customKeys table (allocation optimisation)
  ! Spring.GetUnit{Armored,IsActive} now works on enemies in LoS
  ! Spring.GetUnitMass no longer works on enemies outside of LoS
  ! remove Spring.{Set,Get}UnitTravel
  ! remove Spring.SetUnitToFeature (use the AllowFeatureCreation callin to block features from being spawned)
- Added a boolean parameter to Spring.GetCameraState, if set to false it will return the current camera name
  and then the following camera-specific values on the stack:
  fps, rot -> oldHeight
  ta -> height, angle, flipped
  spring -> rx, ry, rz, dist
  free -> rx, ry, rz, vx, vy, vz, avx, avy, avz
  ov -> (nil)
- Spring.SetUnitWeaponState with "autoTargetRangeBoost" now also lets Cannon
  and StarburstLauncher weapons look ahead and pre-aim at targets just
  outside of nominal range.

AI:

- reveal unit's captureProgress, buildProgress and paralyzeDamage params through
  skirmishAiCallback_Unit_get{CaptureProgress,BuildProgress,ParalyzeDamage} functions

Misc:

- dedicated server now defaults the `AllowSpectatorJoin` springsetting to false (still true for non-dedi)
- Rifle weapontype: remove hardcoded particles
- add MapConvNG executables to official spring builds: https://springrts.com/wiki/MapConvNG
- remove joystick support
- detect hangs during filesystem initialisation
- fix stale thread-id cache in watchdog after reload
- make LuaVFSDownload rescan archives in main thread, and do so when a download finishes (not when one starts)
- account for raw search in AICallback::GetPathLength
- use hidden window for offscreen context rendering
- add /hang command
- add /luagccontrol command
- add /netping command
- add /netmsgsmoothing command
- add /distsortprojectiles command
- remove LuaShaders config-setting
- change PitchAdjust config-setting from a boolean to an integer
  0 disables, 1 scales pitch by the square root of game speed, 2
  scales linearly with game speed
- `/nocost` now accepts 0/1 parameter (still toggles if none given)
- model metadata for assimp/obj can now define pieces hierarchy
  by either nested tables or 'parent' key.
- IME editing support for those with the proper SDL2 version/IME tool combination
  ! Made lockluaui.txt obsolete: no longer necessary for it to exists in order to enable VFS for LuaUI
- use SHA2 rather than CRC32 content hashes
  ! blank map params: new_map_x and new_map_y are now in map dimension sizes rather than map dimension \* 2. new_map_z renamed to new_map_y

Fixes:

- fix #1968 (units not moving in direction of next queued [build-]command if current order blocked)
- fix #6060 (Max{Nano}Particles=0 still allowing 1 {nano}particle to spawn)
- fix #6059 (units with high turnRate/brakeRate/acceleration moving unexpectedly)
- fix #4268 (add Spring.AddObjectDecal)
- fix #4755 (wrong usage of addr2line on FreeBSD)
- fix #4945 (add Spring.GetMapStartPositions)
- fix #4724 (Spring.SetUnitNeutral not informing any attackers)
- fix #5864 (gunships not limited by maxVelocity when colliding with terrain)
- fix #4515 (aircraft jitter when moving and turning)
- fix #6048 (falling features shaking on the ground instead of fully stopping)
- fix #6046 #5326 #5222 #4663 (spurious camera position change if entering MMB mode while moving cursor)
- fix #6019 (Spring.IsAABBInView bug)
- fix #6038 (noExplode weapons not obeying SetUnitWeaponState("range"))
- fix #6036 (no method to detect MMB camera-pan mode)
- fix #5956 (inverted mouse camera panning for Windows 10 Pro users)
- fix #6022 (uniformArray not initializing array-uniforms in gl.CreateShader)
- fix #6012 (air-transports not unloading properly)
- fix #6010 (HoverAirMoveType aircraft ignoring smooth-mesh when idle)
- fix #6009 (aircraft landing on damage-dealing water when idle)
- fix #6007 / #5989 (desyncs on CMD_UNLOAD_UNITS)
- fix #5993 (voidwater triggering water explosion effects)
- fix #5247 (maximized minimap producing graphical glitches)
- fix #5962 (minimap viewport rectangle larger than what actual screen shows)
- fix #5965
- fix #5977 (log-system memory corruption)
- fix #5964 (shields occasionally letting projectiles pass)
- fix #5948 (memory corruption with LoadingMT=1)
- fix #5947 (PFS failure if starting square inside concave corner)
- fix #5934 (crash on malformed build-commands)
- fix #5923 (wrong external format passed to gl{BuildMipmaps,TexImage2D})
- fix #5927 (memory corruption in text-wrapping)
- fix #2127 (units chasing after full transports)
- fix #4972 (builders doing redundant movement to clear build-sites)
- fix #5207 (units not following waypoints)
- fix #2354 (Spring.GetUnitWeaponState not matching Spring.SetUnitWeaponState due to experience)
- fix #5841 (allow setting colour of area selection for custom commands)
- fix #5884 (treat collidee as attacker on unit <-> unit collisions for UnitPreDamaged)
- fix #5716 (add Weapon::{avoid,collision}Flags to Spring.{Get,Set}UnitWeaponState)
- fix #5652 (maneuverBlockTime aircraft control parameter)
- fix #1225 (inaccurate preview range-rings)
- fix #5870 (zero-based months in rotating infolog filenames)
- fix #5862 (Spring.DestroyUnit + Spring.CreateUnit not supporting persistent ID's)
- fix #5860 (units failing to aim uphill at targets near cliff edges)
- fix #5863 (too strict matrix orthonormality check)
- fix #5855 (memory corruption when loading models)
- fix #5852 (units chasing after auto-generated targets going out of range even on hold-pos)
- fix #5854 (broken default formation-move visualization)
- fix #5851 (DynamicWater crash if foamTexture specified as .dds file)
- fix #5850 (squished loadscreens with BumpWater enabled)
- fix #5806 (CEG spikes from underwater explosions drawn on water surface)
- fix #5820 (broken by 3bd78acb00cab732af4278ed324c7ee5dbc7c517)
- fix #5805 (broken water reflections in FPS camera mode)
- fix infinite backtracking loop in PFS
- fix #5814 (broken slopemap indexing for terrain buildability tests)
- fix #5803 (move goals cancelled when issued onto blocked terrain)
- fix Spring.{G,S}etConfigFloat not being callable
- fix Spring.GetPlayerRoster sometimes excluding active players

-- 104.0 --------------------------------------------------------
Major:

- Add LuaMenu - an unsynced stripped down Lua environment that starts before the game is loaded and persists through reload
  this environment receives the Draw{Genesis,Screen,ScreenPost} callins as well as
  a special AllowDraw FPS limiter which enables these if true is returned from it
  (otherwise they are called once every 30 seconds)
  ! make GL3.0 the minimum required OpenGL version; Intel G45's and other relics are now banned
  (any driver that claims support for this _and_ shaders is allowed to run Spring unimpeded)
- improve memory management so Spring.Reload can be safely called multiple times in a row
- kill various desyncs that slipped into 103.0

Misc:

- Assorted bugfixes and optimisations
- force regeneration of ArchiveCache
- Implement VFS modes - VFS.MOD, VFS.MAP & VFS.BASE now actually work. Added VFS.MENU
  ! base archives now have modtype of 4 and menu archive have modtype of 5
  ! one letter command-line flags (e.g. -g instead of --game) are removed
- generally more detailed error-logging
- continued internal cleanup and C++11 adoption to reduce unnecessary data copying and allocing
- don't load-then-immediately-free the LuaGaia handler if neither (synced or unsynced) entrypoint exists
- don't record non-essential profile data in the background if not in /debug mode; reset profiler on reload
- replace dozens of STL containers with faster custom alternatives
- use pooled memory allocations in several engine components
- make Use{V,P}BO=0 reuse buffers when downsized
- recycle audio-stream buffer memory
- recycle demo-streams across reloads
- write demos before destroying game on shutdown in case the latter CTD's
- do not spawn VFS initialization threads if WorkerThreadCount=0
- rebase ThreadPool on a lock-free taskqueue; make IO-bound work unable to block execution of other tasks
- reduce ThreadPool worker-count right after VFS initialization
- asyncify writing screenshots via ThreadPool
- asyncify demo/savegame compression
- disable hang-detection when showing a message box
- defer loading \*Def sounds until simulation asks for them
- use pcall in LuaUtils::Echo
- catch all exceptions in Game::Load and make it non-interruptible
- turn LoadDefs content_error exceptions into clean exits
- set modType to an integer (not string) value in base-content modinfo.lua's
- do not auto-add springcontent dependency to non-game "mod" archives
- downgrade non-writeable springsettings.cfg's to a warning
- allow maps to not supply a type-map; one fewer exception to worry about
- improve RNG statistical quality; sampled ints can now also exceed 0x7fff
- make the \*nix CrashHandler compile on ARM; do not use cpuid on non-x86 builds
- enable compilation without streflop
- change ForceShaders to ForceDisableShaders (which does the opposite)
- add UseLuaMemPools config-option
- add ForceCoreContext config-option
- add ForceSwapBuffers config-option
- add ForceDisableClipCtrl config-option
- add NETMSG_LOGMSG for synced logging
- netlog Lua memory allocation failures
- netlog model parsing failures
- netlog SHA512 digests over cached path-estimator data
- enforce a minimum of 1GB free disk-space (to prevent cache corruption)
- log 'Warning: Dropping packet from unknown IP' only once per IP
- filter out consecutive duplicated log-messages; add LogRepeatLimit config-option
- fix inverted los-test logic in AI GetProperty callback (patch by rlcevg)
- fix memleak in generated C++ AI interface (patch by rlcevg)
- fix ResourceHandler memleak (patch by rlcevg)
- save SkirmishAI data in separate streams (patch by rlcevg)
- wrecks in water emit bubbles instead of smoke (patch by Sprunk)
- store path-estimator cache files as map.pe-hash.zip; forces a refresh
- delete cached path-estimator data which can not be opened or read successfully
- integrate custom selection volumes; also support subtable-based {col,sel}volume definitions
- refactor the object death-dependency system
- trim unused AssParser root-piece transform overrides;
  'rotAxisSigns' and 'rotAxisMap' from 95.0 are also gone
- rip out DynamicSun
  much more convincing day / night cycles can be completely and more cleanly
  (re-)implemented in Lua with Spring.SetSunLighting, Spring.SetSunDirection,
  and Spring.SetAtmosphere

Sim:
! Sonar will now detect ships/hovers - this is since los can't raycast through water.
note that wobble still exists without LoS.

- add 'collideFireBase' weapondef flag
- allow manually sharing nano-frames (patch by Sprunk)
- share unfinished buildees if factory changes team (patch by Sprunk)
- prevent building from dead factories (patch by CommanderSpice)
- restore randomized circling behavior for air-constructors (patch by CommanderSpice)
- UnitExperience is called based on regular exp difference, not limExp (patch by Sprunk)
- set defBaseRadarErrorMult back to 2 instead of 20 (patch by Sprunk)
- expand CEG underground-test lenience from 1 to 20 elmos (patch by Sprunk)
- allow but do not enable staggered LOS updates
- do not hard-assign floatOnWater=true for hovers and ships
  checks for this property now first query a unit's MoveDef
  (if any) so that ship->tank and similar substitutions via
  MoveCtrl.SetMoveDef behave as expected
- disable randomized hovering during CMD_LOAD_UNITS for air-transports
- make Spring.CreateUnit smarter (patch by Sprunk)
  if a builderID parameter is passed to Spring.CreateUnit, the new unit
  will consider that its solo-builder if it has canBeAssisted=false set
- EMGCannon now obeys flightTime override (patch by Sprunk)
- allow setting runtime/initial firestate to 3 [FIREATNEUTRAL] from COB and unitdefs (patch by Sprunk)
- change a weapon reaiming-tolerance constant from 20 radians to 20 degrees (patch by Sprunk)
- experiment with larger medium-resolution PFS blocksize (16*SQUARE_SIZE vs 8*SQUARE_SIZE)
- bump the default unloadSpread multiplier from 1 to 5
- incorporate native "raw move" (straight-line path) support
  inert by default; enable by adding allowRawMovement = true
  to a MoveDef table

Lua:
! Undeprecate {unit,feature}def.modelname and add .modeltype and .modelpath to
allow reading these without loading the model. Removed def.model.{name,type,path}
so they aren't used accidentally.
! The callins GameStart and GamePreload won't be called when a saved game is loaded

- Add VFS.AbortDownload(id) - returns whether the download was found&removed from the queue
- Add Spring.Get{Game,Menu}Name to LuaUnsyncedRead, so LuaMenu and unsynced Lua handles know about each other
- Add new callins (LuaMenu only):
  ActivateMenu() that is called whenever LuaMenu is on with no game loaded.
  ActivateGame() that is called whenever LuaMenu is on with a game loaded.
  AllowDraw() if it returns false, the next draw call is skipped (only active when a game isn't running)
- Add Spring.{Set,Get}VideoCapturingMode() - this doesn't actually record the game in any way,
  it just regulates the framerate and interpolations.
- add Engine and Platform global tables currently containing the following keys:
  Engine.version: string
  Engine.versionFull: string
  Engine.versionPatchSet: string
  Engine.buildFlags: string
  Engine.wordSize: number

      Platform.gpu: string, full GPU device name
      Platform.gpuVendor: string, one of "Nvidia", "Intel", "ATI", "Mesa", "Unknown"
      Platform.gpuMemorySize: number, size of total GPU memory in MBs; only available for "Nvidia", (rest are 0)
      Platform.glVersionShort: string, major.minor.buildNumber
      Platform.glslVersionShort: string, major.minor
      Platform.glVersion: string, full version
      Platform.glVendor: string
      Platform.glRenderer: string
      Platform.glslVersion: string, full version
      Platform.glewVersion: string
      Platform.sdlVersionCompiledMajor: number
      Platform.sdlVersionCompiledMinor: number
      Platform.sdlVersionCompiledPatch: number
      Platform.sdlVersionLinkedMajor: number
      Platform.sdlVersionLinkedMinor: number
      Platform.sdlVersionLinkedPatch: number
      Platform.glSupportNonPowerOfTwoTex: boolean
      Platform.glSupportTextureQueryLOD: boolean
      Platform.glSupport24bitDepthBuffer: boolean
      Platform.glSupportRestartPrimitive: boolean
      Platform.glSupportClipSpaceControl: boolean
      Platform.glSupportFragDepthLayout: boolean
      Platform.osName: string, full name of the OS
      Platform.osFamily: string, one of "Windows", "Linux", "MacOSX", "FreeBSD", "Unknown"

  ! Game.{version,versionFull,versionPatchSet,buildFlags} now reside in the Engine table
  Engine.wordSize indicates the build type and is either 32 or 64 (or 0 in synced code)

- Spring.SetWaterParams() can now be used without /cheat for modifying unsynced values.
- Implemented gl.GetWaterRendering() to expose access to water rendering variables.
  ! Removed water rendering parameters from LuaConstGame, as they're no longer const.
- Added Spring.SetMapRenderingParams() for modifying splatTexMults, splatTexScales, voidWater and voidGround.
- Implemented gl.GetMapRendering() to expose access to the map rendering variables.
  ! Removed voidWater and voidGround constant parameters from LuaConstGame, as they're no longer const.
- add Spring.{Set,Get}{Unit,Feature}SelectionVolumeData callouts
  ! remove Spring.SetSunParameters and Spring.SetSunManualControl
  ! remove gl.Smoothing
- add gl.SwapBuffers callout for LuaMenu
- add Game.envDamageTypes table containing {def}IDs of environmental-damage sources (patch by Sprunk)
- add Spring.{Get,Set}ConfigFloat callouts
- add Spring.{Set,Get}{Unit,Feature}SelectionVolumeData callouts
- add Spring.SpawnExplosion callout
- add Spring.SpawnSFX callout
  equal to the \*UnitScript versions of EmitSFX, but
  takes position and direction arguments (in either
  unit- or piece-space) instead of a piece index
- add DrawScreenPost callin (patch by ivand/lhog)
  Similar to DrawScreenEffects, this can be used to alter the contents of a
  frame after it has been completely rendered (i.e. World, MiniMap, Menu, UI).
- add DrawWorldPreParticles callin
- add start- and hit-position (x,y,z) arguments to ShieldPreDamaged callin
- math.random is now available during execution of defs.lua
  (math.randomseed also exists, but will always be a no-op)
- allow gl.BlitFBO blitting from/to the default FB with userdata FBO's
- bump gl.UniformArray length restriction from 32 to 1024
- add Spring.SetFeatureResources(number metal, number energy [, number reclaimTime [, number reclaimLeft]])
- add Spring.SetUnitPieceMatrix(number unitID, number pieceNum, table matrix)
  sets the local (i.e. parent-relative) matrix of the given piece if any of the
  first three elements are non-zero, and also blocks all script animations from
  modifying it until {0, 0, 0} is passed
  (matrix should be an array of 16 floats, but is not otherwise sanity-checked)
- add LUS ChangeHeading callin
- add LUS {Start,Stop}Skidding callins; called when a (ground) unit reacts to an impulse
  make the start-skidding threshold ('sqSkidSpeedMult') configurable via MoveCtrl.SetGroundMoveTypeData
- make MoveCtrl.Set\*MoveTypeData recognize the 'waterline' key (patch by SanguinarioJoe)
- make Spring.ClearUnitGoal cancel raw movement by default
  ! reorder AllowStartPosition callin parameters and add teamID
  old: clampedX, clampedY, clampedZ, playerID, readyState, rawX, rawY, rawZ
  new: playerID, teamID, readyState, clampedX, clampedY, clampedZ, rawX, rawY, rawZ
- expose weapon salvo vars to {Get,Set}UnitWeaponState (patch by Sprunk)
- extend Spring.SetFeatureResurrect
  old API: arg #2 was parsed as unitdef-name if string
  new API: arg #2 is parsed as unitdef-name if string OR unitdef-id if number; arg #4 is parsed as resurrect-progress
- make Spring.SetFeatureResurrect ignore nil for its second arg and allow unsetting the resurrect-target if id=-1
  ! extend Spring.GetUnitWeaponHaveFreeLineOfFire to take aiming-from coors
  old API: Spring.GetUnitWeaponHaveFreeLineOfFire(unitID, weaponNum, targetID | [ tgtPosX [, tgtPosY [, tgtPosZ ]]]])
  new API: Spring.GetUnitWeaponHaveFreeLineOfFire(unitID, weaponNum, targetID | [srcPosX [, srcPosY [, srcPosZ [, tgtPosX [, tgtPosY [, tgtPosZ]]]]]])
  (if the srcPos\* arguments are all nil, the default aiming location is used as before)
- block getting NoAccessTeam's LoS-state (patch by Sprunk)
- block Spring.GetRadarErrorParams for enemy allyteams (patch by Sprunk)
- implement Path.GetPathNodeCosts
- Spring.GetMouseState now returns a sixth value indicating if the cursor is offscreen
- add Spring.GetGrass(x, y) -> 0|1 to obtain grass map information
  ! exclude spectators from Spring.GetPlayerList if given a normal (id >= 0) team arg

Rendering

- add new terrain-mesh renderer (faster than existing code, but does not respect cliffs well)
- draw bindpose models non-recursively
- setup alpha-masking for 3DO shadows
- bump ROAM node-pool size for more demanding maps
- limit number of ground-scar decals to 4096
- fix jagged shadows on maps with oblique sun-directions
- fix grass not receiving shadows; unify groundShadowDensity application
- fix grass being rendered black in metal-view
- fix particle shadows
- use clip-control (when possible) to improve depthbuffer precision
- micro-optimize LuaMatBinSet sorting
- make shader-flag handling more lightweight
- do not parse info-textures bound to shaders every single frame
- nuke {point,line}-smoothing options; selected primitive AA is obsolete with multisampling
- outsource vsync regulation to SDL2

UI:

- show a splash-screen while initializing VFS; randomly chosen from any
  .png or .jpg images found under SplashScreenDir defined in springsettings.cfg
  (if SplashScreenDir is a relative path, Spring's CWD will be prepended)
- Add FPSClampPos config for whether the engine should verify the camera doesn't hit ground/go too far
  this and other FPS camera configs can be changed midgame and will affect the next frame drawn.
- Add OverheadMaxHeightFactor config - a float multiplier for maximum overhead camera height.
- skip the IsAbove test/callin for input-receivers when mouse is offscreen
- show warning if VMware Mesa drivers are installed
- avoid "Invalid thread number 0" errors if window creation fails
- rename the 'minimised' CLI flag to 'hidden'
- prevent window from being considered unresponsive during startup
- make shift+esc drop to LuaMenu from PreGame
  ! fix window-state restoration
  the WindowState config-setting is gone, a maximized
  non-fullscreen window now gets restored by checking
  if the resolution previously written is maximal (for
  minimized windows the state is not saved, as before)
- fix VRAM size detection
- coax more information out of systems that fail OpenGL context-creation
- add fallbacks for common context-creation failures; catch inadequate
  desktop video-modes; turn exceptions during {window,context}-creation
  into friendlier popups
- remove old warnings about junk GPU drivers that can no longer appear
- downgrade from throwing exception to warning for missing scar-textures
- extend and organize the information shown in /debug
  ! rename '/roam' to '/mapmeshdrawer' which cycles through the renderers
- add '/debugglerrors' command (enables per-frame glGetError reporting)
- add '/debuggl x y z' command (requires DebugGL=1) where x=source,y=type,z=severity
- add '/wire{model,sky,tree,water}' debug commands
  ! change default bindings for chatswitch\* actions
  alt+ctrl+(a,a) now toggles chatswitchally
  alt+ctrl+(s,s) now toggles chatswitchspec
  chatswitchall now has to be bound manually
- allow area commands to be extended off-map
- fix tracking-mode with Free camera

Bugfixes:

- fix LuaParser thread-safety
- fix ProfileDrawer thread-safety
- fix ScopedMtTimer thread-safety
- fix LosMap thread-safety
- fix PathTexture thread-safety
- fix RoamMeshDrawer thread-safety
- fix LuaVFSDownload thread-safety during reload
- fix deadlocks in Windows crash-handler
- fix race-condition between screenshot counter and libIL
- fix sound-thread occasionally accessing a deleted pointer on exit
- fix sound-buffer deletion while buffers were still bound to sources
- fix double-free in CBitmap's move-ctor for DDS images
- fix wrong number of pool-threads being spawned on reload
- fix runaway Text::WrapInPlace string-allocation
- fix FarTextureHandler performance-stalls
- fix potential segfault in SolidObjectDef::GetModelRadius
- fix degenerate GroundFlash quads
- fix ExploSpike projectiles instantly disappearing if they collided with anything on creation
- fix potential gsRNG.Next\*() eval-order desync
- fix OOB memory-read in GameSetup::LoadTeams
- fix map-damage explosions being processed twice
- fix explosions causing terraform-spikes on certain maps
- fix UnitDamaged events following UnitDestroyed
- fix BuilderCAI crash when parsing invalid build-commands
- fix canBeAssisted-check in Builder::StartBuild
- fix ATI uniform-type validation for LuaMaterials
- fix ATI driver crash with BumpWater (on any map without a coastline) in TextureAtlas generation
- fix crash in AdvTreeDrawer on ARB-only junkware
- fix crash in InfoTextureHandler on ARB-only junkware
- fix PlayerRoster causing memory corruption on reload
- fix random WaitingDamage memory corruption
- fix Light::ClearDeathDependencies memory corruption
- fix CollisionHander::MouseHit not taking relMidPos translation into account
- fix LosHandler::UnitLoaded removing LOS from non-stunned units
- fix calling alSourcePlay even if alSourceQueueBuffers produced an error
- fix CheckEnlargeDrawArray for VA_TYPE's larger than 10 floats
- fix crash if unit is assigned an empty COB-script
- fix potential buffer-overflow in GetFeatures AI callback
- do not bitshift negative values (undefined behavior)
- catch invalid CmdDesc indices in GuiHandler::DrawButtons
- insert missing eventHandler.RemoveClient call in RoamMeshDrawer

- fix #5759 (units flip-flopping between paths due to path-estimator expanding unreachable nodes)
- fix #5778 (crash when reloading with QTPFS)
- fix #5730 (crash during nano-particle creation by builders under specific circumstances)
- fix #5728 (canBeAssisted=false factories only being buildable by one constructor at a time)
- fix #5721 (structures created via AI orders sometimes not leveling ground)
- fix #5715 (zero-spread weapons avoiding non-target enemies)
- fix #5690 (Spring.GetProjectileTarget returning <0,0,0> for cannons with ground-attack orders)
- fix #5519 (path-estimator failure if start- and goal-coordinates mapped to the same block)
- fix #5684 (Spring.{Add,Remove}Grass updates not taking effect until camera is moved)
- fix #5658 (projectiles applying two position-updates while bounced)
- fix #5630 (free line-of-fire detection failure)
- fix #5287 (help units in firebases with a collideFireBase weapondef flag)
- fix #2970 ("pushResistant" is now accepted by MoveCtrl.SetGroundMoveTypeData)
- fix #3515 (better error-messages for the /team command)
- fix #5200 (broken ctrl+mousewheel tilt transitions with 'Spring' camera)
- fix #5258 (custom selection volumes)
- fix #5632 (don't show just a black screen on startup)
- fix #5629 (make footprint-terraforming respect buildee's actual facing)
- fix #4447 (FPS-mode locking onto out-of-LOS units)
- fix #4074 (equalize UnitDamaged parameters for LuaUI and LuaRules)
- fix #2855 #5092 #5180 (jagged terrain shadows)
- fix #5619 (area-commands not drawn on minimap)
- fix #5617 (enable non-binary alpha-testing for unit icons)
- fix #5684 (Spring.{Add,Remove}Grass updates not taking effect until camera is moved)
- fix #4164 (shield{Good,Bad}Color are RGBA float4's now)
- fix #5587 (units in sharp holes sinking underground if also skidding)
- fix #5502 (crash with malformed build-commands)
- fix #4723 (EmitSfx crash when referencing effect not defined in unitdef)
- fix #4940 ("Recompressing Map Tiles with ETC1" load-stage triggering watchdog)
- fix #5403 (extra ShieldPreDamaged parameters)
- fix #5056 (let Spring.GetUnitWeaponHaveFreeLineOfFire take firing location as an argument)
- fix #5188 (units refusing to move if ordered to non-reachable locations from factory)
- fix #5639 (stunned transports not updating the position of their cargo)
- fix #5304 (invisible collada models if no secondary texture provided)
- fix #5318 (.ogg audio streams playing too fast)
- fix #5358 (theoretical desync with std::{strtod,atof})
- fix #5457 (TargetBorder broken for non-box collision volumes)
- fix #5411 (PFS not finding optimal paths on maps with extreme speed-modifiers)
- fix #5501 (PFS ignoring obstacles higher up or lower down cliffs)
- fix #5469 (overflow-FPE in GetHeadingFromVectorF)
- fix #5476 (assertion-failure with broken springcontent installation)
- fix #5434 (add logging to detect unicode-aware libIL)
- fix #5387 (GroundDecalHandler crash on exit)
- fix #5421 (Spring.SendCommands("ctrlpanel") crash if sent inside a callin)
- fix #5412 (GameRulesParams persisting through Spring.Reload)
- fix #5493 (crash while reloading with LoadingMT=1)
- fix #5463 (failing to connect to server sends user back to LuaMenu)
- fix #5544 (classified)
- fix #5468 (classified)
- fix #5461 (classified)
- fix #5404 (MoveCtrl.SetGunshipMoveTypeData crash if called for a non-gunship unit)
- fix #5483 (glDeleteList called from multiple threads)
- fix #5423 (random number generation in defs.lua)
- fix #5396 (units lacking sensors on fire-platforms)
- fix #5339 (crash when placing a geothermal)
- fix #5338 (grid-patterns in unsynced heightmap)
- fix #5644 (send BuggerOff notes from opened-but-blocked factories again)
- fix #4745 (broken transport-unloading with non-zero unloadSpread)
- fix #4971 (constructors not assisting if simultaneously given orders to build count-restricted units)
- fix #4481 (check MoveDef maxSlope for units built by mobile constructors)
- fix #4827 (idle gunships jiggling away from their position)
- fix #4232 (gunships snapping to horizontal attitude when ordered to stop after attacking)

-- 103.0 --------------------------------------------------------
General:

- Fix & Improve performance
- Add 'allowTake' modoption (default: true) to enable/disable /take (by lamer)
- Add onlyLocal boolean value to modinfo.lua (default false)
  If true, spring won't listen for incoming connections when running
  this game unless GAME\OnlyLocal is specified in the start script.

Sim:

- Add new UnitDef tag "buildingMask" and LuaSyncedCtrl function "SetSquareBuildingMask". (by ivand)
  Both used in conjunction to allow/disallow construction of certain unit types (via setting buildingMask)
  on certain tiles (via SetSquareBuildingMask). By default all tiles and buildings have mask of 1

Rendering:

- Ghosted buildings are now consistent when changing specTeam/specFullView

Bugfixes:

- Fix #5301 (Units with restricted firearcs do not turn to aim)
- Fix #5302 (extreme slowdown)

-- 102.0 --------------------------------------------------------
General:

- Fix & Improve LoS performance
  ! remove LuaModUICtrl config setting
  ! remove ClassicGroundMoveType
  ! remove SpringLobby from spring installer
- update model matrices on demand instead of once per frame

Sim:
! allow units to unstuck themselves from an unpathable square
into an adjacent pathable one
! COB: deprecate unit/team/allyteam/global vars. Use lua functions instead

- prevent accidental friendly fire from direct line of fire weapons

Lua:

- Add {Set,Get}FeatureRulesParam(s)
  ! Get{Game,Team,Unit,Feature}RulesParams will now return { paramName = value, ... }
- You can now delete RulesParams by calling Set{Game,Team,Unit,Feature}RulesParam with no value argument or a nil one.
- Add new Spring.ClearUnitGoal(unitID) callout.
- add GamePaused and GameProgress callins to base gadget-handler
- extend builtin uniform binding for custom Lua materials
  SEE "fix LuaMaterial uniform management" BELOW FOR THE
  LIST OF NEW LOCATION KEYS THAT ARE RECOGNIZED AS OF 102
- Add Spring.GetFrameTimer(bool synced) to get a timer for the start of the frame
  this should give better results for camera interpolations.
- Add an optional weaponNum argument to SetUnitTarget.
- Update shader errorlog even if compilation was successful
- Remove updateMatrix argument in Spring.UnitScript.{Move,Turn} as matrices will be automatically updated if necessary.
- Add new Spring.Start callout for launching a new Spring instance without terminating the existing one. See Spring.Restart for parameters.
- Add new VFS.GetAvailableAIs([gameName] [, mapName])->{ai1, ai2, ...} callout for getting a list of all Spring AIs. The optional gameName and mapName parameters can be used to include game/map specific LuaAIs in the list.

Save/Load:
! remove UseCREGSaveLoad and add /LuaSave which generates .slsf files
! compress creg saves

- SaveFile can be specified in start script in order to start from a save

Rendering:

- reduce number of team-color updates for custom shaders
- use frame-extrapolation for smoother feature movement
- draw all model nanoframe stages with shaders by default
- skip drawing icons for void-space units
- disable BumpWaterRefraction = 2 for now.

AI:

- make skirmishAiCallback_Unit_getResource{Use,Make} not round their return values to ints
- RulesParams are now accessible through skirmishAiCallback\_{Game,Unit,Team,Feature}\_getRulesParamValue{String,Float}

Misc:

- make the time server spends in sleep calls configurable (for dedicated hosts)
- rename the 'ReflectiveWater' config-key to 'Water' (to match the command)
- set the 'GroundDetail' config-val only when it changes, not on shutdown
- add '/GroundDetail <val>' text command to modify ground detail on the fly
- auto-normalize normals and tangents for all AssImp-loaded models
- add a gameDeferredDraw=5 drawMode value (handy for Draw{Unit,Feature} to know)

Bugfixes:

- fix invisible OBJ models
- fix gl.DeleteShader allowing repeated deletion
- fix drawing of being-built units during deferred pass
- fix invisible ground on voidwater maps when AdvMapShading=0 (in L-view)
- fix Spring.SetSunLighting crash when AdvMapShading=0
- fix Spring.ValidUnitID to accept nils again
- fix wreckage jumping from large _horizontal_ impulse
- fix landed HoverAirMoveType aircraft behavior when given a command within their build range
  ! fix LuaMaterial uniform management for materials with standard and deferred shaders
  material tables are now expected to contain subtables defining the uniforms; instead of
  "material = {shader = ..., cameraposloc = 123, texunits = ..., ...}"
  say
  "material = {shader = ..., uniforms = {cameraposloc = 123, ...}, texunits = ..., ...}"

  for a material that has both types of shader, supply "standardshader",
  "deferredshader", "standarduniforms", and "deferreduniforms" as keys

  new keys for these uniform subtables can be
  "viewmatrixloc" // synonym for "cameraloc"
  "projmatrixloc" // camera projection
  "viprmatrixloc" // camera view-projection
  "viewmatrixinvloc" // synonym for "camerainvloc"
  "projmatrixinvloc" // camera inverse projection
  "viprmatrixinvloc" // camera inverse view-projection
  "shadowmatrixloc" // synonym for "shadowloc"
  "cameradirloc" // camera forward direction
  "sundirloc" // synonym for "sunposloc"
  "rndvecloc" // random 3-vector with length in [0, 1)
  "simframeloc" // current simulation frame
  "visframeloc" // current renderer frame

- fix #5182 (unwanted sun-texture reflections)
- fix #5175 (microscopic FarTexture impostors)
- fix #5166 (wrecks buried underground on maps with negative hardness)
- fix #5165 (rare crash on exit or reload)
- fix #5161 (Spring.IsUnitAllied always returning true in unsynced gadgets)
- fix #5160 (Spring.SetSunLighting not working if applied in \*get:Initialize and crashing when AdvMapShading=0)
- fix #5157 (skirmishAiCallback_Unit_get{Pos,Vel} broken with cheats enabled)
- fix #5155 (MoveCtrl.SetRotation not matching Spring.GetUnitRotation)
- fix #5149 (EVENT_ENEMY\* AI events not being sent)
- fix #5148 (assertion failure with custom shaders)
- fix #5146 (missing wreckage healthbars)
- fix #5141 (glitchy tree rendering)
- fix #5136 (texture corruption with custom Lua materials)
- fix #5135 (missing SFX.FIRE and SFX.SMOKE effects)
- fix #5134 (broken zoom in track-mode with Spring camera)
- fix #5084 (allow LuaUI self-disabling)
- fix #5131 (order-lines visualization)
- fix #5130 (put projectiles created by Spring.SpawnProjectile in Gaia team)
- fix #5127 (icon-transparency over water)
- fix #5181 (aircraft sometimes fail to land)
- fix #5177 (stack-buffer-overflow in TextWrap.cpp)
- fix #5172 (segfault on assimp loading)
- fix #5173 (air transports fail to pick up units)
- fix #5174 (hover move type tend to exactly land on top of each other)
- fix #5176 (crash in CobThread)
- fix #5106 (Strafe planes dont land when reaching target or on 'stop' command)
- fix #5152 (heap-use-after-free when loading a game)
- fix wrong radar icons when switching specfullview
- fix #5216 Spring.GetVisibleUnits
- fix #5219 pink features & units
- fix #5223 buffer-overflow when using /water

-- 101.0 --------------------------------------------------------
Major:

- LoS (sim) overhaul
- SSMF Splat Detail Normal Textures (by Beherith)
- many new Lua rendering related features (custom feature shaders, map shaders, ...)
- many Lua additions
- Lua pr-downloader API
- removal of airbases & fuel
- transport overhaul
- 64bit windows builds

General:
! change default screenshot file type to jpg. to create png use /screenshot png
! pr-downloader dependency is now mandatory (statically linked with spring)
! demo files are now compressed with gzip (and the suffix is .sdfz)

- improve data locality even further with respect to 100.0
- fix memory leaks and misc. crashes (some related to LoadingMT)
- ignore /give commands issued via autohost interface (needs an actual player)

LoS:
! ModInfo: remove losMul & airLosMul -> always handle los ranges in elmos in the _whole_ engine
WARNING: this also rescales the returned values in Lua UnitDefs!
! ModInfo: make `radarMipLevel` configurable
! change default airMipLevel 2->1 & radarMipLevel 3->2
! ModInfo: add new sensors.alwaysVisibleOverridesCloaked (default _off_! before was always on)
! modrules.lua: add new `separateJammers` tag (breaks backward compatibility!)
default true (old behaviour is false)
when enabled each allyTeam only jams their own units
! get rid of los radius limit (you can now create units that cover the whole map and more)

- speed ups in general
- reduced lags introduced by LoS
- multithread LoS updating
- reduced memory usage
  ! fix various bugs related to unit/feature movement not registered
  ! update LoS on terrain changes
  ! correctly handle the height of the emitter (before radarEmitHeight was always from ground height & similar units on a spot shared their emitHeight)
  ! make LoS much smoother (now really gets called per-gameframe when the emitter moves)
- better circle algo
- add Spring.SetGlobalLos(int allyTeamID, bool globallos)

Sim:
! start simulation at frame 0, not at 1

- features now have a non-animated LocalModel and can use per-piece collision volumes
- RockUnit and HitByWeapon script callins now take vectors in unit-space
- fix acceleration rates and turn-speed limits for reversing units
- fix units occasionally zipping across the map after being unloaded
- fix unloaded units being non-colliding if transporter script attached them to piece -1
- fix various aircraft landing/midair-vibrating bugs
- fix builders sometimes ignoring their commands if they were issued without shift
- set better default radii for objects (and use same logic in all model parsers)
- make ground-unit reversing logic configurable via Spring.SetGroundMoveTypeData
- fix ground-units forever trying to move along their final waypoint direction in rare cases
- COB: always use the return value of the Killed thread to determine wreck level (none if omitted)
  ! remap SFX.RECURSIVE piece flag from 1<<8 to 1<<14
  ! removed hardcoded logic to zombify (stun+immobilize) a unit on death
  ! airbases and fuel were removed, use custom commands in combination with
  Spring.SetUnitLandGoal(unitID, x, y, z [, radius]) and the new transport changes
  to implement this functionality in lua.

Transports:

- every unit can now have other units attached to it using:
- Spring.UnitAttach(transporterID, passengerID, pieceNum)
- Spring.UnitDetach(passengerID)
- Spring.UnitDetachFromAir(passengerID)
- units with transportCapacity > 0 and transportMass > 0 will retain backward compatibility
- added Spring.SetUnitLoadingTransport(passengerID, transportID) which disables collisions
  between the two units to allow colvol intersection during the approach.

GUI/Sound:
! fix default drawInMap keyshortcut on german keyboards

- fix displayed tooltip elevation when over minimap
- fix Ready button ignoring clicks after setting startpos and rejoining
- fix potential infinite loop during sound initialization

Rendering:

- new Shatter animation (SFX.Explode in Cob/LUS)
- break the piece into parts and not triangles
- much faster (in creation & render time)
- change the trail drawing of some missile projectiles
- add support for SSMF Splat Detail Normal Textures (by Beherith)
- add deferred pass for features; note that these are drawn into
  the same buffer as units (overwriting it, unless the new config
  variable AllowDeferredModelBufferClear is set to false [default])
- allow custom shaders for being-built units
- allow custom Lua map shaders
- fix metal->los info texture switching visual harshness
- apply anisotropic filtering to all mipmapped SSMF textures
- fix shadow-pass (and general) terrain/model visibility culling
- fix environment reflections (broken in four different ways)
- fix DynamicWater artifacts at large viewing distances
- fix RefractiveWater not showing submerged objects with DualScreenMinimapOnLeft
- fix truncation of deferred model normals
- do not disable model shaders during world alpha-pass
- do not disable model shaders during water ref{le,ra}ction passes
- always draw water surface during alpha-pass
- kill shadows for fully alpha-faded features
- kill impostors for fully alpha-faded features
- write extraColor to the specTex target on the deferred side of ModelFragProg
- use groundSpecularColor when no specularTex exists (i.e. on basic SMF maps)
- shift ground decals visually so they are always aligned with objects
- render all types of decals with shaders
- draw custom commands that have two parameters
  ! nuke support for "special" Spring{Radius,Height} pieces in assimp models

GameServer:

- speedup gameserver exiting (reduced some io/net related sleeps)

Pathing:

- fix cases where either no path was returned or an incorrect one
- switch to octile heuristic to reduce number of explored nodes
- increase high-resolution search range
- fix diagonal movement cost estimation
- handle PathCache crc failures
- make QTPFS neighbor updates a bit faster

AI:

- plug several memory leaks in engine interface and wrappers
- bring ghost model drawing in a usable state again
- add sensor map callbacks
- add shield state callbacks
- add ground info callbacks
- make feature callbacks respect cheats

Lua Callins:
! GameID callin now gets the ID string encoded in hex.
! new Lua callin RenderUnitDestroyed is now called just before a unit is
invalid, after it finishes its death animation.
! ShieldPreDamaged will now be called for LightningCannon and BeamLaser.
Two new arguments were added: beamCarrierWeaponNum and beamCarrierUnitID.
These will be populated in place of the first argument (projID).
! The following callins will now have 1-based weapon index arguments:
DrawShield,
ShieldPreDamaged,
AllowWeaponTargetCheck,
AllowWeaponTarget,
AllowWeaponInterceptTarget,
StockpileChanged
! UnitCommand and UnitCmdDone argument order changed to be consistent with
each other and with CommandFallback & AllowCommand: (unitID,
unitDefID, unitTeam, cmdId, cmdParams, cmdOpts, cmdTag)
Note that UnitCommand will now receive cmdOpts as a table and not as a number

- new callin UnitReverseBuilt, called when a living unit becomes a nanoframe again
- add new callins DrawGroundPreForward and DrawGroundPreDeferred
  these run at the start of their respective passes when a custom
  map shader has been assigned via Spring.SetMapShader (convenient
  for setting uniforms)
- add new callin DrawGroundPostDeferred; this runs at the end of its
  respective deferred pass and allows proper frame compositing (with
  ground flashes/decals/foliage/etc, which are drawn between it and
  DrawWorldPreUnit) via gl.CopyToTexture
- add new callins DrawUnitsPostDeferred and DrawFeaturesPostDeferred
  these run at the end of their respective deferred passes to inform
  Lua code it should make use of the $model*gbuffer\*\* textures before
  another pass overwrites them (and to allow proper blending with e.g.
  cloaked objects which are drawn between these events and DrawWorld
  via gl.CopyToTexture)
  NOTE: the *PostDeferred events are only sent (and only have a real
  purpose) if forward drawing is disabled, whereas the DrawGroundPre\*
  events are only sent if the map has a custom shader!

LuaDefs:
! los related UnitDefTags are now in elmos
! Lua{Feature,Unit}Defs:
make the "model" subtable available (as it is for LuaUnitDefs)
move the {min,max,mid}{x,y,z} values to this table (these can now be accessed
using e.g. *Defs[i].model.midx)
deprecate *Defs[i].modelname, use \*Defs[i].model.path instead

Lua (lighting):
! Game.fogColor constant is no longer available, use gl.GetAtmosphere("fogColor")
! Game.groundAmbientColor, Game.groundSpecularColor and Game.groundSunColor
are no longer available, use gl.GetSun() instead with appropriate parameters

- add unsynced callout Spring.SetSunLighting(table params) -> nil
  It can be used to modify the following sun lighting parameters:
  {ground,unit}{Ambient,Diffuse,Specular}Color and specularExponent
  example usage: Spring.SetSunLighting({
  groundAmbientColor = {1, 0.1, 1},
  groundDiffuseColor = {1, 0.1, 1},
  })
- add unsynced callout Spring.SetAtmosphere(table params) -> nil
  It can be used to modify the following atmosphere parameters:
  fog{Start,End}, {sun,sky,cloud}Color
  example usage: Spring.SetAtmosphere({
  fogStart = 0,
  fogEnd = 0.5,
  fogColor = { 0.7, 0.2, 0.2, 1 },
  })
- add unsyced callout gl.GetAtmosphere(name) -> number | number r, number g, number b
  Works similar to gl.GetSun(), and can be used to extract atmosphere parameters.
  See Spring.SetAtmosphere for details.

Lua (projectiles):

- new Spring.DeleteProjectile(projID) for silently removing projectiles (no explosion).
- new Spring.{Get,Set}UnitWeaponDamages similar to \*UnitWeaponState but for damage and explosion values.
- new Spring.{Get,Set}ProjectileDamages similar to the above, for projectiles in flight.
  ! Weapons with tracking will use a radar dot position unless
  Spring.SetProjectileIgnoreTrackingError(projID, bool ignore) is called.

Lua (model rendering):
! reading from gl*TextureEnvColor[0] in custom shaders to set team color
will no longer work, declare a uniform vec4 named "teamColor" instead
(the engine will fill this in for you)
! gl.UnitShape and gl.FeatureShape now take three extra boolean arguments
and set their relevant GL state internally (so that calling only these
functions is enough to get correctly rendered models; pass in 'false'
for the [new] THIRD argument for the fixed behavior, omit it or pass in
'true' [default] to render with your own custom state instead)
NOTE:
the [new] FOURTH argument should be 'true' if in DrawScreen and 'false'
(default) if in DrawWorld; any transforms for these two functions \_MUST*
include a gl.LoadIdentity call when argument #3 is false (whether they
are used in DrawWorld or in DrawScreen)
the [new] FIFTH argument determines whether the model is drawn opaque
('true' [default]) or with translucency ('false'); only has an effect
when argument #3 is false

- add callouts Spring.Preload{Feature,Unit}DefModel allowing the engine
  to load the unit's model (and texture) in a background thread.
  wreckages and buildOptions of a unit are automatically preloaded.
- new Spring.SetFeatureFade(featureID, bool) to control whether a feature will fade or not when zoomed out.
- add Spring.FeatureRendering API; allows custom shaders (technically,
  materials) for features as its Spring.UnitRendering counterpart does
  for units
  note: the following two functions exist but are no-ops
  Spring.UnitRendering.SetFeatureLuaDraw
  Spring.FeatureRendering.SetUnitLuaDraw
- add callouts
  gl.{Unit,Feature}Textures(number objID, boolean enable) -> nil
  gl.{Unit,Feature}ShapeTextures(number defID, boolean enable) -> nil
  these are primarily intended to be used in conjunction with
  gl.{Unit,Feature}(objID, ..., raw=true)
  gl.{Unit,Feature}Shape(defID, ..., raw=true)
  and apply a model's default textures; easier than gl.Texture("$%...")

Lua:

- fix LuaUI crash if a widget caused an error in its GetConfigData callin
- fix GadgetHandler miscounting if RemoveGadget was invoked during a callin
- allow custom commands to be non-queueing (handy for state toggles)
- LuaUI can now reload itself via Spring.SendCommands
- allow /lua{rules,gaia} reloading on frame 0
- allow Lua{Rules,Gaia} chat actions before frame 0
- extend Spring.TraceScreenRay to intersect a user-given plane
- add unitSpecularColor to the set of values returned by gl.GetSun
- fix typo in maphelper/setupopts.lua breaking several default options
  ! do not start camera transitions from Spring.SetCameraState
  ! LuaSyncedRead::GetAIInfo now returns "UNKNOWN" for non-local AI's, not "UKNOWN"
  ! {Set,Get}UnitPieceCollisionVolumeData now receive 1-based piece index argument (was 0-based).
  ! fix a desync related to printf/number2string conversion
  ! Lua: fix bug that some TransformMatrix functions were revealing the true position of radar dots
  ! fix Spring.GetUnitPieceDirection() (returns now same dir as GetEmitDirPos)
  ! Spring.GetUnitPieceInfo() will now set "[null]" as the parent name of the root piece instead of ""
- add Spring.SetGlobalLos(int allyTeamID, bool globallos)
- add Spring.IsUnitIn[Los, AirLos, ...]
  Note this is _not_ equal with Spring.IsPosInLos with the unit's pos!
  I.e. the ground maybe in radar range, but the unit is jammed and so not visible
- add Spring.GetUnitFeatureSeparation(unitID, featureID[, dist2D])
- add Spring.GetFeatureSeparation(featureID1, featureID2[, dist2D])
- new Spring.GetUICommands function to obtain a list of all UI commands (e.g. /luaui reload)
- new VFS.CalculateHash function that calculates hash (in base64 form) of a given string (with md5 support initially)
- new Spring.{Get,Set}{Unit,Feature}Mass functions
- new Spring.SetFeatureMoveCtrl(featureID [, boolean enable [, number* args]]) to control feature movement
  the number\* arguments are parsed as follows and all optional
  if enable is true:
  [, velVector(x,y,z) -- initial velocity for feature
  [, accVector(x,y,z) -- acceleration added every frame
  ]]
  if enable is false:
  [, velocityMask(x,y,z) -- dimensions in which velocity is allowed to build when not using MoveCtrl
  [, impulseMask(x,y,z) -- dimensions in which impulse is allowed to apply when not using MoveCtrl
  [, movementMask(x,y,z) -- dimensions in which feature is allowed to move when not using MoveCtrl
  ]]]
- add callouts (see their Unit equivalents for documentation)
  Spring.{Get,Set}FeatureRotation
  Spring.{Get,Set}FeaturePieceCollisionVolumeData
  Spring.GetFeatureLastAttackedPiece
  Spring.GetFeaturePieceMap
  Spring.GetFeaturePieceList
  Spring.GetFeaturePieceInfo
  Spring.GetFeaturePiecePosition
  Spring.GetFeaturePieceDirection
  Spring.GetFeaturePiecePosDir
  Spring.GetFeaturePieceMatrix

  Spring.SetFeatureNoDraw
  Spring.GetFeatureNoDraw

  gl.FeatureRaw
  gl.FeatureMultMatrix
  gl.FeaturePiece
  gl.FeaturePieceMatrix
  gl.FeaturePieceMultMatrix

- add unsynced callout SetSkyBoxTexture(string texName) -> nil
  example usage: Spring.SetSkyBoxTexture("myskybox.dds")
- add unsynced callout Spring.SetMapShadingTexture(string texType, string texName) -> boolean
  example usage: Spring.SetMapShadingTexture("$ssmf_specular", "name_of_my_shiny_texture")
- add unsynced callout Spring.SetMapShader(
  number standardShaderID,
  number deferredShaderID
  ) -> nil
  (the ID's must refer to valid programs returned by gl.CreateShader;
  passing in a value of 0 will cause the respective shader to revert
  back to its engine default)
  custom map shaders that declare a uniform ivec2 named "texSquare"
  can sample from the default diffuse texture(s), which are always
  bound to TU 0
- extend the signatures of the Spring.SetDraw{Ground,Models}Deferred
  callouts; allows disabling the forward {Unit,Feature,Ground}Drawer
  passes
  Spring.SetDrawGroundDeferred(
  boolean drawGroundDeferred
  [, boolean drawGroundForward]
  )

  Spring.SetDrawModelsDeferred(
  boolean drawUnitsDeferred,
  boolean drawFeaturesDeferred
  [, boolean drawUnitsForward
  [, boolean drawFeaturesForward ]]
  )
  expose the following $named textures to LuaOpenGL
     "$map_reflection" (same as "$reflection")
     "$sky_reflection" (defined for SSMF maps that have a skyReflectModTex)
  "$detail"
     "$normals" (auto-generated)
  "$ssmf_normals"
     "$ssmf_specular"
  "$ssmf_splat_distr"
     "$ssmf_splat_detail"
  "$ssmf_splat_normals" or "$ssmf_splat_normals:X"
  "$ssmf_sky_refl"
     "$ssmf_emission"
  "$ssmf_parallax"
  'X' can be a value between 0 and 3, defaults to 0 if ":X" is omitted

Lua pr-downloader API:
callouts:

- VFS.DownloadArchive(name, category)
  callins:
- DownloadStarted(id)
- DownloadFinished(id)
- DownloadFailed(id, errorID)
- DownloadProgress(id, downloaded, total)

-- 100.0 --------------------------------------------------------
Major:

- bugfixes

InfoTexture:
! Added radar2 los colour for colouring the inside of the radar edge.

- it is changeable through SetLosViewColors (see Lua changes)

WeaponDefs:

- fix visibleShieldHitFrames not read from shield subtable
  ! scale flighttime with GAME_SPEED and not 32
- warn on unknown tags (so you can de-TA-ism your weapondefs finally)
- weapondefs_post.lua: set legacy weapontype tags to nil, so they don't cause engine warnings

LOS:

- make little bit more accurate

Lua:
! SetLosViewColors changed:

- radar2 colour (see Map changes) is required.
- parameters are now in the form of GetLosViewColors output:
  (always = {r,g,b}, LOS = {r,g,b}, radar = {r,g,b}, jammer = {r,g,b}, radar2 = {r,g,b})
- changed args of Spring.GetGroundNormal
- new 3rd arg: bool smoothedNormal (default false)
- 4th return arg: float slope
  ! convert UnitDef::attackSafetyDistance to a key recognized by Spring.SetMoveTypeData

NetProtocol:

- handle rejected connection attempts (when version differs)

Pathing:

- make path cache part of synced state -> player desyncs right from the start when got an invalid one

ConfigTags:

- make "MaxParticles" & "MaxNanoParticles" runtime changeable

Misc:

- fix #4837: hang in linux crashhandler
- fix #4819: ai interface log messages are now logged into infolog.txt
- fix #4882: endlessloop at start when MaxSounds = 0
- unselect AI when game is selected in old menu
- demotool: add more info to dump output

-- 99.0 ---------------------------------------------------------
Major:

- new shader based InfoTexture
- new linux crashhandler (by Major Bor3d0m)
- new rotatable default camera
- MASSIVE speedup in lategame rendering (cutting rendertime upto 1.5-3 times!)
- add Spring.Reload(script) to change/map & game without exiting spring
- added lua override for InfoMaps (by enetheru)
- allow to enable isolated mode by creating a file isolated.txt next to spring executable
- internal weapon refactor
- fix particle limit

Cameras:
! removed TW, Orbital & Smooth camera
! add new default camera: Spring camera

- it's a merge of TW & Overhead camera
- it's using [alt] as modifier for rotating the camera (in key & middle click scrolling)
- also can be rotated with the screen edges of the upper 30% of the screen
- add new EdgeMoveWidth & EdgeMoveDynamic config tags
- EdgeMoveWidth: defines the screen edge width used for scrolling
- EdgeMoveDynamic: if scroll speed depends on the distance to the screen edge (default on)

SpringSettings.cfg:
! don't read old config file locations any more:
AppData\Local\springsettings.cfg
~/.springrc
! add dedicatedValue & headlesValue to confighandler:
! make spring-dedicated listen at 0.0.0.0 as default again
! add headless values from springrc-tempate-headless.txt

- print user's config to infolog

System:
! new linux crashhandler by Major Bor3d0m

- libunwind is required to compile spring on linux
- fix headless compile on systems w/o x11 & SDL
  ! save windowed size in separate tags (#4388)
- fix crash with intel gpus
- fix bug that caused to recompress groundtextures always to ect1 on intel/mesa even when there was no need for it

(G)UI:

- fix #4576: F6 does not sound mute
- fix #4578: backspace key events aren't captured by chat entry mode
- fix #4455: disable in-map draw when user starts writing
- fix #4706: do not ignore next char in ChatActionExecutor
- document keychains in uikeys.txt
- improve default cmdcolors.txt
- improve default los colors
- OverheadCamera: make angle 100% stable when doing `mousewheel down then up`
  ! remove los view w/o radar a.k.a. [;] (can be emulated with Spring.SetLosViewColors)
- draw weapon debug spheres in /debugcolvol (aimfrom pos, muzzle pos, target pos)

Sim:

- fix harvesting
- fix #4591: builders `randomly` remove build orders from queue
- fix #4587: (stuck units in labs): remove a failing optimization CPathEstimator::FindOffset()
- fix units disobey move cmds
- fix user target being set for all weapons
  ! change TEAM_SLOWUPDATE_RATE to 30 / UNIT_SLOWUPDATE_RATE to 15
- fix aircraft in groups (e.g. brawlers) not being able to attack the target and landing instead at target pos

Pathing:
! fixed shaking units when getting close to blocked squares

- repath GroundMoveType only every SlowUpdate() (to save cpu cycles)
- runtime cache failed paths, too (and use a lower lifeTime for them)

Collisions:

- fix #4592: broken per-piece coldet
- fix #4602 and improve CQuadField::GetUnitsAndFeaturesColVol
- speed up UnitCollisions

Units:

- base automatic attack commands for idle units on weapon priorities

Weapons:

- refactor weapon code:
- use LeadTargetPos more often
- give current unit's target higher priority than weapon ones
- plasmarepulsor: spawn projectiles from QueryWeapon (when defined)
- drop bombs from QueryWeapon() (when defined) and not AimFromWeapon()
- call AimWeapon() more often (depending on fireTolerance tag)
- see fireTolerance & allowNonBlockingAim tags
- fix impactOnly BeamLasers or LightninghtCannons dealing damage to units in the path of a beam blocked by water or a shield. (by GoogleFrog)

WeaponDefs:

- add fallback name:
- weaponVelocity := maxVelocity
- weaponAcceleration := acceleration
- add fireTolerance tag: Angle which prevents a weapon from firing in CanFire. (GoogleFrog)
- add allowNonBlockingAim tag (default false = old behaviour). When enabled it makes AimWeapon() LUS/COB calls non-blocking (still the fireTolerance tag might prevent the weapon firing).
  This is very useful for fast units that need to retarget often (e.g. armflash driving next to a llt).

UnitDefs:

- add new tag attackSafetyDistance used by airplanes (by springraaar)

Projectiles:

- randomize IDs
- add SFX.RECURSIVE flag to Explode(), allowing to draw the piece and all its children recursively.

New InfoTexture Renderer:

- new shader based InfoTexture (faster, shinier & modular)
- add new `/ToggleInfo %name_or_shader_path%`
- fix #4608: add new /showpathtype [movdefname | movedefid | unitdefname | -1]

Rendering:

- removed multiple linked lists from rendering (massive speed up, cutting rendertime upto 2-3 times!)
  ! fix particle limit being ignored for particles created in the same GameFrame
- ShieldParticles: reduce count of particles by more than half
- always draw WaterPlane (that's not the water surface) below current minimum terrain height
- FlyingPieces: add camera frustum check
  ! SMFTexAniso: changed default to 4 (was 0)
- add UseShaderCache config tag (default true)

Map:

- allow to override the following InfoMaps via mapinfo.lua: (by enetheru)
- metalmapTex
- typemapTex
- grassmapTex
- minimapTex
- allow to load dds as detailtex

Lua:

- add Spring.Reload(script) to change/map & game without exiting spring
  ! modified Spring.Restart(parameters, script): it only restarts spring when parameters isn't empty
  ! removed alias "/quit" (use /quitforce)
- add "quit to menu" to quit-box
- add "/reloadforce"
- add callouts to read and set engagement range of a unit (by FLOZi)
  - Spring.GetUnitMaxRange(unitID) -> number maxRange
  - Spring.SetUnitMaxRange(unitID, maxRange) -> nil
- add Spring.GetLosViewColors
- add new call-in: UnitStunned(unitID, unitDefID, unitTeam, stunned) -- called whenever a unit changes its stun status
- add Set/GetProjectileIsIntercepted
- add GetProjectileTimeToLive, GetProjectileOwnerID and GetProjectileTeamID.
- add a second return value to AllowWeaponTargetCheck callin to allow ignoring the callin.
  ! remove Spring.UpdateInfoTexture
  ! fix Spring.GetKeyState & Spring.PressedKeys expecting SDL2 keycodes while whole lua gets SDL1 ones
  ! Spring.PressedKeys now also returns keynames
- fix missing COLVOL_TYPE_SPHERE in luadefs
- add new tag `grayscale16bit` to gl.SaveImage() to save single color 16bit PNG
- add Spring.SetUnitTarget(unitid, nil) syntax
- expose some tags to LuaDefs: (by sprunk)
- expose maxReverseSpeed to LuaUnitDefs
- expose dynDamage tags to LuaWeaponDefs
- expose flightTime tag to LuaWeaponDefs

StartScript:

- support rapid tags
- allow to disable demo file recording in script.txt via RecordDemo = 0
- add DefaultStartScript config setting:
  - if set, automatically start it!
  - add --oldmenu command line parameter to always get to the old menu

AI: (all by rlcevg)

- add Lua.callRules, Lua.callUI to JavaOO
- added weapon related callouts: Unit_getWeapons, Unit_getWeapon, Unit_Weapon_getDef, Unit_Weapon_getReloadTime
- expose resources via Game_getTeamResource* & Economy_get*

OS:

- new linux crashhandler (by Major Bor3d0m)
- HWCursor: support for 48x48 & 96x96 sizes
- allow to enable isolated mode by creating a file isolated.txt next to spring executable

Buildbot/Installer:
! merged spring-headless / spring-dedicated into linux static builds / windows installer
! remove broken maven support

CMake:

- colorize gcc output
  ! remove DEBUG2 & DEBUG3, replaced with just DEBUG
- enable "Identical Code Folding" with ld.gold
- disable unused ASSIMP formats at compiletime
- add cmake option DEBUG_LUANAN to enable lua check for nans in a release build

-- 98.0 ---------------------------------------------------------
Major:

- vaporized PathFinder's runtime precache cpu usage

Cmdline Arguments:

- allow ./spring --game rapid://ba:stable --map DeltaSiegeDry
- give correct error message when cmdline given filepath wasn't quoted and contained spaces

GameServer:

- rename BypassScriptPasswordCheck to AllowSpectatorJoin and default to true

Lua:

- added AssignPlayerToTeam(playerID, teamID)
- added SetAlly(firstAllyTeamId, secondAllyTeamId, ally)
- added VFS.UnmapArchive(string fileName)
- added VFS.GetMaps(), VFS.GetGames(), VFS.GetAllArchives(), VFS.HasArchive(archiveName)
- added VFS.GetArchiveInfo(archiveName)
- added VFS.GetArchiveChecksum(archiveName)
- added VFS.GetArchiveDependencies(archiveName), VFS.GetArchiveReplaces(archiveName)
- allow tcp connections by luasocket as default (udp is still restricted)
- add callin DrawInMiniMapBackground(sizex, sizey) -> nil
- add callin AllowBuilderHoldFire(unitID, unitDefID, action) --> boolean
  action is one of:
  Build -1
  Repair 40 (CMD_REPAIR)
  Reclaim 90 (CMD_RECLAIM)
  Restore 110 (CMD_RESTORE)
  Resurrect 125 (CMD_RESURRECT)
  Capture 130 (CMD_CAPTURE)

Rendering:

- glFont: make inlined colorcodes reuse current set alpha value

Pathing:

- make lowRes PathEstimator using medRes PE instead of maxRes PathFinder (massively reducing cputime)
- massive cleanup & modularization
- reduced memory usage
- modrules: add system.pathFinderUpdateRate tag
- fixed/reduced PathEstimator update rate
- crashing aircraft doesn't spam PE updates anymore (#4500)

CEG:
! fixed booleans in CEGs (sometimes they were randomly overridden with random data)

Weapons:

- shields have a new weapondef tag armorType - specifies the armor category to which the shield belongs (you can input either an armorclass name, or a unitdef name to share that unit's armor category)
  ! move magic number to weapondef*post.lua: crater were 1.5x of craterAreaOfEffect while decals are 1.0x -> make them \_both same size*
  ! fixed #4523: typo in craterAoE loading
  ! LightningCannon visual now obeys the beamTTL weapondef tag (behaviour same as BeamLaser) - previous value was hardcoded 10
  ! wobble and sprayAngle now improve with experience (same as [moving]accuracy)
  ! Shields obey dynamic damage

Sim:

- GroundBlocking: don't align immobile objects to even squares
- reduce load in Collisions handling (by doing blocked ground squares scans only every 2nd frame)

Misc:

- AI is initialized earlier: no more hang on game start
- add /set GrassDetail to console
- slightly speed up scanning for archives
- add HostIPDefault, HostPortDefault to set default host ip/port
- reduce default spintime of ThreadPool workers to 1ms (from 5ms)
  ! remove LogFlush, introduce LogFlushLevel:
  when a log-message with level >= LogFlushLevel is logged,
  logfile is flushed, else not,
  see rts/System/Log/Level.h for int values
  ! rename /nosound to /mutesound

Bugfixes:

- fix #4417 (spring spring://username:passwd@hostname:port doesn't work)
- fix #4413 (widget:GameProgress() was no longer called for rejoining player)
- fix #4407 (SetUnitRotation broken for buildings)
- fix #4406 (crash at start)
- fix #4377 (compile with cmake 3.0)
- fix #3677 (bounceExplosionGenerator tag does not work for bouncing aircraftbomb weapon types)
- fix #4483 (Spring.SetProjectileAlwaysVisible doesn't always make a projectile always visible)
- KingRaptor found the cause of a _very_ old bug: fix green screen tint in startselector stage
- PE: fix bug where buildings weren't registered as blocking, cause they were not snapped to ground when the PE update was called (c03022cc)

AI:

- fixed return-type and value of Pathing::GetApproximateLength (pullrequest by rlcevg)

Buildprocess:

- heavily reduced files which are compiled multiple times
  ! add check for gcc >=4.7 at configure

-- 97.0 ---------------------------------------------------------
Major:

- utf8 support (kudos go to N0U)
- switch from sdl to sdl2:
- added /fullscreen (alt+enter) command ingame
- fixes some window manager bugs/problems
- KeyChains (kudos go to MajBoredom)
- split LuaRules & LuaGaia into their synced/unsynced luaStates

Cmdline Arguments:

- added new `spring spring://username:passwd@hostname:port` syntax (all except hostname are optional, i.e. spring://localhost does work fine)
- Should in longterm replace script.txt for clients, so script.txt is only needed for hosts.
  -> No need to create a file on the filesystem to start spring as a client.
- Spring links spring:// URL scheme in the OS, so you can start such URLs from browser (as UT99 did).
  E.g. you can place a spectator link on your website to join your autohosts and/or you can use it for weblobbies.
  ! changed -s/--server argument to take a param (the ip/hostname listen to)
  ! removed -c/--client argument (was only needed in times when clients didn't retrieved their script.txt from the server)

Hosting & Connecting:

- fixed direct connect in SelectMenu
- by default listen to localhost:8452 now, so you can always connect to localhost started matches

Misc:

- improve debug draw (alt+[b])
  ! demofile string in infolog changed "recording demo: %s" -> "Recording demo to: %s"
- demo filehandle is now opened on start, so you can use lsof to readback the used filename

Buildprocess:

- fix freetype not found on windows / mingw
  ! add check for gcc >=4.6 at configure
- optional fontconfig support
  ! SDL2 dependency

Rendering:

- `DebugGL` configtag works now (cause of SDL2 switch)

KeyBindings/uikeys.txt:
! removed /keyset
! removed /hotbind (Ctrl+insert)

- new KeyChains support:
- example: /bind Ctrl+a,Ctrl+b say Hello World!
- you can double bind a button this way, e.g.:
- "/bind e,e say Bar", "/bind e say Foo" -> 1st press prints Foo, 2nd press prints Bar
- use KeyChains for defaults:
  ! double hitting drawbutton sets label
  ! switching between ally,spec & all chat is now done by double hitting alt,shift or ctrl
  ! alt+return now toggles fullscreen (default shortcut e.g. in mediaplayers)

LuaDefs:

- the parser now checks the returned tables for NaNs & Infs and prints a warning if it finds some (they are sources of desyncs!)

UnitDefs:

- add new "harvestStorage" unitdef tag, UnitHarvestStorageFull lua callin and Spring.GetUnitHarvestStorage(unitid) callout
  it's for engine assisted starcraft/c&c like feature harvesting (atm only metal is supported)
  NOTE: the unload command still needs to be implemented in lua (much less work than the reclaiming one)

Lua:

- LuaRules & LuaGaia are now split into their synced & unsynced parts, and each one got its own luaState
  ! this way unsynced parts of gadgets now get `synced` callins like 'UnitCreated' etc. w/o SendToUnsynced from their synced part
- LuaRules & LuaGaia are now ~equal (except a very minor subset of differences) and get the same callins esp. all AllowXYZ callins
- LuaUI now has a GotChatMsg too (same as LuaRules always had), it's similar to the now deprecated LuaUI-only ConfigCommand callin, but is called more often
- fixed a lot bugs in Spring's c-lua code
- use lua's original math.random() code and only use streflop for it in synced luaStates
- always make math.randomseed() available (synced & unsynced)
- in future this will also allow multithreading of the different luaStates
  ! SYNCED. does a copy on access now and so is slow:
- bad: for i=1,100 do ... SYNCED.foo[i] ... end
- good: local foo = SYNCED.foo; for i=1,100 do ... foo[i] ... end
  ! SYNCED. can't be localized in global scope anymore:
- bad: local foo = SYNCED.foo; function gadget:DrawWorld() ... foo ... end
- good: function gadget:DrawWorld() local foo = SYNCED.foo; ... foo ... end
  ! moved Spring.GetGameSpeed() from SyncedRead to UnsyncedRead cause it was causing desyncs in demos
- fix Spring.Restart on windows
  ! new TextInput(utf8char) callin, needed cause of changed unicode handling in SDL2
- added Spring.SetUnitHarvestStorage(unitid, metal) & Spring.GetUnitHarvestStorage(unitid)
  ! prevent triggering of ATi driver bugs
  ! gl.LineWidth & gl.PointSize throw an error when called with incorrect argument (<=0)
  ! gl.Scissor & gl.CreateTexture throw an error on incorrect sizes, too
  ! fix gl.Viewport
- add missing colvol tags to Unit-/FeatureDefs:
- .collisionVolume.type = {"ellipsoid" | "cylinder" | "box"}
- .collisionVolume.scale{X|Y|Z}
- .collisionVolume.offset{X|Y|Z}
- .collisionVolume.boundingRadius
- .collisionVolume.defaultToSphere
- .collisionVolume.defaultToFootPrint
- .collisionVolume.defaultToPieceTree
- add optional 4th argument to Spring.GetUnitSeperation to calc the distance between the unitsphere surfaces instead of their midpos
  ! fix wrong coords & crash in Spring.RemoveGrass
- added new LOG.NOTICE (should be used instead of LOG.INFO)
- improve Spring.GetKeyBindings output (backward compatible)
- added Script.IsEngineMinVersion(major [,minor [,commits]])
- added Spring.GetClipboard() & Spring.SetClipboard(string text)
- allow to connect to lobby.springrts.com:8200 by luasocket as default

LUS:

- SetUnitValue now accepts booleans, too

Demos:

- fixed desync checker

StartScript:

- added GAME/MUTATOR{0..9} so you can load arbitrary mutator archives before the game
- make GameID the only seed of the synced random generator
  so you can define the gameid in a luaAI battle startscript and you will always get a syncing match

Installer:
! disallow to install spring over existing installations / ask for uninstall first

- don't associate .sdf with spring.exe any more

Linux:
! new BlockCompositing configtag (default true). Disables kwin compositing to fix tearing.

-- 96.0 ---------------------------------------------------------

Major:

- kill memory leaks crippling large games (#4152)
- kill micro-jitter in simulation framerate (#4171)
- kill unclean exits causing various forms of corruption (#4053)

- "nugget" is hereby officially promoted as the new umbrella term for widget or gadget

Misc:

- default disable grass for ATI cards (crashes for some people)
- made stacktrace-translation script recognize more crash/hang patterns
- improve Shard AI for BA
- fix some MSVC compile errors
- TimeProfiler (alt+b) shows now max `lag` of each item
- massively speedup caching with many .sdd archives
- fixed some FreeBSD compile issues (thx AMDmi3)
- dump (engine) shaders to infolog when they fail to compile
- more extensive logging during shutdown
- fix a number of potential issues if a thread crashes on engine shutdown
- increase precision of several timer variables to preserve intermediate results
- Lua: track more memory-allocator statistics for display in debug-mode
- Lua: limit maximum amount of memory allocated globally and per handle
- GameServer: always echo back client sync-responses every 60 frames (see #4140)
- GameServer: removed code that blocks pause / speed change commands from players with high CPU-use in median speedctrl policy
- GameServer: sleep less between updates so it does not risk falling behind client message consumption rate
- LuaHandle: make time and steps spent on garbage collection configurable
  CollectGarbage callin is restricted to run for at most
  MaxLuaGarbageCollectionTime (user-configurable) millisecs
  MaxLuaGarbageCollectionSteps (user-configurable) iterations
- GlobalConfig: add new config-option 'UseNetMessageSmoothingBuffer' (see #4053)
- LogOutput: make enabled 'section' log-levels configurable by users
- MoveDef: add key 'allowTerrainCollisions' for handling units with super-sized footprints (see #4217)
- SolidObject: make {Unit,Feature}{Pre}Damaged events receive the 'attacker' ID when object is crushed
- QuadField: add raytraced projectiles to three cells instead of one
- GameInfo: add map hardness label/value to 'i' overlay

Sim:

- Rifle weapons can hit features too now and apply impulse to the target

Lua:

- add gadgetHandler:AllowWeaponInterceptTarget
- LuaSyncedRead:
  make Get{Unit,Factory}Commands (optionally) return just the queue's size
  (ie. without copying the entire command queue into Lua VM memory, _VERY_
  important for performance when a nugget is not interested in the commands
  themselves)

  "if #Spring.GetCommandQueue(unitID, -1 | nil ) == 0 then return end" --> BAD, CREATES COPY OF QUEUE
  "if Spring.GetCommandQueue(unitID, -1 | nil, false) == 0 then return end" --> GOOD, DO THIS IF POSSIBLE

Bugfixes:

- fix #4215 (stack-buildable units on non-flat terrain)
- fix #4209 (antinukes firing at nukes targeted outside their area)
- fix #4205 (air-transports unable to unload hovercraft on water)
- fix #4204 (float3.\* clang compilation failure)
- fix #4203 (air-transport unable to load units floating on water)
- fix #4202 (S3O textures loaded multiple times instead of being cached)
- fix #4200 (secret)
- fix #4198 (amphibious units without waterweapons chasing hovercraft whilst underwater)
- fix #4197 (units moving right up to their target instead of attacking from range)
- fix #4186 (log-system crash on exit)
- fix #4173 ("Error: [Watchdog::ClearTimer] Invalid thread N")
- fix #4171 (see 'Major' section)
- fix #4163 (corrupted unit environment reflections)
- fix #4161 (Spring.Restart failing when path contains spaces)
- fix #4155 (reverse-built gunships maintain their vertical speed)
- fix #4154 (calling LUS 'AttachUnit(piece, unitID)' turns following 'Move(piece, ...)' calls into no-ops)
- fix #4152 (see 'Major' section)
- fix #4149 (rendering corruption in DynamicWater mode)
- fix #4144 (partly submerged units not hittable by non-waterweapons)
- fix #4145 (depthcharges fired by ships having trouble to reach their targets)
- fix #4138 (same as #4171)
- fix #4137 (units randomly switching team-color, old bug with new cause)
- fix #4135 (ATI driver crash with _disabled_ shadows)
- fix #4133 (cloaked units hidden even when set to alwaysVisible)
- fix #4131 (units moving to position of manualfire command if outside weapon range)
- fix #4129 (air-constructors flailing around construction targets)
- fix #4128 (units stuck in 'skidding' state: repeatedly bouncing on ground while unable to move)
- fix #4091 (gadget:MousePress() and other unsynced gadget functions not called with LoadingMT=0)
- fix #4053 (see 'Major' section)
- fix #3749 ('initCloaked' UnitDef tag overriding user cloak commands for being-built units)

- fixed wreckage heaps could be resurrected
- LaserCannon: (once again) fix range overshooting
- always clamp length of blended wind-vector to [map.minwind, map.maxwind]
- the 'AllowDeferred\*Rendering' config options are now safe to enable

-- 95.0 ---------------------------------------------------------

Major:
! new datadir structure: https://springrts.com/phpbb/viewtopic.php?f=12&t=30044 & https://springrts.com/dl/buildbot/default/master/95.0/manpages/spring.6.html
! new LuaIntro (Lua based loadscreen with examples)

- add map border rendering (+ /mapborder command)
- bugfixes & speedups
  ! remove support for SM3 maps
  (but fix #3942 so SM3 is left in a slightly less broken state for whoever wants to mess with it in the future)

Performance:

- use SSE in some matrix math
- save a little mem in Pathfinder
- reduce PFS update rate by 10%
- little speedup in Grass rendering (it's still slow on some maps)
- use 32bit shadowmap. ~7% faster than 24bit one, cause of better alignment
- move Lua GC to its own callin; fixes bug where GC was too slow to catch up and ate >>100MB ram
- nihiliate MiniMap usage by rendering it to a texture
- PathEstimator: add unsynced caches (_might_ help in big AI games)
- ExplosionGenerator:
  pre-load all static (weapon/model/piece) CEG's and
  never do lazy script-parsing anymore where possible

  (only CEG's delay-spawned by other CEG's are still
  loaded this way, rest can be dynamically reloaded)

- PFS: prevent special-case search expansion when unit is close to goal but goal is blocked
- make FeatureHandler::AllocateNewFeatureIDs more efficient in case of feature spam

Misc:

- force-align all thread entry points so SSE functions don't crash (on win32)
- allow Lua AI's to be selected from the menu
  ! removed "mods" from archive scandirs
  ! converted RotateLogFiles to a bool var (defaults to false)
  ! remove "--projectiledump" and add "--list-ceg-classes" (converted to JSON format )
- do not switch camera-mode on countdown
- colorized stdout (you can use `spring --nocolor` & SPRING_NOCOLOR envvar to disable)
- show LuaMemUsage in /debug view
  ! load only image formats which work on all platforms, current supported image formats are:
  PNG, JPG, TGA, DDS, BMP
  ! remove engine support for gamedata/messages.tdf,
  see examples/Widgets/game_messages.lua for a possible replacement
- more verbose CEG parsing (warns now about unknown tags)
- switch to a high precision clock all over the engine (including Lua)
  NOTE THAT THIS IS DISABLED BY DEFAULT ON WINDOWS BUILDS (cf. UseHighResTimer)
- copied a lot config descriptions from wiki to the engine (thx for all the wiki'ers)
- https://springrts.com/wiki/Springsettings.cfg is now automatically updated with each release
  ! disable build of spring-multithreaded (unmaintained / doesn't compile)
  ! disable luaThreadingModel > 2, can be overridden with EnableUnsafeAndBrokenMT=1
  fixes #3774, #2884, #2784, #3673, #3808
- more verbosity for spring-dedicated on exit & demo-writing
- add config "MenuArchive" to allow background image to be set which is read from the archive in the path bitmaps/ui/background/
- fix /keydebug for release builds
- updated game_end example gadget to handle quit/resigns better, reacts faster, and better code
- GuiTraceRay does not stop at water surface anymore by default
  (all ground-attack positions are adjusted in synced code now)
  --> torpedo launchers cannot overshoot, etc.
- GuiTraceRay now ignores noSelect units
- SmoothCameraController: add SMOOTHing to alt+mousewheel zoom
- change WorldObject::speed to a float4, store magnitude in .w
  (saves a number of unnecessary .Length() calls here and there)
- PathCache: track maximum size + double initial capacity, register hash collisions and switch hash function
- GameServer: add config-option ServerRecordDemos
  non-dedicated servers (eg. headless builds used as "dedicated" standin) can now record games if desired
  ACHTUNG: server demos are now stored in a separate directory "demos-server/" to prevent name conflicts!
- AssParser:
  generalize logic for multiple UV channels
  fix major issue with baked transforms
  restrict support to well-known & -maintained formats (.3DS, .DAE, .LWO, .BLEND)
  allow modellers to work around #3552 by recognizing a number of new piece-table keys
  for the root piece:
  "xaxis" = float3, "yaxis" = float3, "zaxis" = float3 (determine global orientation of model)
  for all pieces:
  "rotAxisSigns" = float3 (determines direction of script-applied rotations around each axis)
  "rotAxisMap" = number (currently can only be 0=XYZ or 3=XZY, determines coordinate conversion)

- SpringApp:
  fix old bug in fullscreen&&dualscreen mode cutting window-size in half on each SDL_VIDEOEXPOSE event
  refactor to allow choice of (win32) timer via cmdline&config
- SpringTime:
  switch to native win32 clock (same as used by SDL) on windows
  select win32 clock based on config&cmdline-parameter 'useHighResTimer'
  remove the last SDL_GetTicks dependencies

Sim:

- reduce target-lost CAI timeout from 8 to 2 seconds
- make calls to script->{Start, Stop}Moving more consistent: only run them
  when a unit transitions from zero to non-zero speed or vice versa, and no
  longer do so during in-place rotations --> animations should no longer be
  interrupted / reset
  THIS APPLIES TO ALL UNIT TYPES (AIR AND GROUND), TEST YOUR SCRIPTS
  ! remove TA-specific code in CobInstance::Create
  (this changes the argument value to COB's SetMaxReloadTime for
  units equipped with BeamLasers and / or more than one weapon)
- {Laser, Emg}Projectile: disable all collision-testing when ttl <= 0
  these projectile types could exceed their range due to fadeout
  and only removed themselves (deleteMe=true) when intensity or
  length reached 0, so without disabling coldet (checkCol=false)
  range-exploits were possible
- remove impulse "capacitance" from ground units (SolidObject::residualImpulse and ::StoreImpulse())
- apply UnitDef::myGravity to ground units (if non-zero)
  NOTE: myGravity acts as a map-gravity MULTIPLIER for aircraft; it REPLACES map gravity for land-based units
- steering/avoidance: actively avoid only mobile units, let PFS/CD handle statics

- SolidObject:
  cleanup internal physical/collision state management

  model physical drag, use it for features and skidding units

  related to the change above: Spring.Set*Physics now takes three extra number
  arguments (Set*Physics(..., number drag.x, number drag.y, number drag.z))
  to control how much an object is affected by drag forces

- Feature: set default impulseMult to 0 for 'immobile' / static wreckage (cf. new FeaturePreDamaged)
- Unit: transfer impulse to transporter if being transported
- Unit: swap order of UnitPreDamaged and HitByWeapon callins (viewtopic.php?f=23&t=30879)

- \*Weapon: fix minor targeting issues due to new SolidObject underwater state definition
- LaserCannon: calculate projectile ttl from weapon range, not weapondef range
- BombDropper:
  _always_ drop bombs/torpedoes in time to hit target
  (even if the distance from weapon to target still exceeds static
  range, range is not meaningful for these weapons anyway since it
  depends on launching unit's velocity, altitude and gravity)

- BeamLaser: fully reimplement the sweepfire logic
  still has no script interaction so use with care
  (respects reloadtime constraint but not much else)

! UnitDef:
read brakeRate for aircraft without scaling by 0.1 and make brakeRate
default to maxAcc, not maxAcc \* 3.0 (--> open up your unitdef_post's)

- WeaponDef: silently ignore bad slavedTo indices
- MoveDef:
  remove old naming-scheme restrictions (which required names to contain "tank", etc)
  instead parse key "speedModClass" with accepted values 0=Tank, 1=KBot, 2=Hover, 3=Ship

  fix insane default value for heatMod parameter (since 7bef7760f4)

- MoveType: fix LOS emit-heights for floating units being relative to ocean floor
- AAirMoveType: do not use fuel while being refueled (yo dawg...)
- StrafeAirMoveType: overshoot less when landing
- HoverAirMoveType:
  remove support for reading (and assigning to) key "brakeDistance"
  via MoveCtrl; this quantity is now calculated dynamically based
  on speed
- GroundMoveType:
  add missing TestMoveSquare checks in places where units are repositioned by coldet
  fix minor bug in GetGroundHeight (caused hovercraft to vibrate in shallow water)

- remove ancient "move transportee 1000 elmos under the map to hide it when attaching to piece -1" hack
  there is now a "void" state for such purposes; objects (units and features) in this state will never

  1. take damage, even from Lua (but can be killed by Lua)
  2. be rendered through any engine path (nor their icons)
  3. be intersectable by either synced or unsynced rays
  4. block any other objects from existing on top of them
  5. be selectable

- hovercraft now take water damage by default

Lua:
! LuaUnitDefs: deprecate the "drag" key, was an internal variable

- LuaUnitDefs:
  add more is\* functions for all unit-types known to Spring
  the complete function list as of 95.0 is:
  isTransport
  isImmobile
  isBuilding
  isBuilder
  isMobileBuilder
  isStaticBuilder
  isFactory
  isExtractor
  isGroundUnit
  isAirUnit
  isStrafingAirUnit
  isHoveringAirUnit
  isFighterAirUnit
  isBomberAirUnit
  fully deprecate the following UnitDef keys:
  after 95.0 these will all become nil, beware
  .builder
  .floater
  .canDGun
  .canCrash
  .isCommander
  .moveData
  .type
  .maxSlope

! LuaUnitDefs: deprecate canHover (check moveDef.family instead to determine if a unit is a hovercraft)

- LuaUnitDefs: log error when explodeAs & selfDestructAs are missing
- LuaUnitScript: optionally make Turn and Move insta-update piece matrices (pass "true" as the fifth argument)
- LuaRules:
  setup "proper" (transformation) state for the DrawProjectile callin

  add callin AllowWeaponInterceptTarget(number interceptorUnitID, number interceptorWeaponID, number targetProjectileID) --> boolean
  add callins FeatureDamaged and FeaturePreDamaged
  FeatureDamaged(
  number featureID, number featureDefID, number featureTeam,
  number damage
  [, number weaponDefID, number projectileID
  [, number attackerID, number attackerDefID, number attackerTeam ]]
  )
  FeaturePreDamaged(
  number featureID, number featureDefID, number featureTeam,
  number damage
  [, number weaponDefID, number projectileID
  [, number attackerID, number attackerDefID, number attackerTeam ]]
  )

! LuaSynced\*: make sure ALL weapon API functions are 1-based
UPDATE YOUR GAME'S UNIT_SCRIPT.LUA IF IT HAS LUA-SCRIPTED
UNITS AND YOU ARE NOT USING SPRING'S (BASECONTENT) VERSION

! LuaSynced\*: make {S,G}et{Unit,Feature}Blocking signatures match
Get{Unit,Feature}Blocking(number objectID) -->
boolean isBlocking,
boolean isSolidObjectCollidable,
boolean isProjectileCollidable,
boolean isRaySegmentCollidable,
boolean crushable,
boolean blockEnemyPushing,
boolean blockHeightChanges

     Set{Unit,Feature}Blocking(
       number objectID,
       boolean isBlocking,
       boolean isSolidObjectCollidable
       [, boolean isProjectileCollidable
       [, boolean isRaySegmentCollidable
       [, boolean crushable
       [, boolean blockEnemyPushing
       [, boolean blockHeightChanges ]]]]]
     )

! LuaSynced\*: slightly change {Get,Set}ProjectileTarget signatures
GetProjectileTarget now returns a number instead of a string to indicate target-type
GetProjectileTarget now always returns two arguments (if given projectileID is valid)
GetProjectileTarget now always returns a target position if projectile has no target object
SetProjectileTarget now takes a number instead of a string to indicate target-type

     GetProjectileTarget(number projectileID) --> nil | [number targetType, number targetID | table targetPos = {[1] = x, ...}]
     SetProjectileTarget(number projectileID, [number targetID [, number targetType]]  |  [number x, number y, number z]) --> nil | boolean

where targetType is one of {string.byte('g') = GROUND, string.byte('u') = UNIT, string.byte('f') = FEATURE, string.byte('p') = PROJECTILE}

- LuaSynced\*:
  add callout Spring.GetRadarErrorParams(number allyTeamID) --> nil | number allyteamErrorSize, number baseErrorSize, number baseErrorMult
  add callout Spring.SetRadarErrorParams(number allyTeamID, number allyteamErrorSize [, number baseErrorSize [, number baseErrorMult ]]) --> nil

  add callout Spring.SetUnitPosErrorParams(number unitID,
  number posErrorVector.x, number posErrorVector.y, number posErrorVector.z,
  number posErrorDelta.x, number posErrorDelta.y, number posErrorDelta.z
  [ , number nextPosErrorUpdate]) --> nil
  add callout Spring.GetUnitPosErrorParams(number unitID) --> nil |
  number posErrorVector.x, number posErrorVector.y, number posErrorVector.z,
  number posErrorDelta.x, number posErrorDelta.y, number posErrorDelta.z,
  number nextPosErrorUpdate

  add callout Spring.SetFeatureBlocking(args = see SetUnitBlocking) --> boolean
  add callout Spring.SetPieceProjectileParams(number projectileID,
  [ number explosionFlags, [ number spinAngle, [ number spinSpeed,
  [ number spinVector.x, [ number spinVector.y, [ number spinVector.z ]]]]]]) --> nil
  deprecate the Spring.SetProjectileSpin\* functions
  add callout Spring.GetFeatureBlocking(number featureID) --> boolean collidable, boolean crushable, boolean blocking
  add third return-value to Spring.GetUnitBlocking (signature is the same as that of GetFeatureBlocking)

- LuaSyncedCtrl:
  add callout SetProjectileAlwaysVisible(number projectileID, boolean alwaysVisible) --> nil
  add callouts SetFeatureVelocity and SetFeaturePhysics (these work like their SetUnit\* counterparts, reference the wiki docs)

  cleanup arg-parsing of Spring.SetUnitPieceCollisionVolumeData callout
  new signature: SetUnitPieceCollisionVolumeData(
  number unitID,
  number pieceIdx,
  boolean enable,
  number scales.x, number scales.y, number scales.z,
  number offset.x, number offset.y, number offset.z
  [, number volumeType [, number primaryAxis]]
  )

! LuaSyncedRead: deprecate the Spring.GetProjectileSpin\* functions

- LuaSyncedRead:
  add Spring.GetPieceProjectileParams(number projectileID) -->
  number explosionFlags, number spinAngle, number spinSpeed,
  number spinVector.x, number spinVector.y, number spinVector.z
  add callout Spring.GetUnitWeaponCanFire(number unitID, number weaponNum) --> boolean
  add callout Spring.GetFeatureVelocity(number featureID[, boolean norm]) --> nil | number vel.x, number vel.y, number vel.z[, number |vel|] and Spring.GetProjectileDirection(number projectileID) --> nil | number dir.x, number dir.y, number dir.z
  add callout Spring.GetProjectileDefID(number projectileID) --> nil | number weaponDefID
  using this to get a weaponDefID is HIGHLY preferred to indexing WeaponDefNames via GetProjectileName
  add callout Spring.GetUnitRotation(number unitID) --> nil | number pitch, number yaw, number roll (WIP)
  extend arguments to Spring.TestMoveOrder
  old signature: TestMoveOrder(
  number unitDefID,
  number pos.x, number pos.y, number pos.z
  ) --> boolean
  new signature: TestMoveOrder(
  number unitDefID,
  number pos.x, number pos.y, number pos.z
  [, number dir.x
  [, number dir.y
  [, number dir.z
  [, boolean testTerrain
  [, boolean testObjects
  [, boolean centerOnly ]]]]]]
  ) --> boolean

  added GetUnitWeaponTarget(unitID, weaponID) ->
  nil | number targetType [, bool isUserTarget, table targetPos = {x, y, z} | number targetID]
  (returned number of args is always 3 if targetType != None)

- LuaUnsyncedRead:
  add callout Spring.GetPixelDir(int x, int y) -> number dirx, number diry, number dirz
  add callout Spring.GetVisibleProjectiles(
  [ number allyTeamID = -1
  [ , boolean addSyncedProjectiles = true
  [ , boolean addWeaponProjectiles = true
  [ , boolean addPieceProjectiles = true ]]]]
  ) --> nil | {[1] = projectileID, ...}

! Spring.GetUnitNearestEnemy doesn't return neutral units anymore

- Spring.GetProjectileVelocity now returns (targetPos - startPos) for BeamLasers and other hitscan weapons
- Spring.Get{}Velocity: always return speed.w (it's free now)
- Spring.SpawnCEG: allow number (cegID) as first arg + return a second value (number cegID)
- add Spring.GetLogSections() & Spring.SetLogSectionFilterLevel(string sectionName, string|number logLevel)
- add new Game. constants: Game.gameName, Game.gameVersion, ...
- allow to save strings in RulesParams
- add 'parent' field to Spring.GetUnitPieceInfo
- GadgetHandler:
  add several missing callins (AllowStartPosition, ...)
  expose actionHandler through gadgetHandler table (seems to be a common enough mod)
  log if callin is not implemented
  unify callin lists (now all in callins.lua)
  add missing projectileID argument to UnitDamaged

- LuaOpenGL::Draw{List,Func}AtUnit: just use drawPos or drawMidPos

- gl: add callout UniformArray(number location, number type, table uniforms) -> nil
  (type can be one of {0 = int, 1 = float})
- gl.CreateShader: support uniform arrays (table values with more than 4 and up to 32 elements)
- gl.CreateShader: make 'definitions' a recognized key (whose value should be a subtable or string of shader #define's)

- Spring.{Get,Diff}Timer(s): save some precision (important with new high-resolution timers)
- Spring.DiffTimers: accept optional third argument (boolean) to return dt in milliseconds

- add some utility functions to the math table, see https://springrts.com/wiki/Lua_MathExtra

Rendering:

- LoadingMT: add a runtime check that should detect incompatibilities
- disable ARB shaders with Intel/Mesa drivers (can be overridden by setting ForceEnableIntelShaderSupport=1)
- reduce used varyings in SMF GLSL shader in the hope it will fix advmapshading on some lowend GPUs
- render MiniMap to a texture and update it async from screen (new configtags: MiniMapRenderToTexture & MiniMapRefreshRate)
- do not draw ground reflections in water if drawGround=false
- 3DModel: do not set tex1 to 'default.png' (it breaks assimp's auto-texturing ability)
- SMFFragProg: fix '#version must occur before any other statement' and eliminate dynamic branching

- implement Lua-bindable map and model normal/depth/material buffers
  note: this is _FULLY_ compatible with custom Lua UnitRendering
  shaders (even easier if some minor framework changes are made)
  so the vast majority of deferred rendering operations is now
  within Lua's abilities

  new configuration-keys to enable/disable G-buffer draw-passes:
  AllowDeferredMapRendering (default true)
  AllowDeferredModelRendering (default true)

  new LuaUnsyncedCtrl callouts to enable/disable G-buffer updates:
  Spring.SetDrawGroundDeferred(boolean) -> nil
  Spring.SetDrawModelsDeferred(boolean) -> nil

  new LuaUnitRendering Material keys:
  "standard_shader" (same as "shader")
  "deferred_shader" = {vertex = ..., ...}
  "deferred" (same as "deferred_shader")

  deferred fragment shaders must _ONLY_ write to gl_FragData[i] where
  i = {0 = normals, 1 = diffuse, 2 = specular, 3 = emissive, 4 = misc}

  new Lua texture names:
  "$map_gb_nt" or "$map_gbuffer_normtex"
  "$map_gb_dt" or "$map_gbuffer_difftex"
  "$map_gb_st" or "$map_gbuffer_spectex"
  "$map_gb_et" or "$map_gbuffer_emittex" // for emissive materials (bloom would be the canonical use)
  "$map_gb_mt" or "$map_gbuffer_misctex" // for arbitrary shader data
  "$map_gb_zt" or "$map_gbuffer_zvaltex"

  "$mdl_gb_nt" or "$model_gbuffer_normtex"
  "$mdl_gb_dt" or "$model_gbuffer_difftex"
  "$mdl_gb_st" or "$model_gbuffer_spectex"
  "$mdl_gb_et" or "$model_gbuffer_emittex"
  "$mdl_gb_mt" or "$model_gbuffer_misctex"
  "$mdl_gb_zt" or "$model_gbuffer_zvaltex"

Buildprocess:
! updated gentoo overlay url to https://github.com/springlobby/overlay

- switch to mingw64 for the windows build
- remove some custom Find\*.cmake files
- fix #3645 Can't build if source directory has white spaces
  ! removed VS project files / fix compile with Visual Studio 2012 Express for Windows Desktop:
  use cmake -G "Visual Studio 11" to generate project files
  dropped support for Visual Studio 8, 9 & 10
  see https://springrts.com/wiki/Building_Spring_on_Windows
  ! compile Spring using c++11 standard
- fix clang compile errors + some bugs detected by clang
  ! slightly refactored build system, now every target can be disabled:
  spring-dedicated, spring-headless, spring-legacy, spring-multithreaded
- more unittests (extended FileSystem, timers, unitsync, SSE Matrix Math, ...)
- fix some errors when running spring in-source (AI / Interfaces are still broken)
  ! remove unused libspringserver

Bugfixes:
! fix {Get,Set}UnitWeapon{Fire,HoldFire,TryTarget,TestTarget,TestRange,HaveFreeLineOfFire} WeaponID argument to use the same 1-starting indexes as rest of lua interface

- fix #4092 (empty pieces Explode()'d with PF_FIRE flag leave behind small stationary fire FX)
- fix #4079 (resurrecting could lead to multiple units created by one wreck)
- fix #4078 (float-->int conversion overflow)
- fix #4073 (fuel-level of unit is lowered upon completion)
- fix #4070 (units can skip attack orders in their queue)
- fix #4068 (the "wrong cursor for certain attack commands" part)
- fix #4057 (double BeamLaser visualisation)
- fix #4054 (homing missiles miss radar dots of flying units)
- fix #4047 (ground decals don't fade away)
- fix #4043 (constructors can build large units ontop of themselves & walk through 'gaps')
- fix #4023 (allow 'key'=value options-table format in Spring.GiveOrderTo\*)
- fix #4019 (receiveTracks not working reliably)
- fix #4011 (GetTeamStartPosition cannot determine whether start position was placed by player)
- fix #4010 (disable /spectator before gamestart)
- fix #4007 (large-radius air transports failing to load transportees)
- fix #3980 (air transports set to 'Land' wander around while idling if loaded)
- fix #3979 (unhelpful error message)
- fix #3974 (out-of-fuel bombers set to 'Land' fly all over the map)
- fix #3973 (aircraft taking off in wrong direction on attack orders)
- fix #3961 (expect ArmorDef subtables in array-format, show warning if armordefs contains old-style subtable entries)
- fix #3959 ("flying" ground-units colliding with aircraft looks unrealistic)
- fix #3957 (Unit icons of underwater units are drawn at water level instead of actual depth)
- fix #3951
  submissile was being checked in the base-class for all
  weapons despite being a TorpedoLauncher-only property
- fix #3948 (broken voidwater shading)
- fix #3941 (CBitmapMuzzleFlame doesn't render when dir is <0 -1 0>)
- fix #3937 (transport unloading on top of non-blocking objects)
- fix #3924 (add callout Spring.UpdateInfoTexture(number texMode) --> boolean)
- fix #3890 (Unloaded units fly across the map)
- fix #3873 (lag while loading model-less features)
- fix #3862 (aircraft overshooting/flailing around repair pads, also #3834)
- fix #3861 (BeamLaser does not hit if it starts within hitvolume)
- fix #3855 (LosHandler bug that made provided full LOS on underwater units in radar coverage)
- fix #3835 (try to avoid clash between Weapon::AutoTarget and MobileCAI::AutoGenerateTarget)
- fix #3785 (Construction aircraft getting stuck)
- fix #3783 (add FailedCommand sound to default sounds.lua)
- fix #3765 (make Gaia the default feature team, not -1/0)
- fix #3754 (high decal-strength unit tracks drawn twice due to alpha-wraparound)
- fix #3752 (safeguard execution of SimFrame() against garbage keyframe messages)
- fix #3743 (partially: make non-hovering aircraft start turning to goal during takeoff)
- fix #3739 (apply impulse to aircraft)
- fix #3737
  gadget:AllowStartPosition() now receives the raw
  unclamped start position as three extra arguments
- fix #3736 (TextureAtlas crash)
- fix #3720 (unit pushed through impassable part of factory)
- fix #3718 (units moving in circles near obstacles)
- fix #3715: make all info-textures individually queryable by Lua (gl.Texture)
  "$info" and "$extra" still map to the active info-texture as before
  "$info_losmap" or "$extra_losmap" map to the L-view texture
  "$info_mtlmap" or "$extra_mtlmap" map to the F4-view texture
  "$info_hgtmap" or "$extra_hgtmap" map to the F1-view texture
  "$info_blkmap" or "$extra_blkmap" map to the F2-view texture

  textures contain no data until user switches to their respective modes
  non-active textures are not updated in the background, so will go stale

- fix #3714 (minimap icons of enemy units visible after /spectator plus /specfullview 0)
- fix #3697
  Spring.GetPositionLosState now returns a 4th argument which indicates
  if a position is actively being radar- or sonar-jammed by any allyteam
- fix #3686 (free flying ground units follow terrain slope)
- fix #3683 (minimap textures not freed on exit)
- fix #3681 (make widgetHandler:LoadConfigData() more robust)
- fix #3675 wrong loglevel in unit_script.lua
- fix #3665
  AllowStartPosition now receives the ready-state of
  each player as an argument too, making its complete
  signature as of 95.0:

  AllowStartPosition(
  number clampedPos.x, number clampedPos.y, number clampedPos.z,
  number playerID,
  number readyState,
  number rawPickPos.x, number rawPickPos.y, number rawPickPos.z
  )

  where readyState can be one of:
  PLAYER_RDYSTATE_UPDATED = 0 -- player picked a position
  PLAYER_RDYSTATE_READIED = 1 -- player clicked ready
  PLAYER_RDYSTATE_FORCED = 2 -- game was force-started (player did not click ready)
  PLAYER_RDYSTATE_FAILED = 3

  ! the default 'failed to choose' start-position is now just (0,0,0), not (0,-500,0)

- fix #3662 (secret)
- fix #3660 (memory leak in Spring.GetVisibleFeatures)
- fix #3655 (UnitDef::canMove misinterpreted by factory CAI code)
- fix #3653 (clash between mutually push-resistant units moving to same spot)
- fix #3643 (get PFS and CD to agree regarding factories)
- fix #3641 (fix several issues with units using per-piece collision volumes)
- fix #3640 (Groundplates don't disappear when out of LOS)
- fix #3637 (desyncs due to not versioning cache-data, also #3692)
- fix #3633 (non-3DO projectile models not being rendered fully)
- fix #3623 (broken PPCV offsets for DAE models)
- fix #3617 (ship transports can't load units)
- fix #3614 (units stuck on narrow terrain passages)
- fix #3606 (TorpedoLauncher force-attack aims at water surface)
- fix #3598 ("vibrating" ground units due to gravity)
- fix #3373 (crash when exiting before countdown finished)
- fix #1368 (AllowBuildStep only called for building)

- fix ground units not moving in FPS mode
- fix (non-hovering) aircraft jittering at high altitude
- fix submarines being considered "underwater" even when pos.y + height > 0
- fix subtle bug in LegacyAtlasAlloc::Allocate exposed by Intel GMA
  atlas could grow LARGER than its maximum size in x-dimension due to TEXMARGIN not being considered
  3DO texture atlas has a maximum size of 2048 pixels but grew to 2050 pixels in at least one \*A game
  on Intel hardware with maxTextureSize=2048 this caused driver memory corruption --> random crashes
- fix units switching team-colors
- fix debug-vectors for ground units being drawn even when not cheating or not spectating
- fix LoadScreen exception being thrown just for failing to load a startpic
- fix crash in Spring.GetVisibleUnits on bad teamID value
- fix C stack overflow in Shard
- fix logLevel of WatchDog stacktraces
- fix decals were rendered twice
- fix spike CEGs
- fix units with PPCV's ignoring mouse-rays; run PPCV test before CV test
- fix possible bug in MoveCtrl.SetMoveDef

Sound:

- don't automatically set "MaxSounds"
- try to fix "[Sound] Error: CSoundSource::Play: Unknown Internal Error" on Mac

AI:

- removed unused AI_UnpauseAfterInit config setting

UnitSync:

- add DeleteSpringConfigKey(const char\* name)

-- 94.1 ---------------------------------------------------------

- bump path cache version

-- 94.0 ---------------------------------------------------------
Misc:

- default LogFlush to true again
- fix crash at start related to Optimus
  ! remove Tobis rapid client (its broken)
- add multi engine type support
- recycle unit and feature ID's less quickly again, a la 91.0 and prior
  although this was an unintentional change in 92._ / 93._, it exposed
  many bugs in widgets and gadgets which INCORRECTLY assumed ID's were
  globally unique for all time --> check your code for more latent bugs

Rendering:

- automatic runtime recompression of groundtextures to ETC1 (future MESA drivers should support ETC)

Bugfixes:

- fix #3591 (UnitFromFactory(..., userOrders) always passes userOrders=false)
- fix #3561 (units getting stuck on edges of impassable areas --> pathing is no longer FUBAR)
- fix #3575 (QTPFS terrain deformation lagspikes)
- fix #3580 (decreased unit/feature ID randomness)
- fix #3557 (TorpedoLauncher weapon fires only when on land)
- fix #3564 (sun-flare visible when obscured by terrain)
- fix #3553 (sun-flare has harsh edge)
- fix #3562 (units are reluctant to fire in crowded situations after 'fix #3488')
- fix #3559 (avoidGround weapon tag is broken)
- fix #3563 (units rub against building edges and slow down to a crawl) and #3592 related to it
- fix #3567 (transported nanotowers periodically teleporting to ground)
- fix #3520 (kamikaze units always have bad attack cursor when giving manual attack command)
- fix #3531 (wrong textures assigned to assimp models)
- fix #3545 (AddUnitImpulse() causes unit to move in random direction)
- fix #3544 (units briefly invisible when transferred to another team)
- fix #3490 (ships cannot be transported by aircraft)
- fix #3596 (in debug-mode the firing-cone visualisation of weapons points up)
- fix 'fix #3506 (wrong calculation of goffset for assimp model pieces)'
- fix pseudo-static units (nanotowers, etc) no longer being transportable
- fix #3569 (texturing for OBJ and ASS projectile models)
- fix OBJ models being parsed by assimp
- fix SEGV on exit
- fix flashing team colors
- fix some trees drawn after being destroyed

Lua:

- add Spring.UnitWeapon{Fire,HoldFire}(number unitID, number weaponID) --> nil
- add callouts Spring.GetUnitWeapon{TryTarget,TestTarget,TestRange,HaveFreeLineOfFire}(number attackerID, number weaponID, number attackeeID | [number targetPosX, number targetPosY, number targetPosZ]) --> boolean
- add Spring.SetProjectileTarget(number projectileID, [number objectID, string objectType] | [number x, number y, number z]) --> boolean
- add Spring.GetProjectileTarget(number projectileID) --> number targetID, string targetType
- add Spring.SpawnProjectile(number weaponDefID, table projectileParams) --> number projectileID | nil
- add MoveCtrl.SetMoveDef(number unitID, number moveDefID | string moveDefName) --> boolean
- add projectileID argument to UnitPreDamaged
  OLD signature: unitID, unitDefID, unitTeam, damage, paralyzer [, weaponDefID [, attackerID, attackerDefID, attackerTeam] ]
  NEW signature: unitID, unitDefID, unitTeam, damage, paralyzer [, weaponDefID, projectileID [, attackerID, attackerDefID, attackerTeam] ]

-- 93.2 ---------------------------------------------------------

- fixed unitsync in linux static builds

-- 93.1 ---------------------------------------------------------
Bugfixes:

- fixed treedrawer crash
- fixed MT crashes
- fixed X11 cursor offset
- fixed smooth camera transitions
- fixed clipped sun
- fixed selection sounds
- fixed attack cursor for kamikaze units
- fixed wrong assimp texture

-- 93.0 ---------------------------------------------------------
Bugfixes:

- fixed disappearing GUI with non-GLSL drivers & grass rendering (3dtrees=1)
- fixed not working assimp textures
- fixed /ally
- fixed /chat[|ally|spec|all], so it doesn't misbehave when called from lua
- fixed attack-ground orders placed on water
- fix bug in footprint overlap detection reported by dnw
- make dead/crashing/stunned units unshareable (may fix #3476)
- fixed objects with cylindrical CVs would not take damage if hit in rear endcap
- block Lua from creating non-uniformly scaled COLVOL_TYPE_SPHERE volumes
- make sure pastetext and buffertext don't strip whitespaces from the attached message
- allow whisper between spec <-> spec (player -> spec and spec -> player whispers are still forbidden)
- fixed flying transport reservation not flushed properly with area commands (#3478)
- fix #3481 (unit targets not flushed on cloak + jam)
- fix #3488 (avoidFriendly check works improperly if firepoint is inside friendly unit)
- fix bug in ray-ground intersection logic
- make mutual push-resistant unit collisions more like 0.82.x (smoth's issue)
- fix some cases of units getting perma-stuck (s.t. even spammed user orders cannot free them) on terrain, esp. #3423
- fix #3506 (wrong calculation of goffset for assimp model pieces)
- fix #3505 (ground-transported units cannot be killed)
- fix #3465 (WeaponDef infolog error messages are unhelpful)
- fix attack-ground orders placed on water
  GuiTraceRay now stops at water surface by default, not ocean floor
  add sixth argument 'ignoreWater' to LuaUnsyncedRead::TraceScreenRay

Misc:

- support double-resolution yardmaps for buildings (these require four times as many characters, beware)
  the engine will interpret a yardmap string as such if its FIRST (and ONLY its first) character is 'h'

Pr-Downloader:

- workaround CVE-2013-0249 (disable all protocols expect http/https)
- reduce log messages
- fix invalid timeout
- fix ignored depends in http-downloads

Assimp:
! apply rotations & scalings

- fixed per-piece colvols

-- 92.0 ---------------------------------------------------------
Major Changes:

- Linux cross-distro binary builds
- much faster Sim (~20% and ~50% with working OMP)
- new cmdline arguments
- better pathing
- add experimental map generator (see https://springrts.com/wiki/Mapgen for details)
- some new sim/weapon features

AI:

- Shard update
- AAI: refactor / fixes
- AAI: add BAR config
- AAI: update XTA config, to work with XTA > 9.71
- fix bug in /aikill

pr-download:

- new --download-engine
- fixes / speedups
- add c-api to shared lib & use it for pr-downloader

Unitsync:
! fix return in GetInfoMapSize (#2996)

- no crashes in unitsync without init any more (#3120)
  ! ProcessUnits, GetUnitCount, GetMapCount, GetMapArchiveCount,
  GetInfoMap, GetPrimaryModInfoCount,GetPrimaryModArchive,
  GetSideCount, GetModOptionCount, GetCustomOptionCount,
  GetSkirmishAICount, GetInfoValueInteger, GetInfoValueFloat,
  GetOptionType, GetOptionNumberDef, GetOptionNumberStep,
  GetOptionStringMaxLen, GetOptionListCount and GetModValidMapCount
  returns now -1/-1.0f on error

Lua:
! make TeamHandler::IsActiveTeam check Team::isDead
! gl.UnitPieceMatrix & gl.UnitPieceMultMatrix do now the same and work from UnitSpace & aren't recursive anymore (don't depend on the parent-piece)
! remove LuaLobby

- enable luasocket as default and always allow to listen (UDP & TCP)
- new Spring.GetTerrainTypeData (already on wiki)
- new Spring.GetUnitCurrentBuildPower(unitID) -> number 0..1
- new Spring.SetUnitNanoPieces(unitID, {piece1, piece2, ...})
- new Spring.GetUnitNanoPieces(unitID) -> table {piece1, piece2, ...}
- replace Spring.Echo with Spring.Log in most places in cont/
- LuaUnsyncedCtrl: make Spring.GetConfig\* return non-nil (= 0) values when LuaModUICtrl=0 (3280)
- Lua: make Spring.GetPlayerRoster really return a boolean for spectator as the wiki says
- fix crash when calling Script.Kill() (3292)
- fix Spring.GetUnitNearestAlly takes self into account (3293)
- Spring.SetTerrainTypeData now only updates pathfinder when there is a real change (performance)
- fix crash in LuaUnitDefs when trying to read deathExpWeaponDef (#3333 & #3325)
- add Spring.GetReplayLength()
- add Spring.GetConfigParams()
- add Spring.GetUnitWeaponTryTarget(uid, wid, [number posx, number posy, number posz] | [number enemyid]) -> nil | boolean
- fix error spam on malformed Lua-issued unload commands (#3378)
- fix crash caused by comparing strings with numbers in base-content's weapondefs_post.lua
- add mapHardness to the Game constants table (#3390); make Spring.GetGroundInfo now match its wiki documentation
- make Create{Feature, Unit} take an optional (7th, 9th) {feature,unit}ID argument to spawn a feature/unit with a specific ID
- make CreateUnit take an optional (10th) builderID argument
  ! deprecate LuaUnitDefs.extractSquare
  ! replace LuaFeatureDefs.deathFeature(string) by LuaFeatureDefs.deathFeatureID(number)
- fix crash in Spring.GetVisibleFeatures (#3400)
- fix Spring.GetFeatureDirection returning direction in (x,z)-plane only (#1599)
  ! couple the AllowWeapon\* callins to SetWatchWeapon state
- add optional 4th argument to Spring.SetUnitImpulse to control a unit's impulse decay-rate (now unhardcoded)
  ! Game.version now returns the same as `spring --sync-version`, e.g. "92.0" or "91.0.1-1283-gf3805a7"
- new Game.buildFlags (unsynced only) to get additional engine buildflags, e.g. "OMP" or "MT-Sim DEBUG"
- Better notification and protection from unbalanced OpenGL matrix stack after return from Lua rendering code

CollisionVolumes:
! drop support for ellipsoid-cylinders and ellipsoids
cylinders with round(!) baseplates are still supported!
simplify how damage from an explosion is calculated, fewer magic numbers, etc
no more distance approximations means no more invincible units (or features)
and no more general "weird" cases like 3103
THIS WILL CAUSE BALANCE CHANGES, BE PREPARED
! get rid of TYPE*FOOTPRINT, it was an alias for TYPE_SPHERE in the continuous case which is now default and the
original intent (a shape defined by the CSG intersection of sphere and footprint) would have been tricky to get
right
instead support a {Unit,Feature}Def key "useFootPrintCollisionVolume" which
makes a collision volume a box with dimensions equal to the object's footprint
(this overrides any custom scaling and/or offsets)
! let projectiles explode \_on* collision volumes and not in a interpolated pos in front of it

- rework projectile-vs-volume special cases so they can be distinguished better for the above
  ! rewrite TraceRay::{TestConeHelper, TestTrajectoryConeHelper} for #3399
  these now no longer use any bounding-radius approximations to
  figure out if an object will potentially be hit, but calculate
  the exact colvol surface distance taking spread into account

Pathing:

- faster updates
- legacy: use omp in ::Update to multithread a function that eats 40% of the time spent in ::Update
  - this gives a _massive_ boost with like >30% more FPS
  - it needs low latency omp threads to work, those are currently broken in gomp, the bug will be fixed with the next release of gcc
- QTPFS: a lot of fixes & optimizations, fixed dev-build desync (#3339), made cache reads/writes atomic, read constants from MapInfo
- give pathfinder debug-data drawing its own shortcut (alt+p)
- rewrite collision handling between units and static objects (again)
  instead of using impulse to push units away, do SCII-style strafing
  results: www.youtube.com/watch?v=4rycY4JfLGs
  ! put on upper limit on directional slope tolerance for "downhill" slopes
  before, units could move across ANY downhill slope if they ended up there
  and directional checking was enabled; now the downhill slope-tolerance is
  limited to twice the uphill tolerance
  ! set avoidMobilesOnPath to true by default again
  pro: groups moving without a ctrl-order clump much less
  contra: groups moving with a ctrl-order are less organized
- fix units moving orthogonally across too-steep faces (and getting stuck afterwards)
- fix humping triggered when unit crushes feature and feature spawns a non-crushable
  replacement (if unit's next waypoint happens to be inside replacement's footprint)
- change precondition for crushing units from 'collider must be moving' to 'collider impulse must exceed collidee impulse'
- units that are mutually forbidden from pushing each other will no longer get stuck blocking the other party
- units moving toward the same spot will no longer clump up into a jiggly ball (#3355 & #3381)
- units with low delta-v no longer receive bogus move orders after being told to stop (#3358)
- actively push units away from speedmod=0 squares when possible

UnitScripts:
! LUS/COB: deprecate QueryNanoPiece() makes SimCode ~10% faster use the new Spring.SetUnitNanoPieces instead
Note, once set the engine won't call `QueryNanoPiece` anymore and instead randomly toggles between the set nanopieces,
saving a lot time-consuming COB/LUS calls.
! LUS/COB: cache QueryNanoPiece() results
Note, this stops QueryNanoPiece() calls after 31 times no matter what!
This breaks ZK's LupsNanoSpray, fixed version can be found here http://gw.gd/Zaae.

Models/Assimp:

- support a 2nd texcoord
- auto-calc midPos
- read texture filepaths from modelfile if available
- extended auto-finding of textures

Weapons/WeaponDefs:
! remove of manualBombSettings (backward-compatibility code in weapondefs_post.lua)
! fix: half craterAreaOfEffect the same way as it is done with damageAreaOfEffect
! removed color1 (hue) & color2 (sat). Use rgbColor instead!
! removed isShield. Use weaponType="Shield" instead!

- new 'interceptSolo', if true (=default) don't allow any other interceptors to target the same projectile
- allow any weapons to be interceptors/targetables (not that all combinations work/make sense)
  ! stop subs/AA-fighters trying to chase hovers and tanks (check all equipped weapons if they can target the unit, before _auto_ chasing enemies in FireAtWill)
  ! all weapons will now obey avoidFriendly, avoidNeutral, avoidFeature + avoidGround
  ! add new avoidGround weapondef tag (default true)

FeatureDefs:
! remove "nodrawundergray" now that there is a proper "floating" tag

GameServer:

- a new GameID tag to script.txt
  ! always account local players (even when host is a spectator!)
  ! don't lower-limit gameSpeed to `userSpeedFactor * 0.5f` when userSpeedFactor is greater than 2
- make game ways more responsive when running at high speeds

Misc:

- add /ReloadShaders
- less jerky camera transitions (thx cleanrock)
- make camera a bit smoother by adding some micro-transitions
- pressing F5 no longer hides the minimap in dualscreen mode (#3095)
- SMF/MapInfo: new tag voidGround
- fix 'only FreeCamera allows to set FOV ingame' (3291)
- do not spam unit self-destruct messages when spectating
- new configtag "StacktraceOnGLErrors": Create a stacktrace when an OpenGL error occurs (only available in DEBUG builds)
- fix performance leak in CSolidObject::Block()
- make UnitScript::{ScriptToModel, ModelToScript} O(1) instead of linear
  (TODO: UnitScript::HaveListeners is another new O(n) source)
  ! remove OSC
- add new `cursorattackbad` mousecursor, used for static defense when giving a CMD_ATTACK
- get rid of the 'extractSquare' non-feature for extractor buildings
- when a team dies, redistribute unit-limits over the remaining teams in that team's allyteam (also means dead teams can no longer produce any units)
- record demos in-memory to make speccheating 1% harder
- make hovercraft with non-zero waterline transition into/out of water more smoothly
- make unit movement more physically-based:
  add modrule 'allowGroundUnitGravity'; allows fast units to catch air
  add modrule 'allowHoverUnitStrafing'; allows hovercraft to slide in turns
  ground units cannot change speed or direction in mid-air
- in god-mode, units can now be selected and commanded by any player even when not spectating (#3412)
- fix multiple build-started sounds being played when ordering single builder to construct something
  ! UnitDef::can{Restore, Repair, Reclaim, Capture, Resurrect} now default to true only if the unit is
  a builder and has the corresponding {Terraform, Repair, Reclaim, Capture, Resurrect}Speed property
  set to a non-zero value

Rendering:

- speedup minimap icon rendering
- fix a few OpenGL errors reported by MESA drivers
- fix SimpleParticleSystem directional particles disappearing (#1665)
- ROAM: resize pool when running out of nodes fixes rendering on maps like Knockout
- disable occlusion query in BumpWater it doesn't work and just makes it slower
- move `lastFrameTime` calculation from the end of a drawframe to the beginning this way vsync locktimes are counted to the previous frame time
  ! removal of DepthBufferBits & StencilBufferBits
- BumpWater renders now with shadowmap & infotex
- clamp map ShadowDensity 0..1 (fixes shadow distortions on maps like Iamma)
- use VBOs for S3O and OBJ model piece geometry
- fix grass shadow rendering (no self-shadowing yet)
  ! changed `usable` range of GrassDetail configtag, so you can set it now >10 to increase viewrange w/o getting Tribble plushies

Sim Bugfixes:
! stop buildframe decaying on reclaiming

- fix unload commands close to map edges are ignored (3175)
- fix nukes (3269)
- fix some issues with bombers (3253)
- fix MANUALFIRE hangs despite no target lock
- fix gunship jumping on takeoff (3526)
- make sure unsupported build orders are never given to new factory buildees (3257)
- fix aircraft failing to land properly on repair pads (3260)
- fix gunships jittering when stunned (3262)
- fix builders 'ignoring' wait-orders when reclaiming
  ! fix units failing ground-attack orders when their "main" weapon != weapons[0] (3276)
  logic change: MobileCAI::ExecuteAttack now cycles through all weapons until
  it finds one which can execute the order, and no longer depends on weapon[0]
  being the "main" weapon
- fix units gaining negative experience when shooting at dead targets
- fix old logic error in obstacle avoidance handling (IMMOBILE objects were being ignored --> no wonder the "infinite bouncing bug" was so pronounced)
- fix 'Crashing aircraft can be reclaimed' (3288)
- fix 'Units get stuck in sea labs on build completion' (3290)
- fix 'Paralyzed landed aircraft start moving forward slowly' (3287)
- fix bug reported by jamerlan (https://springrts.com/phpbb/viewtopic.php?p=529204#p529204)
- make HoverAirMoveType aircraft able to crash (3289)
- fix 'Move/Turn (piece) fails with negative speed' (3300)
- fix "Total velocity loss during a prolonged unit-to-unit [skidding] collision" (3302)
- fix 'Units with height advantage are unable to force fire with that range' (3303)
- fix air transports have trouble loading multiple units if units are large (3307)
- fix partly reclaiming factory allows unit teleportation/remote destruction (3308)
- fix 'SetFeaturePosition snapToGround get reset when there is terraform nearby' (3243)
  ! disable collision checking between transporter & transportee
- fix being-built units having order queues overwritten upon completion (#3342)
- fix FPE's in {Large}BeamLaserProjectile (#3413 & #3414)
- fix impulse not being applied to features (unit wrecks)
- allow underwater units to build stuff on the waterplane

Bugfixes:

- fix network connection is not properly closed when an error occurs in pregame (3166)
- extend desync message (send in debugmode for specs too and send correct & got checksum)
- fix "Transport endpoint is not connected"
- fix secret exploit
- fix secret exploit #2
- fix memory leaks, particularly when the game is minimized
- fix several crashes in multithreaded exe

Cmdline Arguments:

- new --game (-g) & --map (-m)
  - if both parameters are set, this starts spring a with minimal setup and it sets the modoption minimalsetup = 1
  - supports fuzzy search e.g. `spring -g ba -m Delta` or `spring -g zk -m "Delta X"`
- renamed shortarg -m (--minimised) to -b (~background)
- new `spring --list-def-tags`, will in future return all Unit-, Weapon-, FeatureDefs in a `--list-config-vars` manner (not finished yet)
- new benchmark mode (`--benchmark %time_min --benchmarkstart %starttime_min`) writes a benchmark.data parsable by gnuplot

OS:

- set affinity of omp threads
- search script.txt & demofiles in $PWD
- detect Valgrind and disable Watchdog & syncing (it won't let us change the FPU state) if it is running
- don't install crash handler when core dumps are enabled

Buildprocess:

- boost 1.50 compatibility
- detect ld.gold and compress debugsymbols
- update 7z to v9.22
- use tcmalloc when found (new USE_TCMALLOC use-flag)
- add target headercheck

Windows:

- OpenMP build
- update to boost 1.50
- slightly improve speed of zip / sdz archives (#3338)
- Nvidia Optimus support
- removed springsettings.exe as its directly included in springlobby

Linux:

- Linux's ctrl+c: `hardkill` after 10sec (and 5 seconds softkill)
- workaround issue in FOSS ati drivers with NPOT cursors

-- 91.0 ---------------------------------------------------------
Bugfixes:

- fix Linux sync errors
- fix QTPFS deadlocks

-- 90.0 ---------------------------------------------------------
Bugfixes:

- fix endless loop in Shard in mingw32 builds
- fix builders sometimes don't notice when a building was finished by another unit
- fix SkyBox corruption
- fix QTPFS deadlock at start
- partially revert the "lock-on targeting" behavior:
  If a user-selected target moves out of range, the lock is
  now broken as it used to be and automatic targeting is not
  blocked anymore (though this can take some SlowUpdate's).
  For ground-attack orders it is still in place, since the
  behavior seems to be preferred (so a unit that is told to
  attack a position and then to move away will keep firing
  even when other targets are available)
- really fix "endgame stats only show APM for self"
- really fix "Size of unidentified radar blips can be determined with mouseover"
- fix ship wreckage drag
- fix gunships don't obey land/fly state
- fix 'Bombers with fight command do not fire' (3190)
- fix 'Emit-sfxed weapons point at {0, 0, 0}' (3205)
- fix an incorrect watchdog trigger before gamestart
- fix 'Aiming animations ignore maxAngleDif [actually tolerance]' (3180)
- fix 'Grass is not animated in LOS view' & 'LOS view does not render trees in the same way' (3207 & 3203)
- fix `halo` in BumpWater with EndlessOcean & OwnRefractionRenderPass
- fix 'Bombers with area attack dont work ' (3195)
- fix 'If air constructor lands it often fails to do next construction command' (3198)
- make fight commands work with noAutoTarget weapons (3190 #2)
- fix 'Constructor stops before it enters build range' (3210)
- fix bugged/all-white decals when shadows=0
- fix units being able to move down steep cliffs (directional slope-tolerance needs work)
- fix dangling pointer/memory corruption in QuadField (3211)
- fix 'Reclaiming units under construction results in no resources being returned' (3216)
- fix some OpenGL usage errors found by [teh]decay
- fix 'Gunships dont always acquire targets despite fireatwill+roam+fly' (3221)
- fix 'TargetBorder=1 only works when a unit has a specific attack command' (2971)
- fix CFeature::ForcedMove (3226)
- fix Spring crashes when sending large messages (3228)
- fix 'Armed constructors attack neutral units on Fight order'
- restore old shield visibility behaviour (3242)
- fix 'Units with cannons remain out of range when you force fire onto high ground' (3241)
- disable unit pushing bidirectionally when one of the parties is non-blocking

Sim:

- make target locking optional via meta-key

GameServer:

- support start-scripts of arbitrary size (still not advised to use scripts bigger than 256kB, and we won't optimize anything related to such huge scripts)

UnitDefs:

- make cloakCostMoving default to cloakCost

WeaponDefs:

- change default weapon "mainDir" to "0,0,1" (before it was "1,0,0")
- always enable continuous hit-testing (it showed discountinuous one is used <<1%, so it's not worth to keep it)

FeatureDefs:

- add explicit `floating` featureDef tag (before it was implicit via nodrawundergray and failing a lot)

Lua:

- add Spring.SetUnitDirection(unitid, x,y,z)
- add `ignoreLOS` tag to Spring.UpdateMapLight
- LuaRules: add DrawProjectile to complete the Draw\* quartet

ModInfo:

- add `allowAircraftToHitGround` tag (3199)

Rendering:

- runtime create missing DDS mipmaps when supported by the hardware

Windows:

- call SDL_WarpMouse every frame again (fixes `ghost mouse` in middle click scroll)
- explicit pipe --large-address-aware through -Wl (fixes compiling with tdm-gcc)

-- 89.0 ---------------------------------------------------------
Major Changes:

- LuaSocket
- MessageBox for unsupported GPUs (+drivers)
- fixed ATi PointSize bug
- LOS et al. are rendered now through the GLSL terrain shader (-> shadows in LOS-view)

Changes:

- added Shard to the windows installer
- added pr-downloader as submodule (only compiled if libcurldev is available)
- limit infolog.txt buffer to 8kB
- open a MessageBox on systems with unsupported GPUs (+drivers) on engine start (suppressible with a config-tag)
  ! remove selectkeys.txt support (use uikeys.txt instead)
  ! remove TASServer.jar (outdated, use https://springrts.com/wiki/Spring_on_a_LAN )
  ! send gameID and SDF filename with SERVER_STARTPLAYING to autohost
  ! remove deprecated LogSubsystems config var
- do not send GameOver from a client that timed out
- add LogFlush config var for instant writes (disabled as default)
- add UI config-variables AutoAddBuiltUnitsToFactoryGroup (def: false) and AutoAddBuiltUnitsToSelectedGroup (def: false)
  if the first variable is true, new units auto-inherit their group number from the factory that built them
  if the first AND second are true, new units are auto-selected if their factory group is currently selected
- weapondefs.cylinderTargetting spelling fixed, the old one is deprecated
- exit with !=0 if spring can't connect to server
- decrease loglevel for "Load S3O texture now" & luasocket
- change content-exception (for a missing water texture) in BasicWater to a warning message
- dedicated server no longer consumes 100% CPU while waiting for gameID
- headless builds use less cpu-time (nominally 50% instead of 100%)

Bugfixes:

- fix some orders are discarded when issuing commands to landed planes
- fix unfinished units with 100% hp can't be finished
- fix 'crash when getting LOS of a feature without 3d object and drawType!=-1' (3032)
- fix "changing unit speed does not work anymore" (2877/3016)
  (scripts can now arbitrarily override the UnitDef values, which are only used for initialization)
- fix 'LightningCannon can fire into water' (3057)
- fix BeamLasers and LightningCannons not ignoring stunned shields
- fix GameSetupDrawer player-state string (3064)
- fix 'Con/com gets weird move order after stop command is given' (3069)
  (was intended to keep factories clear, any side-effects are up to players to deal with)
- fix stunned=true crashing=true airplanes falling into the void (3078)
- fix zombie coverage from sensor-units (if activated while in a transport, exploit)
- fix crash in unit_script.lua
- fix 'Units stuck in factory' QTPFS bug (3055)
- fix units moving (being pushed) through factory walls
- fix terrain water reflections
- fix game does not end properly for a player that is being kicked
- fix problem to build near map edges
- fix ping display after mid-game (re-)join + try to reduce lag after mid-game join
- fix missing stack trace if out-of-memory occurs
- fix missing stack trace on abnormal termination (sigabrt)
- fix resurrecting an individual wreck results in the resurrector not healing it
- fix /godmode and /{luarules,luagaia} {reload,disable} not working in demos (2701)
- fix targeting bug (weapons sometimes failing to fire at badTargetCategory units) as a result of AllowWeaponTarget misuse
- fix kamikaze units not self-d'ing when given an attack order while on hold-fire (2405)
- fix 3105 (PlayerChanged callin not available in unsynced gadgets)
- fix 2947 (size of unidentified radar blips can be determined with mouseover)
- fix 1843 (Spring.GetUnitHealth returned nothing for buildProgress on in-LOS enemy units with "hideDamage=1")
- fix 3122 (failure to fire at badTargetCategory)
- fix crash when give all units to uncontrolled team
- fix fighters that have a target may refuse to obey orders
- fix crash in GetPlayerTeam (AI)
- fix sync warning when resizing the main window
- fix ctrl/alt/shift keys stuck after alt tab
- make COB's set(MAX_SPEED) permanent again like its Lua counterpart, rather than
  letting it apply only temporarily (for the duration of the current move command
  if any); use negative speed values if you want the change to be non-permanent

Simulation:

- YardMap changes:
- more verbose yardmap parsing
- ignore any whitespaces in yardmap strings (esp. newlines)
- new "i" yardmap code (inverse of "c")
  ! remove TA-ism in buildrange code:
  ! make buildrange not immobile dependent
  ! changed terraform range check a little
  ! make terrain restore range check consistent with new terraform one
  ! don't add target's radius to the builder's builddistance, only do so if the target is bigger than the builddistance (mostly only true for terraform)
  -> from now on the to be built unit's centerpos has to be in buildrange not its border!
  ! remove obsolete default dependence of cloakCostMoving on cloakCost
  ! remove modelCenterOffset tag (can now be dynamically set with Lua)
- improved behaviour of builder masses
- make StartBox clamping of StartPositions synced
- don't reclaim in-the-way features when the buildee is going to morph into a feature of the same type
  (fixes the ancient bug where ordering a row of features with N > 1 builders causes each to be reclaimed and then immediately rebuilt)
- do not apply impulse to units being transported
- make falling wreckage (from aircraft) not lose all its forward speed
- make ground units consume waypoints less aggressively
- remove special treatment for collisions between mutually-pushResistant objects for 3031
- add 'allowUnitCollisionOverlap' (def=true) and 'useClassicGroundMoveType' (def=false) ModInfo keys
  the former allows unit movement like www.youtube.com/watch?v=mRtePUdVk2o at the cost of more clumping (learn to use ctrl+lmb formation orders)
  the latter is a ticket back to unit movement circa 2009: no reversing, no turninplace=false, no smooth collision handling, no nothing (figure it out)
- add mod-rule 'targetableTransportedUnits' (default false)
- introduce an independent Lua-controllable aim-position (defaults to mid-position) for units and features, used by weapons as their aiming/target point
- make ground units less keen on repairing fast units such as patrolling aircraft
- fix distinction between user-selected and internal CommandAI targets

Pathing:

- QTPFS: less loadscreen spam, make F2 functional, enable corner-connected node topology, use staggered layer-updates, speedups and tweaks
- QTPFS: increase MAX_TEAM_SEARCHES, decrease LAYERS_PER_UPDATE (more units can request paths per frame, units get paths back more quickly)
- QTPFS: fix assertion failure, make execution of path requests more asynchronous

Lua:
! some config values are now write protected (Spring.SetConfig\*)

- added luasocket support for widgets, see https://springrts.com/wiki/LuaSocket
- fixed params of Spring.MarkerAddPoint & Spring.MarkerAddLine
- call AllowWeaponTarget at the end of the queue, so lua only gets those units which are _not_ filtered by the engine
  ! LuaSyncedRead: better hide enemy radarblobs unit-info
  ! MoveCtrl: remove SetGroundMoveTypeData(unitID, "wantedSpeed", v) and SetGroundMoveTypeData(unitID, "maxWantedSpeed", v)
  (the preferred way for gadgets to change a unit's speed is by calling SetGroundMoveTypeData(unitID, "maxSpeed", v))
  ! replace Game.gameID by synced callin GameID(string gameID)
- fix TraceScreenRay never returning "sky" when includeSky=true
- fix TraceScreenRay coordinate offset when onlyCoords=true
- fix typo in Spring.GetUnitVelocity
- add extra parameter 'defPriority' to AllowWeaponTarget (what the engine thinks the target priority should be)
- add bool FeatureDefs[xyz].autoreclaim
- add new gl.Texture("$minimap")
- add new gl.Texture("$info")
  ! remove gl.SelectBuffer and co.
- new Spring.LoadCmdColorsConfig(configStr) & Spring.LoadCtrlPanelConfig(configStr)
- add new 3 param 'pairwise' to Spring.GiveOrderArrayToUnitArray(), used to send multiple units a single individual command
- fix crash due to UpdateCallIn removing the current event client from list being iterated
- add missing team/player callins to (unsyced) GadgetHandler
- add new synced callouts Set{Unit,Feature}RadiusAndHeight(number id, number radius, number height) --> boolean
- add new synced callouts Set{Unit,Feature}MidAndAimPos(number id, number mpx, number mpy, number mpz, number apx, number apy, number apz) --> boolean
- add optional 2nd (boolean midPos) and 3rd (boolean aimPos) arguments to Spring.Get{Unit,Feature}Position to go with the above four callouts
  ! change Spring.GetUnitPosition to return a unit's base-position by default, like .GetFeaturePosition does for features
  (this is usually a point on the model closer to the ground than its mid-position, so may affect some Lua drawing code)
- fix keys hang after LuaUI tweakmode
- [MT] disallow other Lua environments to invoke LuaUI if LuaThreadingModel = 2, to prevent deadlock
- [MT] fix desync with LuaThreadingModel > 2
- [MT] fix teams sometimes wrong in UnitGiven() and UnitTaken()

Rendering:

- fix DynWater ship wakes, BumpWater shorewaves
- fixed ATi PointSize bug
- update assimp to svn rev 1231
- fixed assimp lua metafiles
- warn about null normals in models
- add visualization of default object radii (used for AOE calculations, etc) to alt+v
- SMF: render the info-texture through GLSL (noob translation -> shadows in LOS-view)
- improved VSync under Linux (with support for AdaptiveVSync)
- added FeatureFadeDistance & FeatureDrawDistance config tags
- more failsafe shadow FBO creation
- fixed vibrating planes (again)
- fix spring_multithreaded hangs

UI:

- display average draw & sim frame time in debugview
- draw buildRange for mobile selected units too & always render them on shift hover (also for shield range)

Sound:
! all items in sounds.lua are now converted to lowercase

- allow to create a new "default" item in sounds.lua
- WeaponDef: support 'soundHitWet{Volume}' keys for in-water explosion sounds
  (the 'soundHit' key is now deprecated in favor of 'soundHitDry')

Windows:

- set IMAGE_FILE_LARGE_ADDRESS_AWARE (increased >2GB memory limit)

Linux:
! strip CXX_FLAGS & C_FLAGS in build process to prevent sync incompabilities (can be turned off with a new CUSTOM_CFLAGS cmake flag)

- on multicore systems set scheduler to SCHED_BATCH (semi-solves the core hoping issue)
- call SDL_WarpMouse only when needed (fixes massive FPS-drop in middle-click-scrolling)
- use a non-locking vsync mechanism & add support for nvidia's `adaptive vsync`
- explicit disable all CPU extensions except SSE1
- use mtune=generic on 64bit systems

OSX:

- fix stack corruption in sound code

-- 88.0 ---------------------------------------------------------
Changes:
! showing collision volumes has now its own command (`/debugcolvol` & `alt+v`)
! use footprint (not model) radii for obstacle avoidance

- improve reconnecting sim/draw balance
- disable line/point smoothing in safemode
- time profiler updates now even when paused
  ! MoveInfo read "avoidMobilesOnPath" from correct table and disable it by default (before it was default enabled)

Unitsync:
! allow to list files with unsolveable depends

- warn about usage of deprecated functions in unitsync

Lua:

- added new Spring.TestMoveOrder(unitDefID, worldx, worldy, worldz) -> boolean
- added new Spring.Log()
- same as Spring.Echo, but passes metadata to the logsystem
  ! to be able to use LOG.XYZ you need to update your system.lua
- Spring.Log(string logsection, int loglevel, ...) (with loglevel either: LOG.DEBUG, LOG.INFO, LOG.WARNING, LOG.ERROR, LOG.FATAL)
- Spring.Log(string logsection, string loglevel, ...) (with either: "debug", "info", "warning", "error", "fatal" [lower-/uppercase doesn't matter])
- fixed WeaponDefs[123].pairs()
- added myGravity tag to WeaponDefs[] table (thx DeadnightWarrior)
  ! distinguish water-damage in Unit{Pre}Damaged
- water damage has index -5
  ! kill damage was -5 before and is now -6

Bugfixes:

- fixed multiple crashes
- fixed SPRING_ISOLATED=1
- fixed gcc i686 march flag
- fixed an aircraft refueling bug
- fixed multiple pathing bugs/issues
- fixed crash at startup when DynamicSun=1
- fixed 'Nanoframe targeting and water' (3017)
- fixed desync _warning_ between 32- & 64-bit systems
- fixed FarTextures with GPU/drivers that don't support FBOs
- fixed spring-mt & spring-safemode links in window's startmenu
- fixed incorrect drawMidPos (e.g. used for command line drawing)

Installer:
! add vcredist_x86.exe to the installer / remove MSVCR71.dll from mingwlibs

- fixed safemode shortcuts

-- 87.0 ---------------------------------------------------------
Bugfixes:
! restore pre86.0 maxAngleDif arcs
! remove the following Lua consts: wdef[id].areaOfEffect, wdef[id].maxVelocity & wdef[id].onlyTargetCategories

- fix "LuaUI didn't got unprocessed /xyz commands" (mantis 2982)
- fix shield rendering
- support for zlib 1.2.6 (thx sirmaverick)
- fix crash on start when invalid ai is in script.txt
- fix multiple pathing & collision issues
- some Maven (Java) updates
  ! removed ArchiveMover
- fix compile for "make tests" on win32

-- 86.0 ---------------------------------------------------------
Changes:

- new commandline argument "--safemode": It turns off all features that are known to cause problems on some system.
  ! ignore features with `blocking=false` in all RayTracing/Aiming functions (other code still checks them!!!)
- enable ROAM in spring-mt
- IsolationMode: scan ENV{SPRING_DATADIR} & config's SpringData as readonly datadirs
- using now OS functions to expand special paths in spring's include dirs (config's SpringData, ENV{SPRING_DATADIR}, ...)
  ! Windows: now uses windows syntax ("$HOME/foo/bar" is now "%HOME%/foo/bar") (see msdn's ExpandEnvironmentStrings documentation)
- Linux: now supports full bash path syntax ("~/", "${HOME}/.spring/", "/foo/../bar/", etc.) (see manpage of wordexp)
- remove "Start SpringSettings" and "Start the Lobby" from the test/develop menu
  ! remove support for selectionkeys.txt
- add internal_pthread_backtrace for freebsd
- update mingwlibs (fixes rotated textures)

Simulation:

- make globalLOS a per-allyteam variable
  /globallos <n> --> toggle for allyteam <n>, no argument --> toggle for all

Pathing:

- QTPFS:
  fix several minor issues and corner cases
  support partial searches, allow search to start from blocked nodes
  tweak heuristic so it overestimates less on non-flat terrain
  do not let units move before path-request is processed
- UnitDef: add turnInPlaceAngleLimit tag
  for a unit with turnInPlace=true, defines the
  maximum angle of a turn above which it starts
  to brake (defaults to 0.0, values in degrees)
- MoveDef:
  unhardcode default pathfinder cost-adjustment and
  speed-multipliers for squares blocked by _mobile_
  units:
  avoidMobilesOnPath = boolean,
  speedModMults = {
  mobileBusyMult = number,
  mobileIdleMult = number,
  mobileMoveMult = number,
  }
  unhardcode the DepthMod equation:
  depthModParams = {
  minHeight = number,
  maxHeight = number,
  maxScale = number,
  quadraticCoeff = number,
  linearCoeff = number, -- deprecates "depthMod"
  constantCoeff = number,
  }
  new formula is given by
  if h < depthModParams.minHeight: 1.0
  if h > depthModParams.maxHeight: 0.0
  else:
  depthScale = MAX(0.01, MIN(depthModParams.maxScale, (a _ h _ h) + (b \* h) + c))
  depthMod = 1 / depthScale
  where
  h = unit's absolute height below water surface
  a = depthModParams.quadraticCoeff
  b = depthModParams.linearCoeff
  c = depthModParams.constantCoeff

unitsync:

- reload IsolatedMode & Dir via the EnvVar on Init() calls
- warn about invalid version numbers
- add python bindings for unitsync

MacOSX:

- fix signal handling
- use native DialogBoxes

Rendering:

- cleanup and extend SSMF shader, now includes a parallax-mapping stage

Lua:

- add LuaRules callin `DrawShield(number unitID, number weaponID) --> boolean`
  (true skips Spring's own drawing of shield <weaponID> owned by unit <unitID>)
- make LuaUnsyncedCtrl::SendSkirmishAIMessage return a table, not N loose strings
  (but note there is nothing in this table due to "limitations" of the AI interface)
- add weaponDefID parameter to ProjectileCreated
- add facing parameter to AllowUnitCreation
- remove most range-checks in default gadget-handler for registering commands (2930+2952)
- distinguish damage-types for Unit{Pre}Damaged when weaponDefID < 0 (2966)
  weaponDefID -1 --> debris collision
  weaponDefID -2 --> ground collision
  weaponDefID -3 --> object collision
  weaponDefID -4 --> fire damage
  weaponDefID -5 --> kill damage

Bugfixes:

- fixed Intel GPU detection under Windows
- fixed handling of duplicated used hotkeys (i.e. unit groups & specteam switching)
- fixed minor issues with ground collision RayTracing
- fixed compilation with boost >=1.48
- fixed Spring.Restart
- open logfiles on win32 with commit flag set
  ! don't allow spring to start when a depend file is missing

- "Rapid unit ejection from factory" (2864)
- "Units walk in circles on their way to a far awar move goal" (2866)
- "Excessive movement bouncing off buildings and wreckages" (2865)
- "armpw stuck in air while pathing around solars" (2854)
- "Enemied units can get stuck in each other with movectrl" (2832)
- "A factory set to wait continues the queue" (2868)
- "Change in Factory Behaviour wrt AllowUnitCreation" (2873)
- "Building with Meta-Key pressed does not work" (2764)
- "'Move at slowest speed of group' move order (ctrl+alt) semi-permanently changes unit maxvelocity." (2880)
- BeamLasers could hit but not damage units using per-piece colvols
- heap corruption in UnitDrawer::ShowUnitBuildSquare (2903)
- heap corruption in TransportCAI::UnloadDrop (2911)
- mismatched feature colvol matrices
- MoveCtrl.SetRelativeVelocity
- bug in TestTrajectoryCone for ballistic weapons
- transported units being unhittable (2875, 2934)
- SetUnitCrashing inconsistency, caused "Aircraft crashing while landed results in irregular behavior" (2924)
- obscure targeting bug, caused "Gunship jams when on hold fire" (2406)
- deprecate / remove the 'toAirWeapon' WeaponDef tag
  (use "onlyTargetCategory = VTOL" for the same effect)
- 'airmesh is 0 at x >= width or z >= height' (2938)
- GuiHandler crash when unit has no valid selfDExplosion (2852)
- <canSubmerge=false, floater=true> aircraft landing on sea floor (2856)
- 'waterweapon=false BeamLaser can fire into water' (2857)
- the units-humping-obstacles syndrome (forward and reverse)
- silly aircraft refueling bugs
- 'Warning message on SetGroundMoveTypeData(unitID, "maxSpeed", 0)' (and some related possible div0's)
- weird inability of weapons with a non-default onlyTargetCategory to attack the ground
- 'Unit LOD billboards are too dark' (2948)
- rare antinuke bug (2956)
- typo in setting of default turnInPlaceSpeedLimit
- "All terrain units move commands are removed when climbing cliffs" (2958)
- 'Landed gunships do not follow height reduction' (2823)

-- 85.0 ---------------------------------------------------------
Major Changes:

- ROAM \o/
- QTPFS (disabled by default)

CommandLine:

- updated man-pages
- added new argument `--sync-version` (i.e. prints "85" for a 85.0 build or "84.0.1-567-g69ef7a3 develop" for a dev build)
- allow to build proper release builds without being on branch master
- added a full set of new arguments to spring-dedicated (see `spring-dedicated --help`)
- added --isolation & --isolation-dir arguments
- optionally, you may supply a path with SPRING_ISOLATED=<isolation-mode-dir>

Rendering:

- added ROAM. Kudos go to B. Turner (the original author of the code) and Beherith (for integrating it into Spring).
- set SmoothLines & SmoothPoints by default on DONT_CARE (so your gfx driver settings decide if it is enabled or not)
- dynamic viewrange handling, so even a 32x32 map can be rendered w/o clipping in low camera angles
- log available video memory on NVidia/ATi gfx drivers
- disable GLSL shaders on Mesa/Intel drivers

Internal/Bugfixes:

- re-added forced log file flushing when handling crashes
- made the (new) C++ AI Wrapper VS compatible
- name threads so you can identify them in `htop` (Linux only)
- some additional time-profilers in rendering code
  ! hide "(Headless)" in dedicated's returned version name
- fixed stacktrace translation with some Linux setups
- fix crash when specs are cheating in via /team
- fix transport loading failure

(G)UI:

- fix keyshortcuts (handle F3 OR any+F3 but not both at once!)
- fix EndGameBox
- make F2 colors less ambiguous
- the ManualFire action now expects a cursor "cursormanfire"

Sim:

- make units crushable like features
- implement avoidFeature for Cannons and MissileLaunchers
- fix extremely fast unit rotations after being hit
- fix FPS-mode range exploit
- fix teleport-to-smooth-mesh after unloading

Pathing/MoveType:

- fix units sometimes moving onto impassable squares
- fix the mass-clumping unit movement bug
- properly restore MoveType::maxSpeed after a guard-order
- prevent units getting stuck when turning around the corner of other units

CEG:

- give warning when textures used in SimpleParticles don't exists
- really fix default projectile colors

MultiThreaded (GML):

- fix hang/runtime error during pathing
- fix Creating unit in gadget:Initialize()
- lots of other fixes

Sound:

- limit UnitReplies to max one concurrent playback
- play most unit sounds (activate, ...) only if unit is in LOS

UnitDef:
! removed "commander" tag (The Commander selectkeys filter is now defunct (ctrl+c hotkey). Lua it, or add a new one that looks for any canManualFire units.)

- added new "blocking" bool tag
  ! "canDGun" is now a synonym for "canManualFire"
- added new tag "crushResistance" (defaults to unit's mass)
  ! make the default cloakCost\* values 0 instead of -1,
  ! added new "canCloak" bool tag (If an unit can receive user cloak commands. Before, canCloak was auto set to true if cloakCost >= 0.)

FeatureDef

- added new tag "crushResistance" (defaults to feature's mass)

WeaponDef:

- added new "craterAreaOfEffect" float tag (defaults to the value of areaOfEffect)

modrules.lua:

- added "movement.allowCrushingAlliedUnits" (default false)
- added "movement.allowUnitCollisionDamage" (default false)
- added "system.pathFinderSystem" (defaults to 0, 1 is QTPFS)

Lua:
! UnitDef[].isCommander returns now false

- fixed Spring.GetTimer/DiffTimers
- fixed Spring.SetMouseCursor when using hw cursor
- handle Lua code crashes in some events correctly, so they don't crash the whole Lua environment (fixes: "LuaRules::CheckStack top = -1")
  ! call UnitMoveFailed event only for via Script.SetWatchUnit registered UnitDefs
  ! added new WeaponDefs[].craterAreaOfEffect
- extend Spring.SetUnitBlocking with an optional 4th "crashable" bool argument
- added new Spring.GetUnitBlocking(unitid) -> blocking, crushable

demo widgets:

- minimap_startbox.lua: play sound when start-positions are placed

Buildbot:

- create spring\_${VERSION}\_minimal-portable+dedicated.zip

UnitSync:

- if Init() is called, but we are already initialized, do a re-initialization

-- 84.0 ---------------------------------------------------------
Bugfixes & Improvements:

- reduced 'Got invalid Select' message spam
- hide framNums from chat console
- fixed command queue bug (zombie builders)
- always flush stdout/stderr
- workaround a SDL bug with X11 & fullscreen that broke MiddleClickScrolling
- changed target behavior of enemies outside of LOS
- fixed BumpWater with DepthBufferBits=16
- fixed aircraft landing on repairpads
- reset aircraft state after starting from repairpad (`ground hover bug`)
- make UHM available before the gamestart (fixes Spring.GetGroundHeight in :Initialize() etc.)
- workaround 'Bertha' issue (UHM related)
- make C++ AI Wrapper compile with VS
- fixed fog in some shaders
- fixed F2 view
- improve weapon interceptors (anti nukes can now intercept nukes that fly over them)
- fixed free resources by NanoFrameDecay
- handle resigned players (:= /spectator) in game_end.lua gadget
- Lua Spring.KillTeam: disallow killing of Gaia (engine doesn't support so atm)
- fixed lasers not hitting targets at edge of range
- fixed stutter with low-reloadtime weapons
- Lua: removed WeaponDefs[i].graphicsType & renamed WeaponDefs[i].hardStop -> WeaponDefs[i].laserHardStop

-- 83.0 ---------------------------------------------------------
Major Changes:

- EFX/EAX support (sound reverb)
- ASSIMP support (not 100% finished yet)
- AI Interface pure-ification
- many speedups and double as much slowdowns
- use a new engine versioning scheme (RC12)

Engine:

- use more smooth algorithm for wind updates
- fixed a few focus lost issues (keyboard & mouse)
- DebugMode now draws feature collision volumes too
- made ShadowSpace linear
- multithreaded loadingscreen
- hide out-of-LOS heightmap changes
- replace engine start point marker with lua-widget
- use "game" instead of "mod" in many places
- add "isolation" mode (do not use global data-dirs)
- change lobbyserver main address to "lobby.springrts.com"
- use https://springrts.com/ everywhere (deprecate clan-sy links)
- ArchiveMover uses unitsync to detect the writable data dir
- improve version logging on non-Windows OSs
- include player-name in log message when resingning
- add `../` as data-dir if it contains `maps/`, `games/` and `engines/`
- allow to define the default config- nad data-dir on windows, using env var PROGRAMDATA
- refuse to load too heavily compressed archives (see 0.82.3-1043-g2c15040 commit message for details)
- better IP v6 and v4/v6 mixed mode support
- lots of error messages improved and new ones added
- add the first few unit-tests
- add a validation-test
- add network bandwidth limits per player to prevent malicious command spam
- add optional high-loss network modes to make spring playable on very lossy connections (NetworkLossFactor = 0|1|2)

Engine / bugfixes:

- too many to list, see https://springrts.com/mantis/changelog_page.php (incomplete!)
- calculate the team unit-limit without ancient hacks
- stop the maxunits team restriction from applying to Gaia (#2435)
- fix ray-ground intersections when camera is outside map
- fix spring on MacOSX
- CMake configure and make build are much more stable now (no more failure due to wrong order of actions)
- fix DamageArray related sync error
- fix a lot of memory-leaks
- fix unit does sometimes not reach target position when given a restore command
- prevent build time-out when builder or other units are blocking the build pos
- fix "cheat" status disparity between server and client after watching demo
- fix captured unit still getting shot at
- fix repeat commands remain even if the target object is destroyed
- fix unwanted error spam when disconnecting from server
- fix reconnect failure (incorrect password)
- fix rare desync and keyframe mismatch after reconnect
- fix two crashes related to audio channels
- eliminate some false hang detections on load
- fix multithreaded crash in lua material rendering
- fix hang after crash
- fix memory corruption in bitmap handling
- fix desync in PieceProjectile (#2591)
- fix a certain /take exploit
- fix hover aircraft may become unresponsive in conjunction with command chains and repeat enabled

Engine / misc defs:

- replace UnitDef::pieceTrailCEG\* by a Lua subtable of sfxTypes
- deprecate several *Def tags and their Lua*Defs copies (type/maxSlope/isBuilder/canDGun/transportableBuilding/dropped/canCrash/...)
- make a missing UnitDef::humanName non-fatal
- UnitDef: remove hardcoded: if ((waterline >= 5.0f) && canmove) { waterline += 10.0f; }
- make UnitDef::losHeight configurable instead of an unused hardcoded value
- add collide{Enemy, Ground} as weapondef tags
- use a more reasonable default for FeatureDef.autoreclaimable (== reclaimable)
- scan in 'effects/' as well as 'gamedata/explosions' for CEG definitions

Engine / misc commands:

- add a /clearmapmarks command
- rename /advshading to /advmodelshading; add an /advmapshading analogue
- add '/reloadCEGs [cegtag]'; reloads all CEGs or one specific CEG by tag
- add '/dumpstate <minFrame> <maxFrame> [frameInt]'
- add '/CommandList'
- add '/CommandHelp <commandName>'
- more complete word completion for game-commands
- make auto-host command `paused` use a parameter (0|1)
- add scrollbars for player list in "quit" and "share" dialogs

Engine / Userconfigs:

- removed the "FSAA" config tag (FSAALevel is enough)
- removed default 1024x768 res and use desktop one now instead (done when X/YResolution <= 0)
- added a new 3rd state for AtiHacks: -1 (autodetect each run, so you can switch GPUs)

Engine / Simulation:

- never allow zero-area feature footprints
- remove radius halving for default aircraft collision-volumes
- close down another out-of-map attack exploit route
- rewrite collision detection/resolution between ground units
- skip path-estimating for unused MoveDefs
- use the full resolution of the blocking object map in pathestimator
- invoke higher resolution pathfinder if needed
- always add the "geovent" FeatureDef (#2402)
- only send UnitLoaded events for units that were not already being transported (when Attach'ing them)
- fix bizarre skidding behavior for ground units with non-zero slideTolerance
- higher priority for nano particles when capturing and reclaiming other teams units
- print error message for select commands referencing invalid object IDs
- make TorpedoLauncher weapons able to target any in-water unit
- make pushing of enemy units during collisions a ModInfo option ("allowPushingEnemyUnits")
- remove hardcoded enter-crashing-state-on-death behavior for aircraft (now delegated to Lua or COB)

Engine / Map:

- merged both mapinfo.lua's! before there were two, one in the base-dir (-> ArchiveScanner) and one in "maphelper/mapinfo.lua" (-> MapParser/MapInfo)
  engine now always tries to read the base-dir one
- new mapinfo.lua tags:
- "specularExponent" (moved from userconfig)
- "fogEnd"
- "bladeColor"
- check for the smt _next to_ the smf (which can already freely placed in the archive!)
- allow resources_map.lua to specify GroundFX textures for CEGs (that use CSimpleGroundFlash)

Engine / Rendering:

- Lua-scriptable dynamic lighting of models and terrain (www.youtube.com/watch?v=H5je8TKDfNA)
- WIP DynamicSun option for moving shadows
- add optional SSMF normal-mapping and self-illumination stages
- fix holes in shadows cast by non-closed S3O/OBJ models icw. face culling
- always cull (non-3DO) model backfaces for the shadow pass
- squeeze some extra precision out of the shadow-map
- make map/model/tree/projectile shadows individually configurable
- allow weapons to not leave any explosion scars (add "explosionScar=true|false" to a WeaponDef)
- render all SMF maps through GLSL if it is available
- make depth-testing and -masking configurable for CSimpleGroundFlash
- fix mixed up textures in the multithreaded version
- fix awkward performance with multithreaded engine in games using custom model formats
- add peak display for time profiler
- don't display /mtinfo if the value is near zero
- properly fade out features
- make underwater shadows darker in deeper water
- significantly reduced performance impact for Lua rendering widgets in the multithreaded version
- enable creation of OpenGL objects such as display lists in the sim thread in the multithreaded version
- fix BumpWater on ATI cards when atiHacks=false

Engine / Sound:

- added EFX/EAX support (reverb)
- /tset UseEFX [0|1]
- /tset snd_filter %gainlf %gainhf
- /tset snd_eaxpreset "bathroom" (see EFXPresets.cpp for alternatives)
- fixed a bug when runtime changing the volume
- added a warning when trying to play a non-mono sound in 3d (OpenAL doesn't support this!)

Engine / COB&LUS:

- fixed bug in WaitFor... [mantis #2499]
- minor animation-handler optimizations

Engine / Lua:

- fixed "/luagaia xyz" cmds
- added EAX/EFX funcs: Spring.[Get|Set]SoundEffectParams
- totally remove following args of SetUnitPieceCollisionVolumeData: affectLocal, affectGlobal, enableGlobal (it's now always local!)
- fixed gl.PushAttrib(GL.ALL_ATTRIB_BITS)
- added a new channel argument to Spring.PlaySoundFile(). it is always the last argument (independent if you defined pos/speed)
  it can either be:
  "battle", "sfx", 1
  "unitreply", "voice", 2
  "userinterface", "ui", 3
  anything else defaults to "general"
- removed teamcolors.lua (redundant since Spring.SetTeamColor)
- add optional flattenGround parameter to Spring.CreateUnit
- remove the "n" field from array-like tables returned by API functions
- added Spring.GetProjectilesInRectangle
- linked Projectile{Created, Destroyed} to the Spring.SetWatchWeapon state
- added Spring.SetWatchUnit(unitDefID) and Spring.SetWatchFeature(featureDefID) analogues of SetWatchWeapon
- extend LuaPathFinder with support for setting node cost overlays
- extend SyncedRead::Pos2BuildPos: add support for additional param "facing"
- added Spring.{Get, Set}MapSquareTexture (these allow complete dynamic run-time retexturing of SMF maps)
- added Spring.GetMetalMapSize (returns size in metal map coordinates)
- added Spring.GetMetalExtraction (note: uses metal map coordinates)
- added Spring.{Get, Set}MetalAmount (note: uses metal map coordinates)
- added Spring.GetFeaturesInCylinder
- added Spring.GetFeaturesInSphere
- added Spring.GetFeatureCollisionVolumeData
- added Spring.SetWMIcon
- added Spring.SetWMCaption
- added Spring.ClearWatchDogTimer
- added widget call-in GamePaused
- added widget call-in PlayerAdded
- added widget call-in PlayerRemoved
- added callins for collision events
  UnitUnitCollision(colliderID, collideeID) -- needs SetWatchUnit enabled for both parties
  UnitFeatureCollision(colliderID, collideeID, crushKilled) -- needs SetWatchFeature enabled for both parties
- added Spring.SetUnitCrashing
- add All/None buttons for widget selector
- fix multithreaded crashes in Spring.Get{Full,Real}BuildQueue
- fix Immobilebuilder fails near the map edge
- fix Spring.SetUnitWeaponState "range" for cannon weapons
- add control over airStrafe via MoveCtrl.SetGunshipMoveTypeData
- migrate MoveCtrl.SetLeaveTracks to (LuaUnsynced) Spring.SetUnitLeaveTracks
- removed all widgets (moved to examples/Widgets)

Engine / Internal:

- added limited OpenMP support
- added multithreaded OpenGL support via offscreen contexts
- added LinkingTimeOptimization (currently fails with GCC4.5/6)
- more/better error reporting
- new logging system:
  frontend is lightweight
  frontend is C compatible
  is more modular
  supports log levels: DEBUG, INFO, WARNING, ERROR, FATAL
  "sub-systems" renamed to "sections"
  more section then before
  is compatible with unit-testing
  write DEBUG and INFO to stdout, the rest to stderr
  disable console logging for unitsync
  disable console output for dedicated server shared library
  turn off flushing to log-file completely
- added GL_ARB_debug_output support (callback-based opengl error reporting)
  NOTE: this doesn't work yet, because most drivers export this ext only for so called debug contexts.
  Creating those is impossible with near all current GL frameworks (SDL, SFML, ...).
- fixed unnecessary memory allocation in Archive7Zip
- promote the CrashHandler hang-detector thread to a WatchDog
- use a SafeVector for the Command parameter array
- lots of cleanup and refactoring
- get rid of goto
- modularized chat-/game-commands (for example /cheat)
- no more use of AUX_SOURCE_DIRECTORY in CMake (as recommended by CMake team)
- Remove the obsolete SCons build system
- reduces constantly allocated memory for drawing stuff for AIs from ~130MB to ~ 2KB
- add a new build-target "generateSources"
- allow to use a system-supplied minizip library
- allow to disable each build type individually at configure stage
- generate version from git-describe (git tags), instead of hardcoding them into a source-file
- unsynced deletion of synced projectiles
- watchdog monitoring for some additional threads

AI:

- AI Interface pure-ification, see Forum: Development - AI - <sticky-thread>
- add full two-way communications between AI's and unsyced Lua
- let AI's send textual commands (like "/cheat")
- allow AI Interface plugins to provide info about available Skirmish AIs
- install Java-AIInterface and JavaOO-AIWrapper as maven artifacts into the local repository
- Legacy C++ AI: allow fetching the maximum number of units in a game
- fix source generation occasionally failing for Java-AIInterface
- treat allied units as always in LOS
- fetch the correct UnitDef for decoy units, if the unit is allied
- Java AI Interface: pack native sources too
- Maven-ize NullOOJavaAI
- Java AI: allow AIs to support compiling only through Maven
- generally improved Maven support
- make source generation compatible with BWK and GAWK 4.0.0
- E2323AI: bump to latest version
- KAIK: mid-game init fix
- AAI: prevent stack corruption in the case of file paths longer then 500 chars
- RAI: do not use a single/static team-instance callback in a place used by all RAI instances

Unitsync:

- implemented GetDataDirectoryCount() and GetDataDirectory(int index)
- add support for retrieving non-string info value types (string, integer, float, bool)
- introduce GetPrimaryModInfoCount
- fix OpenArchiveFile

Installer:

- create minimal-portable .7z version of engine and reuse it in installer
- ai's are now installed always (removed individual sections)
- add registry key to unitsync.dll HKLM\Software\Spring\SpringEngineHelper
- replace readme.html with https://springrts.com/wiki/Read_Me_First

-- 0.82.7.1 ---------------------------------------------------------
Engine / General:

- disable frame number prefix in messages to the console by default
- add temporary hack to not crash on HostIP=localhost

Engine / Lua:

- change parameter `useOverlay` from type number to bool in config access functions

Engine / Auto-Hosts:

- gracefully exit if the autohost is not reachable
- if the local socket supports IP v6, but AutohostIP is specified as IP v4, use v4 to connect
- stop server on config problem

Engine / Dedicated-Server:

- fix crash when game ends prematurely

Unitsync:

- add new function GetSpringVersionPatchset() (return "1" for a spring version of "0.82.7.1"
- remove accidentally added third parameter to Init()

Installer:

- fix: do not check for running .exe in silent-mode
- Zero-K: desktop-shortcut: rename & add icon

AI:

- KAIK: add missing initialization checks

-- 0.82.7 --------------------------------------
Engine / General:

- fix crash due to invalid lastAttacker
- do not bind a UDP socket for local games -> allows multiple local instances again sans hassle)
- clamp minimum mapHardness to 0.001 to prevent div/0
- fix dangling CUnit\* (crash for everyone in-game)
- fix crash in 3DO Parser
- fix bug in CGround::TrajectoryGroundCol with outside map coordinations
- prevent memory-leak in LoadGrayscale() if there already was pixel-data
- fix some more memory-leaks
- prevent equal unsynced random seeds
- fix SetUnitRotation
- fix enemy units responding to BuggerOff directives
- apply damage wrt. impacted piece if usePieceCollisionVolumes == 1
- make CBitmap::ReverseYAxis() more efficient
- remove excess elements from BasicMapDamage::craterTable
- rename "mod" to "game" in default menu & add "games/" to mod scanDirs
- implement host-definable IP's
- do not save/restore minimized windows state on win32
- convert some crashes because of invalid CEGs into warnings
- properly ignore non-blocking objects during collisions
- change minimum for UnitDef.buildDistance from 128 to 38 (default stays 128)
- broadcast team died events to all players, so it is stored in the demo file

Engine / Logging:

- give S3O model name in error message when a texture is missing
- print name of widget to infolog.txt before it starts to load
- prevent empty content_error messages
- catch model content_error exceptions ASAP
- unify stdout/infolog output

Engine / Lua:

- fix global environment pollution in luaui.lua
- allow Spring.AssignMouseCursor & Spring.ReplaceMouseCursor from LuaRules, too
- Allow loading from subfolders for weapon-def files (tdf and lua)
- implement Lua's GameFrame event via the EventHandler
- added parameter includeSky to TraceScreenRay
- correct `AllowResourceTransfer` params in default gadget handler
- new parameter for LuaSyncedCtrl::SetUnitArmored allows setting of the armor multiple

Engine / Path-Finder:

- fix units-stuck-behind-terrain
- fix Godde-III
- do not assign extra waypoints to aircraft
- decrease ETA-failure response time
- keep speed above UnitDef::turnInPlaceSpeedLimit when turning (if turnInPlace==0)
- fix units slowing down on negative slopes
- set a more reasonable default turnInPlaceSpeedLimit
- PFS: remove circular constraint from run-time searches
- set a more reasonable turnInPlaceSpeedLimit

Engine / Rendering:

- fix cannon visuals
- fix rendering of tri-stripped S3O's
- require GLEW 1.5.1+ because 1.5.0 does not have `glMapBufferRange`
- do not crash on shader compilation error
- fix build-order drawing for OBJ models
- replace 63x63 icons with 64x64 ones in LuaUI/Icons/ (prevents glitches on some older GFX cards)
- fix occasionally strange wake SFX

Engine / Auto-Hosts:

- builds other then dedicated-server now also support commands "kill" and "pause"
- make command `paused` support a parameter (0|1)

Engine / Dedicated-Server:

- fix binary name

Engine / Head-Less:

- crash fix
- remove of "jump depending on uninitialized value"

AI:

- LegacyCpp-AIWrapper: fix some memory leaks
- KAIK: fix memory corruption (one-character-typo...)
- KAIK: clamp costs to 1.0, not to 0.0
- KAIK: compile fix for 64bit
- KAIK: fix infinite loop
- KAIK: do not access invalidated pointer
- KAIK: init value before use
- KAIK: prevent accessing of array at position -1
- KAIK: prevent a crash when enemy unit is out of map bounds
- KAIK: fix mid-game init
- AAI: add version independent config file (by azaremoth) for _The Cursed_ game
- RAI: made compatible with MSVC
- RAI: fix cache file names
- RAI: fix AI considering water as harmful
- RAI: fix building non suitable buildings on certain maps
- RAI: fix a crash at end of game
- E323AI: pump to version 3.25.0

Stack-Trace-Translator:

- fixed for main binary (.exe, windows)

Installer:

- do not check for .Net when installing Zero-K
- delete all files that were installed
- add Tobis rapid client

-- 0.82.6.1 --------------------------------------
Engine / General:

- prevent false hang detection trigger

Engine / Rendering:

- fix occasional ATI crash

Installer:

- rename SpringDownloader to Zero-K Lobby
- re-add MSVCR71.dll

-- 0.82.6 --------------------------------------
Engine / General:

- fix various bugs related to mid-game join
- fix LosHandler ignoring units with losRadius <= 0 but airLosRadius > 0
- fix a leftover icon bug
- fix BFGroundDrawer crash at extreme view-distances
- fix death-wait
- allow death-wait in games with more then 2 ally teams
- reduce risk for deadlock during stack trace
- allow to specify game-start-delay in the start script (GameStartDelay)

Engine / Simulation:

- prevent SEGV when updating skidding units
- improve turnInPlace=0 path-following (#2072)
- units ignored ETA failures when <= 200 elmos from goal, even if goal unreachable
- make pushResistant (allied) units still move out of the way of constructions

Engine / Rendering:

- update the displayed resource production for inbuilt units too
- fix spectators see ghosted buildings
- do not draw icons for noDraw units
- do not cast shadows for noDraw units
- do not draw healthbars for noDraw units either

Engine / AI:

- add feature-ID trace-ray command
- allow fetching of other teams resources (current, income, usage, storage)
- EnableCheatEvents() was a no-op because it did not enable cheats temporarily
- IsUnitInLosOrRadarOfAllyTeam() now considers gs->globalLOS
  -> UnitCreated now may trigger EnemyCreated for non-cheating AI's
- E323AI: new version: 3.22.4
- Python AI Interface: removed a few memory leaks

Unitsync:

- fix mapname/mapfilename conflicts

Installer:

- add portable install option (default: disabled)
- add Python AI Interface to the windows installer

Repository:

- remove SelectionEditor

-- 0.82.5.1 --------------------------------------
Engine / General:

- fix FPS/direct controlling crash

Engine / multi-threaded:

- fix unit icons/healthbars crash

Engine / AI:

- E323AI: NOTA and XTA support enhanced & lots of improvements

-- 0.82.5 --------------------------------------
Engine / General:

- draw icons for cloaked units again
- fix NaNs if map hardness is zero

Lua

- fix UnitPreDamaged Lua CallIn

Engine / AI:

- AAI: do not crash on game end if mod did not spawn a commander in the first 450 frames
- E323AI: lots of fixes & support for NOTA & Conflict Terra and AI options

-- 0.82.4 --------------------------------------
Engine / General:

- fix demo desyncs if commands were issued during pause
- fix abnormal demo speed (during reconnect and temporary lags)
- fix demo runs away from local user
- fix mid-game join
- fix FullScreen mode under Linux
- fix KDE/GNOME detection for error message box
- fix another FPU reset in LuaLobby
- don't reload defaults each time /ctrlpanel command is used (behaves now the same way as /cmdcolors)
- speed up reflection pass a little
- disable heatmapping by default
- fix COB's KILL_UNIT for self-kills
- fix bugged default-argument passing in path-follower
- fix broken return-value handling for UnitPreDamaged
- OBJParser: deal with carriage returns
- fix holdSteady (and sweeping beamlaser) rotations
- fix SEGV when /give'ing features
- fix crash when linking grass shaders on non GLSL system
- fix specs do not see all features
- fix Projectiles do not show up for missile weapons or torpedoes
- fix crash on bad playername message
- disable team highlighting for spectators by default

Lua

- fix incorrect ClipPlanes in DrawInMinimap if gl.SlaveMiniMap is used
- fix metal-maker widget

Engine / AI:

- fix crash on capture from/by AI
- fix Python AI Interface
- AAI support version independent mod config files, for example BA.cfg

Engine / Dedicated server:

- fix/re-add sync-check
- possibly fix crash when sending data to improperly-connected autohost socket

Unitsync:

- add support for more then 16 start positions

Installer:

- fix checks for installed .Net version
- add SpringDownloader desktop shortcut

CMake:

- fix _make install-spring_ to work after initial configure
- fix windows linking (related to -lws2_32)

Documentation:

- updated man pages: spring & spring-dedicated
- new man pages: spring-headless & spring-multithreaded

-- 0.82.3 --------------------------------------
Engine / General:

- rebuild because of stale base content packages
  (different checksums between windows and linux)
- fix NullAI installing into /

-- 0.82.2 --------------------------------------
Engine / General:

- miscellaneous OS X compile/link fixes
- fixed incorrect handling of negative references in OBJParser
- fixed misinterpretation of the OBJ spec
- reorganized WindowState saving on exit on X
- fixed source package build
- always print crash errors to infolog
- added i965_dri.so to OGL driver crash detection on Linux

Engine / Simulation:

- fixed autosharing resources to dead teams
- fixed units disappearing from quadfield after load-unload in transport

Engine / Lua:

- fixed invalid FPU flags after LuaLobby connection is made
- fixed DrawFeature event
- Spring.UnitScript.GetScriptEnv returns nil if the unit does not run a Lua script
- Spring.GetCOBScriptID returns nil if the unit does not run a COB script
- fixed typo in mapdefaults.lua
- possibly fixed GetPlayerInfo

Engine / AI:

- updated E323AI to latest version

-- 0.82.1 --------------------------------------
Engine / General:

- fixed source package build
- fixed MT crash in DrawCommands
- fixed radar blips seen by spectators
- fixed dark metal spots in watert
- reduced default value of max particles
- increased default LOD/icon distances
- glFont::GetTextHeight & glFont::GetTextNumLines return now 0 lines for empty strings
- fixed 3do shadows
- fixed minimap rendering in dualscreen modes
- fixed laggy camera update with UnitTracker
- fixed seismic pings with incorrect size
- fixed incorrect highlight on end game screen

Engine / Simulation:

- prevent units from becoming immobile zombies while maneuvering
- base etaFailures limit on a unit's full-circle turning time
- decrease etaFailures every frame we are not standing still
- fixed vibrating planes
- fixed nanoframe being spawned while game is paused
- fixed slow response when releasing wait command
- fixed the silly unit spinning ad-infinitum bug
- fixed unit occasionally not taking damage when it just left a transport
- fixed some awkward transport behaviour
- properly deal with radars/jammers being transported
- fixed unit returning to pick up point when it dropped out of transport
- skip waypoints that are behind a unit but within its turning circle

Engine / Lua:

- added a LuaRules DrawFeature call-in
- fixed LuaUnsyncedCtrl::SetWaterParams
- fixed Spring.Restart with Win 2k and OpenAL-soft
- LuaSyncedRead::GetUnitExperience: also returns limExperience now
- removed obsolete engine options from EngineOptions.lua

Engine / Dedicated server:

- use default visibility (hidden) for spring-dedicated under non-windows

AI:

- RAI: log files are now under _log/_ sub-dir
- AAI: added config for BA 7.14
- E323AI: added config for BA 7.14

Installer:

- made main part mandatory

-- 0.82 --------------------------------------
Engine / General:

- added HeadLess version of the engine (no graphics, no sound) by hughperkins
- better overall handling of connection problems, with highlighting of uncontrolled units
- players in startscript may reconnect to a running game, even with changed IP address
- players can choose server speed throttling algorithm by voting (/speedcontrol)
- do not crash on AI Interface error
- MT/GML: now a separate build target: make spring-multithreaded
- fix linking under Fedora 13
- allow choosing between available sound devices through config (snd_device)
- add missing /skip variants to word completion
- use new dependency system for maphelper.sdz (by Beherith)
- OS X fixes
- hardcoded startscripts removed (Air Test, etc.)
- more default intercept types added
  EmgCannon -> 1, AircraftBomb -> 8, Flame -> 16, TorpedoLauncher -> 32
  LightningCannon -> 64, Rifle -> 128, Melee -> 256
- engine does not spawn start unit anymore
- engine does not use StartUnit tag in sidedata.lua anymore (but still passes it to Spring.GetSideData)
- engine does not read `Script' map tag anymore
- engine does not read `ScriptName' modoption anymore
- engine does not deal with `StartMetal' and `StartEnergy' modoptions anymore
- pathfinder debugging views can be rendered now when spectating (and not cheating)
- dropped modinfo.tdf support (only modinfo.lua is supported now)
- clients now compare their path-cache CRCs and warn about differences instead of updating their running checksums with them
- Spring's CPU affinity can now also be set on non-win32 platforms, use the SetCoreAffinity ~/.springrc parameter as a bitmask

Engine / Simulation:

- new unit tag: cloakTimeout, int, default 128 frames
- fixed the featureVisibility mod-option
- fixed calls to the engine path-finder to be sync-safe when run in unsynced context
- reduced frequency of units ending up in "can't reach destination" situations
- fixed units not inheriting orders from factories and piling up (due to having their paths aborted)
- removed the TEDclassString UnitDef parameter (side-effect: hub-like units will break if they have a yardmap)
- removed diminishing metal makers option
- changed the semantics of the holdSteady UnitDef tag
  true ==> slave transportees to orientation of transporter attachment piece
  false ==> slave transportees to orientation of transporter body (default)
- fixed canDGun+commandfire to properly reproduce commander-like DGun behavior without script hacks

Engine / Rendering:

- fixed alpha-masked shadows for S3O models
- fixed ATI alpha masked shadows (trees, features)
- fixed far-textures (caused changing teamcolors and disappearing units)
- support the OBJ model format (https://springrts.com/phpbb/viewtopic.php?f=9&t=22635)
- support specular lighting, multiple blendable detail textures, and sky reflections on SMF maps ("SSMF")
  https://springrts.com/phpbb/viewtopic.php?f=13&t=21951
  https://springrts.com/phpbb/viewtopic.php?f=13&t=22564 (slightly outdated wrt. the SMD parameters)
- converted all shadow, tree, grass, model, and map shaders to GLSL
- vertex-animated grass; maps can specify the grass blade- and shading-textures and the dimensions/angle for blades
- refactored and abstracted unit/feature/projectile drawing to be format-agnostic
- increased global decoupling between simulation and rendering code
- fixed model specular lighting (most visible on units)
- S3O model normals are now auto-scaled to unit-length
- maps with misspecified skybox textures will now load (without crashing Spring)

Sound:

- use OpenAL Soft 1.11 under windows (was Creative)
- air-absorpion: absorb high frequencies when sound effects are far away (config-key & default value: snd_airAbsorption=0.1)
- no-sound support through NO_SOUND compile time define or NoSound config-key

Unitsync:

- list of data-dirs for engine & unitsync are now always equal
- API cleaned: no more void\*, all structs deprecated
- data dirs read from /etc/spring/datadir (separator is now ":" instead of " ")

FontRendering:

- added a new inline ctrl command '\008' which resets the TextColor to the
  color that was active when Print() got called, as in the following example:
  font:SetTextColor(0, 0, 1); -- blue
  font:Print("\255\255\000\000red\255\000\255\000green\008blue");

Lua:

- LuaLobby:
  callouts:

  - Script.CreateLobby() -> userdata lobCon
  - lobCon:Connect(string url, int port)
  - lobCon:Disconnect()
  - lobCon:Login(string username, string password)
  - lobCon:Rename(string newname)
  - lobCon:ChangePass(string oldpassword, string newpassword)
  - lobCon:Register(string username, string password)
  - lobCon:ConfirmAggreement()
  - lobCon:JoinChannel(string chan_name [,string password])
  - lobCon:LeaveChannel(string chan_name)
  - lobCon:Say(string chan_name, string message)
  - lobCon:SayEx(string chan_name, string message)
  - lobCon:SayPrivate(string user_name, string message)
  - lobCon:StatusUpdate(bool ingame, bool away)
  - lobCon:Channels()
  - lobCon:KickChannelMember(string chan_name, string user_name, string reason)
  - lobCon:ChangeTopic(string chan_name, string topic)
  - lobCon:Poll()
    callins:
  - lobCon.DoneConnecting(bool sucess, string error_message)
  - lobCon.ServerGreeting(string server_version, string spring_version, int udp_port, int mode)
  - lobCon.RegisterDenied(string reason)
  - lobCon.RegisterAccepted()
  - lobCon.LoginDenied(string reason)
  - lobCon.LoginEnd()
  - lobCon.Aggreement(string text)
  - lobCon.Motd(string text)
  - lobCon.ServerMessage(string text)
  - lobCon.ServerMessageBox(string text, string url)
  - lobCon.AddUser(string user_name, string country, number cpu)
  - lobCon.RemoveUser(string user_name)
  - lobCon.UserStatusUpdate(string user_name, bool away, bool bot, bool ingame, bool moderator, int rank)
  - lobCon.ChannelInfo(string chan_name, int num_users)
  - lobCon.ChannelInfoEnd()
  - lobCon.RequestMutelist(string chan_name)
  - lobCon.Mutelist(string chan_name, table {"userABC","userXYZ",...})
  - lobCon.Joined(string chan_name)
  - lobCon.JoinFailed(string chan_name, string reason)
  - lobCon.ChannelMember(string chan_name, string user_name, bool joined)
  - lobCon.ChannelMemberLeft(string chan_name, string user_name, string reason)
  - lobCon.ChannelMemberKicked(string chan_name, string user_name, string reason)
  - lobCon.ChannelTopic(string chan_name, string author, int time, string topic)
  - lobCon.ChannelMessage(string chan_name, string text)
  - lobCon.Said(string chan_name, string user_name, string text)
  - lobCon.SaidEx(string chan_name, string user_name, string text)
  - lobCon.SaidPrivate(string user_name, string text)
  - lobCon.Disconnected()
  - lobCon.NetworkError(string message)

- added a new los checking argument to Set..RulesParam()
  e.g. new syntax is SetUnitRulesParam(int unitID, string paramName, float paramValue, { public = true } )
  possible values for the table are:
  `private` : only readable by the ally (default)
  `allied` : readable by ally + ingame allied
  `inlos` : readable if the unit is in LOS
  `inradar` : readable if the unit is in AirLOS
  `public` : readable by all
  note: if one condition is fulfilled all beneath it are too (e.g. if an unit is in LOS it can read params with `inradar=true` even if the param has `inlos=false`)
  note2: all GameRulesParam are public, TeamRulesParams can just be `private`,`allied` and/or `public`
- new Spring.GetUnitMetalExtraction(unitID) -> nil | number
- new synced Spring.ShareTeamResource(teamID_src, teamID_dst, type = ["metal"|"energy"], amount) -> nil
- new Spring.GetTeamResourceStats(teamID, resourcetype = ["metal"|"energy"]) -> nil | used, produced, excess, recv, sent
- added Spring.SetProjectileMoveControl(number pid, boolean b) --> nil callout; if (b), makes all piece/weapon projectile movement fully Lua-controllable
- improved Spring.GetTeamStatsHistory. It returns now also the most recent data, not just the data of the cached stats which get updated only all 16 seconds.
- fixed incorrect scaling in Spring.SetUnitResourceParam
- fixed Spring.GetAIInfo, it returned incorrectly the given teamID as first argument
- fixed a bug in Script.RemoveSyncedActionFallback
- added missing `coroutines` module and missing `select` function to unsynced LuaGadgets
- added os.setlocale to LuaUI
- added Save/Load callins, both get a userdatum representing a save file as single argument
  Save is unsynced, Load is synced (both gadget only)
  Should allow implementing savegames mod side with less hacks
  /reloadgame chat command added for development purposes, this fires the Load event again
- modified base {armordefs|movedefs|sidedata}.lua to follow pattern of icontypes.lua
- added a ShieldPreDamaged call-in to LuaRules, triggered before any engine shield-vs-projectile logic executes
- UnitDefs[i].model.textures now actually contains the names of the model textures for unit-type i
- script.txt: Engine ignores GAME\\ModOptions\\LuaRules & interprets GAME\\ModOptions\\LuaGaia just as a boolean now
- fixed GetScreenGeometry on win32 platforms to return the proper screenSizeY, not screenSizeY - taskbarSizeY
- moved Spring.SetUnitRulesParam, Spring.SetTeamRulesParam, Spring.SetGameRulesParam from LuaRules-only to LuaSyncedCtrl
- removed evaluator functions from LuaOpengl
- removed Spring.MakeFont
- removed Spring.SetRulesInfoMap, Spring.GetRulesInfoMap, Script.GetConfigString
- removed Spring.CreateUnitRulesParams, Spring.CreateTeamRulesParams, Spring.CreateGameRulesParams
- removed AllowUnsafeChanges("USE AT YOUR OWN PERIL"), it's always enabled now

Engine / Unit Scripts:

- LUS: SetSignalMask raises error now when called outside thread
- LUS: Signal/SetSignalMask support arbitrary objects as `signal names' now, bitwise logic is still applied to numbers
- LUS: \_G in a Lua Unit Script points to the environment of that Lua Unit Script now
- LUS: TargetWeight and BlockShot are actually called now
- COB: Added KSIN (135, 1024\*sin(x)), KCOS (136), KTAN (137) and SQRT (138) get constants.
- LUS: Fixed a desync bug.

Interface:

- fixed middle click scroll speed on linux
- added new MiddleClickScrollSpeed option
- more responsive mouse movement
- made it easier to select units in factories
- added new command "/buildwarnings [0|1]", analogous to "/movewarnings [0|1]"
- improved unit type mix in front moves
- new middle click scroll icon

AIs:

- added various new debug-draw callouts for profiling, state visualization, etc.
- make sure the Release event always reaches dieing-state AI's
- added a GetUnitVel callback, returns a unit's velocity vector
- new Python AI Interface by theGeorge & abma
- E323AI bug-fixes, logic- and performance enhancements
- AAI: fix some crashes (one on death) & a logic error
- RAI, KAIK and AAI: support unit captured&given from&to them
- RAI, KAIK and AAI: config-, log- & cache-files now compatible with pool archived mods
- KAIK: switched to .lua config-file format, added a basic mod whitelisting mechanism
- KAIK: event-based TM updates
- fix a crash on Skirmish AI death
- fix Java AI Interface loading on Debian
- fix Java compilation on some systems
- fix Java crash: increase initial java heap size (by cranphin)
- send an Update event before the first UnitCreated (when initializing mid-game)

Bugfixes:

- LightingCannon (sic) finally isn't supported anymore, use LightningCannon instead (default weapondef_post.lua includes backward compatibility)
- fixed a bug in LuaParser, so you can now use booleans in lua CEG configfiles (before it just accepted "1"/"0" for some tags)
- fixed occasional bug where countdown time went into negative numbers and game never started
- fixed impactOnly ignored features
- improved error handling on network packet level

-- 0.81.2.1 --------------------------------------
Engine:

- Fixed incorrect projection matrix (i.e. tiny build menu)
- Fixed some Mac and MSVC compile errors
- Switched to OpenAL-soft, that should fix volume escalation
- Slightly improved detection of ATI/Nvidia OpenGL driver crashes
- Fixed team-color bug for both units and flying pieces

AIs:

- AAI: removed obsolete map config files
- AAI: added BA 7.12 config file
- KAIK: removed dependence on TEDClass

-- 0.81.2 --------------------------------------
Engine:

- Removed OpenGL 1.4 version check
- Fixed sync with GCC 4.3 builds
- Fixed crash when sending startpositions of uncontrolled team
- Fixed race condition when taking screenshot
- In 'info' screen, human name instead of archive name is shown
- Reduced input CPU usage
- StartSpectating only has an effect if the player is not already spectating
- Packet resends have higher priority now than new packets
- Print message to infolog when traffic from unknown IP gets dropped
- Fixed vsync for Linux
- Removed OpenGL immediate mode rendering to work around broken ATI drivers

Engine / Simulation:

- Kamikaze distance is now 3D, this fixes mines
- Added new UnitDef tag 'kamikazeUseLOS' (default false)
- Fixed submarine stacking
- Fixed units using smoothMesh unable to land at repair pads near hills
- Fixed COB interpreter incorrect compiler optimizations (fixes e.g. XTA hovercraft)
- Fixed features not taking damage from impactOnly weapons
- Fixed multiple-cons-build failing on structures with open yardmaps
- Fixed maxParalyzeTime when paralyzeOnMaxHealth is enabled

Engine / AI:

- Fixed AI initialization process for games with >1 human player and for Lua AIs
- Fixed NETMSG_AISHARE for message size >32k
- Allow fetching the file path of an AI interface plugin
- Fixed crash on Skirmish AI death

Engine / Lua:

- Spring.MarkerAddPoint now defaults to local
- Spring.CreateFeature heading value is converted to closest-matching facing

Lua Unit Scripts:

- Fixed handling of AimShield return value
- Fixed Sleep/WaitFor not working in Killed
- Slightly improved AimWeapon/AimShield/Killed error messages

AIs:

- E323AI 3.18.1
- AAI: Doesn't use minkowski metric for retreat distances anymore
- AAI: Fixed some NaNs

-- 0.81.1.3 --------------------------------------
Engine:

- Fixed automatically assigning start position if none is chosen
- Fixed bug in DXT5 loading (caused artifacts with teamcolors)
- Fixed F1 heightmap view
- Fixed playing of demos created in a game in which Spring crashed
- Made spectator drawings white instead of random team color
- Disable recording demos from demos (default, DemoFromDemo in springrc to enable)
- Added a mechanism to register/unregister for messages that are normally not broadcasted to game server

AIs:

- E323AI 3.14.5:
  - support for: partial watermaps (plays land only), group merging,
    auto kbot/vehicle lab selection, template configs
  - XTA & BA config improvements
  - Better lab assisting
- KAIK: do not build MExes on spots used by allies

-- 0.81.1 --------------------------------------
Engine / Simulation:

- Terrain types and water damage are now taken into account for pathing checksum
  (Fixes desyncs on maps with map options which modify terrain types or water damage)
- Changed builder patrol/fight behaviour for Hold Pos and Maneuver movestates
- Being-built units are now visible on radar by default

Engine / Rendering, UI:

- Added speed display
- Improved basic player list
- If OpenGL 1.4 is not available, an error is shown
- Fixed being-built unit rendering
- Placing buildings close together works again
- Fixed canceling of first build item when buildiconsfirst is toggled

Engine / Lua:

- Less strict type checking in VFS.DirList (fixes FBI2Lua converter)

Engine / System:

- Map options fixed
- Menu doesn't crash anymore if no mods/maps found

-- 0.81.0 --------------------------------------
Engine / Simulation:

- Builders are now smart enough to to reclaim features that block their
  buildsite, which weren't there when they started moving to it
- Fixed issues with features with 0 metal and 0 energy
- Fixed crashes / desyncs when using area orders with a large radius
- Hold position doesn't imply "don't turn" anymore, and units with small
  firing arcs in general behave a lot better in this case.
- Units on hold position now don't get confused by nearby invalid targets
- Smooth ground mesh for better aircraft behaviour (unit tag: useSmoothMesh)
- Wind fixed to stay in between map boundaries
- Momentum isn't conserved anymore in unit-unit collisions
- Impulses decay every frame instead of every slow update
- Kamikaze units now do a visibility and 3d distance check
- Fixed ships were able to 'climb' cliffs
- Fixed multiple issues with pathmap updates after terraforms

Engine / Rendering, UI:

- Changed default binds of forcestart, singlestep, debug to Alt+f, Alt+o, Alt+b
- Non-reclaimable features show up as red squares now when placing a building
- Out-of-LOS features don't show up anymore when placing a building
- Fixed icons not rendering properly when distance-to-ground method was used
- Better mipmap handling on SMF maps
- Fixed specular lighting calculation on SM3 maps
- Fixed "/shadows" command on SM3 maps
- Fixed SM3 map holes
- Fixed "/wiremap 0|1" (argument was inversed)
- Lots of other drawing fixes
- Far-textures fixed
- Load-screens preserve aspect ratio
- Added checks for insufficient gfx cards
- Added new shadow rendering option ("/shadows 2"): it disables terrain rendering
  in the shadow pass, which gives a huge performance increase
- Changed BumpWaterReflection values:
  0 = disable; 1 = enable (no terrain); 2 = enable (with terrain - was '1' before)
- Fixed occasional hang when word wrapping text
- S3O transparency is now supported even with AdvUnitShading disabled

Engine / Lua:

- Added AllowFeatureBuildStep call-in to included gadget handler
- Fixed typos in certain featuredefs.lua, weapondefs.lua that mutilated error messages
- Spring.Rehost fixed on Win32 when spaces in install path
- Added support for modifying CTAAirMoveType (gunships)
- Add GetUnitMoveTypeData(unitID) call-in, returns a table
- Fixed Spring.SetFeatureDirection

Engine / System:

- Fixed incorrect startpositions in skirmish games
- Proper handling of mod dependencies
- MAN pages support (Linux)
- Support for external debug symbols (Linux)
- Fixed an issue with using the root of a drive as data directory
- Use less memory when reading from 7z archives
- Improved handling of spectators in case of sim slowdown and desync

AIs:

- E323AI: CPU performance improvements & bug fixes
- KAIK: better D-Gun handling

-- 0.80.5.2 --------------------------------------
Bugfixes:

- AI: Properly fix Lua AI loading
- AI: Fix crash when Lua AI team dies
- AI: Fixed clean target of ant scripts
- Fixed deadlock in hang detection
- Fixed .desktop file
- Fixed crash on startup on Ubuntu Karmic
- Fixed reflections
- MT: Don't crash if draw callins access features
- Fixed fading features for ATI (atiHacks)
- Fixed crash when parsing certain chatmessage from autohost
- Fixed crash if OSC stats sender is enabled and no network is reachable
- cmake: Fixed 7-zip detection

Changes:

- AI/cmake: Split system dependent/indepentent install parts (AI_LIBS_DIR, AI_DATA_DIR)
- AI: Debugging is now enabled by default in the ant scripts
- Time profiler cleanup, both code and UI
- Show an AI's shortName + version + nick in tooltips

-- 0.80.5.1 --------------------------------------
Bugfixes:

- fix a bug where empty network packets were spammed (eating bandwidth and demo space)
- fix a possible desync in the pathfinder
- fix mouse jumping in windowed mode (windows)
- fixed crash in KAIK

-- 0.80.5 --------------------------------------
Bugfixes:

- lots of bugs in the AI interface fixed
- fix some network-related problems and improve behaviour under worse conditions
- large performance increase for units on patrol
- fix some performance problems with the new heatmap pathfinder
- fixed a crash which could occur after an AirBase was destroyed

Gui:

- joystick support
- some bugs resolved which could lead to crashes when using ogg samples or music
- properly handle dds textures
- fix some mouse-jumping in windows

Sim:

- lua unit scripts major rework
- some sources of desync fixed

Misc:

- fix some compiling problems related to boost 1.40
- some fixes for OS X

-- 0.80.4.2 --------------------------------------
Bugfixes:

- fix crash when doing lots of mapdrawing (#1632)
- fix crash when loading certain cursornormal files
- fix drawing of ghosted buildings
- properly initialise SDL timer for dedicated server
- fixed GatherWait cursor
- don't send resign-command when already spectating
- fix units with vlaunch missiles (#1647)
- Spring compiles boost 1.40

-- 0.80.4.1 --------------------------------------
Bugfixes:

- Fix missing tree fog, misc fog fixes
- fix crash in AI pathing
- fix crashes in the Java AI interface
- added VFS.MapArchive
- fix possible crash on exit
- fix crash when loading certain ogg-files

-- 0.80.4.0 --------------------------------------
Bugfixes:

- don't reclaim dragonteeth with area-reclaim
- lua unit script framework respects script tag

-- 0.80.3.0 --------------------------------------
Bugfixes:

- fix direct control unit movement (forwards and backwards)
- don't spam empty directories everywhere
- fix BA crash when doom was installed
- fix desync in pathfinder
- fix music timing
- single units should move in straight lines again
- fix shadows on features
- fix LocalModelPiece::GetPiecePosIter,GetPos,GetEmitDirPos
- fix vertical text aligning of multiple GUI controls
- fix jumping in middle click scrolling (windows)
- massive speedup in Spring.GetVisibleUnits
- added changed font vertical alignment opchars in gl.Text & luaFonts
- fix wreckages sinking in the ground
- add a modinfo featureLOS.featureVisibility tag
- fix BumpWater shorewaves
- make /nocost reversible

Lua:

- add snapToGround bool param to SetFeaturePosition (default true)
- add SetFeatureAlwaysVisible (analog to SetUnitAlwaysVisible)
- added changed font vertical alignment opchars in gl.Text & luaFonts (baseline renamed to 'd'->'x', and use 'd' for descender)
- unit script: added GetPieceTranslation, GetPieceRotation, GetPiecePosDir
- unit script: SetSpeed/SetDirection/Go changes (see commit
  b49928454225f19f5c439c908c0a8cad775fb7ed)
- unit script: removed Create (framework handles it)
- -- 0.80.2.0 --------------------------------------
  Bugfixes:

- fix creation of DDS textures
- Unbreak transports when loadingRadius <= some magic number

Lua:

- fix Zlib-compression and decompression for lua
- Added VFS.SubDirs(dir, pattern, modes) callout to LuaParser
- added some more map water configs to Game. table

-- 0.80.1.0 --------------------------------------
Bugfixes:

- don't write spring-files where they don't belong
- some rendering fixes (ATI errors, bumpwater)
- IPs of players not broadcasted
- area reclaim doesn't know of features not in los anymore
- slightly better paralyzeDamage handling
- fixed several issues with ArchiveMover - it now moves files to
  My Documents\My Games\Spring and should be more reliable

-- 0.80.0.0 --------------------------------------
Engine / Simulation:

- units can now move backwards
- notable speed improvement of feature handling (unit wrecks, tress...)
- LuaCOB: make it possible to make units with Lua instead of bos/cob
- some fixes for sm3 map-format
- pathfinding improvements for close groups of units
- reclaim / resurrection now correctly exported to LuaAI
- make better use of SSE functions for speed improvements
- now compatible with Boost 1.34 again (was 1.35 for spring 0.79)

Engine / GUI, System:

- IPv6 support for all parts of the engine
- caching for 7z archives: speed improvements at high compression levels (solid archives now usable)
- rudimentary menu when starting spring executable directly
- BumpWater got a major speed increase, and shorewaves look better
- various improvements for the sound system (multithreading, fixes for music, ogg-effects...)

AIs:

- Added /aikill and /aicontrol commands (only works in singleplayer in this version)
- AIs won't crash the game anymore on startup
- some AIs play better now (especially KAIK)
- a lot of minor bugs

Dedicated Server:

- ingame stats exported to the autohost on game end
- always print IPs of players, even if connection fails
- allow password-protecting client slots to prevent name spoofing (needs lobby support)

-- 0.79.1.1 --------------------------------------
Bugfixes:

- fix microlags and increased CPU usage for dedicated server
- fix the game ending when playign with AIs
- various AI-interface and AI-related fixes
- fallback to Lua for unhandled actions
- fixed exploit where spectator could make Team 0 resign
- fix certain soundeffects

-- 0.79.1 --------------------------------------
Bugfixes:

- fix crash when a direct-controlled unit dies
- fix watching of demos
- various AI-related fixes
- SetUnitShieldState fixed
- fix an infinite memory allocation loop on startup
- restore acceptable framerate on very old cards

-- 0.79 --------------------------------------
Major stuff:

- rewritten AI interface in C
- new font renderer
- sound system and music overhaul
  -> 2292 files changed, 149700 insertions, 290245 deletions

-- 0.78.2.1 --------------------------------------
Bugfixes:

- fix for game refusing to speed up after a slowdown
- set MaxSpeed to 20 on (test)singleplayer
- fix insta-gameover vs. AIs
- fixed the server to run away from the local player on high gamespeeds

-- 0.78.2 --------------------------------------
General bugfixes:

- fix crash on gamestart with lots of players
- should fix most of the desyncs

Engine:

- fix startscript not written in demo header
- units trying to stay in LOS of a target account for mip levels
- fix the no-commander-for-side-Arm problem for local games not based on TA content
- cob: made modelpiece mapping case-insensitive
- initialize relPos in EmitSfx
- fix for a fix in GetEmitDirPos
- fix aircraft not flying back to repair pads when all are reserved
- planes that are being refuelled aren't able to fire anymore
- clamp map position in unitLoader.
- players who didn't choose a start position start in their ally box
- fixed short command line params on windows (mantis #1271)
- don't try to kill dead teams
- add gamemode 3 (no game over) and make it the default in non-AI non-script.txt games
- removed luabind + lua-startscripts
- fixed multithreaded pathing
- removed partial match test for piece mapping
- fixed emitsfx crash (Mantis: #1252,#1238 & #1242)
- stunned planes shouldn't keep repair pads reserved
- always apply a miniscule amount of impulse when a unit is damaged

User Interface:

- fixed AltTab bug on gpus w/o FBO support
- EndGameBox shouldn't render stats if the game hasn't started
- changed minimap_startbox.lua to not show cones in top left (#1272)
- StartPosSelector now unloads itself when game is started without sending start positions.
- fixed refraction=2, disabled endless ocean for maps that don't show any water at the start of the game
- fix #1258 (FBO crash on exit with non-FBO gpus)

Lua:

- gl.Texture() can now load the textures of s3o features, syntax is gl.Texture('%'..(Game.maxUnits+featureDefID)..(':0' or ':1'))
- added a new (unsynced) callout Spring.SetWaterParams, it needs cheating enabled and allows you to edit the mapwater settings ingame
- fix Luamsg if null-characters where inside
- add an GetUnitTransformMatrix unsynced callout (handy for shadow algs)

Installer:

- add RAI to installer
- allow the user to close TASClient / downloader during install

-- 0.78.1.1 --------------------------------------

- fix Features (wrecks, rocks) not being drawn with reflective units

-- 0.78.1 --------------------------------------
(notice the new version number scheme, its MAJOR (0.78) - MINOR (1))
Crashes:

- fix crash on window resizing
- more groundDecal fixes
- do not force useFPShadows to true unless they are supported (to prevent crashes during map-loading)
- fix UDP buffer overrun

Engine:

- use %CSIDL_LOCAL_APPDATA%/springsettings.cfg (e.g. \Users\<user_name>\AppData\Local\) to store settings instead of registry
- use SSE instead of X87 for FP-calculations
- if in Choose ingame startposition mode its possible to play a map with more players than startpositions
- fix the aircraft hovering / drifting
- fix random start positions not being random
- fix area-attack for aircraft (#1144 again)
- GenerateTargets no longer has much information that the team requesting targets doesn't have
- fix spikes / lags in hosting demos
- don't allow gunships to drift off the map
- make planes bobble in place gently by default, and make hoverfactor work as intended
- fix unit reply sound
- don't open a (unused) network socket in single player mode
- Don't let static beamlasers overshoot
- Added new collision volume types: sphere, footprint.
- Units/features with no collision volume defined get 'footprint' type by default now, this restores the 0.76b1 behaviour as default
- Gave COB function BlockShotX a third parameter, userTarget, uses the engine's hasUserTarget value, that seems to reflect when unit has an attack order
- fix teamstats in demofile
- don't fail with "network unreachable" instantly, but wait until timeout or connection fix

Spring start / script.txt changes:

- added Game/MyPlayerName (MyPlayerNum is now deprecated and will be removed sometimes)
- send the gamesetup to each client once they join the server
- Player / Team / Mod - options are not needed anymore for clients, they are transferred from the server once ingame

User Interface:

- Argh's ground-decal shader shadow tweak
- enable startboxes by default
- decreased CPU load and increase drawing speed lines
- fix small memleak when resizing the window
- The protection ring shows up for non-stockpile interceptors
- fix detection of units under mouse for spectators
- fix the CTeam statistics tracking
- do not force useFPShadows to true unless they are supported (to prevent crashes during map-loading)
- fix for annoying flickering of the grounddetail texture in CA if some LUPS fx were visible
- fixed a bug with waterplane on centerrock (the terrain is deeper than the water plane was drawn)
- made forcestart a real shortcut, so you can unbind it
- don't draw the terrain if curMaxHeight<0 and voidwater
- fix long commandline options for linux build
- dynamic sky rednerspeed improvements

Lua:

- added Spring.GetLastMessagePositions() (returns the 10 last message positions)
- moved Spring.GetDrawSelectionInfo() LuaUnsyncedCtrl -> LuaUnsyncedRead
- moved Spring.GetSoundStreamTime() LuaUnsyncedCtrl -> LuaUnsyncedRead
- added a 2nd return value to Spring.GetSoundStreamTime() (total sound stream length in seconds)
- Made it possible to specify start positions in map with LUA .smd file
- Immobile Builders doesn't put factories on patrol and doesn't give a stop order

Unitsync:

- Don't crash/abort() on invalid handles, but throw exception instead
- Fix crash bug in unitsync when UnInit/Init was called after an earlier Init which returned error code
- Doesn't leak exceptions anymore over C interface.
- Doesn't just crash by design anymore after something goes wrong.
- Added API GetNextError, can be used to get error queue (currently limited to 1 error)
- added SetSpringConfigFile(char\*): reinitialise config handler to use the provided path
- added char\* GetSpringConfigFile(): return the currently used path to the config file

- Deprecated APIs:
  - GetCurrentList
  - AddClient
  - RemoveClient
  - GetClientDiff
  - InstallClientDiff
  - IsUnitDisabled
  - IsUnitDisabledByClient
- Removed all code supporting those functions.
- These deprecated functions pop up MessageBox once on windows to nag lobby devs.
- Use engine's logOutput, still writes to unitsync.log (overwrites, doesn't append)
- ProcessUnitsNoChecksum is now identical to ProcessUnits.
- ProcessUnits now returns 0 immediately (but should still be called in loop until it returns 0, in case it's changed sometime later)

Installer:

- Fix groups.ini being overwritten by installer
- removed all maps and mods from the installer

AAI v0.875

- Current amount of land/water within the base is taken into account when selecting factories (e.g. don't order water factories on a land map anymore)
- Improved AAIs behaviour on water maps with start positions on islands/land masses
- Improved metal extractor construction algorithm (spots closer to available builder, faster algorithm, works better if there are land and sea spots available at the same time)
- Continent file version bumped to 0.87
- Fixed a crash on shut down

-- 0.77b5 --------------------------------------
Installer:

- check if common unitsync users are running, abort installation if any is found
- don't download and install RiverDale and SmallDivide when the user already has it

Crashes:

- even more preventions of DIV0

User Interface:

- when watching a demo with a setup locally, overwrite maxspeed with 10
- fix mousehover detection for radar icons
- fixed performance of Lua fontHandler
- make the orbit-controller actually do something on win32
- make the orbit-controller speeds configurable
- rescale fonts on window resize so they don't look ugly
- fixed black border blending of fonts
- ATI drivers lie about not supporting extensions required by shadows. we ignore that extension now

Engine:

- new fbi tag: bankingAllowed - only affects hoverAttack=1 units; controls if the aircraft can bank.
- fbi tag upright now affects gunships
- fix CAirMoveType using an uninitialized variable (maxSpeed)
- fix Aircraft fuel not working properly on units with multiple weapons
- fix SendLuaMsg
- improve heading table resolution, this makes huge units less jerky
- fix the gamespeed not going back up after it was slowed down
- when ArchiveCache has not been written yet, do not add archive dependencies that are already in the chain (was breaking demos)
- fix factories switching their yardmap to fully-blocked when a unit is present inside (so units don't getstuck inside)
- fixed map defined detailtex
- fix LUA Spring.GetTeamResources does not return "received" for one player on each team

Unitsync / Lobby interface:

- engine now loads many start script options from GAME\ModOptions\ section in start script, instead of just GAME\
- added EngineOptions.lua to springcontent.sdz, this contains the default engine options for backward compatibility
- GetModOptions unitsync call parses both EngineOptions.lua and ModOptions.lua, and returns combined list of options

Random:

- lots of fixes to CMake and dedicated server (already released)
- minor KAIK stability fixes

-- 0.77b4 --------------------------------------
Crashes:

- fixed a crash (on exit) in colormap.cpp
- fixed another division /0
- fix crashes on "/take"
- fix crash when loading units into transporters

User Interface:

- fix tabulator toggling overview mode
- reduce the maxBreaking-is-zero message spam
- fix far textures showing up as gray rectangles
- the error message "no writable directories found" now has a windows version too
- Fix explosion spheres appearing at wrong position
- fix attackorder for radar dots

Engine:

- fix attacking gaia units
- fix the collision detection for dgun (and other noExplode) projectiles
- Made gunships (and possibly other units too) point their noses at the midPos of their target instead of the pos, the old behaviour could cause trouble with units where midPos and pos differ much.
- added crashDrag tag (float, default 0.005) to fighter/bomber planes, when the plane is crashing it uses this drag instead of the automatically calculated drag it uses when flying. The calculated drag is no longer used when crashing, even if no crashDrag tag is present in the unit fbi
- fix (AirMoveType) aircraft refueling
- fix gameend detected not properly
- gameslowdown triggers at higher CPU usage and is limited to half the userspeed

  0.77b3:
  User Interface

- added a simple Orbit-Camera controller
  META-key + LMB + drag ==> alter elevation, rotation
  META-key + MMB + drag ==> pan up/down/left/right
  META-key + RMB + drag ==> zoom
- fix transports not killing their transportees when they should
- update installer shortcuts
- when kicking a player, kick only the player and don't disconnect everyone
- fix invalid command spam
- fix some divisions through 0
- fix crash for all players when someone gives away all his units

-- 0.77b1, 0.77b2 --------------------------------------

User Interface:

- all commands are prefixed with '/' instead of '.' (".cheat" becomes "/cheat", etc.)
- Added /mapmarks [0|1]
- Added /allmapmarks [0|1]
- Added /forcestart command
- Added /ally teamID [0|1] (Ingame alliances)
- Added /commands (shows all registered commands in the console)
- whisper private messages to any other player ingame (excluding spectators) by typing "/w Playername Your secret message"
- Added new camera controller: SmoothCamera
  behaves like overview controller, but feels much smoother
- Added possibility to specify files that should be excluded from Spring's
  archive checksumming routines ('springignore.txt').
- Added commandline resolution change:
  ./spring -xresolution 800 -yresolution 600
- Added /unitreplyvolume [0.0-1.0]
- Added /hardwarecursor [0|1]
- Added /minimap drawprojectiles [0|1]
  (you can turn off drawing of projectiles on the minimap now)
- Added /grounddecals [0|1]
- Added /movewarnings [0|1]
- Added /globalLOS [0|1] (needs cheats on)
- Added the /luamoduictrl [0|1] command. Enables LuaRules/LuaGaia access
  to many LuaUI callins/functions.
- Added ground rendering detail factors:
  GroundLODScaleReflection
  GroundLODScaleUnitReflection
  (GroundLODScaleRefraction unused atm)
- Added TextureLODBias to settings (global mipmapping bias)
- Added CompressTextures to settings (it compress textures on runtime)
- Added UsePBO to settings (enables PixelBufferObject usage for smf texture loading)
- Added CamModeName as an alternate for CamMode
  (should still be backwards compatible for settings programs)
- Added SPRING_DATADIR variable on linux
- Added option "AtiHacks" to settings. This fixes an issue with severe
  display corruption during building, on ati drivers, where multiple
  colored lines criss-cross the screen. It also fixes issues with the
  new texture mipmap generation.
- You can now cycle through the last 10 message positions with F3.

Spring Engine:

- multithreaded avi-generator
- new water renderer: bumpwater ("/water 4"), pretty and fast
- the usual speedups and performance improvements
- Font rendering system overhaul
- Functionality of Sensors.tdf and Particles.tdf moved to modrules. removed Sensors.tdf or Particles.tdf
  Transfers resources.tdf to resources.lua, and resources_map.tdf to resources_map.lua.
  Sidedata.tdf -> sidedata.lua
  icontypes.lua
  changed SideData 'commander' tag to 'startUnit'
  Replaced 'messages.tdf' with 'messages.lua'
  modinfo.tdf --> modinfo.lua
  ArchiveCacheV6.txt --> ArchiveCacheV7.lua
- Add 'springcontent.sdz' as the last dependency for all primary mod archives
- Custom collision volumes for Units and Features
- you can ally / unally other teams ingame using the "/ally [teamnumber] [0|1]" command (needs to be activated in lobby)
- Made the groundplate decals follow their parent building's facing.
- Allow modnames (e.g. "OTA Content") in dependX tag (modinfo).
- Removed 50% capture progress after an unit got captured.
- Made units transfer their speed to their wrecks.
- submarines can now travel directly underneath surface vessels and floating obstacles
  (and conversely; ships can move across underwater objects), EDIT your mod's MoveInfo
  entries so that the classes used by submarine-style units have "subMarine=1;" set if
  you want to enable this fully
- Increased weapon limit per unit to 32.
- Added the following new tags to modrules.{lua,tdf}
  - repair.energyCostFactor
  - capture.energyCostFactor
  - resurrect.energyCostFactor
  - reclaim.unitEnergyCostFactor
  - reclaim.featureEnergyCostFactor
  - reclaim.unitMethod
  - reclaim.unitEfficiency
  - reclaim.allowEnemies
  - reclaim.allowAllies
- Allow any key binding to a CMDTYPE_ICON command to use the 'queued' keyword
  (except for build and stockpile commands, which already have special handling)
- Keep units in various situations from deciding to attack units they
  shouldn't (noChaseCatagory, neutral, under water without water weapons).

Units & Weapons:

- Added canFireControl unitdef tag.
- Fixed the stop-turn-move-stop-turn behavior for ground units (new udef tag, turnInPlace)
- Added maxRepairSpeed tag for builders. Limits the rate a con swarm of any
  size can repair a single target.
- Made accuracy, sprayangle and similar tags work for starburst missiles.
  The inaccuracy is applied when the missile turns toward its target.
- Added capturable tag.
- Implemented 'y' value for yardmaps which means this square is not blocked.
- Have fighting, patrolling and guarding aircraft and guarding ground units
  follow noChaseCatagory.
- Added per-weapon highTrajectory tag (int, default 2). It acts like the unit
  tag, 0 is always low trajectory, 1 always high and 2 uses the unit's setting
  (whether the unit uses a switch or fixed setting).
- Added impactOnly weapon tag (bool, default false), if set the weapon does damage
  by colliding with units instead of causing an explosion (the CEG will still run)
  to avoid the shortcomings of small AOEs. It's only implemented for projectile weapons
  so far.
- Custom collision volumes:
  - collisionVolumeType (either: "ellipsoid","cylT" where 'T' is one of {X, Y, Z}, "box")
  - collisionVolumeScales (float3)
  - collisionVolumeOffsets (float3)
  - collisionVolumeTest (0: per-frame hit detection, 1: raytraced)
  - all these tags are the same for Features
- Added shieldRechargeDelay weapon tag (float, default 0). The delay is the number
  of seconds the shield won't charge after being hit (kinda like the idle autoheal
  for hitpoints)
- Added 'sonarStealth' unitDef tag.
- Added new stockpileTime weapondef tag. It allows separate reload and stockpile
  times for stockpile weapons.
- Allow for square metal extraction areas (use the new extractSquare unitdef tag).
- Added moveState and fixed fireState unitdef tags.
- Added strafeToAttack tag. Moving sideways when in range but not shooting optional
  behaviour, the behaviour fails if the unit just takes its time for aiming (or
  cannot shoot at all) or has any other reason not to shoot.
- Added avoidNeutral (targeting) and collideNeutral (projectiles) weapon tags.
- Added verticalSpeed (float, default: 3.0) unitDef tag for non-fighter/bomber
  planes to define the speed of takeoff and landing (or loading).
- Made missiles obey burnblow=1 (existing tag), the missile will explode in midair
  instead of turning away or falling down when its flighttime expires if the tag is set.
- Added the 'customParams' table to both featureDefs and weaponDefs
  (accessible via the lua FeatureDefs and WeaponDefs' tables).
- only add ".wav" extension for unitdef sounds if it is missing
- Made MeleeWeapon obey impulseFactor.

Maps:

- Maps have their own resource_map.lua/resource_map.tdf now.
- converted map config scripts to lua format (LuaParser / MapParser)
  - MapOptions.lua is now handed the 'Map' table information as well
    (allows one to write a generic MapOptions.lua file that accesses
    the SMD/SM3 files to retrieve the defaults)
  - the SM3 format is yet to be converted to the lua format
    (but its MapInfo information is)
- added SMF height overrides in the SMD file, example:
  smf = {
  minheight = -100.0f,
  maxheight = 200.0f,
  }
  (note: these values can be changed by MapOptions)
- new water tags (mostly used by the BumpWater renderer):
- shoreWaves (true)
- forceRendering (false)
- ambientFactor (1.0)
- diffuseFactor (1.0)
- specularFactor (1.0)
- specularPower (20.0)
- diffuseColor float3(1.0, 1.0, 1.0)
- specularColor float3(light.groundSunColor)
- fresnelMin (0.2)
- fresnelMax (0.8)
- fresnelPower (4.0)
- reflectionDistortion (1.0)
- blurBase (2.0)
- blurExponent (1.5)
- perlinStartFreq (8.0)
- perlinLacunarity (3.0)
- perlinAmplitude (0.9)
- foamTexture
- normalTexture
- numTiles
- caustics (texture table)

COB:

- Added new COB get constants MIN (131), MAX(132) and ABS(133).
- Added the SONAR_STEALTH (108) COB get/set parameter

LUA changes:

- Added 'teamcolors.lua' to the installers
- Renamed 'gui.lua' to 'luaui.lua'
- Removed LuaCob, and redirected Cob2Lua calls to LuaRules
- removed the following compatibility defines from luaconf.h
  (can break compatibility with old lua scripts):
  LUA_COMPAT_GETN
  LUA_COMPAT_VARARG
  LUA_COMPAT_MOD
  LUA_COMPAT_LSTR
  LUA_COMPAT_GFIND
  LUA_COMPAT_OPENLIB
- Moved most of the LuaUI call-outs into LuaUnsyncedRead
  and LuaUnsyncedCtrl. The LuaUnsyncedCtrl call-outs require
  that the modUICtrl variable be enabled for gadget scripts
  to use them.
- Moved most of the LuaUI call-ins into LuaCallInHandler and
  LuaHandle. The moved call-ins require that the modUICtrl
  variable be enabled for gadget scripts to receive them.
- Updated included gadgetHandler
- Removed unwanted/confusing widgets.
- fonts.lua can load fonts from the VFS
  (and will create LuaUI/Fonts/ if it does not exist)
- Removed the unused CommandDesc 'hotkey' member
- Renamed CommandDescription's 'onlyKey' to 'hidden'
- lua scripts now get first crack at all mouse events
- Treat SDD file sizes as dynamic, so that lua scripts can
  be reloaded while dev'ing without having file size mismatches
  (LuaRules, LuaGaia, mod widgets, etc...)
- Added some new constants to the lua Game{} table.
- Added the dynamic HeightMapTexture
- FLOAT32/LUMINANCE format, NEAREST/NEAREST filtering
- can be accessed by lua scripts using '$heightmap'
- can be disable by clients with 'HeightMapTex=0' (config parameter)
- Added multiple new callins:
  - UnitEnteredWater(unitID, unitDefID, unitTeam)
  - UnitEnteredAir(unitID, unitDefID, unitTeam)
  - UnitLeftWater(unitID, unitDefID, unitTeam)
  - UnitLeftAir(unitID, unitDefID, unitTeam)
  - PlayerChanged(playerID) (player changed team or spectating state)
  - TeamChanged(teamID) (team changed allyteam, not useful yet)
  - UnitCommand(unitID, unitDefID, unitTeam, cmdID, cmdOpts, cmdParams) (note that cmdOpts is a number)
  - UnitMoveFailed()
  - MoveCtrlNotify(unitID, unitDefID, unitTeam, data)
    - currently called with data = 1 if the unit hits the ground and stops
    - return true to disable the unit's MoveCtrl
  - ProjectileCreated(projectileID, projectileOwnerID) (arg #2 is -1 if no owner)
  - ProjectileDestroyed(projectileID)
  - GamePreload() (Called at the end of the loading process)
  - GameStart() (Called when the game is about to start)
  - TerraformComplete(UnitID, UnitDefID, UnitTeam, BuildUnitID, BuildUnitDefID, BuildUnitTeam)
    (it gets called after the terraforming for a construction is complete (once
    per terraforming builder), the return value is whether the con should stop
    building.)
  - DrawGenesis()
    - called immediately following Update()
    - called even when the game window is minimized
    - modelview and projection matrices are identities
    - can be used to update GPU-side physics simulation
      textures and custom per-unit textures
  - a real ViewResize() call-in
- Added the unicode values to the lua KeyPress() and KeyRelease() call-ins
- Replaced Spring.GetMouseMiniMapState() with:
  Spring.GetMiniMapGeometry() ->
  posX, posY, sizeX, sizeY, minimized, maximized, <false | "left" | "right">
- Changed the GetUnitNearestEnemy() lua call-out to:
  GetUnitNearestEnemy(number unitID[, number range[, boolean useLos=true]])
- Added geometry shader support to LuaOpenGL
- Added Spring.ValidUnitID(number id) -> boolean
- Added Spring.ValidFeatureID(number id) -> boolean
- Added Spring.GetUnitIsDead()
- Added Spring.GetModUICtrl()
- Added Spring.GetViewGeometry() -> sizeX, sizeY, posX, posY
- Added Spring.GetWindowGeometry() -> sizeX, sizeY, posX, posY
- Added Spring.GetScreenGeometry() -> sizeX, sizeY, posX, posY
- Added Spring.IsAboveMiniMap(x, y) -> boolean
- Added Spring.GetUnitLastAttacker()
- Added Spring.GetUnitFuel()
- Added Spring.SetUnitFuel()
- Added Spring.GetUnitSensorRadius(unitID, <type>)
- Added Spring.SetUnitSensorRadius(unitID, <type>, radius)
- new projectile call-outs:
  Spring.GetProjectilePosition(projectileID) -> px, py, pz
  Spring.GetProjectileVelocity(projectileID) -> vx, vy, vz
  Spring.SetProjectilePosition(projectileID, px, py, pz) -> nil
  Spring.SetProjectileVelocity(projectileID, vx, vy, vz) -> nil
  Spring.SetProjectileDetonate(projectileID) -> nil
- added the GL evaluator functions to the lua GL interface
  (note the gl.EvalEnable() and gl.EvalDisable() can only be
  called in a gl.Eval(func) block).
- Added Spring.SetUnitCollisionVolumeData(id, xs, ys, zs, xo, yo, zo, vtype, ptype, paxis)
  and Spring.SetFeatureCollisionVolumeData() (same args)
- Added Spring.SetLastMessagePosition(x,y,z)
- Added Spring.SetUnitLosMask(unitID, allyTeamID, number | table) -> nil
  and Spring.SetUnitLosState(unitID, allyTeamID, number | table) -> nil
- Added Spring.FixedAllies()
- Reverted the GetPositionLosState() to its original configuration (no air los)
- Added Spring.IsPosInLos()
- Added Spring.IsPosInRadar()
- Added Spring.IsPosInAirLos()
- Added Spring.PauseSoundStream(), Spring.GetSoundStreamTime(),
  Spring.SetSoundStreamVolume(number)
- Added burst, burstRate and projectiles to Spring.SetUnitWeaponState
- Added Spring.AddUnitIcon(string name, string texture
  [, number size[, number distance[, bool radiusAdjust]]])
  Spring.FreeUnitIcon(string name)
- Added Spring.SetUnitSonarStealth()
- Added Spring.SpawnCEG(tag, px, py, pz, dx, dy, dz, rad, dmg) synced callout
- Added Spring.SetHeightMapFunc(lua_func,arg1,arg2,..),
  Spring.AddHeightMap(x, y, h) -> `final height`
  Spring.SetHeightMap(x, y, h [,t]) -> `absolute value changed`
- Added Spring.ExtractModArchiveFile(path) callout
  For example, Spring.ExtractModArchiveFile("sounds/boom.wav")
  will create a directory "sounds/" and extract "boom.wav" to it.
- Added Spring.GetUnitPiecePosDir(unitID, piece) -> posx,posy,posz, dirx,diry,dirz
- Added Spring.GetUnitVectors(unitID) -> {front_x,front_y,front_z},{up_x,up_y,up_z},{right_x,right_y,right_z}
- Added Spring.GetUnitWeaponVectors(unitID,weaponNum) -> wpos_x,wpos_y,wpos_z, wdir_x,wdir_y,wdir_z
- Added gl.ActiveShader(shaderID, function(), ...) (safe way to set shader uniforms in non-drawing code)
- Added gl.DepthClamp(bool) (not always available)
- Added gl.UnsafeState(number, [bool reverse, ] function, ...)
  (glEnable() / glDisable() for unknown states; considered 'unsafe')
- Added Occlusion Queries:
  - Added gl.CreateQuery() -> query
  - Added gl.DeleteQuery(query)
  - Added gl.RunQuery(query, function, ...)
  - Added gl.GetQuery(query) -> number
- Added the following unsynced query call-outs:
  - Spring.GetUnitLuaDraw()
  - Spring.GetUnitNoDraw()
  - Spring.GetUnitNoMinimap()
  - Spring.GetUnitNoSelect()
- Added the fbo 'readbuffer' parameter
- Added gl.ReadPixels(x, y, w, h [,format])
- Added StencilTests:
  NOTES: The engine expects that the stencil buffer be left in a
  state of all 0's when a task has finished with it. This
  is not enforced by the lua backend.
  - Added gl.StencilTest(bool)
  - Added gl.StencilMask(bits)
  - Added gl.StencilFunc(func, ref, mask)
  - Added gl.StencilOp(fail, zfail, zpass)
  - Added gl.StencilMaskSeparate()
  - Added gl.StencilFuncSeparate()
  - Added gl.StencilOpSeparate()
- Added gl.BlendEquation()
- Added gl.BlendEquationSeparate()
- Added gl.BlendFunc() -- redundant
- Added gl.BlendFuncSeparate()
- Added gl.Blending() convenience names:
  name blend_src blend_dst enabled
  "add" ONE ONE true
  "add_alpha" SRC_ALPHA ONE true
  "alpha" SRC_ALPHA ONE_MINUS_SRC_ALPHA true
  "reset" SRC_ALPHA ONE_MINUS_SRC_ALPHA true
  "disable" SRC_ALPHA ONE_MINUS_SRC_ALPHA false
  "color" SRC_COLOR ONE_MINUS_SRC_COLOR true
  "modulate" DST_COLOR ZERO true
- Added Picking support:
  gl.RenderMode()
  gl.SelectBuffer()
  gl.SelectBufferData()
  gl.InitNames()
  gl.PushName()
  gl.PopName()
  gl.LoadName()
- Added gl.Texture($shading) & gl.Texture($grass)
- Allowed to use gl.CopyToTexture with non GL_TEXTURE2D, i.e. GL_TEXTURE_RECTANGLE
- Modified Spring.GetVisibleUnits() so that the team specifier can
  use the ALL_UNITS, ALLY_UNITS, ENEMY_UNITS, and MY_UNITS values.
- 'facing' parameters can now use long strings: 'north', 'south', 'east', 'west'
- Spring.UnitCreated() call-out now accepts unitDef names and unitDef IDs
- Spring.SendCommands() can use a bunch of string arguments as well as the old
  table format. e.g Spring.SendCommands('echo aoeu', 'info 2', 'specteam 0')
- Fixed Spring.UnitRendering interface to make it possible to emulate the default 3do/s3o shaders
  - fixed $shadows texture params (you can now use shadow2DProj() in glsl)
  - fixed wrong (!=0) check if uniform locations are set (they begin with 0!)
  - added new uniforms: cameraposloc,shadowloc,shadowparamsloc
  - added new culling tag, so you can enable s3o backface culling w/o using a displaylist
  - auto enable alphatest (+blending in case of cloaked stuff) for luaUnitMaterials
    (alphatest: 0.5 for opaque and 0.1 for cloaked stuff)
- Fixed missing return;'s in gl.LoadMatrix() and gl.MulMatrix()
- Fixed WarpMouse() for dual mode (minimap on the left).
- Fixed the 'frame' table value returned by Spring.GetTeamStatsHistory()
  (and added the 'time' value, which is in seconds).
- Fixed full-view spectators get all LuaUI RecvLuaMsg() messages now.
- Fixed Spring.SetFeaturePosition().
- Fixed Spring.SetFeatureDirection().
- If Spring.SetDrawSky(false) is set, do not draw the sun.
- Fixed s3o recognition in LuaUnitDefs

AI:

- Added new ai value AIVAL_SCRIPT returning start script text.
- Removed ABIC AI stuff that no-one is using.
- Added AIHCGetUnitDefById, AIHCGetWeaponDefById, AIHCGetFeatureDefById

Unitsync:

- Added LuaParser API to unitsync.
- Added OpenArchiveType(name, type) to unitsync
  (type can be "zip", "7z", "dir", or "hpi").
- Added 'const char\* GetSideStartUnit(int side)' to unitsync.
- Added InitDirListVFS(path, pattern, modes) and
  InitSubDirsVFS(path, pattern, modes)
- Mod/Map Options : 'section' option type added
- Mod/Map Options : 'style' option added, syntax not defined yet

Bugfixes:

- Fixed Aircraft units that can't land not stopping when reaching the final waypoint.
- Fixed display of times in the EndGameBox (For example 9:07 displayed as 9: 7)
- Fixed the grey flashing that sometimes happened when making new buildings
- Prevent decay when the builder or factory is waiting (executing a wait command).
- Fixed positions occupied by units with a maxspeed of zero (hubs, etc) being considered
  valid build-sites.
- Fixed division by zero if two units are in the exact same spot and collide.
- Fixed negative pings
- Fix a dangling pointer when using ground decals. Could have caused desyncs
  and LuaUI bugs.
- Fixed possible out of bounds map reads that could cause crashes
- Fixed /cheat and /skip not working in demo-playback mode
- Fixed /noSpectatorChat
- Fixed /fpshud polarity
- Randomize unitIDs
- Fixed dds mipmapping filter (was GL_NEAREST_MIPMAP_LINEAR).
- Fixed dynamic water with geforce 8x00
- Fixed it so that units using ScriptMoveType can not be pushed
- Fixed an infinite recursion in the LuaUI layout handler
- Fixed the Lua SendToUnsynced() stack check bug
- Fixed ScriptMoveType vs. unit pushing
- Fixed "cloaked units were invisible underwater" with DynWater
- Blocked team changes for dead units
- Fixed beamlaser damage vs shields
- Fixed smoke textures loading from resource.tdf (resources/graphics/smoke/smoke00 .. smoke11).
- Fixed clipping issue with units begin built.
- Don't show two statistic windows after demo watching.
- Always create/update the perlin texture used by shields and not only if shadows are enabled.
- Fixed ctrl+c, it searches now for the iscommander tag instead for the commander category.
- Fixed a wrong rotation in ApplyTransform() (which is used by lua's gl.UnitPieceMultMatrix).
- Fixed black lines on CEGs when AntiAliasing is enabled.

-- 0.76b1 --------------------------------------

Installer:

- Add new installer SideBanner, Spring icon and ArchiveMover icon all
  contributed by AF.
- Put springcontent.sdz and bitmaps.sdz in upgrading installer.
- Installer greatly improved - all content is downloaded from the internet based
  on which mods and maps you choose to install.
- No more updating installer because updating and full one would be nearly
  identical (in size and functionality).
- Added the following original content mods to the installer (downloads):
  -Gundam
  -Kernel Panic
  -Evolution
  -Spring: 1944
  -Simbase
- Added springsettings.exe by koshi.
- Added ArchiveMover by zizu. Automatically moves .sdz and .sd7 files to the
  correct place (Spring folder\maps\ or Spring folder\mods\)
- Moved to Satirik's lobby, huge amount of changes.
- Moved shaders into springcontent.sdz
- Replacement Bark, bleaf and foam images.

Spring engine varying changes:

- A lot of networking code was refactored.
- Reintroduced heightMod to TryTarget.
- New aircraft command CMD_IDLEMODE, it switches between landing when idle
  and not landing.
- Cache input and output of GetWantedDir to save a lot of expensive calculations
  for a little loss of accuracy.
- Tiny speedups in LosAdd and SafeLosAdd.
- Optimized unnecessary sqrt in GuiTraceRay.
- Made the replay format better:
  - It has a proper header now with version field instead of the entire file
    being just a data blob.
  - The header has made the gameSetup script.txt modifications redundant: the
    UnixTime and Spring Version are in the header, and the gameSetup script is
    stored unmodified now.
  - It stores a game ID, this can be used to match replays of the same game together.
  - It stores the winning ally team, if any.
  - It stores player stats.
  - It stores team stats.
  - It stores game duration, both in game time and wallclock time.
- Small messages are once again send together in one packet.
- Stop animations when network lags (timeout set to 0.5 sec).
- Added some OGGVorbis sound support.
- Added behavior to mobile units so that they will start chasing when the target
  is running away, but before the target exits weapons range, allowing the unit
  to fire continuously at the retreating unit, instead of the cycles of firing,
  then running to catch up that existed previously.
- Added client side network traffic tallying (per packet type).
- Lasers no longer use their full texture on shots that hit before their full length.
- Negative damage now allowed.
- Log the version of some of the major libraries (SDL, GL, GLEW, AL)
- Bump the unit-limit to 10000.

Lobby interface (unitsync, game setup):

- Added new start positions mode Choose Before Game. Useful for AIs, you can
  select the starting position of players from the client.
- Gap removal in GameSetup, players/teams/allyteams can now be given\
  with gaps in the numbering, e.g. PLAYER10 and PLAYER14 in an 1 vs 1.
  Ordering does still matter.
- Unitsync Java bindings now have a version number.
- Checksum is now read from 7z/zip archive header instead of checksumming entire
  file, making things like map list reloading faster by a huge amount.
- Added GetPrimaryModVersion() to unitsync.
- Bumped to ArchiveCache version to 6 ... ArchiveCacheV6.txt
- Applied patch by tc making Spring's configuration variables available through
  unitsync.
- Disabled recursive directory scanning for .sdd and omitted files within
  .../hidden/\* directories from the scan.
- Added the following unitsync calls:
  int GetModValidMapCount()
  const char\* GetModValidMapCount(int index)
  - Valid maps are listed in the mod's "ValidMaps.lua" file.
  - A valid map count of 0 means that any map can be used with the mod.
  - This can be used in concert with LuaRules to setup multi-player scenarios
    and tutorials.

AI interface:

- Exposed unit->maxSpeed to the AI interface.
- Added CallLuaRules() to CAICallback.
- Added 'AIDLL=LuaAI:' -> CTeam::luaAI handling for LuaAI.
- Added AI unloading after team is destroyed.
- Allow GlobalAIs to send a startposition.

User Interface:

- Added the CTRL modifier to area reclaims to allow reclaiming your own wrecks
- Removed some hard-coded key bindings and replaced them with:
  edit_return
  edit_escape
  edit_complete
  edit_backspace
  edit_delete
  edit_home
  edit_end
  edit_prev_line
  edit_next_line
  edit_prev_char
  edit_next_char
  edit_prev_word
  edit_next_word
- Added some reasonable default keybindings for all/ally/spec chat messaging:
  enter -> send to <last send type>
  Ctrl+enter -> send to All
  Alt+enter -> send to Allies
  Shift+enter -> send to Spectators
  (Note that the modifier versions can be used to change the send mode ever
  while typing the message.)
- Added the CTRL modifier to area reclaims for reclaiming all features.
- /specfullview 3 is the new default for spectating and watching demos.
  (can view and select all units)
- Added the ctrlpanel.txt "frontByEnds" option. (let's you use the endpoints to
  define a front instead of the midpoint and one of the ends.)
- When a factory is set on repeat, alt+build-click inserts the order as a
  no-repeat and right after the currently built unit. Behavior is unchanged
  if repeat is off.
- Added water lines while placing unit builds.
- Moved the clock and fps indicators to the upper right hand corner.
  (the default setup results in overlaps with the player info text.)
- Modified the LOS view to better show the los-only and radar-only conditions
  dark = no los, no radar
  white = los + radar
  blue = los-only
  green = radar-only
  red = jammed area
- Disabled shadow generation when using LOS viewmode. (would be better to add
  shadows in LOS mode, but might as well avoid the shadow generation overhead
  until the shadows are added.)
- Make Fight be able to be given a line command, like move already can be.
- Only add the SelectAI button if the current selection has valid GroupAIs.
- Change default keybinding for createvideo to Ctrl+Shift+F10 (was Any+F10),
  so it is less easy to accidentally hit it.
- Change savegame default keybinding from Any+F8 to Ctrl+Shift+F8,
  for the same reason as the createvideo keybind change.
- Improved view centering for the Free Camera mode.
- Made nano-particles unsynced. (and respect maxParticles)
- Added '/cross [size]'.
- Added .godmode [0|1] -- allows all players (even specs) to control all units.
- Added a team parameter to .give: .give [amount] <unitName> [team] @x,y,z
- .kick and .kickbynum messages are filtered.
- Changed '/grabinput' to '/grabinput [0|1]', and added state reporting.
- Added the "/grounddecals [0|1]" command.

Modinfo / modrules tags:

- Allow for the modder to define whether the crashing planes (and/or units
  running long Killed() scripts) can be fired on via modrules.tdf new section:
  [FIREATDEAD]
  {
  fireAtKilled=0;
  fireAtCrashing=0;
  }
  The defaults are to disallow both (Spring 0.75 behaviour, both variables set
  to 0). Spring 0.74 behaviour (allow both) can be emulated by setting both
  variables to 1.
- Added modinfo.tdf 'version', 'mutator', and 'shortName' tags.
- Added the following to modrules.tdf (default values shown):
  [DISTANCE] {
  builderUse2D=0;
  }
  [EXPERIENCE] {
  powerScale=1.0;
  healthScale=0.7;
  reloadScale=0.4;
  }
- Adds [experience] { experienceMult = 1.0; } to modrules.
- Adds [flankingBonus] { defaultMode = 1; } to modrules.
- Replaces the old bonus shield system with flanking bonus

Unit / feature definition tags:

- The TDF tags MetalStorage and EnergyStorage now override the StartMetal and
  StartEnergy specified in the startscript, instead of the other way round. If
  MetalStorage or EnergyStorage isn't specified the default values for commander
  units are StartMetal and StartEnergy. For other units the default remains 0.
- Moved the buildOptions definitions into the unitDef definitions, LuaParser
  handles backwards compatibility.
- Changed init_cloaked to initcloaked.
- Add a tag 'pushResistant' to UnitDef. If set to true, a unit will not be
  pushed out of the way by another when their footprints overlap, nor will
  it respond to BuggerOff directives when the other party has an important
  move order. Defaults to false.
- Added a per-soundfile volume control format for unitdefs
  sounds = {
  ok = 'oksound',
  cancel = {
  'cancel1',
  'cancel2',
  },
  select = {
  { file = 'selsound1', volume = 2.0 },
  { file = 'selsound2', volume = 2.5 },
  etc...
  },
  }
- Features have new tag "reclaimTime" defining how long they take to reclaim,
  default is metal+energy.
- Added tags:
  transportUnloadMethod = <integer>; // 0 - land unload, 1 - flyover drop, 2 - land flood
  fallSpeed = <float>
  unitFallSpeed = <float>
  These dictate the speed of units being dropped from the transport.
  fallSpeed is used on the transports FBI file, to dictate the speed of all
  units it drops, unitFallSpeed is used on each transported unit to override
  fallSpeed.
- Added flighttime functionality to cannons.
- Added flighttime to bombdroppers.
- Added FBI tag cantBeTransported=0|1, if set to 1 no transport can load this unit.
- Added FBI tag minTransportSize=int, determines the smallest size footprint
  that the transport can load
- Added FBI tag minTransportMass=float, determines the smallest mass unit that
  the transport can load.
- Removed the check for Air movetype for holdSteady FBI tag, so now ground
  transports can use it too.
- New FBI tag for transports; float unloadSpread: Controls the spacing between
  units in an area unload command, works as a multiple, default is 1.
- Added the 'decloakSpherical' unitDef tag (defaults to true).
- Added optional 3D ranging for all builder operations.
  (build, repair, reclaim, etc...) (per-unitdef buildRange3D=true)
- New unitDef parameters:
  int flankingBonusMode
  float flankingBonusMax
  float flankingBonusMin
  float flankingBonusMobilityAdd
  float3 flankingBonusDir
- Added the "canSelfDestruct = true" unitDef tag.
- Enable CEGs to be tied to pieces of exploding units
  (CEG replaces the default smoke if used, but not the miniscule fire-effect)
- Added CEG opcodes y, x, a, p and q. Except for p those operate on a set of 16
  buffers that store a float value each.
  y# - Yank: Stores the running value in the buffer # and returns 0.
  x# - Multiply: Multiplies the value with the contents of buffer #.
  a# - Add: Adds the contents of buffer #.
  p# - Pow: Returns the #th power of the value.
  q# - Buffer Pow: Like Pow but uses the contents of buffer # as the exponent.
- Add a tag "factoryHeadingTakeoff" (on request of the SWS folks to prevent some
  clipping issues) which controls the takeoff behavior of TAAirMoveType aircraft.
  True (default) means planes start matching their factory's heading right after
  taking off from the pad, false means they maintain the heading of the pad
  until reaching wantedHeight.
- Added [CONSTRUCTION] section to Modruless.tdf:
  constructionDecay (bool, default true): Should constructions decay if noone is working on them?
  constructionDecayTime (float, default 6.66): How long until the decay starts?
  constructionDecaySpeed (float, default 0.03): The speed of the decay,
  whereby buildTime\*decaySpeed = time for a total decay so higher = slower decay
- Added unit FBI collide tag (boolean, default: 1), currently affects only planes:
  If set to 0 the plane will not collide with any other unit. Most notably this
  saves the CPU-heavy collision checks every frame (big performance boost!) and
  avoids the damage planes normally take from bumping into each other.
  Projectiles will still collide with the units normally.

Weapon definition tags:

- Added heightBoostFactor - a float tag that gives control of cannon height
  range boost (more than 1 means increased range, 0 means the cannon has fixed
  range regardless of height difference to target. default is a magic value
  calculated on weapon init, based on range tag and theoretical max range.)
- Added heightMod tag (height difference is multiplied by this value before
  checking range), default 0.8 for Cannon weapon type, 0.2 for everything else
  (i.e. stays as it was). Beamlasers have heightmod=1 by default so they use
  pure spherical targeting.
- Added GroundBounce, WaterBounce, NumBounce, BounceSlip, BounceRebound, and
  bounceExplosionGenerator tags.
- New tags beamttl (default: 0) and beamdecay (default: 1) for the beamlaser.
  Beamttl defines the time (in frames) the beam will stay after being fired and
  beamdecay defines the speed of the fadeout (each frame the beam's intensity is
  multiplied with that value). This applies to the single-frame beams the weapon
  emits, if it has a beamtime it will emit one such beam every frame, which is
  only useful if the source of the beam moves a lot (gives a cool trail effect).
  It's primarily intended for beams with a beamtime of one frame (which often
  simulate very fast shots instead of constant streams, e.g. railguns) so they
  don't disappear immediately.
- Added fixedLauncher tag (bool) for missiles, starburst missiles and torpedoes,
  this makes the projectile spawn with the orientation of the shooting piece
  instead of their normal orientation. The weapon will not properly check if
  allies are in the way so make sure you align the launchers in a way that
  won't hammer right into your own forces and perhaps use collideFriendly=0.
  FixedLauncher conflicts with trajectoryHeight and IMO combining them is
  pointless anyway so don't use both on a weapon.
- Added projectiles tag (int), each time a weapon fires this many projectiles
  are spawned (on the same frame). Make sure you put them on different
  trajectories somehow (sprayangle or different firepoints) because otherwise
  they'll all be clumped in one shot. Can be set to 0 as well for script trigger
  weapons and can be combined with burst.
- Missiles and starburst missiles now obey smoketrail, set it to 0 to disable
  the smoke. To get a missile without smoketrail you have to define the weapon
  as a missile with WeaponType=MissileLauncher in the tdf.
- Missiles, starburst missiles and torpedoes now use flighttime if present
  instead of the hardcoded formula (which is still used if flighttime is not
  present)
- Added SubMissile tag (bool) for TorpedoLaunchers, if set to 1 the torpedo will
  travel like a missile outside of water. It will not emit a smoketrail, though.
  When fired from a plane it will behave the same way but the plane will
  probably be considered a fighter, not a bomber.
- Added fireSubmersed tag (bool), defaults to the value of waterweapon. If set
  the weapon can fire underwater, if not then not. Use it for weapons that
  cannot fire underwater but can hit underwater targets. Works even for
  torpedoes.
- Starburst missiles now obey turnrate for the turn at the peak of their flight
  (if supplied, otherwise the old value is used).
- All weapons (save for the Rifle which noone uses anyway) can now attack
  underwater targets if they have waterweapon=1 set.
- Bombs can use manually defined burst and burstrate values instead of
  autogenerated ones by using the new manualBombSettings tag (bool). Keep in
  mind that (roughly) burst\*burstrate defines the area in which the bomber is
  willing to drop the bomb so if you make it too small the bomber might miss
  the drop area and not drop any bomb.
- Added myGravity tag (float) for ballistic weapons (Cannon and Bomb), it
  overrides the map gravity if used. Regular map gravity is around 0.2.
- Bombs now obey Accuracy and SprayAngle.
- Added ShieldStartingPower tag (float) for shields, if set the shield starts
  with this much power instead of 0.
- Renamed flareEfficency -> flareEfficiency.
- Added the weaponDef 'cameraShake' tag.
- Added the dance tag for missilelaunchers that makes the missile move around
  randomly. Unlike wobble it doesn't rotate the missile and it prevents the
  missile from going too far off course so a dancing swarm will remain coherent.
- Beamlasers ignore beamtime if new tag "beamburst" is set. (for emulating
  weapons that do damage in bursts)
- New weapon FBI tags:
  WaterBounce and GroundBounce: (default: false)
  Boolean to allow a projectile to bounce off of a surface.
  BounceSlip: (default: 1)
  Amount of lateral speed kept by the projectile after it bounces.
  BounceRebound: (default: 1)
  Amount of vertical speed keeps and reverses.
  NumBounce: (default: -1)
  The number of bounces a weapon will make. -1 indicates no limit.
- Added the ability to bounce for Torpedoes, Missiles, Lasers, Cannons, and Emgs.
- Added bounceExplosionGenerator tag, which defines an explosion generator to
  run when a projectile bounces.
- Added the tag "hardStop" (bool, default 0) which, if used, makes the laser not
  fade if it doesn't hit before hitting its range limit and instead has the
  laser stop immediately and contract, as if it had hit something. This looks
  better with really long lasers, especially those that are longer than their range.
- Weapon projectiles can now spawn a CEG at their position every frame using
  the cegTag weapon tag.
- ProximityPriority tag - acts as a multiplier for the distance to the target
  in the priority calculation. Note that negative values make weapons prefer
  distant targets.
- Adds three opcodes to the CEG definitions:
- m (modulo or sawtooth wave) returns the remainder of the running value
  divided by m's parameter (100 m24 returns 4),
- s (sine) returns the sine of the running value with the parameter acting as
  the amplitude,
- k (discrete) returns the running value floored to an integral multiple of
  the parameter.
- All weapon projectiles that have a TTL now supply their remaining ttl-value as
  the damage param to their trail CEG (Flame projectiles use curTime instead).
- Added alwaysVisible weapon tag, makes the projectile visible, even without LOS.
- Added the weapon tag leadLimit that limits the maximum distance a unit will
  lead a target (default is -1 which means infinite).
- Added the weapon tag leadbonus which is multiplied with the unit's experience
  and added to the lead limit.
- Added the weapon tag predictBoost (default 0 for non-burnblow, 0.5 for
  burnblow weapons).

Bugfixes:

- Fixed cannons not able to shoot as far from high ground as they should.
- Fixed infinite lower cylinder when cylindertargetting is active.
- Loaded air transports no longer land on stop command .
- Fixes in glBallisticCircle. (use heightmod also in drawing range circles.)
- Spring now returns 0 when started with -V, -h or -p.
- Fix random start positions. (positions are handed out before gaia team is added)
- CMD_REMOVE also works for idle factories (with move/patrol queues)
- Changed Cannon::GetRange2D so it uses something closer to real-world physics.
- Fixed Melee weapon always aiming west.
- Do not allow commands with out-of-map destinations.
- "Connecting to server" message now does not pop up when starting without startscript.
- Commandline is now parsed before the filesystemhandler starts up (speedup
  starting with arguments like -V or -h).
- Fixed builders with area reclaim sometimes going out of their designated circle
- Bombs now obey Accuracy and SprayAngle.
- Fixed lightning weapons having a hardcoded inaccuracy, made it obey accuracy
  and sprayangle tag instead.
- Fixed Lightning weapons not doing damage to shields.
- Possible fix for the disappearing healthbars.
- Fixed crashbug when exiting the GroupAI, sharing and quit boxes in an unusual way.
- Fixed starburst missiles so they stop their engines after they travel out of range.
- Fixed a possible load/save bug.
- Fixed crash in DamageArray() copying constructor.
- Don't say 'content error' if modrules.tdf isn't found.
- Improved error reporting in GetTDFName and LoadMap.
- Fixed replays.
- Fixed savefiles on non-Windows OSes.
- Fixed the run-time Spring.MakeFont() call-out.
- Fixed beamlasers so they don't ignore CollideFriendly.
- Fixed both CollideFeature and CollideFriendly for weapontype=Rifle.
- Fixed unloading of aircraft.
- Fixed crash when sharing units with a GroupAI other player doesn't have.
- Fixed .team, .setminspeed and .setmaxspeed when playing a demo locally.
- Fix groundscars turning white after using LOS overlays or enabling shadows.
- Better error reporting in case of global AI version mismatch.
- Fix SM3 when resizing windows.
- Don't profile the water renderer on voidwater maps.
- Weapons now use AimFromWeapon to determine the part to aim from, this prevents
  waggling and failing to aim at close ranges. To simulate the old behaviour
  make AimFromWeapon return the same piece as QueryWeapon does.
- Melee weapon now reports a proper angle to AimWeapon.
- Made starburst missiles obey startvelocity and weaponacceleration.
- Try to minimize damage caused by LuaUI crashes until cause is found by
  disabling LuaUI after several errors happen.
- Hackish fix for units sometimes not going to factory designated rally point
- Install the game/fonts/\*.ttf files to fonts/.
- Handle a missing argument to .skip a little better.
- Fix error message for missing font.
- Take screenshot of entire window (rather than left half only) in dual-screen mode.
- Lua stack was changed to fix some segfaults.
- Prohibit spectators (except for the host) from changing the gamespeed unless
  cheating is enabled.
- Fix an exploit where partially reclaimed corpses that are turned into wreckage
  end up being worth more than the remaining value of the corpse itself.
- Fix weaponAcceleration typo causing major bugs with nukes and vlaunch weapons et al.
- Fix two "unknown net msg" errors in CPreGame when playing certain demos.
- Fixed a crash bug (lastColWarning == NULL) in TAAirMoveType.
- Fixed a crash in AirMoveType.cpp.
- Removed the automatic 'userInputPrefix' tracking for incoming messages
- Fixed the unitsync GetUnitFullName().
- Ignore used writing keypresses (for bindings like: 'bind a+u edit_next_word')
- fixes a bug in LuaUnsyncedRead::TraceScreenRay() when using dualscreen mode
  and minimap is on left.
- Unit pieces inherit alpha threshold from owner.
- Fixed jittery falling units.
- Fix timeout while loading / pathing
- CommandFallback was broken wrt return values, fixed.
- Improved dgun responsiveness. (still not perfect, but at least shoots through
  features now)
- Construct a proper orthonormal basis for muzzle-flame bitmaps.
- Gracefully handle missing script failure.
- Fixed speed and pause in replay.
- Fix for teleporting units when area-unloading out of map.
- Fix for uadded tracking of PushMatrix and PopMatrix Lua call-ins to reduce
  number of gl errors.
- Fix ghosted buildings always being drawn with default facing.
- Fixed weapon TDF 'fireStarter' tag, for all weapon types.
- Units, when they are within range, but unable to hit the target of an attack
  command, will move sideways.
- Fixed the bug where laserprojectiles had weird garbage on the front if the
  projectile was within lod distance.
- Fixed set HEADING COB variable so it updates unit direction.
- Fixed giving commands while recording a video.
- Fixed bug where after load all moving ground units stops with message
  "Can't reach destination".
- Fix for patrolling builders repairing stuff that's being reclaimed
- In the tooltip displayed on mouseover, deconstructing nanoframes didn't show
  how much metal they are producing on mouseover.
- Don't set the default shareTeam to a dead team ever.
- Fixed random mod.
- Made glPrintOutlined() almost 50% faster by lowering quality.
- Reclaimers list is now saved.
- Now error messages are broadcast to all clients.
- Paralyzers no longer damage features.
- Fixed the wait/teleport factory bug.
- Fix flying/underground units bug.
- Unhide the gaia team, and colorize by allegiance.
- Made demo not found / demo corrupt / format outdated exceptions instead of logOutput-warnings
- /quit will now quit immediately if you are the only active player.
- Fixed a 'bad team' segv in Spring.CreateUnit().
- Added the ScopedMapLoader helper to unitsync.
- Paralyzers no longer shake the camera.
- Fixed the missile and starburst texture2 only applying to the first segment of
  the smoketrail (the segment that's still part of the projectile, not a
  CSmokeTrailProjectile). Now the entire trail uses the texture.
- Fix possible segmentation faults after loading a shared object failed.
- Fixed bug where server dont send remaining data (including NETMSG_QUIT) to server
- Set ACTIVATION issues CMD_ONOFF instead of activating/deactivating, fixing
  issues with on/off not showing correct state.
- Added a check for weapon 'slavedTo' values (content_error).
- Fixed some bugs in gl.TexEnv / gl.MultiTexEnv / gl.TexGen / gl.MultiTexGen
- Used the decoyDef ghost building models when appropriate.
- Fixed CAirMoveType planes not rotating on factory pads.
- Fixed CTAAirMoveType planes always taking off from labs facing south.
- Fixed missile-gunships from being unable to force-attack ground.
- Fixed CTRL-MOVE group orders causing units to move at snail's pace.
- Fixed mid-air explosions that occur over water triggering a water splash effect.
- Fixed the isAttackCapable() check.
- Prevent CMissileProjectile weapons with trajectoryHeight from over- or
  undershooting when firing at targets below or above attack position.
  Based the angular correction factor on missile speed.
- Not being able to lock the config file on linux is not a fatal error anymore.
- Remove the (bestDist < radius) bugs.
- Make sure the (paralyzeDamageTime >= 0).
- Added bestDist method for all area searches.
- Added a check for immobile builders that will stop internal.
  commands for when the unit has moved out of the build range.
- Fixed FlareProjectile. (flare projectile was always removed after one Update.)
- Allow for "non-decloakable" units.
- CGameHelper::GetClosestEnemyUnitNoLosTest() now includes the target's radius
  for spherical tests.
- Stunned factories don't open, build, or close.
- Players from demo once again can't change speed / pause.
- Factored out some glGetDoublev() calls (may be faster on certain GPU architectures)
- Fixed issue that some people can only get sound when hosting.
- Fixed bug in map damage loading from savegame.
- Fixed possible bug in explosions map damage.
- Fixed crash with saving after nuke was intercepted.
- Fixed ground-transports being unable to unload units on certain maps.
- Units that are still inside a non-firebase transport do not destun.
- Fixed units whose state is floating but are not in water taking water-damage.
- Print "spectator <name> left" rather than "player <name> left" if the guy
  leaving is a spectator.
- Added serializing for InMapDraw.
- Fixed bug with saving builders that terraform ground.
- Fixed stunned units being unhittable if pushed.
- Made a separate update thread while the game loads to prevent network timeouts
  on slow computers. (thread is joined when loading finished.)
- CCobInstance::RealCall now checks if stack is big enough before getting
  arguments from stack.
- made StartBuilding measure the pitch from the center of the unit instead of
  the base to deal with units with large vertical offset.
- Put CMinimap always on back so it does not draw over the user interface.
- Moved the QueryNanoPiece COB call into synced code to hopefully fix a desync.
- Fixed the bug where a repair order on a construction whose terraforming is
  incomplete will start construction anyway.
- Fixed units terraforming the ground under units being built by factories.
- Added some protection against .wav's whose headers contain a garbage datalen.
  (was causing SEGV's with XTA 9.2)
- Change lots of printf calls to logOutput.Print().
- Don't send NETMSG_GAMEOVER when watching a demo with only 1 allyteam left.
- Throw content_error if a mod/map archive can not be found instead of only
  printing a warning in the log. This should stop the 'missing default cursor'
  message actually meaning 'missing a mod archive'.
- Added minimap ping on game start and set lastMsgPos to something meaningful.
  Notably this fixes F3 zooming to top left corner before something interesting
  happens.
- Fixed crash when singlestep was executed in client mode.
- Spring and ArchiveMover.exe requires administrative privileges now on Windows,
  as a workaround until user directories can be specified as data directories.
- "Can't reach destination" message spam should be reduced a bit.

Lua:

- Added call-in gl.Smoothing(point, line, polygon).
- Added Spring.GetViewSizes() to LuaOpenGL.
- Added the SetLosViewColors() call-in.
- Added the StockpileChanged() call-in.
- Added the DefaultCommand() call-in.
- Added the LuaUI WorldTooltip() call-in.
- Added the LuaUI GameSetup() call-in.
- Added GetUnitHeight() call-out.
- Added GetFeatureHeight() call-out.
- Added GetFeatureRadius() call-out.
- Added GetFeatureResurrect() call-out.
- Added SetTeamColor() call-out.
- Added GetTeamColor() call-out.
- Added GetTeamOrigColor() call-out.
- Added the Spring.SetDrawSky() call-out.
- Added the Spring.SetDrawWater() call-out.
- Added the Spring.SetDrawGroud() call-out.
- Added the gl.UnitMultMatrix() call-out.
- Added the GetLuaAIDesc() call.
- Added the Spring.AddUnitDamage(
  number unitID,
  number damage,
  number paralyzeTime,
  number attackerID,
  number weaponID,
  number impulseX, number impulseXY, number impulseZ) lua call-out.
- Added the gl.UnitPieceMultMatrix() lua call-out.
- Added the gl.EdgeFlag() lua call-out.
- Added the Spring.GetCameraFOV() lua call-out.
- Added the Spring.GetCameraDirection() lua call-out.
- Added unit count protection to Spring.CreateUnit().
- Added the DrawUnit() LuaRules unsynced call-in. (for SetUnitLuaDraw())
- Added the Spring.SetUnitLuaDraw() LuaRules call-out.
- Added the GameFrame() call-in to LuaUI.
- Added the AICallIn() LuaRules unsynced call-in. (for CallLuaRules())
- Added the Spring.GetTeamLuaAI() lua call-out.
- Added LuaUI Spring.GetGatherMode().
- Added LuaUI Spring.GetInvertQueueKey().
- Added Spring.SetUnitLineage(unitID, teamID[, bool isRoot]).
- Added Spring.GetBuildFacing() for LuaUI.
- Added Spring.GetBuildSpacing() for LuaUI.
- Removed the color info from GetTeamInfo().
- Added GetHasLag(), returns true if currently lagging behind host.
- Added PlaySoundStream and StopSoundStream for OggVorbis audio.
- Added the rank parameter to GetPlayerInfo(). (arg 7)
- Added 'autoland' to GetUnitStates().
- Added the UnitCmdDone() call-in.
- Added the UnitCloaked() call-in.
- Added the UnitDecloaked() call-in.
- Adedd the MapDrawCmd() call-in. (only for LuaUI)
- Added the unsynced Spring.GetVisibleUnits([teamID] [,radius] [,useIcons]) call-out
- Added the following to both UnitDefs[] and FeatureDefs[]
  height, radius
  minx, midx, maxx
  miny, midy, maxy
  minz, midz, maxz
- Disabled the lua DrawScreen() call-in when the interface is hidden. (note that
  the DrawScreenEffects() call-in remains active.)
- The majority of mod files are now loaded via LuaParsers, which can be edited
  to load different types of files. The default LuaParsers simply load the
  normal style files with improved error checking and speed.
- Game.armorTypes[] was changed to a bidirectional map.
- WeaponDefs[x].damages.damages[] was replaced with WeaponDefs[x].damages[index]
  (starts at 0 for the default armorType)
- Added the 'teamcolors.lua' local team recoloring LuaParser script.
- Added the gui_game_setup.lua GameSetup widget.
- Added 'ghostedBuildings' to Game[].
- Changed Game.modName to use the humanName value.
- UnitDef names are always lower case now.
- Added on-the-fly lua font texture / specfile generation.
- Added localization support in the lua tooltips and ctrlpanel.
- Generated local speed-ups for some of the installed widgets.
- Created FeatureDefNames and WeaponDefNames for LuaUI.
- Added Spring.SetUnitNeutral(unitID, bool) and Spring.GetUnitNeutral(unitID).
- Neutral units (mainly for gaia and fake features), will not be attacked.
  automatic when the attacker to be has its fireState set to "Fire At Will".
- Added a hidden fourth fireState level (3) that will shoot at neutrals.
- Added SetCustomCommandDrawData.
- Added 'extractorRadius' to the LUA Game table.
- Added Spring.IsGodModeEnabled() to LuaSyncedRead.
- Added the "gamedata/LockLuaUI.txt" check for LuaUI. If this file exists in
  the mod, then LuaUI can only load its gui.lua" file from the archive
  filesystem, thus putting the mod in full control of what LuaUI can be used
  to do (including disabling all user widgets).
- LuaAI is now supported.
- Added custom mod / map options via Lua:
- MapOptions.lua is to be used with LuaGaia, and belongs in the map archive,
- ModOptions.lua is to be used with LuaRules, and belongs in the mod archive.
- Renamed cmdDesc iconname -> texture for LuaUI LayoutButtons() input.
- Renamed cmdDesc mouseicon -> cursor for LuaUI LayoutButtons() input.
- Changed the cmdDesc double texture format to:
  '&<xscale>x<yscale>&<tex1>&<tex2>'
  (both textures can use either the named textures or '#<unitDefID>' format)
- - Added the Spring.RequestPath() lua call-out
- RequestPath(<number moveID | string moveName>,
  startX, startY, startZ, endX, endY, endZ [, goalRadius]) -> nil | path
- it returns a lua userdata or nil
- the userdata has the following calls:
  path:Next([currX, currY, currZ[, minDist]]) -> nil | x, y, z
  path:GetEstimatedPath() ->
  {[1] = {x,y,z}, [2] = {x,y,z}, etc... }, -- waypoints
  {[1] = index, [2] = index, etc... } -- resolution start points
- Added Spring.GetUnitEstimatedPath() -- same return as path:GetEstimatedPath()
- Added GetEstimatedPoints() to CPathManager
- Added the NETMSG_LUAMSG net packet type
- Added the following calls to LuaUI:
  Spring.SendLuaUIMsg(msg [,mode = ''|'a'|'allies'|'s'|'specs'])
  Spring.SendLuaCobMsg(msg)
  Spring.SendLuaGaiaMsg(msg)
  Spring.SendLuaRulesMsg(msg)
- Added the RecvLuaMsg(msg, playerID) call-in to all lua scripts
- Added the weaponID to the lua UnitDamaged() for fullRead scripts
- Removed imageSizeX and imageSizeY (potential desync). The gl.TextureInfo() call can be used instead
- Made decloakDistance a per-unit property, and made it accessible to
  synced lua scripts as the third parameter to SetUnitCloak().
- Added GetModOptions() and GetMapOptions()
- Added UnitSeismicPing() (with a sound)
- Added the ability to set the stockpile percent
- Added the Spring.SetUnitTarget(unitID, targetID | <x,y,z>, bool userTarget) call-out
- Added the Spring.SetExperienceGrade(number) call-out (sets the experience
  steps that will trigger the UnitExperience() call-in, based on the
  limExperience variable value (exp / (exp + 1)))
- Added the UnitExperience() lua call-in
- Added VFS.UseArchive(filename, function(...) ... end, ...)
- allows for packaged widgets (source, textures, sounds, etc...)
- currently only works for raw filesystem access (limited to LuaUI)
- Added the following lua call-outs to allow unsynced read access to COB global variables:
  GetCOBUnitVar(unitID, varID)
  GetCOBTeamVar(teamID, varID)
  GetCOBAllyTeamVar(allyTeamID, varID)
  GetCOBGlobalVar(varID)
  (varIDs start at 0)
- Added FBO objects to lua GL, with MRT (multiple render targets) capability, and blitting
- Added RBO objects to lua GL, which should allow for depth + stencil format
- Added lua Scream objects, which can call a function when they are garbage collected
- Added VFS.SubDirs(), to get a list of subdirectories
- Spring.SendMessage() and friends can now be used by unsynced lua code
- added Lua SyncedCtrl function Spring.SetUnitMoveGoal(unitID, posX, posY, posZ[, radius[, speed]]),
  this makes the unit move closer to the target location, to a distance of <radius> (default 0).
  This is mostly for custom commands that should have the unit moving into range,
  it doesn't affect the order queue. It's marked as unsafe.
- New Lua call-out (SyncedCtrl): Spring.SetUnitWeaponState(unitID, weaponNum, stateTable)
  StateTable is a table containing one or more of reloadstate, reloadtime (this time in seconds),
  accuracy, sprayangle, range, projectilespeed. Obviously the values will be assigned to the
  variables the keys refer to.
- Added the Spring.GetMapList() call-out to the mod ValidMaps.lua script
- Added the Spring.GetMapInfo() call-out for the ValidMaps.lua script
- Added mapSizeX and mapSizeZ to the lua Game table
- Fix GetCmdDescIndex()
- Added a seventh parameter to AllowCommand, "synced". It's a bool that is true
  when the command was sent by either a Spring-internal action or synced Lua and
  it's false if the command came from the user or LuaUI. This should facilitate
  preventing user commands to a unit while allowing LuaRules commands.
- Removed the third return value of Spring.GetTeamInfo (active) since it was broken.

COB

- Give a COB error instead of crashing for invalid explosion generator indices
  or weapon indices in emit-sfx. #
- Added COB call to "ShotX" that happens shortly before weaponX uses
  QueryWeaponX, use this for switching firepoints in bursts or when using
  multiple projectiles.
- StartBuilding now supplies the pitch as the second parameter to help with
  custom build FX.
- Fixed cob bitwise not operator (was logical not.)
- Added a MOD COB opcode for modulo division
- Introduces the COB get/set variable 103, ALPHA_THRESHOLD. It ranges from 0-255
  and changes the glAlphaFunc to use that alpha value as its minimum. In
  practice this allows disintegration-style effects or the reverse for build animations.
- Transport AI now calls the cob function 'StartUnload' which is called once
  when transport starts to lower during land flood method.
- Dropped units call the cob functions 'falling' and 'landed' so that you may
  start and stop a parachute animation.
- Added the 'cobID' unitDef tag, and the COB_ID (100) COB GET type
- Added the BlockShot#(unitID, block) COB call-in - set block to non-zero to
  block the unit firing at unitID
- Added the cob GET/SET STEALTH command (96)
- Added CRASHING (97) get/set constant to COB. Can be used to get/set whether an
  aircraft is crashing. Is ignored for non-CAirMoveType units.
  (including CTAAirMoveType aircraft)
- Added global, team, and ally GET/SET variables to COB scripts
  1024 - 2047: 1024 global variables
  2048 - 2175: 128 variables per team (the COB script unit's team)
  2176 - 2303: 128 variables per allyteam (the COB script unit's allyteam)
- Added 8 shared per-unit variables (example: GET UNIT_VAR_START -unitID value)
- Added the SET_WEAPON_UNIT_TARGET (106) and SET_WEAPON_GROUND_TARGET (107)
  COB GET codes (used GET because of its extra parameters)
  - GET SET_WEAPON_UNIT_TARGET weaponID unitID userTarget
  - GET SET_WEAPON_GROUND_TARGET weaponID xzCoord yCoord userTarget
- Added TargetWeightX COB call-in and CHANGE_TARGET cob set var.
  COB call-in TargetWeightX(targetID, weight), weight defaults to 65536 which
  represents 1.0 and is multiplied with the target's value to determine priorities.
  Set CHANGE_TARGET to Y makes weaponY pick a new target.
- Adds the COB set constant CEG_DAMAGE (99), which defines the damage value for
  all CEGs triggered by emit-sfx 1024 + x to allow for some variability in the
  emitted effects (to e.g. make a charging energy ball grow larger or have smoke
  get progressively darker as the unit's HP decreases).
- Added a new get call, PLAY_SOUND, to play a sound that can be heard only by
  allies, enemy, in los, etc. GET PLAY_SOUND(sound number, volume,
  who hears the sound, where to play the sound) (see r4974 for full docs)
- Get health now allows a unit ID to be passed.
- Added COB and Lua access to some more weapon variables.
  New COB constants, all get-only (but not read-only):
  WEAPON_RELOADSTATE 125 (frame number of the next shot/burst)
  WEAPON_RELOADTIME 126 (in frames)
  WEAPON_ACCURACY 127
  WEAPON_SPRAY 128
  WEAPON_RANGE 129
  WEAPON_PROJECTILESPEED 130
  All of these use the first param (starting at 1) as the weapon index whereby positive
  means read and negative means write the second param to the value. In the latter case
  the previous value is returned.
- New COB GET/SETS
  FLANK_B_MODE 120 // set or get
  FLANK_B_DIR 121 // set or get, set is through get for multiple args
  FLANK_B_MOBILITY_ADD 122 // set or get
  FLANK_B_MAX_DAMAGE 123 // set or get
  FLANK_B_MIN_DAMAGE 124 // set or get

-- 0.75b2 --------------------------------------

- Fixed possible crash while waiting for connection to the host.
- Fixed unknown net msg in read ahead bugs when large UDP packets were received.
- Fixed a crash when empty network packets where destructed.

- Fixed rapier like units being unable to attack enemy units.
- TargetBorder now takes the correct vertical distance into account.
- ".kick" and ".kickbynum" now just kick the given player instead of kicking
  everyone including the host himself.
- Game end condition is now fixed to check allyteams instead of teams.

- Added gui_xray_shader.lua widget
- Added gui_team_platter.lua widget
- Added camera_smooth_move.lua widget

-- 0.75b1 --------------------------------------

Uncategorized:

- Added LuaCob, LuaGaia, LuaRules synchronized scripting:
  - LuaCob: (mod) provides access to lua functions for COB scripts
  - LuaGaia: (map) controls the Gaia team
  - LuaRules: (mod) gives full read/ctrl to setup custom mod game rules
- Added LuaRules Spring.UnitRendering (LuaMaterials, unfinished).
- Merged the LuaUI code with the new lua library setup.
- Made LuaUI mod widgets possible.
- Added support for the Gaia team.
- Added SimpleGroundFlash to explosion generator.
- Added 'dds' and 'png' as default buildpic formats.
- Load/save now uses the creg serializer.
- Netcode has been refactored to be more readable.

AI:

- Added Load/Save interface for AIs.
- Load/save support for EconomyAI and MexUpgraderAI.
- Added GetStartPos to IAICallback.
- Fixed CCommandQueue in AI interface.
- Added AIVAL_UNITLIMIT enabling AIs to query the maximum number of units.
- Added the ability to unload a specific unit using the UNLOAD_UNIT command
  (the unitID should be added as the fourth parameter)
- Added AIVAL_STOCKPILED to query the amount of stockpiled ammo.
- Added AIVAL_STOCKPILE_QUED to query the amount of queued stockpile ammo.

Behavioral:

- Allow up to 16 pads per air base.
- Units can not build stuff on top of cloaked stuff anymore.
- Units guarding other units now properly repair the other unit if it's damaged.
- Units guarding factories prioritize repairing the factory instead of building
  new units.
- Stunned (and transported) units can not build anymore.
- Units (that are still paralyzed) are not un-stunned anymore during unloading.
- Changed the unit resource updates so that they occur even while stunned.
- Transporters can carry hovers and ships now.
- Transporters now check ground slope before unloading.
- Units on a slope steeper than their max slope will now skid down it.
- Disabled the ground flattening when building mobile units.
- Added paralyzed unit skipping for paralyzers.
  (use ALT to disable skipping, same modifier as for permanent area commands)
- Made a unit's ability to slow itself relative to the angle of the slope.
- Improved aiming of Cannon type weapons. They will no longer over/under shoot
  due to large height differences.
- Weapons don't target dead and crashing units anymore.
- Mobile units under hold position in a temporary attack order don't move away
  from their patrol route anymore.
- Added ScriptMoveType (incomplete, but functional).
- Commander doesn't randomly D-gun buildings anymore when FPS mode is disabled.
- Missiles now honour SprayAngle weapon TDF tag.

Bugfixes:

- Removed the maximum 8 sides limit.
- Fixed incorrect searching order in the virtual file system.
- Fixed teamcolor for S3O and 3DO features.
- Fixed cloaked S3O units losing their texture.
- Fixed teamcolor rendering for MissileProjectile and WeaponProjectile.
- Fixed handling of .give so team unitlimit is respected.
- Dynamic and refractive water are now reinitialized after window resize.
- Fixed rendering of shadows of S3O models with transparency.
- Fixed rendering of underwater grass.
- Made LuaUI compatible with VFS access.
- Fixed SM3 shader for ATI x1\* series.
- Fixed incorrect cancelling of rotated buildings build orders.
- And many more

Chat commands:

- Added an optional [0|1] parameter to the pause command.
- Added an optional [0|1] parameter to the showHealthBars command.
- Added an optional [0|1] parameter to the updatefov command.
- Added an optional [0|1] parameter to the drawtrees command.
- Added an optional [0|1] parameter to the dynamicsky command.
- Added an optional [0|1] parameter to the hideinterface command.
- Added ".editdefs [0|1]" to allow lua based UnitDef editing.
- Added ".reloadcob <unitName>" to reload cob scripts.
- Added ".skip <[+|f]seconds>" for in-game and replay fast-forwarding.
- Added "/safegl [0|1]" command for testing lua rendering overhead.
- Added "/buffertext <text>", copies text into the console history.
- Added dynamic "/vsync" control.
- Added the "/fpshud [0|1]" action.
- Added "endgraph [0|1]" action.
- Changed ".take" so that cheating must be enabled for spectators to use it.
- Added the ability to give features using ".give"
  (only if the unit name doesn't match, player's team is used)
- Added input text editing
  - backspace & delete
  - home & end (or ALT+{left|right})
  - CTRL+{left|right} for word skipping
- Added the '//' -> '/' command prefix conversion syntax
- Added the following LuaUI widget handler commands:
  "/luaui togglewidget <widgetname>"
  "/luaui enablewidget <widgetname>"
  "/luaui disablewidget <widgetname>"
- Added the '/lodscale [shadow|reflection|refraction] <scale>' command.

COB:

- HitByWeaponId(z,x,id,damage) will be called instead of HitByWeapon if it exist
  in script.
- Implemented COB get/set constant STANDINGMOVEORDER (2)
- Implemented COB get/set constant STANDINGFIREORDER (3)
- Added a new COB query call, "QueryLandingPadCount".
  - if not defined, Spring will assume 16 as the maximum number of pads
  - it takes one argument, which is initially set to 16, the number
    returned in arg0 will be used as the maximum number of pads.
- Fixed logical XOR,AND,OR (they were implemented as binary XOR,AND,OR)
- Added COB get/set constant UPRIGHT (79)
- Added COB get constant POW (80) with 2 arguments x and y: it returns x^y.
- Added COB get constant PRINT (81) which prints it's 4 arguments to infolog.
- Added COB get constant HEADING (82)
- Added COB get constant TARGET_ID (83)
- Added COB get constant LAST_ATTACKER_ID (84)
- Added COB get/set constant LOS_RADIUS (85)
- Added COB get/set constant AIR_LOS_RADIUS (86)
- Added COB get/set constant RADAR_RADIUS (87)
- Added COB get/set constant JAMMER_RADIUS (88)
- Added COB get/set constant SONAR_RADIUS (89)
- Added COB get/set constant SONAR_JAM_RADIUS (90)
- Added COB get/set constant SEISMIC_RADIUS (91)
- Added COB get constant DO_SEISMIC_PING (92)
- Added COB get/set constant CURRENT_FUEL (93)
- Added COB get constant TRANSPORT_ID (94)
- Added COB get/set constant SHIELD_POWER (95)
- Added COB get/set constants LUA0-LUA9 (110-119)

Config:

- Added "SMFTexAniso" config parameter (default = 0, disabled)
  (application controlled anisotropic texture filtering for SMF maps)
- Removed "SM3MaxTextureStages", it's confusing and the fallback mode isnt
  really useful (still slow).
- Added config parameter MaximumTransmissionUnit. This can be used to control
  the maximum UDP packet size Spring uses. The default is 512, which should keep
  all UDP packets below the required minimum MTU of 576 bytes (512 payload).
  If the hypothese that many player drops are because of too big UDP packets is
  correct, then this may solve it.

Feature TDF tags:

- Added boolean feature TDF tag NoSelect. (default: 0)

Fixed exploits:

- Fixed exploit: UnitRestricted could be circumvented by making multiple
  factories and starting a build of the restricted unit in all factories at
  the same time.
- Fixed exploit: CanAssist could be circumvented if multiple builders were
  ordered an identical build at the same time out of their immediate range.
- Fixed exploit: Aircraft can not cross map boundaries anymore in FPS mode.

Game Setup:

- Added 'lineage' game mode (2). If the commander dies, all units directly or
  indirectly built by him die, regardless of the current owner of the unit.
- Added player 'CountryCode' tag for the gameSetup script.
- Added game setup script parameter 'Savefile'.
- SaveGame can also be loaded by command line: 'spring QuickSave.ssf' or
  'spring C:\Spring\Saves\QuickSave.ssf'
- Added NoHelperAIs tag: determines whether helper AIs are allowed.
- Added LuaGaia tag: is LuaGaia enabled this game?
- Added LuaRules tag: is LuaRules enabled this game?

GUI:

- Added land at 80% for aircraft.
- Better handling and display of widgets which are enabled, but not active.
- Disable unit tracking when using middle mouse button scrolling.
- Disable unit tracking when the viewport is set via the minimap.
- New unit selection filter "in hotkey group".
- Units being transported can not be selected anymore unless the transporter
  isfireplatform.
- Mobile units can be loaded onto a transport by clicking on the transport with
  the mobile unit selected.
- Added Free camera mode (to help make better videos). It's primarily intended
  to be used with keyboard input (CTRL/ALT/SHIFT/META behave differently then
  the normal camera modes). It's is not in the toggle list by default.
- Added CTRL+F5 default keybinding for free camera mode.
- Added text coloring for ally and spec message input.
- Changed unit sharing so that allied units do not clear their command
  queue, and cleaned up some more state for enemy transfers.
- Allowed build command descriptions to use text in the 4 corners of a button
  (0 = SW, 1 = NE, 2 = NW, 3 = SE)
- Added the "hotspot" command for the \*.txt mouse cursor spec file
- Added side preference prefix for start pictures.
- Allow "allside\_" start pictures to always be used (regardless of side).
- FPS mode now shows weapon names instead of numbers.
- Area build now builds every second row in the opposite direction.
- Perimeter build now builds everything in clockwise or counterclockwise order.
- Circle build now builds everything in clockwise or counterclockwise order.
- Circle build now works correctly when trying to use non-square buildings for
  circling anything.
- Improved QuitBox player selection visibility.
- Gunships and air builders now have a repair level switch.

Mapping:

- New ocean.jpg (public domain) comes from golgotha texture set and tiles better
  then original ocean.jpg.
- Changed default repeat settings so the new texture fits better (wrt. scale)
- Added "MAP\WATER\WaterRepeatX" and "MAP\WATER\WaterRepeatY" SMD tags, defining
  the amount of times the water texture should be repeated in X and Y direction.
- Aspect ratio of water texture is taken into account now.
- Added 'gamedata/resources_map.tdf' for gaia unit projectile textures.

Unit FBI tags:

- Added boolean FBI tag AirStrafe. Only effective if HoverAttack=1.
  If AirStrafe=1, gunships and construction aircraft will strafe in a circle
  around the target. If AirStrafe=0, they will not strafe, they will sit
  stationary in the air firing their weapons. If set to 0 they sit at one place.
- Added string FBI tag Gaia.
- Added string FBI tag DecoyFor: the name of the unit this one is a decoy for.
- Added boolean FBI tag CanBeAssisted. (default: 1)
  - Disabling assists to factories should work well.
  - Builders have to die to release their hold on a unit being built,
    and once the unit is released, multiple builders can repair the unit.
- Added boolean FBI tag CanSelfRepair. (default: 0)
- Added boolean FBI tag TransportByEnemy. (default: 1)
- Added boolean FBI tag HoldSteady, causes transports to hold units such that
  their updirs are always aligned.
- Added boolean FBI tag ReleaseHeld for transports. When set to 1, the units
  carried by the transport will survive the destruction of the transport itself.
- Added float FBI tag SlideTolerance, multiplies the the max slope for
  determining when skidding should begin, due to slope. Values less than 1 will
  cause the unit to never start skidding due to slope. (default: 0)
- Added float FBI tag MinCollisionSpeed, indicating the minimum net impact speed
  that will cause damage to a unit.
- Added vector FBI tag CollisionSphereOffset
- Added float FBI tag LoadingRadius
- Added boolean FBI tag FullHealthFactory, controls whether or not a unit can
  leave the factory if it is not at full health. (default: 0)
- Added float FBI tag RepairSpeed (default: WorkerTime)
- Added float FBI tag ReclaimSpeed (default: WorkerTime)
- Added float FBI tag ResurrectSpeed (default: WorkerTime)
- Added float FBI tag CaptureSpeed (default: WorkerTime)
- Added float FBI tag TerraformSpeed (default: WorkerTime)
- Added boolean FBI tag DecloakOnFire (default: 1)
- Up to 8 ok, select and arrived sounds can now be defined.
  (use tags ok1, ok2, etc.)
- Added float FBI tag CollisionSphereScale, this is a scaling factor for the
  unit's collision sphere. (default: 0.5 for aircraft, 1 for other units)

Weapon TDF tags:

- Added weapon tdf command SweepFire, if 1 (default 0) it will make the weapon
  continue firing while it aims for a new target. Only works on continuous beam
  lasers currently.
- Added boolean weapon TDF tag CanAttackGround. (default: 1)
- Added float weapon TDF tag float TargetBorder: if nonzero, targeting units
  will TryTarget at the edge of collision sphere (radius\*tag_value, [-1;1])
  instead of its centre. Defaults to 1 for Melee type weapons, 0 otherwise.
- Added float weapon TDF tag CylinderTargetting: if greater than 0, range will
  be checked in a cylinder (half height=unitradius\*cylinderTargetting) instead
  of a sphere. Defaults to 1 for Melee type weapons, 0 otherwise.
- Added float weapon TDF tag MinIntensity: for beamlasers - always hit with some
  minimum intensity (a damage coefficient normally dependent on distance).
- Added float weapon TDF tag SizeDecay: how much a plasma particle is smaller
  than the previous within the same plasma shot. (default: 0)
- Added float weapon TDF tag AlphaDecay: how much a plasma particle is more
  transparent than the previous particle. (default: 1)
- Added float weapon TDF tag Separation: distance between each plasma particle.
  (default: 1)
- Added int weapon TDF tag Stages: number of particles used in one plasma shot.
  (default: 5)
- Added boolean weapon TDF tag NoGap: if set to 1, the distance between plasma
  particles is proportional to the size of the two particles. If set to 0, the
  distance between plasma particles will be proportional to the size of the
  first particle. (default: 1)
- Added the weapons TDF tag 'visibleShieldHitFrames' (defaults to 0 frames)
  - can be used along with 'visibleShieldHit'
  - the alpha value fades after the hit
- Added colormap for ballistic ('cannon' type) weapons.
- Added boolean weapon TDF tag AvoidFeature, this makes the weapon avoid
  features while targeting. (default: 1)
- Added float weapon TDF tag FallOffRate: rate at which the beamweapon fades out
  after it goes past its maximum range. 0 is never fade out, 1 is finish fading
  out exactly at max range, the default 0.5 is the previous behaviour.
- Added float weapon TDF tag LodDistance: minimum camera distance at which the
  default beamweapon projectile is simplified to rectangle. (default: 1000)
- Added float weapon TDF tag DynDamageExp: exponent of the damage formula, 0
  (the default) disables dynamic damage. 1 means linear scaling.
- Added float weapon TDF tag DynDamageMin: minimum damage value. (default: 0)
- Added boolean weapon TDF tag DynDamageInverted: inverts the damage curve.
- Added float weapon TDF tag DynDamageRange: if set it will use this value
  instead of the range value from the weapon in the calculation.

New projectile handler textures:

- sbtrailtexture - default first section of starburst missile trail texture
- missiletrailtexture - default first section of missile trail texture
- muzzleflametexture - default muzzle flame texture
- repulsetexture - texture of impact on repulsor
- dguntexture - dgun texture
- flareprojectiletexture - texture used by flares that trick missiles
- sbflaretexture - default first section of starburst missile trail texture
- missileflaretexture - default first section of missile trail texture
- beamlaserflaretexture - default beam laser flare texture
- bubbletexture - torpedo trail texture
- gfxtexture - nanospray texture
- projectiletexture - appears to be unused
- repulsegfxtexture - used by repulsor
- sphereparttexture - sphere explosion texture
- wrecktexture - smoking explosion part texture
- plasmatexture - default plasma texture

Scons build system:

- Added debugdefines configure option: true by default, if it's set to false
  (debugdefines=no) then a debug build will be made with release preprocessor
  defines. This can be used to generate debugging symbols for release builds.
- Added dc_allowed configure option to enable/disable direct control.

Unitsync:

- Added GetSpringVersion() function to unitsync. This returns the Spring version.
- Deprecated InitArchiveScanner() unitsync function.
- Added java bindings to unitsync.
- Unitsync is now installed to libdir on Linux. It can be assumed this is
  either /usr/local/lib/spring/unitsync.so or /usr/lib/spring/unitsync.so.

-- 0.74b3 --------------------------------------

- Added LuaUI v0.1 (with widget management code, somewhat experimental)
  (requires the LuaUI setting to be on to be autoloaded,
  "/luaui disable" to disable in-game
  "/luaui reload" to load in-game
  "/luaui reload fresh" to load in-game, but with fresh state)

- Renamed docs/lua_ui_interface.txt to LuaUI/API.txt

- Added "/minimap slavemode [0|1]" and "/minimap draw" for use with LuaUI

- Added "/specfullview 2" spectator mode, where you can both view and select all units

- Added "/volume <fraction>" command (0.0 to 1.0 range)

- Added "group <int> [ "set" | "add" | "selectadd" | "selectclear" | "selecttoggle" ]"
  action as an alternative to the group[0|9] commands (note that the key modifiers can
  be setup differently from the default)

- Fixed FBI tag floater for planes,
- Added FBI tag canSubmerge for planes. The following rules apply:

  1. If a plane only has the 'floater' tag set to 1, it will land on the ocean surface.
  2. If a plane only has the 'canSubmerge' tag set to 1, it will land on the ocean floor.
  3. If a plane has both tags set to 1, it will obey rule 2.

- Fixed the factory cancelled-build delay bug.
- Fixed units get stuck while guarding factories bug.
- Fixed planes can't find landing pad on large maps bug.
- Fixed in game video creation (default binding F10).
- Fixed a bug causing much metal/energy to be lost instead of shared to allies.

- Smoothed LOS / Radar view.

-- 0.74b2 --------------------------------------

- Planes can land on water surface if FBI tag floater is set to true.

- Added GROUND_WATER_HEIGHT (78) to get in cob script.
  Same as GROUND_HEIGHT (16) but in water it returns negative water depth instead of 0.

- Units don't slowdown on intermediate waypoints anymore.

- Added mingw compatible crashhandler.

- Fixed sharebox and quitbox for 16 players.
- Fixed FPS mode aiming to the northwest.
- Fixed FPU control word assertion that made spring unplayable with some sound drivers.

- Added sorting options to "/info", you can use names or numbers:
  0 - "off", disable display
  1 - "ally", allies (you, your team, your allyteam, your enemies, then spectators)
  2 - "team", player team id
  3 - "name", player name alphabetic sort
  4 - "cpu", player cpu usage
  5 - "ping", player ping time

- Added the "maxparticles <value>" action
- Added the "minimap simplecolors [0|1]" action
- Added the "specfullview [0|1]" action
  (setting it to 0 gives the same minimap and viewport visibility as the selected team)

- Added "OverheadTiltSpeed" config parameter

- Fixed the unit-limit vs. sharing bug

- Shadow mapping works for SM3

-- 0.74b1 --------------------------------------

- Linux vs Windows is now synced if binaries are built using gcc 4.0 or higher.

- Games with more then 10 players now work properly.
- Several improvements and fixes for autohosting, including a .kickbynum command.

- Added minimap commands (two modes, see the MiniMapFullProxy config param)

- Added "MiniMapGeometry" config parameter
- Added "TooltipGeometry" config parameter
- Added "TooltipOutlineFont" config parameter
- Added "InfoConsoleGeometry" config parameter

- Added "/resbar [0|1]" action
- Added "/tooltip [0|1]" action
- Added "/specteam <number>" action (sets the spectator team)

- Added "newAttackMode" and friends to "ctrlpanel.txt"

- Added animated line stippling for queue rendering (see cmdcolors.txt)

- Cursors can now use TGA and PNG file formats
- Added an optional cursor control file for easier timing/frame control

- Renamed the /layout command to /luaui
- Renamed the "ctrlpanel.lua" file to "gui.lua"

- Added "timewait [seconds] ["queued"]" action
- Added "deathwait ["queued"]" action
- Added "squadwait [count] ["queued"]" action
- Added "gatherwait" action
- Added "gathermode [0|1]" action

- Added a number of features to the LuaUI
  (see Documentation/lua_ui_interface.txt)

- Improved the outline font rendering speed

- numbers in .info list are now team numbers, so they match with '.team X' commands.
- names are now prefixed with 's:' 'a:' or 'e:' for respectively spectators, allies and enemies.
- if spectating, an arrow is displayed in front of the team you are currently watching.

- added Shift and Alt/Meta group selection modifier features
  (Shift adds members to current selection, Alt/Meta toggles them)

- added the "viewsave <name>" and "viewload <name>" actions
- added GetCameraState() and SetCameraState() to the lua GUI interface

- added "selectcycle [restore]" action

- fixed queue build rendering (GL_DEPTH_TEST)

- added GetUnitHealth(), GetTeamResources(), and GetCommandQueue() to the lua GUI interface
- enabled loading the "ctrlpanel.lua" file from the VFS

- added "wait queued" and "selfd queued" to the default key bindings
- fixed WAIT behaviour for factories
- enabled queuing of Self Destruction commands
- added a pathetic SELFD cursor

- added "alwaysDrawQueue" and "buildBoxesOnShift" to cmdcolors.txt

- added the "invqueuekey [0|1]" action, and [InvertQueueKey=0] config parameter
- added mouse rocker gestures for immediate mode access when using inverted queue keying

- AIs can query a unit current fuelstorage via GetProperty() with property AIVAL_CURRENT_FUEL (2).
- AIs receive seismic pings via HandleEvent() function with message AI_EVENT_SEISMIC_PING (5).
- The icontype of enemy units is remembered if they are under continuous radar coverage.
- Ghosted buildings under continuous radar coverage don't have an error vector in their position anymore.
- Ghosted buildings also turn into unit icons when far away.

- Improved clickability of unit icons.

- Calling emit-sfx 2048 from a script will now always fire the weapon. Emit-sfx 4096 will now take over the previous behavior of detonating the weapon at the piece position.

- Added unit FBI tag UnitRestricted to limit the number of a unit type allowed in a game.

- Added CLOAKED (76) and WANT_CLOAK (77) to get/set in cob script.

-- 0.73b1 --------------------------------------

- added the ability to layout the control panel widget layout with a LUA script
  (uses a file called "ctrlpanel.lua")
- added the "/layout" command for direct interaction with the layout lua script

- Map packs (ie. multiple maps in one archive) are officially unsupported now because of technical reasons.

- Added keyword "dir" to explosiongenerator, this is a vector and can be different depending on what created the explosion.
  If created at a weapon impac it is the direction of the impact.

- Added the ability to call explosiongenerators and weaponfire from within script via emit-sfx command.
  Explosiongenerators to be called via script are defined in the unit TDF file in a section called SFXTypes, with tags explosiongenerator+n=some_explosion
  and then called via emit-sfx 1024+n.
  Weaponfire are called via emit-sfx 2048+weapon_to_fire.

- added "ctrlpanel.txt" configuration file to control how the Control Panel
  is rendered (x/y icons counts, sizes, positions, prev/next menu icon
  locations, etc...)
- added an option in "ctrlpanel.txt" to render some of the screen text with
  an outlined font. This is a hack, and can cause FPS decreases on slower
  computers (main factor is blended fill-rate)

- added an outline around the minimap

- fixed reclaim exploit where extra metal was gained if you destroyed the building just before it would finish reclaiming.

- added "cmdcolors.txt" settings file to configure control indicator rendering
  (this includes command queue lines and ranges, highlighted unit ranges, the
  mouse selection box, and the selected unit boxes)
- added line width specification for items in "cmdcolors.txt"
- added blending mode specification for items in "cmdcolors.txt"

- removed "NoSound" config parameter in favour of "MaxSounds=0"
- passthru to the NullSound driver if there is a sound driver error

- added a page number to the control panel
- added "buildiconsfirst" command to toggle the order of control panel icons
  (also set the control panel page number to the cmdcolors.txt build color
  when the build icons are being placed first)

- added build range circles for immobile builders when holding shift over them

- added the techLevel variable to UserDef, with automatic detection of the
  tech levels in a mod based on the information in the sidedata.tdf file
- added the "techlevels" action, to save the tech levels to techlevels.txt
  (as well as sending them to stdout)
- display a unit's TechLevel in the tooltip window when cheats are enabled

- key commands now work while the GUI interface is hidden, and the default
  command cursor icon is updated as well
- change the control panel page when a key command accesses an off-page icon
- change the tooltip to reflect the unit being built rather then the builder
  while selecting the build position
- added cycling for command actions when more then one is available
- added "iconpos <#>" action, for positional access to control panel icons
- added "firstmenu" action (resets the control panel to its first page)

- added "keyset" command for named keysets
- added "keysym" command for custom key symbols
- added "fakemeta" key binding command to provide another modifier

- added SmoothLines and SmoothPoints config parameters (anti-aliasing)
  (0 = off, 1 = fastest, 2 = don't care, 3 = nicest)

- changed GameInfo so that it always displays map name and mod name

- group AI code now permits multiple AIs per dll/so. This is handled by
  identifying group AIs by a new datatype, 'AIKey', rather than using just
  the dll name. Group AI dlls should now implement GetAiNameList to return an
  array of available AI names in string (const char\*) form, terminated by NULL.
  The existing group AI interface functions GetNewAi, ReleaseAi, and
  IsUnitSuited now take an unsigned aiNumber as their first parameter: this
  is a zero-based AI index, matching the array returned by GetAiNameList.

- improved default command selection

- added "/water [0-3]" to control water rendering in-game
  (the optional number is the mode; without the number, it cycles through the modes)
- added "/shadows [0-1]" to control shadow rendering in-game
  (the optional number means off/on; without the number, it toggles)

- Fix mouse-drag-move to make units spread across the front if it's big enough, also somewhat take into account unit sizes. Added CTRL-Move, CTRL-Mouse-Drag(front) move to make units move at the speed of slowest unit.
- [bugfix] Resource expenditure now shows correct pull when economy is stalling.

- - Added transparency support from texture2 in s3o drawing, requires advanced shading.
- added DualScreenMode and DualScreenMiniMapOnLeft configuration flags.
  This causes the game view width to be one half of the screen width, so the minimap can take the other half.
  You will need to spread your spring window across the 2 screens.
  Specifying DualScreenMiniMapOnLeft will place the minimap on the left hand screen, rather than the right (the default)
- geothermal vents with a geothermal unit on top will not create smoke.

- Added the following Unit FBI tags:
  canPatrol=1; Can the unit go on patrol
  canGuard=1; Can the unit guard things
  canAttack=1; Can the unit attack other units
  Reclaimable=1; Determines if a unit can be reclaimed by other units
  noAutoFire=0; (if set to 1, unit will fire once then switch to next target, and needs to be told to fire manually)
  canBuild (can the unit start new construction projects)
  canRepair (can the unit repair other things)
  canRestore (can the unit restore deformed ground)
  canReclaim (can the unit reclaim other things)
  canAssist (can the unit assist construction projects already started)

- added support for teamcolored nanospray (user option). Can be forced off by modder using gamedata/particles.tdf.
- new fbi tag shownanoframe: set to 0 to disable nanoframe while the unit is being built.
- new fbi tag shownanospray: set to 0 to disable nanospray.
- new fbi tag nanocolor: color of the nanospray and the nanoframe in red green blue format (white = 1 1 1).
  (These three fbi tags apply to the unit itself; not to the unit it's building.)

- linux vs linux is now synced, independently of the distribution & gcc version
- if host finishes loading before client executable is started spring no longer loses connection

- added the "say" action (does work with the .give command)

- changed .clock and .info to local commands (/clock and /info now work as well)

- replaced key binding code with a more flexible solution:

  - can specify key for almost all commands (including unit builds)
  - can specify modifiers (Alt, Ctrl, Meta, Shift)
  - can bind the same key to more then one action
    (first available action is used, handy for build orders)

- added internal default bindings (see rts/Game/UI/KeyBindings.cpp)
- added "unbindall" command
- added "unbindKeyset <keyset>" command
- added "unbindAction <action>" command
- added "unbind <keyset> <action>" command
- added "bindbuildtype ..." command (aka: auto-binder)

- added "hotbind" action for the control panel icons (Ctrl+insert)
- added "hotunbind" action for the control panel icons (Ctrl+delete)

- added /keysave and /keyload commands
  (keysave saves to "uikeys.tmp", not "uikeys.txt")
- added /keyprint, /keydebug, /keysyms, and /keycodes commands
  (these commands send information to stdout)

- added ability to specify item in icon mode buttons via its binding
  (ex: "firestate 0" binding will set firestate to hold fire)

- added ability to specify item in combo box buttons via its binding
  (ex: "aiselect Radar AI" binding will assign the selected units to the Radar AI)

- pseudo mouse actions now discard key repeats

- added command queue icons
- added build command queue directional lines
- added AreaAttack command queue area display

- added game info panel (with "gameinfo" and "gameinfoclose" actions) (i)

- versioned the "archivecache.txt" filename (ex: "ArchiveCacheV4.txt")

- Quit and Share menus close when they receive ESC

- added "drawlabel" action (avoid the double-click, and key repeats)

- added "screenshot png" action

- added "chatAll" action
- added "chatSwitchAll" action
- added "chatAlly" action
- added "chatSwitchAlly" action
- added "chatSpec" action
- added "chatSwitchSpec" action

- added tracked unit rotation
- added Average and Extents tracking modes
- added "trackoff" action
- added "trackmode" action

- added "viewfps" action (Ctrl+f1)
- added "viewta" action (Ctrl+f2)
- added "viewtw" action (Ctrl+f3)
- added "viewrot" action (Ctrl+f4)

- added "firestate" action
- added "movestate" action
- added "prevmenu" action (,)
- added "nextmenu" action (.)
- added "buildunit\_<name>" action
- added "deselect" action
- added "select <selection criteria>" action (redundant)

- added new water rendering mode, refracting+reflecting but without the heavy dynamic simulation.
- Added '--quit=T', '/quit T' commandline option to auto-quit spring on game over or after T seconds.
- Added '--minimise', '/minimise' commandline option to start spring minimised.
- added gradual feature reclaiming, see mantis #262
- support for rotating buildings by 90 degrees, bind to "incbuildfacing" and "decbuildfacing" (bound to ']' and '[' by default)
- modified the ground attack command for queued attack and area attack patterns
  (the last queued attack position is repeated for backwards compatibility)
- removed the height factor from AirCAI ground attack command drawing
- added word completion, with dot commands, player names, and unit names
  (does partial match reporting, and the unit names are only active for .give commands)
  -> "/<tab>" will show local commands
  -> ".<tab>" will show remote commands
- added console input history buffer
- added [DoubleClickTime=200] config parameter
- added [WindowEdgeMove=1], for folks who run windowed with fullscreen size
  (ex: good with X11/icewm to allow moving between virtual screens,
  and to avoid display lockups during crashes)
- disabled rendering while inactive (minimized)
- added a null sound driver [NoSound=1]
- merged global synced/unsynced "drawdebug" variables
- added vsync for GLX
- windows users can use [VSync=-1] to use system default
- allow selection of vsync rate

- [feature] A new group AI, "MexUpgrader AI" has been added. See https://springrts.com/wiki/MexUpgrader_AI for more info.
- [feature] Custom unit icons are supported. See https://springrts.com/wiki/CustomUnitIcons for more info.
- [bugfix] 'Quit and Resign' dialog now also appears when you're hosting, and stays hidden when the game is over.
- [bugfix] Commands on enemy units (attack, reclaim) get canceled automatically when the unit leaves LOS / radar.
- [change] 'Quit and Resign' dialog now only gives everything to allies (and not enemies) if they are still alive.
- [change] The LOS drawing mode is remembered when you switch back and forth to others (e.g. metalmap, heightmap).
- [change] Radar and jammer range is shown by default if you press 'L'.
- [change] Radar and jammer toggle ';' happens instantly.

- New weapon tdf tag ColorMap, used for some weapon to define the color shift during their lifetime
  Used in flamethrower.
  Tag specify either a bitmap file, ex: "colormap = mycolormap.tga;"
  or defines the color directly in the weapon file in rgba order where 1 is full brightnes,
  ex: "colormap=1 0 0 1 0 0 0 1" start with red color and shift to black at the end.

- New weapon tdf tag rgbcolor2, used to make laser weapon have different color at the center, defaults to white.
- New weapon tdf tag corethickness, how thick the inner color of a laser is, this is a percentage value, default is 0.25
- New weapon tdf tag laserflaresize, size of the flare for laser weapons, multiplier for thickness, default 15.
- New weapon tdf tag LargeBeamLaser, if set to 1 a beamlaser uses an alternate graphics effect.
  New tags usable for a LargeBeamLaser is:
  tilelength: defines the length before the texture used is repeated (tiled).
  scrollspeed: how fast the beam appears to be moving towards it target.
  pulseSpeed: how fast the pulsating effect at the beam start is.

- Texture tag added to the explosiongenerator for HeatCloud and Dirt
- New weapon FBI tags, texture1, texture2, texture3, texture4, the texture must be defined in resources.tdf in the projectile section.
  MissileProjectile (standard rockets and guided missiles):
  texture1 = flare
  texture2 = smoke
  LaserProjectile:
  texture1 = laserbeam
  texture2 = Start and end of the beam (half of the texture used at either end.)
  BeamLaserProjectile:
  texture1 = laserbeam
  texture2 = Start and end of the beam (half of the texture used at either end.)
  texture3 = muzzle flare
  LargeBeamLaserProjectile:
  texture1 = laserbeam
  texture2 = Start and end of the beam (half of the texture used at either end.)
  texture3 = Extra graphical effect at the start of the beam.
  texture4 = muzzle flare
  StarBurstProjectile (starburst weaponry, like nukes):
  texture1 = flare
  texture2 = smoke
  texture3 = flame
  EmgProjectile
  texture1 = texture for the projectile
  LightningProjectile
  texture1 = texture for the lightning
  ExplosiveProjectile (used in most plasma cannons)
  texture1 = texture of the projectile, will be blended multiple times.

- tdf tag duration is used for laser weapons.
- tdf tag rgbcolor is used for EmgProjectile and ExplosiveProjectile.
- tdf tag intensity is used for EmgProjectile and ExplosiveProjectile.
- New weapon tdf tag soundstartvolume and soundhitvolume.
- New weapon tdf tag size, defines the visible size of projectiles.
- New weapon tdf tag CollisionSize, defines the physical size of projectiles, percentage of size for flamethrower and absolute value emg, plasma and laser.
- New weapon tdf tag sizegrowth, defines how fast flame projectiles will grow in size.
- New weapon tdf tag flamegfxtime, this makes the flame projectile stay alive as a graphical effect (doesnt do any damage) after it has reached maxrange.
  Default is 1.2 which makes it stay alive 20% beyond maxrange.
- New weapon tdf tag AvoidFriendly, if set to 0 units will not try to avoid friendly units in their line of fire.
- New weapon tdf tag CollideFriendly, if set to 0 this weapon will not hit friendly units.
- New weapon tdf tag ExplosionSpeed
- edgeEffectiveness added for weapons

- Added limited fuel settings for units (only work for aircraft so far, need a good mechanism for refueling other stuff)
- New fbi tag MaxFuel, max fuel in seconds that the unit can carry, default 0 = unlimited
- New fbi tag RefuelTime, how long time it takes to fully refuel the unit, default 5
- New fbi tag MinAirBasePower, minimum build power an airbase unit must have for the aircraft to land on it, default 0
- New fbi tag WeaponFuelUsage<1-16>, how much fuel in seconds that firing the weapon takes from the owning unit, default 0
- New fbi tag SeismicDistance, give the unit a seismic sensor for detecting movement through the ground.
- New fbi tag SeismicSignature, The magnitude of seismic disturbance when this unit is moving, default sqrt(mass/100).
- New COB "get"/"set" constant: 75 MAX_SPEED get/sets the max speed of the unit (multiplied with 64k)

- New archive format (for map/mod development): _.sdd. It is a directory, but it is treated as an archive just like _.sdz and \*.sd7.
- New map format SM3, currently supported for testing purposes.

-- 0.72b1 --------------------------------------

- Bos/Cob script for units can now animate the death of the unit. Just have the Kill function return after a while instead of directly and return wreck level in return value instead of in function argument.
- Fixed the com dies, game ends exploit: sharing your com now kills you
- Fixed a bug causing clients not to quit when server quits
- Fixed crash bug normally resulting in dialog with "no rtti data"
- ".give" cheat command doesn't desync the game anymore
- Made "Team0() is no more" message customizable by modders (see XTA for an example).
- Added weapontag "paralyzetime", controls how long a unit can maximaly be paralyzed after being hit by this weapon, default 10
- Adds some new options in the settings program for scroll speeds
- Added new fbi tag "levelground" set this to 0 to prevent a building from leveling ground beneth it. Make sure that the model continue some way below ground so that they dont look like they float in air.
- Fixed torpedo aircraft not being able to attack subs
- Added support for decals on ground below buildings
- New fbi tag UseBuildingGroundDecal, set to 1 to enable decals for building
- New fbi tag BuildingGroundDecalType, name of a bitmap in unittextures folder
- New fbi tag BuildingGroundDecalSizeX,BuildingGroundDecalSizeY, size of decal (same scale as footprint for unit)
- New fbi tag BuildingGroundDecalDecaySpeed, how fast the decal will fade out after the building has died, default 0.1 (fades out in 10s)
- New fbi tag ShowPlayerName, if set to 1 the tooltip shows the player name instead of the unit description
- Fbi tag HideDamage now actually works: set it to 1 to hide most details in the unit's tooltip from enemies
- New weapon tag "rgbcolor", overrides the old color1 and color2 tags if it exists, gives the color in red green blue format (1 1 1 = white).
- Removed old plasmarepulser system and replaced it with a more generalized shield system
- New weapon tdf tag IsShield, if 1 mean the weapon is a shield and now a weapon
- New weapon tdf tag shieldrepulser, 0=destroy projectile,1=repulse (bounce) projectile
- New weapon tdf tag smartshield, 1=shield doesnt affect allies weapons
- New weapon tdf tag exteriorshield, 1=shield doesnt affect weapons fired from within shield
- New weapon tdf tag visibleshield, 1=see a visible shell representing the shield
- New weapon tdf tag visibleshieldrepulse, 1=see a beam showing when shield repulses something
- New weapon tdf tag shieldenergyuse, how much energy the shield use when repulsing/destroying something
- New weapon tdf tag shieldforce, how hard shield can accelerate some sort of projectiles away
- New weapon tdf tag shieldradius, size of shield effect
- New weapon tdf tag shieldmaxspeed, maximum speed the shield can push some sort of projectiles away
- New weapon tdf tag shieldpower, how much power the shield can store, power is drained by the affected weapons damage, 0=infinite
- New weapon tdf tag shieldpowerregen, how fast the shield regenerates power
- New weapon tdf tag shieldpowerregenenergy, how much energy the shield uses when regenerating power
- New weapon tdf tag shieldintercepttype, which types of weapon the shield can affect (bitfield)
- New weapon tdf tag interceptedbyshieldtype, which types of shields can affect this weapon, if (shieldInterceptType & interceptedByShieldType) is not zero the shield can affect the weapon, default 1 for plasma, 2 for laser, 4 for missiles, 0 for other
- New weapon tdf tag shieldgoodcolor, color of shield when power is full
- New weapon tdf tag shieldbadcolor, color of shield when power is empty
- New weapon tdf tag shieldalpha, how visible the shield shell is with visibleshield=1
- Added auto zooming using scroll + Left ALT key (patch from colorblind)
- Added simplified LOS drawing (patch from colorblind)
- Fixed mouse speed problems in windows with holding shift.
- Demo filenames with spaces in them work again.
- Changed non-positional sound behavior: only selected units give non-positional sound
- Missile weapons no longer tracks to exact position of non flying units (make missile units weaker against radar targets)
- Stun damage units now receives 1/10 of normal experience and no experience for attacking already stunned units
- Units no longer receive experience for "overkill" damage
- Adds support for using arbitrary team colors, teamcolor textures for 3do need to be slightly changed to accomendate this (see the base texture archive to see how its done)
- Increased max camera zoom out (zooming to far can cause slowdowns/graphical glitches)
- Added Radar group AI from colorblind: It notifies the player of incoming enemy units.
- Added custom explosion generator: It can be enabled using "explosiongenerator=custom:SomeExplosionGeneratorName" in the weapon TDF. See the wiki page https://springrts.com/wiki/CustomExplosionGenerators for more info.
- Tab will now bring zoom out to show all map in any camera mode, tab again zooms back to the mouse cursor.
- Added GetGravity(), GetUnitDefHeight() and GetUnitDefRadius() to AI interface
- Mod can now be selected in the single player menu.
- Checksums of the map and mod are checked on game start.
- Mod can be selected when starting the spring exe directly.
- Various other small changes.

-- 0.71b1 --------------------------------------

- Added new water renderer
- Fixed lasers firing two projectiles at once, efficiently doing double damage.
- Fixed several exploits with FPS mode.
- Added new fbi variable CanLoopbackAttack, only affects fighter aircraft, if 1 this will enable the player to set the aircraft to do an immelman turn after firing instead of overflying the target.
- Changed so units in maneuver mode will abort anything that moves them further than a certain distance from the patrol path
- Added statistics to the end game dialog
- Units with S3O models can explode into pieces.
- Added a " all" option to .give to give all units in loaded mod
- Fixed autoheal being about 4x to fast (or measuring heal per 0.25s depending on how one sees it)
- Improved exception handling and crashreport handling in windows.
- Added DateTime to the [VERSION] block in the demo recorder file
- Removes los from units being transported
- To disable terrain deformation:
  Mapmakers: Put 'NotDeformable=1' in the .smd.
  script.txt: Put 'DisableMapDamage=1' in with the other properties.
- VSync can be disabled with the settings application.
- Several other smaller fixes and improvements.
- Unit reply sounds are non-positional and have a separate volume setting in the config application. Their volume is also affected by the global volume.

-- 0.70b3 --------------------------------------

- Shift/ctrl do not change rotation speed in rotating camera mode anymore.
- Submarines work again.
- Fixed attack claxxon.
- Fixed units getting stuck in buildings.
- Added possibility to bind mouse1, mouse2 and mouse3 to keyboard.
- Fixed shift+letter bug.
- Fixed Pathing Map (F2 view) bug.

-- 0.70b2 --------------------------------------

- Fixed some (debug) message colors.
- Fixed keyboard layout problems.
- Fixed repeat orders/high trajectory etc not indicating their current state correctly
- Fixed problem with reading hpi archives
- Added custom SDL.dll (1.2.9) with refresh rate patch from SDL CVS, should help with the 60hz issue
- Fixed right shift/ctrl while scrolling.
- Fixed shift/ctrl while middle click drag scrolling.
- Fixed map loading by less strict tdf parser.
- Don't crash on parse errors, but show an error message instead.

-- 0.70b1 --------------------------------------

- First use of crossplatform code! This required a lot of bugfixing and changes, which are not mentioned here.
  Among these changes are switches to using boost, SDL, freetype, instead of Win32 SDK + DirectX. For a full list of changes, see the subversion log.

- XTA Pimped Edition Version 3, a version of XTA with high detail models, is used as the default mod now.
- Binding for mouse button 4/5 added (defaults to Z and X), so you can set build spacing.
- You can enable advanced unit rendering without enabling shadow mapping (in the settings app).
- Various resource files are now read from gamedata/resources.tdf instead of hardcoded into the game exe.
- Added "dontland" unit tag, to specify aircraft that should not land. Don't use this on transport units!
- std::sort related bug fixed in projectile handler
- Fixed flipped features, they are now rendered exactly the same as units.
- A few new COB "get" constants:
  70 MAX_ID returns maximum number of units - 1
  71 MY_ID returns ID of current unit
  72 UNIT_TEAM returns team of unit given with parameter
  73 UNIT_BUILD_PERCENT_LEFT basically BUILD_PERCENT_LEFT, but comes with a unit parameter
  74 UNIT_ALLIED is unit given with parameter allied to the unit of the current COB script. 1=allied, 0=not allied
- Fixed login packets being sent every frame instead of once a second when trying to connect
- Fixed ghosted buildings option.
- Fixed isairbase=1 unit tag.

-- 0.67b3 --------------------------------------

-AI dll's can be used with startscripts again

-- 0.67b2 --------------------------------------

-Fixed black map bug
-Impulsefactor now affects ground damage

-- Lobby changes

- battle hosts can now use /ring <username> command on players participating in their battles
- fixed problem with notification dialog switching focus back to application
- added /rename command which will rename your account (so that people who wish to add clan tags
  in front of their names don't have to reregister and lose their ranks). Player names may now also
  contain "[" and "]" characters.
- fixed small bug in demo script parser
- added /mute, /unmute and /mutelist commands for admins/mods
- clients behind same NAT now get local IPs instead of external one (from the server).
  Note that some issues still exist with currently implemented nat traversal method,
  so players hosting battles are advised to forward their port nonetheless.

-- 0.67b1 --------------------------------------

-Catching AI exceptions can be disabled with the settings app.
-Fixed access-violation bug when creating sky.
-Fixed numerous division by zero bugs.
-Fixed default cursor for indestructible features.
-Beamlaser uses thickness value.
-COB support for Getting/Setting veteran level and unit speed.
The returned veteran level is unit experience multiplied by 100
Unit speed can be set to something nonzero only when the current speed is nonzero, it can always be set to zero.
-Overhead camera can be tilted with control key + mousewheel now.
-Debug message verbose level can now be set with the settings application. (Modified update by Alantai)
This will only affect AI console messages.
-Ghosted buildings can now be disabled by the lobby. They are enabled by default.

-- 0.66b1 --------------------------------------

-Fixed transport units being able to load themself.
-Fixed gunship like aircraft always having the same altitude
-Fixed torpedo aircraft now being able to attack underwater targets
-Support for upright, flammable and indestructible feature tags
-Fixed division-by-zero bug in bomb-dropper.
-Fixed access-violation bug in pathfinder.

-Added new s3o unit format

-Added new tags to .fbi file
autoHeal - health per second that a unit always will be autohealed for (default 0)
idleAutoHeal - health per second that a unit will be autohealed for whenever its idling (default 10)
idleTime - time in ticks without any action before a unit is considered idling (default 600)
-Added new tags to weaponfile file
NoSelfDamage - Units take no splash damage from its own weapon
impulsefactor - Explosion impulse will be multiplied by this

-Added read access to the virtual filesystem for the AI
-Further AI interface updates will now be backwards compatible (old AI's will not break on a new spring release)
-Added user input functions from Alik to the AI interface
-The global AI will now be notified if a unit changes team.
-UnitDestroyed/EnemyDestroyed get the attacker unit id if any (by submarine)

-- 0.65b2 --------------------------------------

-Fixed crash with factories closing at the moment they die

-Fixed sync error on water maps
-Increased max players/teams to 32/16

-Added support for selecting which globalai-dll to use when starting spring.exe directly
-Added support for having more than one lua startscript. Put them in subfolder "startscripts".

-Added UnitMoveFailed to global AI interface. Called when ground units fail to move to their destination.

-- 0.65b1 --------------------------------------

-Fixed crash bug involving radars on non square maps
-Fixed crash bug involving gunships landing on repairpads that get destroyed
-Fixed crash bug involving large units exploding

-Fixed resurrected building not having a footprint
-Increased time to reclaim wreckages
-Fixed acceleration for units not being correctly set from fbi file
-Fixed slope of map not getting updated properly for craters
-Fixed sonar jamming
-Can now switch viewed team when spectating using keys 1-9
-Added faster reload speed for experienced units
-Added support for multiplayer replays where several players can watch a reply at once
-Added option for minimap to show colors only based on owned(green),ally(blue),enemy(red)

-Added new FBI tag TransportMass, this is the maximum weight a tranporter can transport, default 100000
-Added new FBI tag Mass, this is the weight of the unit, default is metalcost of unit
-Added new FBI tags to set the control variables for aircraft
-Added new FBI tag OnlyTargetCategory<weaponnum>, sort of like badtargetcategory but the weapon can only target units with these categories,toairweapon in weaponfile is the same as OnlyTargetCategory=VTOL; default is all categories
-Added bindings for creating startscripts in lua. See testscript.lua for an example. Note that you can only have one script (must be called testscript.lua), and that it is possible that the interface will change.

-Fixed bugs in the AI interface GetMetal* and GetEnergy* functions, they returned the values of the human player.
-Moved shared functions in the GroupAI callback and GlobalAI callback to a new AICallback interface. For AI developers: This means a small adjustment of your current code will be required to get it compiled again.

-- 0.64b1 --------------------------------------

-Solved troublesome crash bug involving several pointers to a single object
-Fixed crash when calls such as get PIECE_XZ were used in the Create function of a unit

-SetSfxOccupy now updates correctly when unit is being transported
-Added support for aircraft auto landing to repair when health gets low
-Removed autorepair while unit is flying
-When a unit has finished firing a burst the script function EndBurst<weaponnum> will now be called, use this instead of Fire<weaponnum> to switch pod/barrel etc to fire from to lessen friendly fire incidents
-Removed warning message when using transports

-New map smd tag map/voidwater, creates a void where there would normally be water, this is a purely graphical effect,default 0
-New command .give, .give <num> <unitname> gives you num units of type unitname, requires cheats to be on, only use in singleplayer
-Added support for aircraft auto landing to repair when health gets low
-Added command .kick <playername> that server can use to kick out players from game
-Updated global AI interface with GetNumUnitDefs() and GetUnitDefList()

-- 0.63b2 --------------------------------------
-Fixed crash when using new formation move commands
-Script interpreter now warns about incorrect piecenames in scripts instead of crashing

-- 0.63b1 --------------------------------------

-Fixed multi delete error in LosHandler
-Fixed crash with features defined in mapfile

-Fixed problem with having more than one map in the same archive
-Pathmap display (F2) can now also show where selected building can be built (slow though, esp with large maps and buildings)
-Spring now remember the last a: etc prefix used in chat msgs
-Resurrecting a unit now cost half the energy of building it
-Changed so spring can keep several different path precalcs on disk for a single map, this will save recalculations for ppl switching mods a lot

-Added CMD_WAIT command to have units wait for a transport. Can be issued with the W key, can also be used to que up stuff and then releasing the unit to do it.
-Added tag "LIGHT\\SpecularSunColor" to smd file, modifies the color of the specular highlights on units, defaults to LIGHT\\UnitSunColor
-Added tag "isAirBase" to unit FBI file, sets if the units that are being transported will be stunned or not, defaults to 0.
-Spring can now use up to 16 weapons, use Weapon<1-16> and BadTargetCategory<1-16> in fbi file and QueryWeapon<1-16> AimWeapon<1-16> AimFromWeapon<1-16> FireWeapon<1-16> functions in scripts. Primary,secondary etc is aliased to weapon 1-3 but try not to mix old and new names in the same unit.
-Added new fbi tag WeaponSlaveTo<x>=y, this will force the weapon x to use the same target as weapon y (y must be a lower numbered weapon than x). Might be useful for say a mech like unit with many different weapons that should target in the same direction.
-Added new fbi tags WeaponMainDir<1-16> and MaxAngleDif<1-16>. WeaponMainDir is a vector and defines the center direction of a cone in which the weapon can aim while MaxAngleDif defines how broad the cone is. Default WeaponMainDir = 0 0 1; (forward) and MaxAngleDif=360; (can fire everywhere)
-Added support for loading TA:K COB files and implemented the TA:K-specific opcode play-sound.
-Added some simple formation handling to the standard groups, hold alt on move to make them form an approximate square and drag mouse to make them form up behind a front
-Added missile tdf tag wobble, this is in the same as turnrate except that it will turn in random directions instead of toward the target,default 0
-Added missile tdf tag trajectoryHeight, this sets in how high an arc the missile will move toward the target,0 = straight, 1=45 degree above, can be anything>=0, default 0
-New start file tag LimitDgun, if 1 it restricts dgunning to a radius around your startpos, size depending on mapsize, default 0
-New start file tags AllyTeam<x>/StartRect<Top.Bottom,Left,Right>, allows host to define where different allyteams might start on the map, given in the range 0-1 and scaled with map size

-- 0.62b1 --------------------------------------

-Fixed crash when units exploded

-Mines no longer block terrain and can be overrun (and if you are crazy built on top of :) ), removed ability for mines to hold fire since they would be completely undetectable.
-Heightmap view (F1) now updates correctly when terrain changes
-Pathcost view (F2) now works in non cheating mode but will then only show terrain modifiers
-Fixed gunships being able to fly below ground when coming in on step slopes
-Auto sharing now starts at 99% instead of 100% at startup meaning that there is less chance of the team losing resources because someone forgets to set it

-Installer associates .sdf files with spring.exe
-Spring.exe now accepts a demo filename on the commandline and starts playing it directly
-Added tag "MAP\\DetailTex" to smd file to specify map specific detail texture, default is bitmap\detailtex2.bmp, map specific textures should be placed in maps directory.
-Added possibility for surface of units to be shiny/reflective by using the alpha channel of the textures
-Added command .take, will take the units from all teams allied with you that is missing a player (due to disconnect etc) and transfer them to your team

-- 0.61b2 --------------------------------------

-Added tag "WATER\\WaterTexture" to smd file to specify another texture for the map than the default ocean.jpg (mapspecific water textures should be in the maps directory)
-Added tag "WATER\\WaterSurfaceColor" to smd file, changes the surface color of the water when using reflective water (default is 0.75 0.8 0.85 ).
-Fixed error msg while loading certain jpeg files
-Fixed spawn.txt not being readable from archives
-Fixed problem with reading large files from sd7 archives (such as skyboxes)

0.61b1 --------------------------------------

-Fixed crash in weapon aiming
-Fixed some other crashes
-Fixes some memory errors

-Fixed aircraft landing outside map
-Fixed infinitely rotating starburst missiles
-Fixed different tracks getting superimposed on each other
-Plasma cannons should now detect mountains in the fire path better
-The generated path information for maps is now stored zipped in the subdirectory "paths"

-Added scar marks on map to explosions
-Can now control spacing when rowbuilding using the forward/back buttons (xButtons) on mouse
-Spring now attempts to find bmp or jpg files in bitmaps\loadpictures to use as background while loading
-Some messages now show their location on the minimap
-Moved spawn.txt into xta archive by default to allow replacements by other mods, and replaced it with "Storm Easy AI v1.0"
-New weapon tdf value, TargetMoveError, makes the weapon worse at hitting moving targets, default 0
-New weapon tdf value, MovingAccuracy, replaces the normal accuracy value while the unit is moving, defaults to normal accuracy of weapon
-New weapon tdf value, thickness, controls the thickensess of lasers, default 2
-New weapon tdf value, intensity, controls the alpha value of lasers, default 0.9
-New smd terraintype value, receivetracks, if this is 0 there wont be any tracks from units on this terrain type, default 1

-- 0.60b1 --------------------------------------
-Added tag SmoothAnim=0/1 to the fbi. If it is 1, unis will use animation interpolation
-Added MAP\\WATER\\WaterPlaneColor tag to maps to set the color of the waterplane beneath the ground
-Added MAP\\ATMOSPHERE\\SkyBox tag to maps to use a dds cubemap as a sky instead of the cloud rendering
-Added an optional BuildPic tag to the units so they can use other than pcx buildpics
-Fixed crash when unit closed in on their destination

-Slopes are now calculated differently
-Random start order should now be more random
-Fixed committing delayed animations before using getunitval
-Radar now jams everyones radar including your own. We will see how this turns out.
-New map format, adds ability to read texture from external (shared) files and a new terrain type overlay map
-Made ballistic weapons range somewhat more dependent on relative height of weapon and target
-Paralyzer tag for weapons now work
-CanCapture tag for units now works
-HighTrajectory now requires a HighTrajectory tag in the unit file (0=only low, 1=only high,2=choose)
-Ground under DT/FW get same kind of protection as under buildings, we will see how it works out
-"Slow" explosions, stuff far away from big explosions will have their damage delayed a bit

-Added globalai interface and changed groupai interface a bit
-Added resurrection capability
-Added support for regular zip archives. Using extension "sdz" to avoid confusion.
-Added support for 7zip archives. (http://www.7-zip.org/). Using extension "sd7" to avoid confusion. Note that it is probably not a good idea to use solid archives since Spring does not access the files sequentially.
-Added beamlaser type weapon
-Added possibility of units leaving tracks on map
-Added some new options to the game start file
-Added new camera mode rotatable overhead camera
-Added Flare capability to units to decoy incoming missiles

-Added boot option for battleroom host
-Hopefully fixed unit sync and some other bugs in battleroom

-- 0.51b1 --------------------------------------
-Fixed a cob animation related crash
-Fixed crash using "simple formation ai"

-Fixed auto resource sharing, could share to enemies or not at all before
-Floating features now works
-Con air now stay within radius of build object better
-Spectators can no longer select start pos
-Yet more pathfinding/movement fixes
-Fixed timeout when host calculating paths
-New ingame command for host .nopause , prevents player from pausing game. Also harder to spam log with changespeed msgs now.
-Changed targeting calculations, now switch from badtargetcategory targets faster and never switch to them from other sort of targets even if those go out of los/radar
-Changed so several units cant reclaim a feature at once
-Flakker like units can now target fast units better

-- 0.50b2 --------------------------------------

-Changed default DamageModifier to be 1 instead of 0
-Fixed so units no longer can dip slightly into lava and explode
-Adds a random enemies script for those that really want to test in single player. The enemies are read from spawn.txt (frame/enemyname pairs) and spawns at startpos 1-3 randomly

-- 0.50b1 --------------------------------------

-Added support for set/get of unit variable ACTIVATION (allowing solars to close under attack for example)
-Added call to cob function SetMaxReloadTime for better ta unit compatibility
-Added stack boundary checking to cob interpreter
-Fixed buffer overrun problems in the sunparser
-Fixed threads contending over single global netbuf (hopefully no more netmsg errors)
-Now connects from the same udp port as connecting to (semi random before)

-Units can no longer move over infinite sloops
-Improved the pathfinder/movesystem
-Construction units can no longer repair themselves
-Repair/Build/Reclaim now turn of cloak
-Unit paths are now only visible when cheats are enabled in debug mode
-F2 now shows the map from a pathfinding perspective if cheats are enabled
-Fixed grass not being drawn
-Previously visible buildings now stays on the map in a ghost state

-Added support for larger maps
-Added some more range circles
-Added keyboard bindings for mousewheel (pageup/down as default)
-Added guard order to aircraft
-New .smd command MAP/WATER/WaterDamage. Indicates the amount of damage per second units take for being in "water". Make it >1000 to prevent ground units from entering water entirely, >10000 prevents hovers from moving over water.
-New in game commands for host .setmaxspeed .setminspeed sets the maximum and minimum game speeds

-- 0.41b1 --------------------------------------

-Removed log spam with Use of GetPieceDir..
-Fixed "Error looking up server from ip" errors when trying to connect
-Fixed zero size units bug
-Can no longer overflow network buffer with large build orders
-Non square maps no longer crash
-Fixed some random crash bugs
-Added more filtering to the server and clients to filter out invalid net msgs

-Host can now force start the game with ctrl+return if not everyone show up
-Fixed repeat order for factories
-Units can no longer move onto terrain they cant move off (boats on land etc)
-Units can no longer die due to getting outside map
-Changed the alignment on buildings, fixes two buildings qued beside each other ending up blocking each other
-Buildings no longer decay when builder has slow animation
-Radar/jammer units now correctly updated when changing team

-New .smd value, MAP/AutoShowMetal. Set it to 0 if you dont want players to switch to metalmap view when selecting an extractor. (because you have created visible metal deposits on the map)
-New .smd value, MAP/MapHardness. Set how hard it is to create craters in map, default 100.
-Made it slightly easier to build buildings on slopes
-Made it somewhat harder to dig deep holes

-- Lobby changes

-Added extended ascii support in chatrooms. Can now use accent characters
-Added test button to battle host menu. It will tell the server to try and conncet to you on the port you specified, letting you know if others will be able to as well.
-Now can tell which battles are in progress, and which are still open
-Fixed bug that allowed more than the max number of players into a room
-Added ability to launch a browser when links are sent in a chatroom
-Added unit syncing. Now if users have different units, the differences are disabled
-Fixed a bug that allowed invalid log in names to be submitted

-- 0.40b2 --------------------------------------

First public release
