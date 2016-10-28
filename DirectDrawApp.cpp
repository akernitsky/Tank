#include "Headers.h"
#include "DirectDrawApp.h"
#include "DirectDrawWin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
extern bool bOgon;
extern bool bPul;
extern bool bVzr;
int timer=0;
int timer1=0;

BEGIN_MESSAGE_MAP(DirectDrawApp, CWinApp)
	//{{AFX_MSG_MAP(DirectDrawApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DirectDrawApp::DirectDrawApp()
{
	ddwin=0;
	m_pMainWnd=0;
}

BOOL DirectDrawApp::InitInstance()
{
	ASSERT(m_pMainWnd);
	m_pMainWnd->ShowWindow( SW_SHOWNORMAL );
	m_pMainWnd->UpdateWindow();
	ShowCursor( TRUE );

	return TRUE;
}

BOOL DirectDrawApp::OnIdle(LONG) 
{
	timer1++;
	timer++;

	if(timer == 1200 && bOgon && !bVzr)
	{	
		bOgon=false;
		bVzr=true;
	}

	if (bVzr && timer > 2300)
	{
		bVzr = false;
		bOgon = false;
		timer = 0;
	}
	if(timer1==60)
	{
		bPul=false;
		timer1=1;
	}
	
	if (ddwin->PreDrawScene())
	{
		ddwin->DrawScene();
	}
	return TRUE;
}

int DirectDrawApp::ExitInstance() 
{
	delete ddwin;
	
	return CWinApp::ExitInstance();
}
