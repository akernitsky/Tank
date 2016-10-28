#ifndef DIRECTDRAWWIN_H
#define DIRECTDRAWWIN_H


const int MAXDRIVERS = 10;
const int MAXDISPLAYMODES = 100;


class DirectDrawWin : public CFrameWnd
{
	struct DriverInfo
	{
		LPGUID guid;
		LPSTR desc, name;
	};

	struct DisplayModeInfo
	{
		DWORD width, height, depth;
	};

public:
	DirectDrawWin();
	BOOL Create(const CString& sTitle,int icon);
	virtual void DrawScene() = 0;
	void CheckResult( HRESULT result );
	LPDIRECTDRAW2 GetDDraw()  { return ddraw2; }
	BOOL DetectDisplayMode();

	//------- driver functions ---------
	int GetNumDrivers() { return totaldrivers; }
	BOOL GetDriverInfo( int index, LPGUID* guid, LPSTR* desc, LPSTR* name );
	virtual int SelectDriver() { return 0; }

	//------- display mode functions ---------
	int GetNumDisplayModes() { return totaldisplaymodes; }
	BOOL GetDisplayModeDimensions( int mode, DWORD& width, DWORD& height, DWORD& depth );
	int GetDisplayModeIndex( DWORD w, DWORD h, DWORD d );
	int GetCurDisplayMode()  { return curdisplaymode; }
	int GetCurDisplayDepth()  { return displaydepth; }
	void RestoreDisplayMode();
	BOOL ActivateDisplayMode( int mode );
	BOOL ActivateDisplayMode( int mode, DWORD rate );
	virtual int SelectInitialDisplayMode() = 0;

	//------- surface functions ---------
	virtual BOOL CreateCustomSurfaces() = 0;
	BOOL ClearSurface( LPDIRECTDRAWSURFACE surf, DWORD clr, RECT* rect=0 );
	BOOL ClearSurface( LPDIRECTDRAWSURFACE surf, DWORD r, DWORD g, DWORD b, RECT* rect=0 );
	LPDIRECTDRAWSURFACE CreateSurface(DWORD w, DWORD h );
	LPDIRECTDRAWSURFACE CreateSurface(const std::wstring& filename, bool installpalette=false);
	BOOL LoadSurface(LPDIRECTDRAWSURFACE surf, LPCTSTR filename);
	BOOL SaveSurface(LPDIRECTDRAWSURFACE surf, LPCTSTR filename);
	BOOL GetSurfaceDimensions( LPDIRECTDRAWSURFACE surf, DWORD& w, DWORD& h );
	BOOL GetSurfaceRect( LPDIRECTDRAWSURFACE surf, RECT& rect);
	virtual void RestoreSurfaces() = 0;
	BOOL BltSurface( LPDIRECTDRAWSURFACE destsurf, LPDIRECTDRAWSURFACE srcsurf, int x, int y, BOOL srccolorkey=FALSE );
	DWORD RGBtoPixel(DWORD r, DWORD g, DWORD b);	
	void PrintSurfaceInfo(LPDIRECTDRAWSURFACE surf, LPCTSTR name);

	//------- misc functions ---------
	static int GetMouseX()    { return mousex; }
	static int GetMouseY()    { return mousey; }
	static BOOL GetBmpDimensions( LPCTSTR filename, int& w, int& h );
	int GetDisplayDepth()  { return displaydepth; }
	const CRect& GetDisplayRect() { return displayrect; }

protected:
	//{{AFX_MSG(DirectDrawWin)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove( UINT state, CPoint point );
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	static BOOL WINAPI DriverAvailable(LPGUID guid, LPSTR desc, LPSTR name, LPVOID p );
	static HRESULT WINAPI DisplayModeAvailable(LPDDSURFACEDESC, LPVOID);
	static int CompareModes( const void *arg1, const void *arg2 );
	virtual BOOL CreateFlippingSurfaces();
	BOOL StorePixelFormatData();
	BOOL PreDrawScene();
	BOOL Copy_Bmp_Surface(LPDIRECTDRAWSURFACE surf, BITMAPINFOHEADER* bmphdr, BYTE* buf);
	BOOL Copy_Bmp08_Surface08(LPDIRECTDRAWSURFACE surf, BYTE* bmpbuf, int w, int h);
	BOOL Copy_Bmp24_Surface16(LPDIRECTDRAWSURFACE surf, BYTE* bmpbuf, int w, int h);
	BOOL Copy_Bmp24_Surface24(LPDIRECTDRAWSURFACE surf, BYTE* bmpbuf, int w, int h);
	BOOL Copy_Bmp24_Surface32(LPDIRECTDRAWSURFACE surf, BYTE* bmpbuf, int w, int h);
	BOOL SaveSurface08(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h);
	BOOL SaveSurface16(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h);
	BOOL SaveSurface24(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h);
	BOOL SaveSurface32(LPDIRECTDRAWSURFACE surf, FILE* fp, DWORD w, DWORD h);
	WORD LowBitPos(DWORD);
	WORD HighBitPos(DWORD);
	BOOL CreatePalette(RGBQUAD* quad, int ncolors);

protected:
	LPDIRECTDRAW2 ddraw2;
	LPDIRECTDRAWSURFACE primsurf, backsurf;
	LPDIRECTDRAWCLIPPER clipper;
	LPDIRECTDRAWPALETTE palette;

	WORD loREDbit;
	WORD numREDbits;
	WORD loGREENbit;
	WORD numGREENbits;
	WORD loBLUEbit;
	WORD numBLUEbits;

private:
	
	DriverInfo driver[MAXDRIVERS];
	int totaldrivers;

	DisplayModeInfo displaymode[MAXDISPLAYMODES];
	int curdisplaymode;
	int totaldisplaymodes;
	CRect displayrect;
	int displaydepth;

	static long mousex;
	static long mousey;
	static UINT mousestate;

	BOOL window_active;
	static DirectDrawWin* thisptr;
	CRect clientrect;
	friend class DirectDrawApp;
	friend DirectDrawWin* GetDDWin();

	bool videobacksurf;
};


inline DirectDrawWin* GetDDWin()
{
	return DirectDrawWin::thisptr;
}

void Fatal(LPCTSTR msg);


#ifdef _DEBUG

#undef ASSERT
#define ASSERT(c) \
	if (!(c)) \
	{ \
		if (GetDDWin()) \
		{ \
			GetDDWin()->GetDDraw()->RestoreDisplayMode(); \
			GetDDWin()->GetDDraw()->Release(); \
		} \
		AfxAssertFailedLine(THIS_FILE, __LINE__); \
		AfxDebugBreak(); \
	}

#endif _DEBUG

#endif
