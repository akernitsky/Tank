#include "TankWin.h"
#include "DirectDrawApp.h"
#include "DriverDialog.h"
#include "Headers.h"
#include "resource.h"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

const DWORD desiredwidth = 2880;
const DWORD desiredheight = 1800;
const DWORD desireddepth = 32;

namespace {
const int kMinDirection = 1;
const int kMaxDirection = 16;
const int kDirectionCount = 16;
const int kAnimationTickMs = 500;
const int kTankSpriteXOffset = 20;
const int kTankHullSpriteYOffset = 45;
const int kTankTurretSpriteYOffset = 42;
const int kTurretSpriteIndexOffset = 16;
const int kDirectionToIndexOffset = 1;
// Muzzle centers in each 100x75 turret bitmap (1.bmp through 16.bmp).
// These sprites are hand-drawn, so a common radius does not match every barrel.
const std::pair<int, int> kMuzzleOffsets[kDirectionCount] = {
    {50, 3}, {61, 5}, {74, 13}, {80, 22},
    {84, 36}, {82, 44}, {77, 55}, {59, 67},
    {51, 69}, {41, 67}, {22, 55}, {17, 44},
    {15, 36}, {19, 22}, {25, 13}, {39, 5}};
const int kProjectileSpriteHalfWidth = 1;
const int kProjectileSpriteHalfHeight = 1;
const int kExplosionSpriteHalfWidth = 60;
const int kExplosionSpriteHalfHeight = 40;
const int kExplosionStartTime = 1200;
const int kExplosionEndTime = 2480;
const int kExplosionFrameDuration = 40;
const int kExplosionSurfaceStartIndex = 73;
const int kProjectileTimerDivisor = 3;
const int kAngleQuarterTurnDivisor = 2;
const int kProjectileSpeedDivisor = 2;
const int kUppercaseFileNameStart = 18;
const int kExplosionStartFrame = 101;
const int kExplosionEndFrame = 132;
const int kSingleDriverIndex = 1;
const int kDefaultDisplayModeIndex = 0;
const int kColorKeyValue = 0;
const int kDisplayModeSetFlags = 0;
const int kShiftCloseCode = -1;
const int kDirectionRight = 5;
const int kDirectionLeft = 13;
const int kDirectionUp = 1;
const int kDirectionDown = 9;
const int kRightMinTurn = 6;
const int kRightMaxTurn = 13;
const int kLeftMinTurn = 5;
const int kLeftMaxTurn = 12;
const int kUpMinTurn = 2;
const int kUpMaxTurn = 9;
const int kDownMinTurn = 10;
const int kDownMaxTurn = 16;

void normalizeDirection(int &direction) {
  while (direction > kMaxDirection) {
    direction -= kDirectionCount;
  }
  while (direction < kMinDirection) {
    direction += kDirectionCount;
  }
}

int roundToInt(double value) {
  return static_cast<int>(::lround(value));
}
} // namespace

BEGIN_MESSAGE_MAP(TankWin, DirectDrawWin)
//{{AFX_MSG_MAP(TankWin)
ON_WM_KEYDOWN()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TankWin::TankWin() {
  isFacingRight = FALSE;
  animationTick = kAnimationTickMs;

  projectile.isFiring = false;
}

LPDIRECTDRAWSURFACE TankWin::createCustomSurface(const std::wstring &fileName) {
  LPDIRECTDRAWSURFACE surf = CreateSurface(fileName);

  if (surf == nullptr) {
    const std::wstring errorText = std::wstring(L"failed to load: " + fileName);
    Fatal(errorText.c_str());
  }
  return surf;
}
void TankWin::addToSurfaces(IDirectDrawSurface *surface) {
  assert(surface != nullptr);

  if (surface != nullptr) {
    DDCOLORKEY colorkey;
    colorkey.dwColorSpaceLowValue = kColorKeyValue;
    colorkey.dwColorSpaceHighValue = kColorKeyValue;
    surface->SetColorKey(DDCKEY_SRCBLT, &colorkey);

    surfaces.push_back(std::shared_ptr<IDirectDrawSurface>(
        surface, std::mem_fn(&IUnknown::Release)));
  }
}

