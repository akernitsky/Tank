#include "Headers.h"
#include "resource.h"
#include "TankApp.h"
#include "TankWin.h"


BEGIN_MESSAGE_MAP(TankApp, DirectDrawApp)
	//{{AFX_MSG_MAP(TankApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TankApp theapp;

BOOL TankApp::InitInstance()
{
#ifdef _DEBUG
//	afxTraceEnabled=FALSE;
#endif
	TankWin* win = new TankWin;
	if(!win->Create("High Performance Tank Demo", IDI_ICON))
		return FALSE;

	m_pMainWnd = win;
	return DirectDrawApp::InitInstance();
}
