#ifndef TANKWIN_H
#define TANKWIN_H

#include "DirectDrawWin.h"

class TankWin : public DirectDrawWin {
public:
  struct TurretState {
    int direction = 5;
  };

  struct ProjectileState {
    bool isFiring = false;
    bool isTriggered = false;
    int activeDirection = 5;
    int originX = 0;
    int originY = 0;
    int lastX = 0;
    int lastY = 0;
  };

  struct TankState {
    int x = 100;
    int y = 100;
    int hullDirection = 5;
    TurretState turret;
  };

  int animationTick;
  BOOL isFacingRight;
  TankState tank;
  ProjectileState projectile;
  TankWin();

protected:
  //{{AFX_MSG(TankWin)
  afx_msg void TankWin::OnDestroy();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg int TankWin::OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  BOOL CreateAviSurface();
  BOOL InstallPalette();
  BOOL LoadAvi();
  int SelectDriver();
  BOOL UpdateAviSurface();
  int SelectInitialDisplayMode();
  BOOL CreateCustomSurfaces();
  void DrawScene();
  void addToSurfaces(IDirectDrawSurface *surface);
  void createSurfaces();
  IDirectDrawSurface *getTerrainSurface();
  IDirectDrawSurface *getProjectileSurface();
  void drawSurface();
  void drawTank();
  void drawTankHull();
  void drawTankTurret();
  std::pair<int, int> calculateCanonsTip(int turretPosition);
  void drawProjectileInPosition(int xPos, int yPos);
  void drawProjectile();
  void drawExplosion(int xPos, int yPos);
  IDirectDrawSurface *getProjectileSurfaceEraser();

  std::wstring pathname;
  std::vector<std::shared_ptr<IDirectDrawSurface>> surfaces;
  std::map<int, std::wstring> fileNameMapping;
  int terrainSurfaceIndex = -1;
  int projectileSurfaceIndex = -1;
  int projectileEraserSurfaceIndex = -1;

  void RestoreSurfaces();
  LPDIRECTDRAWSURFACE createCustomSurface(const std::wstring &fileName);
};

#endif
