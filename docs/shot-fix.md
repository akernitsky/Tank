# Projectile firing and muzzle alignment

Enter and Shift launch a projectile from the visible end of the barrel for each of the 16 turret sprites. Moving the tank or turning its turret after launch no longer cancels or redirects the shot.

Flight uses elapsed milliseconds instead of idle-loop iterations. After 1.2 seconds the projectile reaches its impact point; all 32 explosion sprites play for 1.28 seconds, then firing becomes available again. The debug trajectory line is disabled during normal firing.

The Visual Studio project uses 64-bit build tools, consistent dynamic MFC/CRT settings, separate compiler/linker PDB files, and copies bitmap assets into the executable output directory.

## Screenshot

![Projectile impact in the running renderer](media/shot-screenshot.png)

## Animation

![Projectile flight and explosion](media/shot-demo.gif)

These are actual DirectDraw back-buffer captures from a temporary capture build running the same game renderer and firing logic. The GIF combines sampled frames; it is not a desktop screen recording. Capture instrumentation is excluded from the committed game sources.

## Validation

- Debug and Release Win32 builds succeed with Visual Studio 2026.
- Every muzzle coordinate was checked against the outer barrel pixels in its source bitmap.
- Logic checks cover all 16 directions, moving/rotating after launch, repeated shots, fixed impact coordinates and tick-counter wraparound.
- Enter/Shift firing, explosion and repeated firing were exercised through the running game interface.
