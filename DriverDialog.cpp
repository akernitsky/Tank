// DriverDialog.cpp : implementation file
//

#include "Headers.h"
#include "resource.h"
#include "DriverDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DriverDialog dialog


DriverDialog::DriverDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DriverDialog::IDD, pParent)
{
	selection=0;
	//{{AFX_DATA_INIT(DriverDialog)
	//}}AFX_DATA_INIT
}


void DriverDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DriverDialog)
	DDX_Control(pDX, IDC_DRIVERLIST, driverlist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DriverDialog, CDialog)
	//{{AFX_MSG_MAP(DriverDialog)
	ON_LBN_DBLCLK(IDC_DRIVERLIST, OnDblclkDriverlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DriverDialog message handlers

BOOL DriverDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int size=list->GetSize();
	for (int i=0;i<size;i++)
		driverlist.InsertString( i, list->GetAt(i) );
	driverlist.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL DriverDialog::SetContents( CArray<CString, CString>* drivers)
{
	list=drivers;

	return TRUE;
}

void DriverDialog::OnOK() 
{
	selection=driverlist.GetCurSel();
	
	CDialog::OnOK();
}

void DriverDialog::OnDblclkDriverlist() 
{
	OnOK();
}
