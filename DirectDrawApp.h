#ifndef DIRECTDRAWAPP_H
#define DIRECTDRAWAPP_H


class DirectDrawApp : public CWinApp
{
public:
	DirectDrawApp();
public:
	//{{AFX_VIRTUAL(DirectDrawApp)
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(DirectDrawApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	friend class DirectDrawWin;
	DirectDrawWin* ddwin;
};

#endif
