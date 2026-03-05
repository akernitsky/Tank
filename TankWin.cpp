#include "TankWin.h"
#include "DirectDrawApp.h"
#include "DriverDialog.h"
#include "Headers.h"
#include "resource.h"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

extern int timer;

bool bVzr = false;

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
const int kProjectileSpriteHalfWidth = 1;
const int kProjectileSpriteHalfHeight = 1;
const int kExplosionSpriteHalfWidth = 60;
const int kExplosionSpriteHalfHeight = 40;
const int kExplosionStartTime = 1200;
const int kExplosionEndTime = 2280;
const int kExplosionFrameDuration = 40;
const int kExplosionSurfaceStartIndex = 73;
const int kProjectileLaunchTimerThreshold = 10;
const int kProjectileOriginXOffset = 105;
const int kProjectileOriginYOffset = 76;
const int kProjectileTimerDivisor = 3;
const int kCannonTipDistance = 34;
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
const int kInitialFireTimer = 1;
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
  const int xCenter = projectile.originX;
  const int yCenter = projectile.originY;
  const double angle =
      (turretPosition - kDirectionToIndexOffset) * 2 * M_PI / kDirectionCount -
      M_PI / kAngleQuarterTurnDivisor;

  return std::make_pair(
      roundToInt(xCenter + std::cos(angle) * kCannonTipDistance),
      roundToInt(yCenter + std::sin(angle) * kCannonTipDistance));
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

void TankWin::drawExplosion(int xPos, int yPos) {
  if (bVzr && timer >= kExplosionStartTime && timer < kExplosionEndTime) {
    BltSurface(backsurf,
               surfaces[kExplosionSurfaceStartIndex +
                        (timer - kExplosionStartTime) / kExplosionFrameDuration]
                   .get(),
               xPos, yPos,
               TRUE);
  }
}

void TankWin::drawProjectile() {
  if (bVzr) {
    projectile.isFiring = false;
  }

  if (projectile.isFiring) {
    if (timer <= kProjectileLaunchTimerThreshold) {
      projectile.originX = kProjectileOriginXOffset + tank.x;
      projectile.originY = kProjectileOriginYOffset + tank.y;
      projectile.activeDirection = tank.turret.direction;
    }
    const auto canonsTip = calculateCanonsTip(projectile.activeDirection);
    const auto extent = calculateProjectileDistanceInCoordinates(
        projectile.activeDirection, timer / kProjectileTimerDivisor);

    const int projectileXPos = canonsTip.first + extent.first;
    const int projectileYPos = canonsTip.second + extent.second;
    drawProjectileInPosition(projectileXPos, projectileYPos);

    projectile.lastX = projectileXPos;
    projectile.lastY = projectileYPos;
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
    if (!projectile.isTriggered) {
      projectile.isTriggered = true;
    }
    return true;
  case VK_RETURN:
    if (!projectile.isFiring && !bVzr) {
      timer = kInitialFireTimer;
      projectile.originX = kProjectileOriginXOffset + tank.x;
      projectile.originY = kProjectileOriginYOffset + tank.y;
      projectile.activeDirection = tank.turret.direction;
      projectile.isFiring = true;
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
    projectile.isFiring = false;
    if (tank.hullDirection == kDirectionRight) {
      ++tank.x;
    } else {
      RotateHullAndTurretToward(kDirectionRight, kRightMinTurn,
                             kRightMaxTurn, true);
    }
    return true;
  case VK_LEFT:
    projectile.isFiring = false;
    if (tank.hullDirection == kDirectionLeft) {
      --tank.x;
    } else {
      RotateHullAndTurretToward(kDirectionLeft, kLeftMinTurn,
                             kLeftMaxTurn, false);
    }
    return true;
  case VK_UP:
    projectile.isFiring = false;
    if (tank.hullDirection == kDirectionUp) {
      --tank.y;
    } else {
      RotateHullAndTurretToward(kDirectionUp, kUpMinTurn, kUpMaxTurn, true);
    }
    return true;
  case VK_DOWN:
    projectile.isFiring = false;
    if (tank.hullDirection == kDirectionDown) {
      ++tank.y;
    } else {
      RotateHullAndTurretToward(kDirectionDown, kDownMinTurn,
                             kDownMaxTurn, true);
    }
    return true;
  case VK_SPACE:
    projectile.isFiring = false;
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
    projectile.isFiring = false;
    ++tank.turret.direction;
    return true;
  case VK_HOME:
    projectile.isFiring = false;
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
