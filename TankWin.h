#ifndef TANKWIN_H
#define TANKWIN_H

#include "DirectDrawWin.h"

class TankWin : public DirectDrawWin {
public:
  static constexpr int kDefaultDirection = 5;
  static constexpr int kDefaultTankPosition = 100;

  struct TurretState {
    int direction = kDefaultDirection;
  };

  struct ProjectileState {
    bool isFiring = false;
    bool isTriggered = false;
    int activeDirection = kDefaultDirection;
    int originX = 0;
    int originY = 0;
    int lastX = 0;
    int lastY = 0;
    bool keepDebugLine = false;
  };

  struct TankState {
    int x = kDefaultTankPosition;
    int y = kDefaultTankPosition;
    int hullDirection = kDefaultDirection;
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
  bool HandleFireKeys(UINT nChar);
  bool HandleSystemKeys(UINT nChar);
  bool HandleHullMovementKey(UINT nChar);
  bool HandleTurretKey(UINT nChar);
  void RotateHullAndTurretToward(int targetDirection, int minTurn, int maxTurn,
                                   bool decrementInRange);
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
  void drawDebugLineBetweenPoints(int startX, int startY, int endX, int endY);
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
