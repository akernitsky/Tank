#ifndef TANKWIN_H
#define TANKWIN_H


#include "DirectDrawWin.h"

class TankWin : public DirectDrawWin
{
public:
	
	int k;
	BOOL right;
	int w;
	int t;
	int y;
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

	std::string pathname;
	LPDIRECTDRAWSURFACE surf11;

	void RestoreSurfaces();
	LPDIRECTDRAWSURFACE createCustomSurface(const std::string& fileName);
};


#endif