void TankWin::createSurfaces() {
  const std::vector<std::wstring> staticSurfaceFiles = {L"tankback.bmp",
                                                        L"tankright1.bmp",
                                                        L"tankrightback.bmp",
                                                        L"tankright2.bmp",
                                                        L"tankright3.bmp",
                                                        L"tankright4.bmp",
                                                        L"tankright6.bmp",
                                                        L"tankright7.bmp",
                                                        L"tankfront.bmp",
                                                        L"tankleft4.bmp",
                                                        L"tankleftfront.bmp",
                                                        L"tankleft3.bmp",
                                                        L"tankleft.bmp",
                                                        L"tankleft23.bmp",
                                                        L"tankleftback.bmp",
                                                        L"tankleft1.bmp",
                                                        L"1.bmp",
                                                        L"2.bmp",
                                                        L"3.bmp",
                                                        L"4.bmp",
                                                        L"5.bmp",
                                                        L"6.bmp",
                                                        L"7.bmp",
                                                        L"8.bmp",
                                                        L"9.bmp",
                                                        L"10.bmp",
                                                        L"11.bmp",
                                                        L"12.bmp",
                                                        L"13.bmp",
                                                        L"14.bmp",
                                                        L"15.bmp",
                                                        L"16.bmp",
                                                        L"ter.bmp",
                                                        L"trackh.bmp",
                                                        L"trackv.bmp",
                                                        L"point.bmp",
                                                        L"er.bmp"};

  fileNameMapping.clear();
  terrainSurfaceIndex = -1;
  projectileSurfaceIndex = -1;
  projectileEraserSurfaceIndex = -1;
  for (size_t i = 0; i < staticSurfaceFiles.size(); ++i) {
    fileNameMapping[static_cast<int>(i)] = staticSurfaceFiles[i];
  }

  const int upperCaseOffset = static_cast<int>(fileNameMapping.size());
  for (size_t i = 0; i < staticSurfaceFiles.size() - 1; ++i) {
    std::wstring name = staticSurfaceFiles[i];
    if (i >= kUppercaseFileNameStart) {
      for (auto &ch : name) {
        if (ch >= L'a' && ch <= L'z') {
          ch = ch - L'a' + L'A';
        }
      }
    }
    fileNameMapping[upperCaseOffset + static_cast<int>(i)] = name;
  }

  const int explosionOffset = static_cast<int>(fileNameMapping.size());
  for (int i = kExplosionStartFrame; i <= kExplosionEndFrame; ++i) {
    fileNameMapping[explosionOffset + i - kExplosionStartFrame] =
        std::to_wstring(i) + L".bmp";
  }

  for (const auto &element : fileNameMapping) {
    IDirectDrawSurface *surface = createCustomSurface(element.second);
    addToSurfaces(surface);

    if (element.second == L"ter.bmp") {
      terrainSurfaceIndex = static_cast<int>(surfaces.size()) - 1;
    } else if (element.second == L"point.bmp") {
      projectileSurfaceIndex = static_cast<int>(surfaces.size()) - 1;
    } else if (element.second == L"er.bmp") {
      projectileEraserSurfaceIndex = static_cast<int>(surfaces.size()) - 1;
    }
  }
}

BOOL TankWin::CreateCustomSurfaces() {
  createSurfaces();

  ClearSurface(primsurf, 0);
  ClearSurface(backsurf, 0);
  return TRUE;
}

IDirectDrawSurface *TankWin::getTerrainSurface() {
  assert(terrainSurfaceIndex >= 0 &&
         terrainSurfaceIndex < static_cast<int>(surfaces.size()));
  return surfaces[terrainSurfaceIndex].get();
}

IDirectDrawSurface *TankWin::getProjectileSurface() {
  assert(projectileSurfaceIndex >= 0 &&
         projectileSurfaceIndex < static_cast<int>(surfaces.size()));
  return surfaces[projectileSurfaceIndex].get();
}

IDirectDrawSurface *TankWin::getProjectileSurfaceEraser() {
  assert(projectileEraserSurfaceIndex >= 0 &&
         projectileEraserSurfaceIndex < static_cast<int>(surfaces.size()));
  return surfaces[projectileEraserSurfaceIndex].get();
}

void TankWin::drawSurface() {
  BltSurface(backsurf, getTerrainSurface(), 0, 0, TRUE);
}

void TankWin::drawTank() {
  drawTankHull();
  drawTankTurret();
}

void TankWin::drawTankHull() {
  BltSurface(backsurf, surfaces[tank.hullDirection - kDirectionToIndexOffset].get(),
             kTankSpriteXOffset + tank.x, kTankHullSpriteYOffset + tank.y, TRUE);
}

void TankWin::drawTankTurret() {
  BltSurface(backsurf,
             surfaces[tank.turret.direction + kTurretSpriteIndexOffset -
                      kDirectionToIndexOffset]
                 .get(),
             kTankSpriteXOffset + tank.x, kTankTurretSpriteYOffset + tank.y,
             TRUE);
}

