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

void normalizeDirection(int &direction) {
  while (direction > kMaxDirection) {
    direction -= kDirectionCount;
  }
  while (direction < kMinDirection) {
    direction += kDirectionCount;
  }
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
  animationTick = 500;

  projectile.isFiring = false;
}

LPDIRECTDRAWSURFACE TankWin::createCustomSurface(const std::wstring &fileName) {
  LPDIRECTDRAWSURFACE surf = CreateSurface(fileName);

  if (surf == 0) {
    const std::wstring errorText = std::wstring(L"failed to load: " + fileName);
    Fatal(errorText.c_str());
  }
  return surf;
}
void TankWin::addToSurfaces(IDirectDrawSurface *surface) {
  assert(surface != nullptr);

  if (surface != nullptr) {
    DDCOLORKEY colorkey;
    colorkey.dwColorSpaceLowValue = 0;
    colorkey.dwColorSpaceHighValue = 0;
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
    if (i >= 18) {
      for (auto &ch : name) {
        if (ch >= L'a' && ch <= L'z') {
          ch = ch - L'a' + L'A';
        }
      }
    }
    fileNameMapping[upperCaseOffset + static_cast<int>(i)] = name;
  }

  const int explosionOffset = static_cast<int>(fileNameMapping.size());
  for (int i = 101; i <= 132; ++i) {
    fileNameMapping[explosionOffset + i - 101] = std::to_wstring(i) + L".bmp";
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
  BltSurface(backsurf, surfaces[tank.hullDirection - 1].get(), 20 + tank.x,
             45 + tank.y, TRUE);
}

void TankWin::drawTankTurret() {
  BltSurface(backsurf, surfaces[tank.turret.direction + 16 - 1].get(),
             20 + tank.x, 42 + tank.y, TRUE);
}

std::pair<int, int> TankWin::calculateCanonsTip(int turretPosition) {
  const int xCenter = projectile.originX;
  const int yCenter = projectile.originY;
  const int height = 54;
  const int width = 74;

  const double angle = (turretPosition - 1) * 2 * M_PI / 16 - M_PI / 2;

  return std::make_pair(
      static_cast<int>(xCenter + std::cos(angle) * width / 2),
      static_cast<int>(yCenter + std::sin(angle) * height / 2));
}

static std::pair<int, int>
calculateProjectileDistanceInCoordinates(int turretPosition, int time) {
  const double angle = (turretPosition - 1) * 2 * M_PI / 16 - M_PI / 2;
  auto xExtent = std::cos(angle) * time / 2;
  auto yExtent = std::sin(angle) * time / 2;

  const int height = 54;
  const int width = 74;
  double ratio = static_cast<double>(height) / width;

  xExtent /= ratio;
  yExtent *= ratio;

  return std::make_pair(static_cast<int>(xExtent), static_cast<int>(yExtent));
}

void TankWin::drawProjectileInPosition(int xPos, int yPos) {
  BltSurface(backsurf, getProjectileSurface(), xPos - 37, yPos, TRUE);
}

void TankWin::drawExplosion(int xPos, int yPos) {
  if (bVzr && timer >= 1200 && timer < 2280) {
    BltSurface(backsurf, surfaces[73 + (timer - 1200) / 40].get(), xPos, yPos,
               TRUE);
  }
}

void TankWin::drawProjectile() {
  if (bVzr) {
    projectile.isFiring = false;
  }

  if (projectile.isFiring) {
    if (timer <= 10) {
      projectile.originX = 105 + tank.x;
      projectile.originY = 76 + tank.y;
      projectile.activeDirection = tank.turret.direction;
    }
    const auto canonsTip = calculateCanonsTip(projectile.activeDirection);
    const auto extent = calculateProjectileDistanceInCoordinates(
        projectile.activeDirection, timer / 3);

    const int projectileXPos = canonsTip.first + extent.first;
    const int projectileYPos = canonsTip.second + extent.second;
    drawProjectileInPosition(projectileXPos, projectileYPos);

    projectile.lastX = projectileXPos;
    projectile.lastY = projectileYPos;
  }
  drawExplosion(projectile.lastX - 37, projectile.lastY - 37);
}

void TankWin::DrawScene() {
  ClearSurface(backsurf, 0);
  CRect client;
  GetClientRect(client);
  const int width = client.Width();
  const int height = client.Height();

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
  if (numdrivers == 1)
    return 0;

  CArray<CString, CString> drivers;
  for (int i = 0; i < numdrivers; i++) {
    LPSTR desc, name;
    GetDriverInfo(i, 0, &desc, &name);
    drivers.Add(desc);
  }

  DriverDialog dialog;
  dialog.SetContents(&drivers);
  if (dialog.DoModal() != IDOK)
    return -1;

  return dialog.GetSelection();
}

int TankWin::SelectInitialDisplayMode() {
  DWORD curdepth = GetDisplayDepth();
  int i, nummodes = GetNumDisplayModes();
  DWORD wd, h, d;

  if (curdepth != desireddepth)
    ddraw2->SetDisplayMode(2880, 1800, curdepth, 0, 0);

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

  return 0;
}

void TankWin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  switch (nChar) {
  case VK_SHIFT:
    if (!projectile.isTriggered) {
      projectile.isTriggered = true;
    }
    break;
  case VK_RETURN:
    if (!projectile.isFiring && !bVzr) {
      timer = 1;
      projectile.isFiring = true;
    }
    break;
  case VK_ESCAPE:
    PostMessage(WM_CLOSE);
    break;
  case VK_SPACE:
    ++tank.hullDirection;
    ++tank.turret.direction;
    break;
  case VK_RIGHT:
    projectile.isFiring = false;
    if (tank.hullDirection == 5) {
      ++tank.x;
    } else {
      const int directionDelta =
          (tank.hullDirection >= 6 && tank.hullDirection <= 13) ? -1 : 1;
      tank.hullDirection += directionDelta;
      tank.turret.direction += directionDelta;
    }
    break;
  case VK_LEFT:
    projectile.isFiring = false;
    if (tank.hullDirection == 13) {
      --tank.x;
    } else {
      const int directionDelta =
          (tank.hullDirection >= 5 && tank.hullDirection <= 12) ? 1 : -1;
      tank.hullDirection += directionDelta;
      tank.turret.direction += directionDelta;
    }
    break;
  case VK_UP:
    projectile.isFiring = false;
    if (tank.hullDirection == 1) {
      --tank.y;
    } else {
      const int directionDelta =
          (tank.hullDirection >= 2 && tank.hullDirection <= 9) ? -1 : 1;
      tank.hullDirection += directionDelta;
      tank.turret.direction += directionDelta;
    }
    break;
  case VK_DOWN:
    projectile.isFiring = false;
    if (tank.hullDirection == 9) {
      ++tank.y;
    } else {
      const int directionDelta =
          (tank.hullDirection >= 10 && tank.hullDirection <= 16) ? -1 : 1;
      tank.hullDirection += directionDelta;
      tank.turret.direction += directionDelta;
    }
    break;
  case VK_END:
    projectile.isFiring = false;
    ++tank.turret.direction;
    break;
  case VK_HOME:
    projectile.isFiring = false;
    --tank.turret.direction;
    break;
  default:
    break;
  }

  normalizeDirection(tank.turret.direction);
  normalizeDirection(tank.hullDirection);
  DirectDrawWin::OnKeyDown(nChar, nRepCnt, nFlags);
}
