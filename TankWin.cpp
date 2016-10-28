#include "Headers.h"
#include "resource.h"
#include "DriverDialog.h"
#include "TankWin.h"
#include "DirectDrawApp.h"


#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dxguid.lib")


extern int timer;
extern int timer1;

bool bOgon = false;
bool bVzr = false;
bool bPul = false;

const DWORD desiredwidth = 2880;
const DWORD desiredheight = 1800;
const DWORD desireddepth = 32;

int xx = 0;
int yy = 0;

int xx1 = 0;
int yy1 = 0;


BEGIN_MESSAGE_MAP(TankWin, DirectDrawWin)
	//{{AFX_MSG_MAP(TankWin)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


TankWin::TankWin()
{
	right=FALSE;	
	k=500;


	t = 100;
	w = 100;
	y = 5;
	hh = 5;
	bOgon = false;
}

LPDIRECTDRAWSURFACE TankWin::createCustomSurface(const std::wstring& fileName)
{
	LPDIRECTDRAWSURFACE surf = CreateSurface(fileName);
	
	if(surf == 0)
	{
		const std::wstring errorText = std::wstring(L"failed to load: " + fileName);
		Fatal(errorText.c_str());
	}
	return surf;
}
void TankWin::addToSurfaces(IDirectDrawSurface* surface)
{
	assert(surface != nullptr);
	
	if (surface != nullptr)
	{	
		DDCOLORKEY colorkey;
		colorkey.dwColorSpaceLowValue = 0;
		colorkey.dwColorSpaceHighValue = 0;
		surface->SetColorKey(DDCKEY_SRCBLT, &colorkey);
		
		surfaces.push_back(std::shared_ptr<IDirectDrawSurface>(surface, std::mem_fn(&IUnknown::Release)));
	}
}

void TankWin::createSurfaces()
{
	fileNameMapping = { 
		std::make_pair(0, L"tankback.bmp"),
		std::make_pair(1, L"tankright1.bmp"),
		std::make_pair(2, L"tankrightback.bmp"),
		std::make_pair(3, L"tankright2.bmp"),
		std::make_pair(4, L"tankright3.bmp"),
		std::make_pair(5, L"tankright4.bmp"),
		std::make_pair(6, L"tankright6.bmp"),
		std::make_pair(7, L"tankright7.bmp"),
		std::make_pair(8, L"tankfront.bmp"),
		std::make_pair(9, L"tankleft4.bmp"),
		std::make_pair(10, L"tankleftfront.bmp"),
		std::make_pair(11, L"tankleft3.bmp"),
		std::make_pair(12, L"tankleft.bmp"),
		std::make_pair(13, L"tankleft23.bmp"),
		std::make_pair(14, L"tankleftback.bmp"),
		std::make_pair(15, L"tankleft1.bmp"),
		std::make_pair(16, L"1.bmp"),
		std::make_pair(17, L"2.bmp"),
		std::make_pair(18, L"3.bmp"),
		std::make_pair(19, L"4.bmp"),
		std::make_pair(20, L"5.bmp"),
		std::make_pair(21, L"6.bmp"),
		std::make_pair(22, L"7.bmp"),
		std::make_pair(23, L"8.bmp"),
		std::make_pair(24, L"9.bmp"),
		std::make_pair(25, L"10.bmp"),
		std::make_pair(26, L"11.bmp"),
		std::make_pair(27, L"12.bmp"),
		std::make_pair(28, L"13.bmp"),
		std::make_pair(29, L"14.bmp"),
		std::make_pair(30, L"15.bmp"),
		std::make_pair(31, L"16.bmp"),
		std::make_pair(32, L"ter.bmp"),
		std::make_pair(33, L"trackh.bmp"),
		std::make_pair(34, L"trackv.bmp"),
		std::make_pair(35, L"point.bmp"),
		std::make_pair(36, L"er.bmp"),
		std::make_pair(37, L"tankback.bmp"),
		std::make_pair(38, L"tankright1.bmp"),
		std::make_pair(39, L"tankrightback.bmp"),
		std::make_pair(40, L"tankright2.bmp"),
		std::make_pair(41, L"tankright3.bmp"),
		std::make_pair(42, L"tankright4.bmp"),
		std::make_pair(43, L"tankright6.bmp"),
		std::make_pair(44, L"tankright7.bmp"),
		std::make_pair(45, L"tankfront.bmp"),
		std::make_pair(46, L"tankleft4.bmp"),
		std::make_pair(47, L"tankleftfront.bmp"),
		std::make_pair(48, L"tankleft3.bmp"),
		std::make_pair(49, L"tankleft.bmp"),
		std::make_pair(50, L"tankleft23.bmp"),
		std::make_pair(51, L"tankleftback.bmp"),
		std::make_pair(52, L"tankleft1.bmp"),
		std::make_pair(53, L"1.bmp"),
		std::make_pair(54, L"2.bmp"),
		std::make_pair(55, L"3.BMP"),
		std::make_pair(56, L"4.BMP"),
		std::make_pair(57, L"5.BMP"),
		std::make_pair(58, L"6.BMP"),
		std::make_pair(59, L"7.BMP"),
		std::make_pair(60, L"8.BMP"),
		std::make_pair(61, L"9.BMP"),
		std::make_pair(62, L"10.BMP"),
		std::make_pair(63, L"11.BMP"),
		std::make_pair(64, L"12.BMP"),
		std::make_pair(65, L"13.BMP"),
		std::make_pair(66, L"14.BMP"),
		std::make_pair(67, L"15.BMP"),
		std::make_pair(68, L"16.BMP"),
		std::make_pair(69, L"ter.BMP"),
		std::make_pair(70, L"trackh.BMP"),
		std::make_pair(71, L"trackv.BMP"),
		std::make_pair(72, L"point.BMP") 
	};

	for (int i = 101; i <= 132; ++i)
	{
		fileNameMapping.insert(fileNameMapping.end(), std::make_pair(i - 101 + 73, std::to_wstring(i) + L".bmp"));
	}

	for (const auto& element : fileNameMapping)
	{
		IDirectDrawSurface* surface = createCustomSurface(element.second);
		addToSurfaces(surface);
	}
}