std::pair<int, int> TankWin::calculateCanonsTip(int turretPosition) {
  const auto &muzzle = kMuzzleOffsets[turretPosition - kDirectionToIndexOffset];
  return std::make_pair(projectile.originX + muzzle.first,
                        projectile.originY + muzzle.second);
}

static std::pair<int, int>
calculateProjectileDistanceInCoordinates(int turretPosition, int time) {
  const double angle =
      (turretPosition - kDirectionToIndexOffset) * 2 * M_PI / kDirectionCount -
      M_PI / kAngleQuarterTurnDivisor;
  const auto xExtent = std::cos(angle) * time / kProjectileSpeedDivisor;
  const auto yExtent = std::sin(angle) * time / kProjectileSpeedDivisor;

  return std::make_pair(roundToInt(xExtent), roundToInt(yExtent));
}

void TankWin::drawProjectileInPosition(int xPos, int yPos) {
  BltSurface(backsurf, getProjectileSurface(),
             xPos - kProjectileSpriteHalfWidth,
             yPos - kProjectileSpriteHalfHeight, TRUE);
}

void TankWin::drawDebugLineBetweenPoints(int startX, int startY, int endX,
                                         int endY) {
  const int deltaX = endX - startX;
  const int deltaY = endY - startY;

  const int steps = (std::max)(std::abs(deltaX), std::abs(deltaY));

  if (steps == 0) {
    drawProjectileInPosition(startX, startY);
    return;
  }

  for (int i = 0; i <= steps; ++i) {
    const double t = static_cast<double>(i) / static_cast<double>(steps);
    const int x = roundToInt(startX + deltaX * t);
    const int y = roundToInt(startY + deltaY * t);
    drawProjectileInPosition(x, y);
  }
}

void TankWin::drawExplosion(int xPos, int yPos) {
  const DWORD elapsed = GetTickCount() - projectile.startedAt;
  if (projectile.isFiring && elapsed >= kExplosionStartTime && elapsed < kExplosionEndTime) {
    BltSurface(backsurf,
               surfaces[kExplosionSurfaceStartIndex +
                        (elapsed - kExplosionStartTime) / kExplosionFrameDuration]
                   .get(),
               xPos, yPos,
               TRUE);
  }
}

void TankWin::drawProjectile() {
  if (!projectile.isFiring) {
    return;
  }
  const DWORD elapsed = GetTickCount() - projectile.startedAt;
  if (elapsed >= kExplosionEndTime) {
    projectile.isFiring = false;
    return;
  }
  // Freeze the impact point even when rendering skips the end of the flight.
  const int flightTime = static_cast<int>(
      (std::min)(elapsed, static_cast<DWORD>(kExplosionStartTime)));
  const auto canonsTip = calculateCanonsTip(projectile.activeDirection);
  const auto extent = calculateProjectileDistanceInCoordinates(
      projectile.activeDirection, flightTime / kProjectileTimerDivisor);
  projectile.lastX = canonsTip.first + extent.first;
  projectile.lastY = canonsTip.second + extent.second;
  if (elapsed < kExplosionStartTime) {
    drawProjectileInPosition(projectile.lastX, projectile.lastY);
  }
  if (projectile.keepDebugLine) {
    drawDebugLineBetweenPoints(canonsTip.first, canonsTip.second,
                               projectile.lastX, projectile.lastY);
  }
  drawExplosion(projectile.lastX - kExplosionSpriteHalfWidth,
                projectile.lastY - kExplosionSpriteHalfHeight);
}

void TankWin::DrawScene() {
  ClearSurface(backsurf, 0);
  CRect client;
  GetClientRect(client);
  drawSurface();
  drawTank();

  drawProjectile();

  ///	primsurf->Flip( 0, DDFLIP_WAIT );
  CRect screenRect = client;
  ClientToScreen(screenRect);
  primsurf->Blt(&screenRect, backsurf, &client, DDBLT_WAIT, 0);
}

void TankWin::RestoreSurfaces() {
  for (size_t index = 0; index < surfaces.size(); ++index) {
    if (surfaces[index]->IsLost() != FALSE) {
      surfaces[index]->Restore();
      LoadSurface(surfaces[index].get(), fileNameMapping[index].c_str());
    }
  }
}

int TankWin::SelectDriver() {
  int numdrivers = GetNumDrivers();
  if (numdrivers == kSingleDriverIndex)
    return kDefaultDisplayModeIndex;

  CArray<CString, CString> drivers;
  for (int i = 0; i < numdrivers; i++) {
    LPSTR desc, name;
    GetDriverInfo(i, 0, &desc, &name);
    drivers.Add(desc);
  }

  DriverDialog dialog;
  dialog.SetContents(&drivers);
  if (dialog.DoModal() != IDOK)
    return kShiftCloseCode;

  return dialog.GetSelection();
}

