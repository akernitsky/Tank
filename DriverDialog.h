// DriverDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DriverDialog dialog

class DriverDialog : public CDialog
{
// Construction
public:
	DriverDialog(CWnd* pParent = NULL);   // standard constructor
	BOOL SetContents( CArray<CString, CString>* );
	int GetSelection()  { return selection; }
// Dialog Data
	//{{AFX_DATA(DriverDialog)
	enum { IDD = IDD_DRIVERDIALOG };
	CListBox	driverlist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DriverDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DriverDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkDriverlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CArray<CString, CString>* list;
	int selection;
};
