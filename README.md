# TV-Guided Missile

## Overview

This repo contains a TV-Guided Missile sample project built in UE4.

Run the project in the target UE4 version and play directly in the editor to test the gameplay.

## Target UE4 Version

4.27.0 (Launcher)

## Input

- WASD controls to move the player
- Mouse x-axis, y-axis to move the camera
- Mouse left-click to fire the projectile
- Mouse right-click to explode the projectile mid-air
- Left-Shift to boost the projectile

## Notes

- Projectile control is more limited than player camera control. This is by design.
- When the projectile is boosted, velocity increases but control decreases.
- Projectile has a life span of 5 seconds by default, after which it explodes automatically.

## Extras

If I had time, the following are what I'd like to add to the project:
- Sound effects
- Camera shake on explosion
- HUD
- Scoring