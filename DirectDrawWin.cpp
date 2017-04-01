#include "Headers.h"
#include "DirectDrawApp.h"
#include "DirectDrawWin.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(DirectDrawWin, CWnd)
	//{{AFX_MSG_MAP(DirectDrawWin)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATEAPP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


long DirectDrawWin::mousex;
long DirectDrawWin::mousey;
UINT DirectDrawWin::mousestate;
DirectDrawWin* DirectDrawWin::thisptr;


BOOL DirectDrawWin::ActivateDisplayMode( int mode, DWORD rate )
{
	if ( mode<0 || mode>=totaldisplaymodes )
		return FALSE;

	DWORD width = displaymode[mode].width;
	DWORD height = displaymode[mode].height;
	DWORD depth = displaymode[mode].depth;
	
	displayrect.left=0;
	displayrect.top=0;
	displayrect.right=width;
	displayrect.bottom=height;
	displaydepth=depth;

	ddraw2->SetDisplayMode( width, height, depth, rate, 0 );
	curdisplaymode = mode;

	TRACE("------------------- %dx%dx%d (%dhz) ---------------\n", width, height, depth, rate);

	if (CreateFlippingSurfaces()==FALSE)
	{
		TRACE("CreateFlippingSurfaces() failed");
		return FALSE;
	}

	StorePixelFormatData();

	if (CreateCustomSurfaces()==FALSE)
	{
		TRACE("CreateCustomSurfaces() failed");
		return FALSE;
	}

	return TRUE;
}

BOOL DirectDrawWin::ActivateDisplayMode( int mode )
{
	if ( mode<0 || mode>=totaldisplaymodes )
		return FALSE;

	DWORD width = displaymode[mode].width;
	DWORD height = displaymode[mode].height;
	DWORD depth = displaymode[mode].depth;
	
	displayrect.left=0;
	displayrect.top=0;
	displayrect.right=width;
	displayrect.bottom=height;
	displaydepth=depth;

	ddraw2->SetDisplayMode( width, height, depth, 0, 0 );
	curdisplaymode = mode;

	TRACE("------------------- %dx%dx%d ----------------------\n", width, height, depth);

	if (CreateFlippingSurfaces()==FALSE)
	{
		TRACE("CreateFlippingSurfaces() failed");
		return FALSE;
	}

	StorePixelFormatData();

	if (CreateCustomSurfaces()==FALSE)
	{
		TRACE("CreateCustomSurfaces() failed");
		return FALSE;
	}

	return TRUE;
}

void DirectDrawWin::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	CFrameWnd::GetClientRect(&clientrect);
	CFrameWnd::ClientToScreen(&clientrect);

	if (videobacksurf)
		return;

	DDSURFACEDESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = clientrect.Width();
	desc.dwHeight = clientrect.Height();
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
		DDSCAPS_SYSTEMMEMORY;
	if (backsurf)
		backsurf->Release(), backsurf = 0;

	HRESULT r = ddraw2->CreateSurface(&desc, &backsurf, 0);
	if (r != DD_OK)
	{
		TRACE("failed to create 'backsurf'\n");
		return;
	}
	else
		TRACE("backsurf w=%d h=%d\n",
			clientrect.Width(), clientrect.Height());

}

BOOL DirectDrawWin::BltSurface( LPDIRECTDRAWSURFACE destsurf, LPDIRECTDRAWSURFACE srcsurf, int x, int y, BOOL srccolorkey )
{
	if (destsurf==0 || srcsurf==0)
		return FALSE;

	BOOL use_fastblt=TRUE;

	DDSURFACEDESC destsurfdesc;
	ZeroMemory( &destsurfdesc, sizeof(destsurfdesc) );
	destsurfdesc.dwSize = sizeof(destsurfdesc);
	destsurf->GetSurfaceDesc( &destsurfdesc );

	CRect destrect;
	destrect.left=0;
	destrect.top=0;
	destrect.right=destsurfdesc.dwWidth;
	destrect.bottom=destsurfdesc.dwHeight;

	DDSURFACEDESC srcsurfdesc;
	ZeroMemory( &srcsurfdesc, sizeof(srcsurfdesc) );
	srcsurfdesc.dwSize = sizeof(srcsurfdesc);
	srcsurf->GetSurfaceDesc( &srcsurfdesc );

	CRect srcrect;
	srcrect.left=0;
	srcrect.top=0;
	srcrect.right=srcsurfdesc.dwWidth;
	srcrect.bottom=srcsurfdesc.dwHeight;
	
	// make sure that there is something to do...
	if (x+srcrect.left>=destrect.right)
		return FALSE;
	if (y+srcrect.top>=destrect.bottom)
		return FALSE;
	if (x+srcrect.right<=destrect.left)
		return FALSE;
	if (y+srcrect.bottom<=destrect.top)
		return FALSE;

	// clip the source rect if nessesary
	if (x+srcrect.right>destrect.right)
		srcrect.right-=x+srcrect.right-destrect.right;
	if (y+srcrect.bottom>destrect.bottom)
		srcrect.bottom-=y+srcrect.bottom-destrect.bottom;

	CRect dr;
	if (x<0)
	{
		srcrect.left=-x;
		x=0;
		dr.left=x;
		dr.top=y;
		dr.right=x+srcrect.Width();
		dr.bottom=y+srcrect.Height();
		use_fastblt=FALSE;
	}

	if (y<0)
	{
		srcrect.top=-y;
		y=0;
		dr.left=x;
		dr.top=y;
		dr.right=x+srcrect.Width();
		dr.bottom=y+srcrect.Height();
		use_fastblt=FALSE;
	}

	DWORD flags;
	if (use_fastblt)
	{
		flags=DDBLTFAST_WAIT;
		if (srccolorkey)
			flags |= DDBLTFAST_SRCCOLORKEY;
		destsurf->BltFast( x, y, srcsurf, &srcrect, flags );
	}
	else
	{
		flags=DDBLT_WAIT;
		if (srccolorkey)
			flags |= DDBLT_KEYSRC;
		destsurf->Blt( &dr, srcsurf, &srcrect, flags, 0 );
	}
	return TRUE;
}

BOOL DirectDrawWin::ClearSurface( LPDIRECTDRAWSURFACE surf, DWORD clr, RECT* rect )
{
	if (surf==0)
		return FALSE;

	DDBLTFX bltfx;
	ZeroMemory( &bltfx, sizeof(bltfx) );
	bltfx.dwSize = sizeof(bltfx);
	bltfx.dwFillColor = clr;

	HRESULT r;
	r=surf->Blt( rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx );
	if (r==DD_OK)
		return TRUE;

	if (r==DDERR_NOBLTHW && clr==0 && rect==0)
	{
		DDSURFACEDESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		desc.dwSize = sizeof(desc);
		desc.dwFlags=DDSD_WIDTH | DDSD_HEIGHT;
		if (surf->GetSurfaceDesc( &desc )!=DD_OK)
			return FALSE;
		
		int h=desc.dwHeight;

		r=surf->Lock( 0, &desc, DDLOCK_WAIT  | DDLOCK_WRITEONLY, 0 );
		if (r!=DD_OK)
			return FALSE;

		BYTE* surfbits = (BYTE*)desc.lpSurface;

		for (int i=0;i<h;i++)
		{
			ZeroMemory( surfbits, desc.lPitch );
			surfbits += desc.lPitch;
		}
		surf->Unlock( 0 );
	}
	
	return FALSE;
}