BOOL TankWin::CreateCustomSurfaces()
{
	createSurfaces();

	ClearSurface( primsurf, 0 );
	ClearSurface( backsurf, 0 );
	return TRUE;
}

IDirectDrawSurface* TankWin::getTerrainSurface()
{
	return surfaces[32].get();
}

IDirectDrawSurface* TankWin::getProjectileSurface()
{
	return surfaces[35].get();
}

IDirectDrawSurface* TankWin::getProjectileSurfaceEraser()
{
	return surfaces[36].get();
}

void TankWin::drawSurface()
{
	BltSurface(backsurf, getTerrainSurface(), 0, 0, TRUE);
}

void TankWin::drawTank()
{
	drawTankHull();
	drawTankTurret();
}

void TankWin::drawTankHull()
{
	BltSurface(backsurf, surfaces[y - 1].get(), 20 + t, 45 + w, TRUE);
}

void TankWin::drawTankTurret()
{
	BltSurface(backsurf, surfaces[hh + 16 - 1].get(), 20 + t, 42 + w, TRUE);
}

std::pair<int, int> TankWin::calculateCanonsTip(int turretPosition)
{
	const int xCenter = xx;
	const int yCenter = yy;
	const int height = 54;
	const int width = 74;

	const double angle = (turretPosition - 1) * 2 * M_PI / 16 - M_PI / 2;

	return std::make_pair(static_cast<int>(xCenter + std::cos(angle) * width / 2), static_cast<int>(yCenter + std::sin(angle) * height / 2));
}

