+++
title = "Running changelog"
[cascade]
  [cascade.params]
    type = "docs"
+++

This is the bleeding-edge changelog **since version 2025.03**, which is still in the pre-release phase.
See [the 2025.03 page]({{% ref "changelogs/changelog-2025-03" %}}) for the upcoming release.

## Caveats

- removed Python bindings for AI. Apparently unmaintained and unused.
- removed `UpdateWeaponVectorsMT`, `UpdateBoundingVolumeMT`, and `AnimationMT` springsettings. These were just in case, but MT seems safe enough after some time live testing.
- removed `/AdvModelShading` command and the `AdvUnitShading` springsetting, the adv mode is now always on. In practice there wasn't any difference since GLSL became mandatory.
- removed `gl.UnitGL4`, `gl.FeatureGL4`, `gl.FeatureShapeGL4`, and `gl.UnitShapeGL4`. These did not do anything.

### Deprecation notice

- `wupget:DrawUnit`, `DrawFeature`, `DrawShield` and `DrawMaterial` are deprecated.
  Nothing changes in behaviour, but they are (and have been for a long time) a bad idea to use as a performance anti-pattern.
  Use shaders as a replacement.

## Features

### Camera callins

- added `wupget:CameraRotationChanged(rotX, rotY, rotZ) → nil`.
- added `wupget:CameraPositionChanged(posX, posY, posZ) → nil`.

### GL debugging

- added `DebugGLReportGroups` boolean springsetting, default true. Shows OpenGL push/pop groups in GL debug.
- the `/debugGL` option now takes an optional numerical argument, 0-15.
  0 and 1 control the whole debug view without touching anything else (i.e. work as before).
  Otherwise values 2-15 are treated as a bitmask: 8 controls stacktraces, 4 report groups, 2 the whole debug enabled/disabled state, 1 ignored.

### New Website

- Slashscreen, in an effort to procrastinate from university finals, completely redid the website.
- Reworked the Api docs.
- New guides.

### New Docs

- Documented RmlUi bindings.

### Unsynced commands

- `group` command `add` no longer selects entire group after execution.