BOOL DirectDrawWin::ClearSurface( LPDIRECTDRAWSURFACE surf, DWORD r, DWORD g, DWORD b, RECT* rect )
{
	if (surf==0)
		return FALSE;

	DDBLTFX bltfx;
	ZeroMemory( &bltfx, sizeof(bltfx) );
	bltfx.dwSize = sizeof(bltfx);
	bltfx.dwFillColor = RGBtoPixel( r, g, b );
	HRESULT ret=surf->Blt( rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx );

	return ret==DD_OK;
}

int DirectDrawWin::CompareModes( const void *arg1, const void *arg2 )
{
	DisplayModeInfo* mode1=(DisplayModeInfo*)arg1;
	DisplayModeInfo* mode2=(DisplayModeInfo*)arg2;

	DWORD volume1=mode1->width*mode1->height;
	DWORD volume2=mode2->width*mode2->height;

	if (volume1<volume2)
		return -1;
	else if (volume1>volume2)
		return 1;

	if (mode1->depth<mode2->depth)
		return -1;
	else if (mode1->depth>mode2->depth)
		return 1;
	
	return 0;
}

BOOL DirectDrawWin::Copy_Bmp08_Surface08( LPDIRECTDRAWSURFACE surf, 
										 BYTE* bmpbuf, int w, int h )
{
	if (surf==0 || bmpbuf==0)
		return FALSE;

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	HRESULT r=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
	if (r!=DD_OK)
	{
		TRACE("ShowBmp: Lock() failed\n");
		return FALSE;
	}
	int bytesgiven=(w+3) & ~3;
	BYTE* surfbits = (BYTE*)desc.lpSurface;
	BYTE* imagebits = (BYTE*)(&bmpbuf[(h-1)*bytesgiven]);
	for( int i=0; i<h; i++ )
	{
		memcpy( surfbits, imagebits, w );
		surfbits += desc.lPitch;
		imagebits -= bytesgiven;
	}
	surf->Unlock( 0 );
	return TRUE;
}

BOOL DirectDrawWin::Copy_Bmp24_Surface16( LPDIRECTDRAWSURFACE surf, 
										 BYTE* bmpbuf, int w, int h )
{
	if (surf==0 || bmpbuf==0)
		return FALSE;
	
	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	HRESULT hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("Copy_Bmp24_Surface16: Lock() failed\n");
		return FALSE;
	}
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	BYTE* surfbits = (BYTE*)desc.lpSurface;
	BYTE* imagebits = (BYTE*)(&bmpbuf[(h-1)*bytesgiven]);

	float REDdiv=(float)256/(float)pow( 2, numREDbits );
	float GREENdiv=(float)256/(float)pow( 2, numGREENbits );
	float BLUEdiv=(float)256/(float)pow(2, numBLUEbits );

	for( int i=0; i<h; i++ )
	{
		USHORT* pixptr=(unsigned short*)surfbits;
		RGBTRIPLE* triple=(RGBTRIPLE*)imagebits;
		for (int p=0;p<w;p++)
		{
			float rf=(float)triple->rgbtRed/REDdiv;
			float gf=(float)triple->rgbtGreen/GREENdiv;
			float bf=(float)triple->rgbtBlue/BLUEdiv;

			WORD r=(WORD)((WORD)rf<<loREDbit);
			WORD g=(WORD)((WORD)gf<<loGREENbit);
			WORD b=(WORD)((WORD)bf<<loBLUEbit);
			*pixptr = (WORD)(r|g|b);

			triple++;
			pixptr++;
		}
		surfbits += desc.lPitch;
		imagebits -= bytesgiven;
	}
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::Copy_Bmp24_Surface24( LPDIRECTDRAWSURFACE surf, 
										 BYTE* bmpbuf, int w, int h )
{
	if (surf==0 || bmpbuf==0)
		return FALSE;

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	HRESULT hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("Copy_Bmp24_Surface24: Lock() failed\n");
		return FALSE;
	}
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	BYTE* surfbits = (BYTE*)desc.lpSurface;
	BYTE* imagebits = (BYTE*)(&bmpbuf[(h-1)*bytesgiven]);

	// check if file and surface format are identical
	// if so, use memcpy() to speed things up
	if (loREDbit==16 && loGREENbit==8 && loBLUEbit==0)
	{
		TRACE("using optimized code...\n");
		for (int i=0;i<h;i++)
		{
			memcpy( surfbits, imagebits, bytesrequired );
			surfbits += desc.lPitch;
			imagebits -= bytesgiven;
		}
	}
	else
	{
		TRACE("not using optimated code...\n");
		for(int i=0; i<h; i++ )
		{
			RGBTRIPLE* surf1=(RGBTRIPLE*)surfbits;
			RGBTRIPLE* image=(RGBTRIPLE*)imagebits;
			for (int p=0;p<w;p++)
			{
				DWORD r=image->rgbtRed   << loREDbit;
				DWORD g=image->rgbtGreen << loGREENbit;
				DWORD b=image->rgbtBlue  << loBLUEbit;
				DWORD* data=(DWORD*)surf1;
				*data = r|g|b; 
				surf1++;
				image++;
			}
			surfbits += desc.lPitch;
			imagebits -= bytesgiven;
		}
	}
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::Copy_Bmp24_Surface32( LPDIRECTDRAWSURFACE surf, 
										 BYTE* bmpbuf, int w, int h )
{
	if (surf==0 || bmpbuf==0)
		return FALSE;

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	HRESULT hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("Copy_Bmp24_Surface32: Lock() failed\n");
		return FALSE;
	}
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	BYTE* surfbits = (BYTE*)desc.lpSurface;
	BYTE* imagebits = (BYTE*)(&bmpbuf[(h-1)*bytesgiven]);

	for(int i=0; i<h; i++ )
	{
		DWORD* surf1=(DWORD*)surfbits;
		RGBTRIPLE* image=(RGBTRIPLE*)imagebits;
		for (int p=0;p<w;p++)
		{
			DWORD r=image->rgbtRed   << loREDbit;
			DWORD g=image->rgbtGreen << loGREENbit;
			DWORD b=image->rgbtBlue  << loBLUEbit;
			DWORD* data=(DWORD*)surf1;
			*data = r|g|b; 
			surf1++;
			image++;
		}
		surfbits += desc.lPitch;
		imagebits -= bytesgiven;
	}

	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::Copy_Bmp_Surface( LPDIRECTDRAWSURFACE surf, 
		BITMAPINFOHEADER* bmphdr, BYTE* buf)
{
	if (surf==0 || bmphdr==0 || buf==0)
		return FALSE;
	
	int imagew=bmphdr->biWidth;
	int imageh=bmphdr->biHeight;
	int imagebitdepth=bmphdr->biBitCount;

	BOOL ret=FALSE;
	if (imagebitdepth==8)
	{
		if (displaydepth==8)
			ret=Copy_Bmp08_Surface08( surf, buf, imagew, imageh );
	}
	else if (imagebitdepth==24)
	{
		if (displaydepth==16)
			ret=Copy_Bmp24_Surface16( surf, buf, imagew, imageh );
		else if (displaydepth==24)
			ret=Copy_Bmp24_Surface24( surf, buf, imagew, imageh );
		else if (displaydepth==32)
			ret=Copy_Bmp24_Surface32( surf, buf, imagew, imageh );
	}

	return ret;
}