int TankWin::SelectInitialDisplayMode() {
  DWORD curdepth = GetDisplayDepth();
  int i, nummodes = GetNumDisplayModes();
  DWORD wd, h, d;

  if (curdepth != desireddepth)
    ddraw2->SetDisplayMode(desiredwidth, desiredheight, curdepth,
                           kDisplayModeSetFlags, kDisplayModeSetFlags);

  for (i = 0; i < nummodes; i++) {
    GetDisplayModeDimensions(i, wd, h, d);
    if (wd == desiredwidth && h == desiredheight && d == desireddepth)
      return i;
  }

  for (i = 0; i < nummodes; i++) {
    GetDisplayModeDimensions(i, wd, h, d);
    if (d == desireddepth)
      return i;
  }

  return kDefaultDisplayModeIndex;
}

void TankWin::RotateHullAndTurretToward(int targetDirection, int minTurn,
                                       int maxTurn,
                                       bool decrementInRange) {
  if (tank.hullDirection == targetDirection) {
    return;
  }

  const int directionDelta =
      (tank.hullDirection >= minTurn && tank.hullDirection <= maxTurn)
          ? (decrementInRange ? -kDirectionToIndexOffset
                              : kDirectionToIndexOffset)
          : (decrementInRange ? kDirectionToIndexOffset
                              : -kDirectionToIndexOffset);
  tank.hullDirection += directionDelta;
  tank.turret.direction += directionDelta;
}

bool TankWin::HandleFireKeys(UINT nChar) {
  switch (nChar) {
  case VK_SHIFT:
  case VK_RETURN:
    if (!projectile.isFiring ||
        GetTickCount() - projectile.startedAt >= kExplosionEndTime) {
      projectile.startedAt = GetTickCount();
      projectile.originX = kTankSpriteXOffset + tank.x;
      projectile.originY = kTankTurretSpriteYOffset + tank.y;
      projectile.activeDirection = tank.turret.direction;
      projectile.isFiring = true;
      projectile.keepDebugLine = false;
      const auto tip = calculateCanonsTip(projectile.activeDirection);
      projectile.lastX = tip.first;
      projectile.lastY = tip.second;
    }
    return true;
  default:
    return false;
  }
}

bool TankWin::HandleSystemKeys(UINT nChar) {
  if (nChar == VK_ESCAPE) {
    PostMessage(WM_CLOSE);
    return true;
  }
  return false;
}

bool TankWin::HandleHullMovementKey(UINT nChar) {
  switch (nChar) {
  case VK_RIGHT:
    if (tank.hullDirection == kDirectionRight) {
      ++tank.x;
    } else {
      RotateHullAndTurretToward(kDirectionRight, kRightMinTurn,
                             kRightMaxTurn, true);
    }
    return true;
  case VK_LEFT:
    if (tank.hullDirection == kDirectionLeft) {
      --tank.x;
    } else {
      RotateHullAndTurretToward(kDirectionLeft, kLeftMinTurn,
                             kLeftMaxTurn, false);
    }
    return true;
  case VK_UP:
    if (tank.hullDirection == kDirectionUp) {
      --tank.y;
    } else {
      RotateHullAndTurretToward(kDirectionUp, kUpMinTurn, kUpMaxTurn, true);
    }
    return true;
  case VK_DOWN:
    if (tank.hullDirection == kDirectionDown) {
      ++tank.y;
    } else {
      RotateHullAndTurretToward(kDirectionDown, kDownMinTurn,
                             kDownMaxTurn, true);
    }
    return true;
  case VK_SPACE:
    ++tank.hullDirection;
    ++tank.turret.direction;
    return true;
  default:
    return false;
  }
}

bool TankWin::HandleTurretKey(UINT nChar) {
  switch (nChar) {
  case VK_END:
    ++tank.turret.direction;
    return true;
  case VK_HOME:
    --tank.turret.direction;
    return true;
  default:
    return false;
  }
}

void TankWin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  HandleFireKeys(nChar) || HandleSystemKeys(nChar) ||
      HandleHullMovementKey(nChar) || HandleTurretKey(nChar);

  normalizeDirection(tank.turret.direction);
  normalizeDirection(tank.hullDirection);
  DirectDrawWin::OnKeyDown(nChar, nRepCnt, nFlags);
}