static std::pair<int, int> calculateProjectileDistanceInCoordinates(int turretPosition, int time)
{
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

void TankWin::drawProjectileInPosition(int xPos, int yPos)
{
	BltSurface(backsurf, getProjectileSurface(), xPos - 37, yPos, FALSE);
}

void TankWin::drawExplosion(int xPos, int yPos)
{
	if (bVzr && timer < 2280)
	{
		BltSurface(backsurf, surfaces[73 + (timer - 1200) / 40].get(), xPos, yPos, TRUE);
	}
}

void TankWin::drawProjectile()
{
	if (bOgon)
	{
		if (timer <= 10)
		{
			xx = 105 + t;
			yy = 76 + w;
			ww = hh;
		}
		const auto canonsTip = calculateCanonsTip(ww);
		const auto extent = calculateProjectileDistanceInCoordinates(ww, timer / 3);

		const int projectileXPos = canonsTip.first + extent.first;
		const int projectileYPos = canonsTip.second + extent.second;
		drawProjectileInPosition(projectileXPos, projectileYPos);

		lastProjectileXPos = projectileXPos;
		lastProjectileYPos = projectileYPos;
	}
	drawExplosion(lastProjectileXPos - 37, lastProjectileYPos - 37);
}

void TankWin::DrawScene()
{
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

void TankWin::RestoreSurfaces()
{
	for (size_t index = 0; index < surfaces.size(); ++index)
	{
		if (surfaces[index]->IsLost() != FALSE)
		{
			surfaces[index]->Restore();
			LoadSurface(surfaces[index].get(), fileNameMapping[index].c_str());
		}
	}
}

int TankWin::SelectDriver()
{
	int numdrivers=GetNumDrivers();
	if (numdrivers==1)
		return 0;

	CArray<CString, CString> drivers;
	for (int i=0;i<numdrivers;i++)
	{
		LPSTR desc, name;
		GetDriverInfo( i, 0, &desc, &name );
		drivers.Add(desc);
	}

	DriverDialog dialog;
	dialog.SetContents( &drivers );
	if (dialog.DoModal()!=IDOK)
		return -1;

	return dialog.GetSelection();
}

int TankWin::SelectInitialDisplayMode()
{
	DWORD curdepth=GetDisplayDepth();
	int i, nummodes=GetNumDisplayModes();
	DWORD wd,h,d;

	if (curdepth!=desireddepth)
		ddraw2->SetDisplayMode( 2880, 1800, curdepth, 0, 0 );

	for (i=0;i<nummodes;i++)
	{
		GetDisplayModeDimensions( i, wd, h, d );
		if (w==desiredwidth && h==desiredheight && d==desireddepth)
			return i;
	}

	for (i=0;i<nummodes;i++)
	{
		GetDisplayModeDimensions( i, wd, h, d );
		if (d==desireddepth)
			return i;
	}

	return 0;
}

void TankWin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_SHIFT && bPul==false)
	{
		//timer=0;
		bPul=true;
	}

	if(nChar == VK_RETURN && bOgon == false)
	{
		timer = 1;
		bOgon = true;
	}
	
	if (nChar == VK_ESCAPE)
	{
		PostMessage( WM_CLOSE );
	}
	
	if (nChar == VK_SPACE)
	{
		y++;
		hh++;
	}
	
	if(nChar==VK_RIGHT) 
	{
		bOgon=false;
		if(y==5)
		{
			t=t+1;
		}
		else
		{
			if(y>=6 && y<=13 )
			{
				y--;
				hh--;
			}
			else
			{
				y++; 
				hh++; 
			} 
		}
	}
	
	if(nChar==VK_LEFT) 
	{
		bOgon=false;
		if(y==13)
		{
			t=t-1;
		} 
		else
		{
			if(y>=5 && y<=12)
			{
				y++;
				hh++;
			}
			else
			{
				y--;
				hh--;
			} 
		}
	}
	
	if(nChar==VK_UP)
	{
		bOgon=false;
		if(y==1)
		{
			w=w-1;
		} 
		else
		{
			if(y>=2 && y<=9)
			{
				y--;
				hh--;	
			}
			else
			{
				y++;
				hh++;
			} 
		}
	}	
	
	if(nChar==VK_DOWN)
	{
		bOgon=false;
		if(y==9)
		{
			w=w+1;
		} 
		else
		{
			if(y>=10 && y<=16)
			{
				y--;
				hh--;
		
			}
			else
			{
				y++;
				hh++;
			} 
		}
	}	
	
	if (nChar==VK_END)
	{
		bOgon=false;
		hh++; 
		if(hh>=17)
		{
			hh=1;
		}
	}
	if (nChar==VK_HOME)
	{
		bOgon=false;
		hh--; 
		if(hh<=0)
		{
			hh=hh+16;
		}
	}

		if(hh>=17)
	{
		hh=hh-16;
	}
	
	if(hh<=0)
	{
		hh=hh+16;
	}
	if(y>=17)
	{
		y=y-16;
	}
	if(y<=0)
	{
		y=y+16;
	}
	DirectDrawWin::OnKeyDown(nChar, nRepCnt, nFlags);
}