BOOL DirectDrawWin::Create(const CString& title,int icon)
{
	CString sClassName;

	sClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,LoadCursor(0, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW + 1), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(icon)));

	return CWnd::CreateEx( WS_EX_APPWINDOW, sClassName, title, WS_THICKFRAME | WS_MAXIMIZE | WS_MINIMIZE | WS_OVERLAPPEDWINDOW, 0,0,640,480, 0, 0 );
}

BOOL DirectDrawWin::CreateFlippingSurfaces()
{
	HRESULT r;

	DDSURFACEDESC desc;
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	r = ddraw2->CreateSurface(&desc, &primsurf, 0);
	if (r != DD_OK)
	{
		TRACE("FAILED to create 'primsurf'\n");
		return FALSE;
	}

	r = ddraw2->CreateClipper(0, &clipper, 0);
	if (r != DD_OK)
	{
		TRACE("CreateClipper() failed\n");
		return FALSE;
	}

	r = clipper->SetHWnd(0, GetSafeHwnd());
	if (r != DD_OK)
	{
		TRACE("SetHWnd() failed\n");
		return FALSE;
	}

	r = primsurf->SetClipper(clipper);
	if (r != DD_OK)
	{
		TRACE("SetClipper() failed\n");
		return FALSE;
	}

	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = displayrect.Width();
	desc.dwHeight = displayrect.Height();
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
		DDSCAPS_VIDEOMEMORY;

	r = ddraw2->CreateSurface(&desc, &backsurf, 0);
	if (r != DD_OK)
	{
		TRACE("failed to create 'backsurf' in video\n");
		videobacksurf = false;
	}
	else
	{
		TRACE("Created backsurf in video\n");
		videobacksurf = true;
	}

	return TRUE;
}

BOOL DirectDrawWin::CreatePalette(RGBQUAD* quad, int ncolors)
{
	if (palette)
		palette->Release(), palette=0;
	
	PALETTEENTRY pe[256];
	ZeroMemory( pe, sizeof(pe) );
	for( int i=0; i<ncolors; i++)
	{
		pe[i].peRed   = quad[i].rgbRed;
		pe[i].peGreen = quad[i].rgbGreen;
		pe[i].peBlue  = quad[i].rgbBlue;
	}

	HRESULT r=ddraw2->CreatePalette( DDPCAPS_8BIT | DDPCAPS_ALLOW256, 
								pe, &palette, 0 );
	if (r!=DD_OK)
	{
		TRACE("failed to create DirectDraw palette\n");
		return FALSE;
	}

	primsurf->SetPalette( palette );

	return TRUE;
}

LPDIRECTDRAWSURFACE DirectDrawWin::CreateSurface(const std::wstring& filename, bool installpalette)
{
	int imagew, imageh;
	GetBmpDimensions( filename.c_str(), imagew, imageh );
	
	LPDIRECTDRAWSURFACE surf=CreateSurface( imagew, imageh );
	if (surf==0)
	{
		TRACE("CreateSurface(filename) failed to create surface\n");
		return 0;
	}
	
	std::ifstream bmp( filename, std::ios::binary | std::ios::_Nocreate );
	if (!bmp.is_open())
	{
		TRACE("LoadSurface: cannot open Bmp file\n");
		return 0;
	}

	BITMAPFILEHEADER bmpfilehdr;
	bmp.read( (char*)&bmpfilehdr, sizeof(bmpfilehdr) );
	char* ptr=(char*)&bmpfilehdr.bfType;
	if (*ptr!='B' || *++ptr!='M')
	{
		TRACE("invalid bitmap\n");
		return 0;
	}

	BITMAPINFOHEADER bmpinfohdr;
	bmp.read( (char*)&bmpinfohdr, sizeof(bmpinfohdr) );
	bmp.seekg( sizeof(bmpfilehdr)+bmpinfohdr.biSize, std::ios::beg );
	int imagebitdepth=bmpinfohdr.biBitCount;

	int imagesize=bmpinfohdr.biSizeImage;
	if (imagesize==0)
		imagesize=((imagew*(imagebitdepth/8)+3) & ~3)*imageh;

	if (bmpinfohdr.biCompression!=BI_RGB)
	{
		TRACE("compressed BMP format\n");
		return 0;
	}

	TRACE("loading '%s': width=%d height=%d depth=%d\n", filename, imagew, imageh, imagebitdepth);

	if (imagebitdepth==8)
	{
		int ncolors;
		if (bmpinfohdr.biClrUsed==0)
			ncolors=256;
		else
			ncolors=bmpinfohdr.biClrUsed;

		RGBQUAD* quad=new RGBQUAD[ncolors];
		bmp.read( (char*)quad, sizeof(RGBQUAD)*ncolors );
		if (installpalette)
			CreatePalette( quad, ncolors );
		delete [] quad;
	}

	BYTE* buf=new BYTE[imagesize];
	bmp.read( reinterpret_cast<char*>(buf), imagesize );

	if (!Copy_Bmp_Surface( surf, &bmpinfohdr, buf))
	{
		TRACE("copy failed\n");
		delete [] buf;
		surf->Release();
		return 0;
	}

	delete [] buf;

	return surf;
}

LPDIRECTDRAWSURFACE DirectDrawWin::CreateSurface( DWORD w, DWORD h )
{
	DWORD bytes=w*h*(displaydepth/8);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = w;
	desc.dwHeight = h;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

	LPDIRECTDRAWSURFACE surf;
	HRESULT r=ddraw2->CreateSurface( &desc, &surf, 0 );
	if (r==DD_OK)
	{
		TRACE("CreateSurface(%d,%d) created in video memory (%d bytes)\n", w, h, bytes);
		return surf;
	}

	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	r=ddraw2->CreateSurface( &desc, &surf, 0 );
	if (r==DD_OK)
	{
		TRACE("CreateSurface(%d,%d) allocated in system memory (%d bytes)\n", w, h, bytes);
		return surf;
	}

	TRACE("CreateSurface(%d,%d) failed\n", w, h);
	return 0;
}


DirectDrawWin::DirectDrawWin() :
	videobacksurf(false),
	loREDbit(0), 
	numREDbits(0),
	loGREENbit(0),
	numGREENbits(0),
	loBLUEbit(0),
	numBLUEbits(0)
{
	thisptr=this;

	totaldrivers=0;
	totaldisplaymodes=0;
	curdisplaymode=-1;

	ddraw2=0;
	primsurf=0;
	backsurf=0;
	palette=0;
	clipper = nullptr;

	window_active=TRUE;


    DirectDrawApp* pApp = (DirectDrawApp*)AfxGetApp();
    pApp->ddwin = this;
}

