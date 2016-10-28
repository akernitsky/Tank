#ifndef TANKWIN_H
#define TANKWIN_H


#include "DirectDrawWin.h"

class TankWin : public DirectDrawWin
{
public:
	
	int k;
	BOOL right;
	int w;
	int y;
	int t;
	int hh;
	int ww;
	int ww1;
	TankWin();

protected:
	//{{AFX_MSG(TankWin)
	afx_msg void TankWin::OnDestroy() ;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void TankWin::OnTimer(UINT nIDEvent); 
	afx_msg int TankWin::OnCreate(LPCREATESTRUCT lpCreateStruct) ;
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
	void addToSurfaces(IDirectDrawSurface* surface);
	void createSurfaces();
	IDirectDrawSurface* getTerrainSurface();
	IDirectDrawSurface* getProjectileSurface();
	void drawSurface();
	void drawTank();
	void drawTankHull();
	void drawTankTurret();
	std::pair<int, int> calculateCanonsTip(int turretPosition);
	void drawProjectileInPosition(int xx, int yy);
	void drawProjectile();
	void drawExplosion(int xPos, int yPos);
	IDirectDrawSurface* getProjectileSurfaceEraser();

	std::wstring pathname;
	std::vector<std::shared_ptr<IDirectDrawSurface>> surfaces;
	std::map<int, std::wstring> fileNameMapping;

	void RestoreSurfaces();
	LPDIRECTDRAWSURFACE createCustomSurface(const std::wstring& fileName);
};


#endif