HRESULT WINAPI DirectDrawWin::DisplayModeAvailable( LPDDSURFACEDESC desc, LPVOID p )
{
	DirectDrawWin* win=(DirectDrawWin*)p;

	int& count=win->totaldisplaymodes;
	if (count==MAXDISPLAYMODES)
		return DDENUMRET_CANCEL;

	win->displaymode[count].width  = desc->dwWidth;
	win->displaymode[count].height = desc->dwHeight;
	win->displaymode[count].depth  = desc->ddpfPixelFormat.dwRGBBitCount;

	count++;

	return DDENUMRET_OK;
}

BOOL WINAPI DirectDrawWin::DriverAvailable(LPGUID guid, LPSTR desc, LPSTR name, LPVOID p )
{
	DirectDrawWin* win=(DirectDrawWin*)p;

	if (win->totaldrivers >= MAXDRIVERS)
		return DDENUMRET_CANCEL;

	DriverInfo& info=win->driver[win->totaldrivers];

	if (guid)
	{
		info.guid=(GUID*)new BYTE[sizeof(GUID)];
		memcpy( info.guid, guid, sizeof(GUID) );
	}
	else
		info.guid=0;

	info.desc=_strdup( desc );
	info.name=_strdup( name );

	win->totaldrivers++;

    return DDENUMRET_OK;
}

BOOL DirectDrawWin::GetBmpDimensions( LPCTSTR filename, int& w, int& h)
{
	std::ifstream bmp( filename, std::ios::binary | std::ios::_Nocreate );
	if (!bmp.is_open())
	{
		TRACE("GetBmpDimensions: cannot open file\n");
		return FALSE;
	}

	BITMAPFILEHEADER bmpfilehdr;
	bmp.read( (char*)&bmpfilehdr, sizeof(bmpfilehdr) );

	char* ptr=(char*)&bmpfilehdr.bfType;
	if (*ptr!='B' || *++ptr!='M')
	{
		TRACE("GetBmpDimensions: invalid bitmap\n");
		return FALSE;
	}

	BITMAPINFOHEADER bmpinfohdr;
	bmp.read( (char*)&bmpinfohdr, sizeof(bmpinfohdr) );

	w=bmpinfohdr.biWidth;
	h=bmpinfohdr.biHeight;

	return TRUE;
}

BOOL DirectDrawWin::GetDisplayModeDimensions( int mode, DWORD& width, DWORD& height, DWORD& depth )
{
	if ( mode < 0 || mode > totaldisplaymodes )
		return FALSE;

	width  = displaymode[mode].width;
	height = displaymode[mode].height;
	depth  = displaymode[mode].depth;

	return TRUE;
}

int DirectDrawWin::GetDisplayModeIndex( DWORD w, DWORD h, DWORD d )
{
	for (int i=0;i<totaldisplaymodes;i++)
	{
		DisplayModeInfo& m=displaymode[i];
		if (m.width==w && m.height==h && m.depth==d)
			return i;
	}
	return -1;
}

BOOL DirectDrawWin::GetDriverInfo( int index, LPGUID* guid, LPSTR* desc, LPSTR* name )
{
	if (index<0 || index>totaldrivers)
		return FALSE;

	if (name)
		*name=driver[index].name;
	if (desc)
		*desc=driver[index].desc;
	if (guid)
		*guid=driver[index].guid;

	return TRUE;
}

BOOL DirectDrawWin::GetSurfaceDimensions( LPDIRECTDRAWSURFACE surf, DWORD& w, DWORD& h )
{
	if (surf==0)
		return FALSE;

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	desc.dwFlags=DDSD_WIDTH | DDSD_HEIGHT;

	if (surf->GetSurfaceDesc( &desc )!=DD_OK)
		return FALSE;

	w=desc.dwWidth;
	h=desc.dwHeight;

	return TRUE;
}

BOOL DirectDrawWin::GetSurfaceRect( LPDIRECTDRAWSURFACE surf, RECT& rect)
{
	if (surf==0)
		return FALSE;

	DDSURFACEDESC desc;
	desc.dwSize=sizeof(desc);
	desc.dwFlags=DDSD_WIDTH | DDSD_HEIGHT;

	if (surf->GetSurfaceDesc( &desc )!=DD_OK)
		return FALSE;

	rect.left=0;
	rect.top=0;
	rect.right=desc.dwWidth;
	rect.bottom=desc.dwHeight;

	return TRUE;
}

WORD DirectDrawWin::HighBitPos( DWORD dword )
{
	DWORD test=1;
	test<<=31;
	for (WORD i=0;i<32;i++)
	{
		if ( dword & test )
			return (WORD)(31-i);
		test>>=1;
	}
	return 0;
}

BOOL DirectDrawWin::LoadSurface( LPDIRECTDRAWSURFACE surf, LPCTSTR filename)
{
	std::ifstream bmp( filename, std::ios::binary | std::ios::_Nocreate );
	if (!bmp.is_open())
	{
		TRACE("LoadSurface: cannot open Bmp file\n");
		return FALSE;
	}

	BITMAPFILEHEADER bmpfilehdr;
	bmp.read( (char*)&bmpfilehdr, sizeof(bmpfilehdr) );

	char* ptr=(char*)&bmpfilehdr.bfType;
	if (*ptr!='B' || *++ptr!='M')
	{
		TRACE("LoadSurface: invalid bitmap\n");
		return FALSE;
	}
	
	BITMAPINFOHEADER bmpinfohdr;
	bmp.read( (char*)&bmpinfohdr, sizeof(bmpinfohdr) );
	bmp.seekg( sizeof(bmpfilehdr)+bmpinfohdr.biSize, std::ios::beg );

	int imagew=bmpinfohdr.biWidth;
	int imageh=bmpinfohdr.biHeight;
	int imagebitdepth=bmpinfohdr.biBitCount;

	int imagesize=bmpinfohdr.biSizeImage;
	if (imagesize==0)
		imagesize=((imagew*(imagebitdepth/8)+3) & ~3)*imageh;

	if (bmpinfohdr.biCompression!=BI_RGB)
	{
		TRACE("compressed BMP format\n");
		return 0;
	}

	TRACE("loading '%s': width=%d height=%d depth=%d\n", filename, imagew, imageh, imagebitdepth);

	if (imagebitdepth==8)
	{
		// skip over palette info...
		int ncolors = (bmpinfohdr.biClrUsed==0) ? 256 : bmpinfohdr.biClrUsed;
		bmp.seekg( sizeof(RGBQUAD)*ncolors, std::ios::cur );
	}

	char* buf=new char[imagesize];
	bmp.read( buf, imagesize );

	if (!Copy_Bmp_Surface( surf, &bmpinfohdr, reinterpret_cast<BYTE*>(buf)))
	{
		TRACE("copy failed\n");
		delete [] buf;
		return 0;
	}

	delete [] buf;

	return TRUE;
}

WORD DirectDrawWin::LowBitPos( DWORD dword )
{
	DWORD test=1;
	for (WORD i=0;i<32;i++)
	{
		if ( dword & test )
			return i;
		test<<=1;
	}
	return 0;
}

void DirectDrawWin::OnActivateApp(BOOL bActive, DWORD hTask) 
{
	CWnd::OnActivateApp(bActive, hTask);

	window_active = bActive;
}

int DirectDrawWin::OnCreate(LPCREATESTRUCT)
{
	//DirectDrawEnumerate( DriverAvailable, this );

	//if (totaldrivers==0)
	//{
	//	AfxMessageBox("No DirectDraw drivers detected");
	//	return -1;
	//}

	//// insure the first entry is the primary display driver
	//if (driver[0].guid!=0 && totaldrivers>1)
	//{
	//	for (int i=1;i<totaldrivers;i++)
	//		if (driver[i].guid==0)
	//		{
	//			DriverInfo tmp;

	//			tmp.guid=0;
	//			tmp.desc=driver[i].desc;
	//			tmp.name=driver[i].name;

	//			driver[i].guid=driver[0].guid;
	//			driver[i].desc=driver[0].desc;
	//			driver[i].name=driver[0].name;

	//			driver[0].guid=0;
	//			driver[0].desc=tmp.desc;
	//			driver[0].name=tmp.name;
	//		}
	//}

	//int driverindex=SelectDriver();
	//if (driverindex<0)
	//{
	//	TRACE("No DirectDraw driver selected\n");
	//	return -1;
	//}
	//else if (driverindex>totaldrivers-1)
	//{
	//	AfxMessageBox("Invalid DirectDraw driver selected\n");
	//	return -1;
	//}

	LPDIRECTDRAW ddraw1;
	DirectDrawCreate(0, &ddraw1, 0);
	HRESULT hr = ddraw1->QueryInterface(IID_IDirectDraw2, reinterpret_cast<void**>(&ddraw2));
	if (hr != S_OK)
	{
		AfxMessageBox(L"DirectDraw2 interface not supported");
		return -1;
	}
	ddraw1->Release();
	ddraw1 = 0;

	ddraw2->SetCooperativeLevel(GetSafeHwnd(), DDSCL_NORMAL);

	DetectDisplayMode();

	if (CreateFlippingSurfaces() == FALSE)
	{
		AfxMessageBox(L"CreateFlippingSurfaces() failed");
		return FALSE;
	}
	StorePixelFormatData();
	if (CreateCustomSurfaces() == FALSE)
	{
		AfxMessageBox(L"CreateCustomSurfaces() failed");
		return FALSE;
	}

	return 0;
}

BOOL DirectDrawWin::DetectDisplayMode()
{
	DDSURFACEDESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);

	if (ddraw2->GetDisplayMode(&desc) != DD_OK)
	{
		TRACE("GetDisplayMode() failed\n");
		return FALSE;
	}

	displayrect.left = 0;
	displayrect.top = 0;
	displayrect.right = desc.dwWidth;
	displayrect.bottom = desc.dwHeight;
	displaydepth = desc.ddpfPixelFormat.dwRGBBitCount;

	return TRUE;
}

void DirectDrawWin::OnDestroy()
{
	if (primsurf)
		primsurf->Release(), primsurf=0;

	if (palette)
		palette->Release(), palette=0;

	if (ddraw2)
		ddraw2->Release(), ddraw2=0;

	if (clipper != nullptr)
	{
		clipper->Release();
		clipper = nullptr;
	}
	for (int i=0;i<totaldrivers;i++)
	{
		if (driver[i].guid)
			delete [] driver[i].guid;
		free( driver[i].desc );
		free( driver[i].name );
	}
}

BOOL DirectDrawWin::OnEraseBkgnd(CDC* pDC) 
{
	CBrush br( RGB(0,0,0) );
	CRect rc;
	CWnd::GetClientRect( &rc );
	pDC->FillRect( &rc, &br );

	return TRUE;
}

void DirectDrawWin::OnMouseMove( UINT state, CPoint point )
{
	mousestate = state;
	mousex = point.x;
	mousey = point.y;
}

BOOL DirectDrawWin::PreDrawScene()
{
	if (window_active && primsurf->IsLost())
	{
		HRESULT r;
		r=primsurf->Restore();
		if (r!=DD_OK)
			TRACE("can't restore primsurf\n");

		if (backsurf)
		{
			r=backsurf->Restore();
			if (r!=DD_OK)
				TRACE("can't restore backsurf\n");
		}

		RestoreSurfaces();
	}

	return window_active;
}

void DirectDrawWin::PrintSurfaceInfo(LPDIRECTDRAWSURFACE surf, LPCTSTR name)
{
	ASSERT(surf);
	ASSERT(name);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	surf->GetSurfaceDesc( &desc );
	
	TRACE("Description for surface '%s':\n", name );

	if (desc.ddsCaps.dwCaps & DDSCAPS_COMPLEX)
		TRACE("  Complex\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_FLIP)
		TRACE("  Flipable\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
		TRACE("  Primary surface\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER)
		TRACE("  Front buffer\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
		TRACE("  Back buffer\n");
	
	if (desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
		TRACE("  Video memory\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
		TRACE("  Local Video memory\n");
	
	if (desc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
		TRACE("  System memory\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN)
		TRACE("  Offscreen/plain\n");

	if (desc.ddsCaps.dwCaps & DDSCAPS_MODEX)
		TRACE("  Mode X\n");
}

DWORD DirectDrawWin::RGBtoPixel(DWORD r, DWORD g, DWORD b)
{
	if (displaydepth==8 && palette)
	{
		static PALETTEENTRY pal[256];
		palette->GetEntries( 0, 0, 256, pal);
		DWORD bestmatch=0;
		DWORD bestscore=30000;
		for (int i=0;i<256;i++)
		{
			DWORD pr=pal[i].peRed;
			DWORD pg=pal[i].peGreen;
			DWORD pb=pal[i].peBlue;

			DWORD rdif = abs((int)(pr - r));
			DWORD gdif = abs((int)(pg - g));
			DWORD bdif = abs((int)(pb - b));

			DWORD score=rdif+gdif+bdif;
			if (score==0)
				return i;
			if (score<bestscore)
			{
				bestscore=score;
				bestmatch=i;
			}
		}
		return bestmatch;
	}
	else if (displaydepth==16)
	{
		int REDdiv=256/(int)pow( 2, numREDbits );
		int GREENdiv=256/(int)pow( 2, numGREENbits );
		int BLUEdiv=256/(int)pow( 2, numBLUEbits );
	
		float rf=(float)r/(float)REDdiv;
		float gf=(float)g/(float)GREENdiv;
		float bf=(float)b/(float)BLUEdiv;

		DWORD rt=((DWORD)rf<<loREDbit);
		DWORD gt=((DWORD)gf<<loGREENbit);
		DWORD bt=((DWORD)bf<<loBLUEbit);

		return rt|gt|bt;
	}
	else if (displaydepth==24 || displaydepth==32)
		return (r<<loREDbit) | (g<<loGREENbit) | (b<<loBLUEbit);

	return 0;
}

BOOL DirectDrawWin::SaveSurface(LPDIRECTDRAWSURFACE surf, LPCTSTR filename)
{
	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
	surf->GetSurfaceDesc( &desc );

	DWORD width  = desc.dwWidth;
	DWORD height = desc.dwHeight;
	DWORD depth  = desc.ddpfPixelFormat.dwRGBBitCount;

	BITMAPFILEHEADER filehdr;
	BITMAPINFOHEADER infohdr;
	RGBQUAD quads[256];
	PALETTEENTRY pe[256];

	ZeroMemory( &filehdr, sizeof(filehdr) );
	*((char*)&filehdr.bfType)='B';
	*(((char*)&filehdr.bfType)+1)='M';
	filehdr.bfOffBits=sizeof(filehdr)+sizeof(infohdr);
	
	if (depth==8)
		filehdr.bfOffBits+=sizeof(quads);

	ZeroMemory( &infohdr, sizeof(infohdr) );
	infohdr.biSize=sizeof(infohdr);
	infohdr.biWidth    = width;
	infohdr.biHeight   = height;
	infohdr.biPlanes   = 1;
	infohdr.biBitCount = depth==8 ? 8 : 24;
	infohdr.biCompression=BI_RGB;
	infohdr.biSizeImage = 0;
	if (depth==8)
		infohdr.biClrUsed = 256;
	else
		infohdr.biClrUsed = 0;

	LPDIRECTDRAWPALETTE tempPalette;
	if (depth==8)
	{
		primsurf->GetPalette( &tempPalette);
		tempPalette->GetEntries( 0, 0, 256, pe );
		for (int i=0;i<256;i++)
		{
			quads[i].rgbRed   = pe[i].peRed;
			quads[i].rgbGreen = pe[i].peGreen;
			quads[i].rgbBlue  = pe[i].peBlue;
		}
	}

	FILE* fp = nullptr;
	_wfopen_s( &fp, filename, L"wb" );
	fwrite( &filehdr, sizeof(filehdr), 1, fp );
	fwrite( &infohdr, sizeof(infohdr), 1, fp );
	if (depth==8)
		fwrite( quads, sizeof(RGBQUAD), 256, fp );

	BOOL ret=FALSE;

	if (depth==8)
		ret=SaveSurface08( surf, fp, width, height );
	else if (depth==16)
		ret=SaveSurface16( surf, fp, width, height );
	else if (depth==24)
		ret=SaveSurface24( surf, fp, width, height );
	else if (depth==32)
		ret=SaveSurface32( surf, fp, width, height );
	else
		ASSERT(FALSE);

	fclose(fp);

	return ret;
}

BOOL DirectDrawWin::SaveSurface08(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h)
{
	ASSERT(surf);
	ASSERT(fp);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	HRESULT r;
	r=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_READONLY, 0 );
	if (r!=DD_OK)
	{
		TRACE("SaveSurface(): Lock() failed\n");
		return FALSE;
	}

	int fillerbytes=((w+3) & ~3)-w;
	BYTE filler[]={0,0,0};

	for (int row=h-1;row>=0;row--)
	{
		BYTE* surfrow=(BYTE*)desc.lpSurface+row*desc.lPitch;
		fwrite( surfrow, w, 1, fp );
		fwrite( filler, 1, fillerbytes, fp );
	}
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::SaveSurface16(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h)
{
	ASSERT(surf);
	ASSERT(fp);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	HRESULT hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_READONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("SaveSurface(): Lock() failed\n");
		return FALSE;
	}
	
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	int fillerbytes=bytesgiven-bytesrequired;
	BYTE filler[]={0,0,0};

	RGBTRIPLE* bmpbuffer=new RGBTRIPLE[w];
	
	int rmult = (numREDbits  ==5) ? 8 : 4;
	int gmult = (numGREENbits==5) ? 8 : 4;
	int bmult = (numBLUEbits ==5) ? 8 : 4;

	for (int row=h-1;row>=0;row--)
	{
		WORD* surfpixel=(WORD*)((BYTE*)desc.lpSurface+row*desc.lPitch);
		RGBTRIPLE* bmppixel=bmpbuffer;

		for (DWORD col=0;col<w;col++)
		{
			WORD r = (*surfpixel & (WORD)desc.ddpfPixelFormat.dwRBitMask) >> loREDbit;
			WORD g = (*surfpixel & (WORD)desc.ddpfPixelFormat.dwGBitMask) >> loGREENbit;
			WORD b = (*surfpixel & (WORD)desc.ddpfPixelFormat.dwBBitMask) >> loBLUEbit;
			
			bmppixel->rgbtRed   = (BYTE)(r * rmult);
			bmppixel->rgbtGreen = (BYTE)(g * gmult);
			bmppixel->rgbtBlue  = (BYTE)(b * bmult);
			
			surfpixel++;
			bmppixel++;
		}
		fwrite( bmpbuffer, sizeof(RGBTRIPLE), w, fp );
		fwrite( filler, 1, fillerbytes, fp );
	}

	delete [] bmpbuffer;
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::SaveSurface24(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h)
{
	ASSERT(surf);
	ASSERT(fp);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	HRESULT hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_READONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("SaveSurface(): Lock() failed\n");
		return FALSE;
	}
	
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	int fillerbytes=bytesgiven-bytesrequired;
	BYTE filler[]={0,0,0};

	RGBTRIPLE* bmpbuffer=new RGBTRIPLE[w];
	
	for (int row=h-1;row>=0;row--)
	{
		RGBTRIPLE* surfpixel=(RGBTRIPLE*)((BYTE*)desc.lpSurface+row*desc.lPitch);
		RGBTRIPLE* bmppixel=bmpbuffer;

		for (DWORD col=0;col<w;col++)
		{
			DWORD r = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwRBitMask) >> loREDbit;
			DWORD g = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwGBitMask) >> loGREENbit;
			DWORD b = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwBBitMask) >> loBLUEbit;
			
			bmppixel->rgbtRed   = (BYTE)r;
			bmppixel->rgbtGreen = (BYTE)g;
			bmppixel->rgbtBlue  = (BYTE)b;
			
			surfpixel++;
			bmppixel++;
		}
		fwrite( bmpbuffer, sizeof(RGBTRIPLE), w, fp );
		fwrite( filler, 1, fillerbytes, fp );
	}

	delete [] bmpbuffer;
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::SaveSurface32(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h)
{
	ASSERT(surf);
	ASSERT(fp);

	DDSURFACEDESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize=sizeof(desc);
	HRESULT hr;
	hr=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_READONLY, 0 );
	if (hr!=DD_OK)
	{
		TRACE("SaveSurface(): Lock() failed\n");
		return FALSE;
	}
	
	int bytesrequired=w*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	int fillerbytes=bytesgiven-bytesrequired;
	BYTE filler[]={0,0,0};

	RGBTRIPLE* bmpbuffer=new RGBTRIPLE[w];
	
	for (int row=h-1;row>=0;row--)
	{
		RGBQUAD* surfpixel=(RGBQUAD*)((BYTE*)desc.lpSurface+row*desc.lPitch);
		RGBTRIPLE* bmppixel=bmpbuffer;

		for (DWORD col=0;col<w;col++)
		{
			DWORD r = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwRBitMask) >> loREDbit;
			DWORD g = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwGBitMask) >> loGREENbit;
			DWORD b = (*(DWORD*)surfpixel & desc.ddpfPixelFormat.dwBBitMask) >> loBLUEbit;
			
			bmppixel->rgbtRed   = (BYTE)r;
			bmppixel->rgbtGreen = (BYTE)g;
			bmppixel->rgbtBlue  = (BYTE)b;
			
			surfpixel++;
			bmppixel++;
		}
		fwrite( bmpbuffer, sizeof(RGBTRIPLE), w, fp );
		fwrite( filler, 1, fillerbytes, fp );
	}

	delete [] bmpbuffer;
	surf->Unlock( 0 );

	return TRUE;
}

BOOL DirectDrawWin::StorePixelFormatData()
{
	DDPIXELFORMAT format;
	ZeroMemory( &format, sizeof(format) );
	format.dwSize=sizeof(format);
	if (primsurf->GetPixelFormat( &format )!=DD_OK)
	{
		TRACE("StorePixelFormatData() failed\n");
		return FALSE;
	}

	loREDbit = LowBitPos( format.dwRBitMask );
	WORD hiREDbit = HighBitPos( format.dwRBitMask );
	numREDbits=(WORD)(hiREDbit-loREDbit+1);

	loGREENbit = LowBitPos( format.dwGBitMask );
	WORD hiGREENbit = HighBitPos( format.dwGBitMask );
	numGREENbits=(WORD)(hiGREENbit-loGREENbit+1);

	loBLUEbit  = LowBitPos( format.dwBBitMask );
	WORD hiBLUEbit  = HighBitPos( format.dwBBitMask );
	numBLUEbits=(WORD)(hiBLUEbit-loBLUEbit+1);

	return TRUE;
}

void DirectDrawWin::CheckResult( HRESULT result )
{
	TRACE("DirectDraw error: ");
	switch ( result )
	{
	case DD_OK:
		TRACE("ok\n");
		break;
	case DDERR_ALREADYINITIALIZED:
		TRACE("DDERR_ALREADYINITIALIZED\n");
		break;
	case DDERR_BLTFASTCANTCLIP:
		TRACE("DDERR_BLTFASTCANTCLIP\n");
		break;
	case DDERR_CANNOTATTACHSURFACE:
		TRACE("DDERR_CANNOTATTACHSURFACE\n");
		break;
	case DDERR_CANNOTDETACHSURFACE:
		TRACE("DDERR_CANNOTDETACHSURFACE\n");
		break;
	case DDERR_CANTCREATEDC:
		TRACE("DDERR_CANTCREATEDC\n");
		break;
	case DDERR_CANTDUPLICATE:
		TRACE("DDERR_CANTDUPLICATE\n");
		break;
	case DDERR_CANTLOCKSURFACE:
		TRACE("DDERR_CANTLOCKSURFACE\n");
		break;
	case DDERR_CANTPAGELOCK:
		TRACE("DDERR_CANTPAGELOCK\n");
		break;
	case DDERR_CANTPAGEUNLOCK:
		TRACE("DDERR_CANTPAGEUNLOCK\n");
		break;
	case DDERR_CLIPPERISUSINGHWND:
		TRACE("DDERR_CLIPPERISUSINGHWND\n");
		break;
	case DDERR_COLORKEYNOTSET:
		TRACE("DDERR_COLORKEYNOTSET\n");
		break;
	case DDERR_CURRENTLYNOTAVAIL:
		TRACE("DDERR_CURRENTLYNOTAVAIL\n");
		break;
	case DDERR_DCALREADYCREATED:
		TRACE("DDERR_DCALREADYCREATED\n");
		break;
	case DDERR_DIRECTDRAWALREADYCREATED:
		TRACE("DDERR_DIRECTDRAWALREADYCREATED\n");
		break;
	case DDERR_EXCEPTION:
		TRACE("DDERR_EXCEPTION\n");
		break;
	case DDERR_EXCLUSIVEMODEALREADYSET:
		TRACE("DDERR_EXCLUSIVEMODEALREADYSET\n");
		break;
	case DDERR_GENERIC:
		TRACE("DDERR_GENERIC\n"); 
		break;
	case DDERR_HEIGHTALIGN:
		TRACE("DDERR_HEIGHTALIGN\n");
		break;
	case DDERR_HWNDALREADYSET:
		TRACE("DDERR_HWNDALREADYSET\n");
		break;
	case DDERR_HWNDSUBCLASSED:
		TRACE("DDERR_HWNDSUBCLASSED\n");
		break;
	case DDERR_IMPLICITLYCREATED:
		TRACE("DDERR_IMPLICITLYCREATED\n");
		break;
	case DDERR_INCOMPATIBLEPRIMARY:
		TRACE("Incampatible Primary\n"); 
		break;
	case DDERR_INVALIDCAPS:
		TRACE("DDERR_INVALIDCAPS\n");
		break;
	case DDERR_INVALIDCLIPLIST:
		TRACE("DDERR_INVALIDCLIPLIST\n");
		break;
	case DDERR_INVALIDDIRECTDRAWGUID:
		TRACE("DDERR_INVALIDDIRECTDRAWGUID\n");
		break;
	case DDERR_INVALIDMODE:
		TRACE("DDERR_INVALIDMODE\n");
		break;
	case DDERR_INVALIDOBJECT:
		TRACE("DDERR_INVALIDOBJECT\n");
		break;
	case DDERR_INVALIDPARAMS:
		TRACE("DDERR_INVALIDPARAMS\n");
		break;
	case DDERR_INVALIDPIXELFORMAT:
		TRACE("DDERR_INVALIDPIXELFORMAT\n");
		break;
	case DDERR_INVALIDPOSITION:
		TRACE("DDERR_INVALIDPOSITION\n");
		break;
	case DDERR_INVALIDRECT:
		TRACE("DDERR_INVALIDRECT\n");
		break;
	case DDERR_INVALIDSURFACETYPE:
		TRACE("DDERR_INVALIDSURFACETYPE\n");
		break;
	case DDERR_LOCKEDSURFACES:
		TRACE("DDERR_LOCKEDSURFACES\n");
		break;
//	case DDERR_MOREDATA:
//		TRACE("DDERR_MOREDATA\n");
//		break;
	case DDERR_NO3D:
		TRACE("DDERR_NO3D\n");
		break;
	case DDERR_NOALPHAHW:
		TRACE("DDERR_NOALPHAHW\n");
		break;
	case DDERR_NOBLTHW:
		TRACE("DDERR_NOBLTHW\n");
		break;
	case DDERR_NOCLIPLIST:
		TRACE("DDERR_NOCLIPLIST\n");
		break;
	case DDERR_NOCLIPPERATTACHED:
		TRACE("DDERR_NOCLIPPERATTACHED\n");
		break;
	case DDERR_NOCOLORCONVHW:
		TRACE("DDERR_NOCOLORCONVHW\n");
		break;
	case DDERR_NOCOLORKEY:
		TRACE("DDERR_NOCOLORKEY\n");
		break;
	case DDERR_NOCOLORKEYHW:
		TRACE("no colorkey hardware\n");
		break;
	case DDERR_NOCOOPERATIVELEVELSET:
		TRACE("DDERR_NOCOOPERATIVELEVELSET\n");
		break;
	case DDERR_NODC:
		TRACE("DDERR_NODC\n");
		break;
	case DDERR_NODDROPSHW:
		TRACE("DDERR_NODDROPSHW\n");
		break;
	case DDERR_NODIRECTDRAWHW:
		TRACE("DDERR_NODIRECTDRAWHW\n");
		break;
	case DDERR_NODIRECTDRAWSUPPORT:
		TRACE("DDERR_NODIRECTDRAWSUPPORT\n");
		break;
	case DDERR_NOEMULATION:
		TRACE("DDERR_NOEMULATION\n");
		break;
	case DDERR_NOEXCLUSIVEMODE:
		TRACE("DDERR_NOEXCLUSIVEMODE\n");
		break;
	case DDERR_NOFLIPHW:
		TRACE("DDERR_NOFLIPHW\n");
		break;
	case DDERR_NOGDI:
		TRACE("DDERR_NOGDI\n");
		break;
	case DDERR_NOHWND:
		TRACE("DDERR_NOHWND\n");
		break;
	case DDERR_NOMIPMAPHW:
		TRACE("DDERR_NOMIPMAPHW\n");
		break;
	case DDERR_NOMIRRORHW:
		TRACE("DDERR_NOMIRRORHW\n");
		break;
	case DDERR_NOOVERLAYDEST:
		TRACE("DDERR_NOOVERLAYDEST\n");
		break;
	case DDERR_NOOVERLAYHW:
		TRACE("DDERR_NOOVERLAYHW\n");
		break;
	case DDERR_NOPALETTEATTACHED:
		TRACE("DDERR_NOPALETTEATTACHED\n");
		break;
	case DDERR_NOPALETTEHW:
		TRACE("DDERR_NOPALETTEHW\n");
		break;
	case DDERR_NORASTEROPHW:
		TRACE("DDERR_NORASTEROPHW\n");
		break;
	case DDERR_NOROTATIONHW:
		TRACE("DDERR_NOROTATIONHW\n");
		break;
	case DDERR_NOSTRETCHHW:
		TRACE("DDERR_NOSTRETCHHW\n");
		break;
	case DDERR_NOT4BITCOLOR:
		TRACE("DDERR_NOT4BITCOLOR\n");
		break;
	case DDERR_NOT4BITCOLORINDEX:
		TRACE("DDERR_NOT4BITCOLORINDEX\n");
		break;
	case DDERR_NOT8BITCOLOR:
		TRACE("DDERR_NOT8BITCOLOR\n");
		break;
	case DDERR_NOTAOVERLAYSURFACE:
		TRACE("DDERR_NOTAOVERLAYSURFACE\n");
		break;
	case DDERR_NOTEXTUREHW:
		TRACE("DDERR_NOTEXTUREHW\n");
		break;
	case DDERR_NOTFLIPPABLE:
		TRACE("DDERR_NOTFLIPPABLE\n");
		break;
	case DDERR_NOTFOUND:
		TRACE("DDERR_NOTFOUND\n");
		break;
	case DDERR_NOTINITIALIZED:
		TRACE("DDERR_NOTINITIALIZED\n");
		break;
	case DDERR_NOTLOCKED:
		TRACE("DDERR_NOTLOCKED\n");
		break;
	case DDERR_NOTPAGELOCKED:
		TRACE("DDERR_NOTPAGELOCKED\n");
		break;
	case DDERR_NOTPALETTIZED:
		TRACE("DDERR_NOTPALETTIZED\n");
		break;
	case DDERR_NOVSYNCHW:
		TRACE("DDERR_NOVSYNCHW\n");
		break;
	case DDERR_NOZBUFFERHW:
		TRACE("DDERR_NOZBUFFERHW\n");
		break;
	case DDERR_NOZOVERLAYHW:
		TRACE("DDERR_NOZOVERLAYHW\n");
		break;
	case DDERR_OUTOFCAPS:
		TRACE("DDERR_OUTOFCAPS\n");
		break;
	case DDERR_OUTOFMEMORY:
		TRACE("Out of Memory\n");
		break;
	case DDERR_OUTOFVIDEOMEMORY:
		TRACE("DDERR_OUTOFVIDEOMEMORY\n");
		break;
	case DDERR_OVERLAYCANTCLIP:
		TRACE("DDERR_OVERLAYCANTCLIP\n");
		break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		TRACE("DDERR_OVERLAYCOLORKEYONLYONEACTIVE\n");
		break;
	case DDERR_OVERLAYNOTVISIBLE:
		TRACE("DDERR_OVERLAYNOTVISIBLE\n");
		break;
	case DDERR_PALETTEBUSY:
		TRACE("DDERR_PALETTEBUSY\n");
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		TRACE("DDERR_PRIMARYSURFACEALREADYEXISTS\n");
		break;
	case DDERR_REGIONTOOSMALL:
		TRACE("DDERR_REGIONTOOSMALL\n");
		break;
	case DDERR_SURFACEALREADYATTACHED:
		TRACE("DDERR_SURFACEALREADYATTACHED\n");
		break;
	case DDERR_SURFACEALREADYDEPENDENT:
		TRACE("DDERR_SURFACEALREADYDEPENDENT\n");
		break;
	case DDERR_SURFACEBUSY:
		TRACE("DDERR_SURFACEBUSY\n");
		break;
	case DDERR_SURFACEISOBSCURED:
		TRACE("DDERR_SURFACEISOBSCURED\n");
		break;
	case DDERR_SURFACELOST:
		TRACE("DDERR_SURFACELOST\n");
		break;
	case DDERR_SURFACENOTATTACHED:
		TRACE("DDERR_SURFACENOTATTACHED\n");
		break;
	case DDERR_TOOBIGHEIGHT:
		TRACE("DDERR_TOOBIGHEIGHT\n");
		break;
	case DDERR_TOOBIGSIZE:
		TRACE("DDERR_TOOBIGSIZE\n");
		break;
	case DDERR_TOOBIGWIDTH:
		TRACE("DDERR_TOOBIGWIDTH\n");
		break;
	case DDERR_UNSUPPORTED:
		TRACE("DDERR_UNSUPPORTED\n");
		break;
	case DDERR_UNSUPPORTEDFORMAT:
		TRACE("DDERR_UNSUPPORTEDFORMAT\n");
		break;
	case DDERR_UNSUPPORTEDMASK:
		TRACE("DDERR_UNSUPPORTEDMASK\n");
		break;
	case DDERR_UNSUPPORTEDMODE:
		TRACE("DDERR_UNSUPPORTEDMODE\n");
		break;
	case DDERR_VERTICALBLANKINPROGRESS:
		TRACE("DDERR_VERTICALBLANKINPROGRESS\n");
		break;
//	case DDERR_VIDEONOTACTIVE:
//		TRACE("DDERR_VIDEONOTACTIVE\n");
//		break;
	case DDERR_WASSTILLDRAWING:
		TRACE("DDERR_WASSTILLDRAWING\n");
		break;
	case DDERR_WRONGMODE:
		TRACE("DDERR_WRONGMODE\n");
		break;
	case DDERR_XALIGN:
		TRACE("DDERR_XALIGN\n");
		break;
	default:
		TRACE("Unknown return value\n");
		break;
	}
}

void Fatal(LPCTSTR msg)
{
	if (GetDDWin())
	{
		LPDIRECTDRAW2 ddraw2=GetDDWin()->GetDDraw();
		if (ddraw2)
		{
			ddraw2->RestoreDisplayMode();
			ddraw2->Release();
		}
	}
	AfxMessageBox( msg );
	exit(1);
}

