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
//	int iInstallResult;
//	iInstallResult=SetTimer(1,10,NULL);
//	if (iInstallResult==0)
//	{
//		MessageBox("cannot install timer!");
//	}

	surf1 = 0;
	y = 5;
	t = 0;
	w = 0;
	hh = 5;
	bOgon = false;
	surf2 = 0;
	surf3 = 0;
	surf4=0;
	surf5=0;
	surf6=0;
	surf7=0;
	surf8=0;
	surf9=0;
	surf10=0;
	surf11=0;
	surf12=0;
	surf13=0;
	surf14=0;
	surf15=0;
	surf16=0;
	surf17 = 0;
	surf18 = 0;
	surf19 = 0;
	surf20 = 0;
	surf21 = 0;
	surf22 = 0;
	surf23 = 0;
	surf24 = 0;
	surf25 = 0;
	surf26 = 0;
	surf27 = 0;
	surf28 = 0;
	surf29 = 0;
	surf30 = 0;
	surf31 = 0;
	surf32 = 0;
	surf33 = 0;
	surf34 = 0;
	surf35 = 0;
	surf36 = 0;
	surf37 = 0;
	tsurf1=0;
	tsurf2=0;
	tsurf3=0;
	tsurf4=0;
	tsurf5=0;
	tsurf6=0;
	tsurf7=0;
	tsurf8=0;
	tsurf9=0;
	tsurf10=0;
	tsurf11=0;
	tsurf12=0;
	tsurf13=0;
	tsurf14=0;
	tsurf15=0;
	tsurf16=0;

	for (int i = 0; i < 32; ++i)
		surf[i] = nullptr;

}

LPDIRECTDRAWSURFACE TankWin::createCustomSurface(const std::string& fileName)
{
	LPDIRECTDRAWSURFACE surf = CreateSurface(fileName);
	
	if(surf == 0)
	{
		const std::string errorText = std::string("failed to load: " + fileName);
		Fatal(errorText.c_str());
	}
	return surf;
}
void TankWin::addToSurfaces(IDirectDrawSurface* surface)
{
	assert(surface != nullptr);
	if (surface != nullptr)
	{
		surfaces.push_back(std::shared_ptr<IDirectDrawSurface>(surface, std::mem_fn(&IUnknown::Release)));
	}
}

void TankWin::createSurfaces()
{
	std::map<int, std::string> fileNameMapping = { std::make_pair(0, "tankback.bmp") };

	for (const auto& element : fileNameMapping)
	{
		IDirectDrawSurface* surface = createCustomSurface(element.second);
		addToSurfaces(surface);
	}
}

BOOL TankWin::CreateCustomSurfaces()
{
	createSurfaces();

	std::string fileName ="tankright1.bmp";
	if (surf2)
	surf2->Release(),surf2=0;
	surf2=CreateSurface(fileName);
	if (surf2==0)
	{
	Fatal("failed to load BMP2");
	return FALSE;
	}
	fileName="tankrightback.bmp";
	if (surf3)
	surf3->Release(),surf3=0;
	surf3=CreateSurface(fileName);
	if (surf3==0)
	{
	Fatal("failed to load BMP3");
	return FALSE;
	}
	fileName="tankright2.bmp";//
	if (surf4)
	surf4->Release(),surf4=0;
	surf4=CreateSurface(fileName);
	if (surf4==0)
	{
	Fatal("failed to load BMP4");
	return FALSE;
	}
	fileName="tankright3.bmp";//
	if (surf5)
	surf5->Release(),surf5=0;
	surf5=CreateSurface(fileName);
	if (surf5==0)
	{
	Fatal("failed to load BMP5");
	return FALSE;
	}
	fileName="tankright4.bmp";//
	if (surf6)
	surf6->Release(),surf6=0;
	surf6=CreateSurface(fileName);
	if (surf6==0)
	{
	Fatal("failed to load BMP6");
	return FALSE;
	}
	fileName="tankright6.bmp";//
	if (surf7)
	surf7->Release(),surf7=0;
	surf7=CreateSurface(fileName);
	if (surf7==0)
	{
	Fatal("failed to load BMP7");
	return FALSE;
	}
	fileName="tankright7.bmp";//
	if (surf8)
	surf8->Release(),surf8=0;
	surf8=CreateSurface(fileName);
	if (surf8==0)
	{
	Fatal("failed to load BMP8");
	return FALSE;
	}
	fileName="tankfront.bmp";//
	if (surf9)
	surf9->Release(),surf9=0;
	surf9=CreateSurface(fileName);
	if (surf9==0)
	{
	Fatal("failed to load BMP9");
	return FALSE;
	}
	fileName="tankleft4.bmp";
	if (surf10)
	surf10->Release(),surf10=0;
	surf10=CreateSurface(fileName);
	if (surf10==0)
	{
	Fatal("failed to load BMP10");
	return FALSE;
	}
	fileName="tankleftfront.bmp";
	if (surf11)
	surf11->Release(),surf11=0;
	surf11=CreateSurface(fileName);
	if (surf11==0)
	{
	Fatal("failed to load BMP11");
	return FALSE;
	}
	fileName="tankleft3.bmp";
	if (surf12)
	surf12->Release(),surf12=0;
	surf12=CreateSurface(fileName);
	if (surf12==0)
	{
	Fatal("failed to load BMP12");
	return FALSE;
	}
	fileName="tankleft.bmp";
	if (surf13)
	surf13->Release(),surf13=0;
	surf13=CreateSurface(fileName);
	if (surf13==0)
	{
	Fatal("failed to load BMP13");
	return FALSE;
	}
	fileName="tankleft23.bmp";
	if (surf14)
	surf14->Release(),surf14=0;
	surf14=CreateSurface(fileName);
	if (surf14==0)
	{
	Fatal("failed to load BMP14");
	return FALSE;
	}
	fileName="tankleftback.bmp";
	if (surf15)
	surf15->Release(),surf15=0;
	surf15=CreateSurface(fileName);
	if (surf15==0)
	{
	Fatal("failed to load BMP15");
	return FALSE;
	}
	
	fileName="tankleft1.bmp";
	if (surf16)
	surf16->Release(),surf16=0;
	surf16=CreateSurface(fileName);
	if (surf16==0)
	{
	Fatal("failed to load BMP16");
	return FALSE;
	}


	
	fileName="1.bmp";
	if (surf17)
	surf17->Release(),surf17=0;
	surf17=CreateSurface(fileName,TRUE);
	if (surf17==0)
	{
		Fatal("failed to load BMP17");
		return FALSE;
	}

	
	fileName="2.bmp";
	if (surf18)
	surf18->Release(),surf18=0;
	surf18=CreateSurface(fileName,TRUE);
		
	if (surf18==0)
	{
		Fatal("failed to load BMP18");
		return FALSE;
	}

		fileName="3.bmp";
	if (surf19)
	surf19->Release(),surf19=0;
	surf19=CreateSurface(fileName,TRUE);
	
	
	if (surf19==0)
	{
		Fatal("failed to load BMP19");
		return FALSE;
	}
		fileName="4.bmp";
	if (surf20)
	surf20->Release(),surf20=0;
	surf20=CreateSurface(fileName,TRUE);
	
	
	if (surf20==0)
	{
		Fatal("failed to load BMP20");
		return FALSE;
	}
		fileName="5.bmp";
	if (surf21)
	surf21->Release(),surf21=0;
	surf21=CreateSurface(fileName,TRUE);
	
	
	if (surf21==0)
	{
		Fatal("failed to load BMP21");
		return FALSE;
	}
		fileName="6.bmp";
	if (surf22)
	surf22->Release(),surf22=0;
	surf22=CreateSurface(fileName,TRUE);
	
	
	if (surf22==0)
	{
		Fatal("failed to load BMP22");
		return FALSE;
	}
		fileName="7.bmp";
	if (surf23)
	surf23->Release(),surf23=0;
	surf23=CreateSurface(fileName,TRUE);
	
	
	if (surf23==0)
	{
		Fatal("failed to load BMP23");
		return FALSE;
	}
		fileName="8.bmp";
	if (surf24)
	surf24->Release(),surf24=0;
	surf24=CreateSurface(fileName,TRUE);
	
	
	if (surf24==0)
	{
		Fatal("failed to load BMP24");
		return FALSE;
	}
		fileName="9.bmp";
	if (surf25)
	surf25->Release(),surf25=0;
	surf25=CreateSurface(fileName,TRUE);
	
	
	if (surf25==0)
	{
		Fatal("failed to load BMP25");
		return FALSE;
	}
		fileName="10.bmp";
	if (surf26)
	surf26->Release(),surf26=0;
	surf26=CreateSurface(fileName,TRUE);
	
	
	if (surf26==0)
	{
		Fatal("failed to load BMP26");
		return FALSE;
	}
		fileName="11.bmp";
	if (surf27)
	surf27->Release(),surf27=0;
	surf27=CreateSurface(fileName,TRUE);
	
	
	if (surf27==0)
	{
		Fatal("failed to load BMP27");
		return FALSE;
	}
		fileName="12.bmp";
	if (surf28)
	surf28->Release(),surf28=0;
	surf28=CreateSurface(fileName,TRUE);
	
	
	if (surf28==0)
	{
		Fatal("failed to load BMP28");
		return FALSE;
	}
			fileName="13.bmp";
	if (surf29)
	surf29->Release(),surf29=0;
	surf29=CreateSurface(fileName,TRUE);
	
	
	if (surf29==0)
	{
		Fatal("failed to load BMP29");
		return FALSE;
	}	
	fileName="14.bmp";
	if (surf30)
	surf30->Release(),surf30=0;
	surf30=CreateSurface(fileName,TRUE);
	
	
	if (surf30==0)
	{
		Fatal("failed to load BMP30");
		return FALSE;
	}

	fileName="15.bmp";
	if (surf31)
		surf31->Release(),surf31=0;
		surf31=CreateSurface(fileName,TRUE);
		
	if (surf31==0)
	{
		Fatal("failed to load BMP31");
		return FALSE;
	}

	fileName="16.bmp";
	if (surf32)
	surf32->Release(),surf32=0;
	surf32=CreateSurface(fileName,TRUE);
	
	
	if (surf32==0)
	{
		Fatal("failed to load BMP32");
		return FALSE;
	}
	

	fileName="ter.bmp";
	if (surf33)
	surf33->Release(),surf33=0;
	surf33=CreateSurface(fileName,TRUE);
	
	
	if (surf33==0)
	{
		Fatal("failed to load BMP33");
		return FALSE;
	}

		fileName="trackh.bmp";
	if (surf34)
	surf34->Release(),surf34=0;
	surf34=CreateSurface(fileName,TRUE);
	
	
	if (surf34==0)
	{
		Fatal("failed to load BMP34");
		return FALSE;
	}
	

	fileName="trackv.bmp";
	if (surf35)
	surf35->Release(),surf35=0;
	surf35=CreateSurface(fileName,TRUE);
	
	
	if (surf35==0)
	{
		Fatal("failed to load BMP35");
		return FALSE;
	}

		fileName="point.bmp";
	if (surf36)
	surf36->Release(),surf36=0;
	surf36=CreateSurface(fileName,TRUE);
	
	
	if (surf36==0)
	{
		Fatal("failed to load BMP36");
		return FALSE;
	}

		fileName="er.bmp";
	if (surf37)
	surf37->Release(),surf37=0;
	surf37=CreateSurface(fileName,TRUE);
	
	
	if (surf37==0)
	{
		Fatal("failed to load BMP37");
		return FALSE;
	}

	for(int m=101; m<=132; m++)
	{
		std::string filename = std::to_string(m) + ".bmp";
		
		if (surf[m-101])
			surf[m-101]->Release(),surf[m-101]=0;
			surf[m-101] = CreateSurface(filename, true);
		
		
		if (surf[m-101]== 0)
		{
			Fatal("failed to load BMP312");
			return FALSE;
		}
	}

	DDCOLORKEY  colorkey;
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surfaces.front()->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf2->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf3->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf4->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf5->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf6->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf7->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf8->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf9->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf10->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf11->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf12->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf13->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf14->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf15->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf16->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	

	surf17->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf18->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf19->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf20->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf21->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf22->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf23->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf24->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf25->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf26->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf27->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf28->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf29->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf30->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf31->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf32->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf33->SetColorKey( DDCKEY_SRCBLT, &colorkey );	
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;
	
	surf34->SetColorKey( DDCKEY_SRCBLT, &colorkey );
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf35->SetColorKey( DDCKEY_SRCBLT, &colorkey );	
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf36->SetColorKey( DDCKEY_SRCBLT, &colorkey );	
	colorkey.dwColorSpaceLowValue = 0;
	colorkey.dwColorSpaceHighValue = 0;

	surf37->SetColorKey( DDCKEY_SRCBLT, &colorkey );	


	for(int n=101; n<=132; n++)
	{
		colorkey.dwColorSpaceLowValue = 0;
		colorkey.dwColorSpaceHighValue = 0;
		surf[n-101]->SetColorKey( DDCKEY_SRCBLT, &colorkey );	
	}

	ClearSurface( primsurf, 0 );
	ClearSurface( backsurf, 0 );
	return TRUE;
}

void TankWin::DrawScene()
{
	ClearSurface(backsurf, 0);
	CRect client;
	GetClientRect(client);
	int width = client.Width();
	int height = client.Height();

	BltSurface(backsurf,surf33,0,0,TRUE);


switch (y)	
{
			case 1:{BltSurface(backsurf,surfaces.front().get(),20+t,45+w,TRUE);break;}

			case 2:{BltSurface(backsurf,surf2,20+t,45+w,TRUE);break;}

			case 3:{BltSurface(backsurf,surf3,20+t,45+w,TRUE);break;}

			case 4:{BltSurface(backsurf,surf4,20+t,45+w,TRUE);break;}

			case 5:{BltSurface(backsurf,surf5,20+t,45+w,TRUE);break;}

			case 6:{BltSurface(backsurf,surf6,20+t,45+w,TRUE);break;}

			case 7:{BltSurface(backsurf,surf7,20+t,45+w,TRUE);break;}

			case 8:{BltSurface(backsurf,surf8,20+t,45+w,TRUE);break;}

			case 9:{BltSurface(backsurf,surf9,20+t,45+w,TRUE);break;}

			case 10:{BltSurface(backsurf,surf10,20+t,45+w,TRUE);break;}

			case 11:{BltSurface(backsurf,surf11,20+t,45+w,TRUE);break;}

			case 12:{BltSurface(backsurf,surf12,20+t,45+w,TRUE);break;}

			case 13:{BltSurface(backsurf,surf13,20+t,45+w,TRUE);break;}

			case 14:{BltSurface(backsurf,surf14,20+t,45+w,TRUE);break;}

			case 15:{BltSurface(backsurf,surf15,20+t,45+w,TRUE);break;}

			case 16:{BltSurface(backsurf,surf16,20+t,45+w,TRUE);break;}

			if(y==17)
			{
				y=1;
			}

}
	switch (hh)
	{
		case 1: 
			{
				BltSurface(backsurf,surf17,20+t,42+w,TRUE);	
				break;
			}
		case 2: 
			{
				BltSurface(backsurf,surf18,20+t,42+w,TRUE);
				break;
			}
		case 3: 
			{
				BltSurface(backsurf,surf19,20+t,42+w,TRUE);
				break;
			}
		case 4: 
			{
				BltSurface(backsurf,surf20,20+t,42+w,TRUE);
				break;
			}
		case 5: 
			{
				
				BltSurface(backsurf,surf21,20+t,42+w,TRUE);
			
				break;
			}
		case 6: 
			{
				BltSurface(backsurf,surf22,20+t,42+w,TRUE);
				break;
			}
		case 7: 
			{
				BltSurface(backsurf,surf23,20+t,42+w,TRUE);
				break;
			}
		case 8: 
			{
				BltSurface(backsurf,surf24,20+t,42+w,TRUE);
				break;
			}
		case 9: 
			{
				BltSurface(backsurf,surf25,20+t,42+w,TRUE);
				break;
			}
		case 10: 
			{
				BltSurface(backsurf,surf26,20+t,42+w,TRUE);
				break;
			}
		case 11: 
			{
				BltSurface(backsurf,surf27,20+t,42+w,TRUE);
				break;
			}

		case 12: 
			{
				BltSurface(backsurf,surf28,20+t,42+w,TRUE);
				break;
			}
		case 13: 
			{
				BltSurface(backsurf,surf29,20+t,42+w,TRUE);
				break;
			}
		case 14: 
			{
				BltSurface(backsurf,surf30,20+t,42+w,TRUE);
				break;
			}
		case 15: 
			{
				BltSurface(backsurf,surf31,20+t,42+w,TRUE);
				break;
			}
		case 16: 
			{
				BltSurface(backsurf,surf32,20+t,42+w,TRUE);
				break;
			}
				if(hh==17)
				{
					hh=1;
				}
				
	}
	
	if(bOgon)
	{
		if(timer<=10)
		{
			xx=105+t;
			yy=76+w;
			ww=hh;
		}
	
		switch (ww)
		{
				case 1:
				{BltSurface(backsurf,surf36,xx-37,yy-27-timer*3,FALSE);break;}
				case 2:
				{BltSurface(backsurf,surf36,xx-27+(timer*3)*0.414,-28+yy-(timer*3),FALSE);break;}
				case 3:
				{BltSurface(backsurf,surf36,xx-18+timer*1.8,yy-18-timer*1.8,FALSE);break;}
					
				case 4:
				{BltSurface(backsurf,surf36,xx-5+(timer*3),yy-14-(timer*3)*0.414,FALSE);break;}

				case 5:
				{BltSurface(backsurf,surf36,xx+timer*3,yy,FALSE);break;}

				case 6:
				{BltSurface(backsurf,surf36,xx-5+(timer*3),yy+8+(timer*3)*0.414,FALSE);break;}

				case 7:
				{BltSurface(backsurf,surf36,xx-15+timer*1.8,yy+15+timer*1.8,FALSE);break;}

				case 8:
				{BltSurface(backsurf,surf36,xx-28+(timer*3)*0.414,28+yy+(timer*3),FALSE);break;}
				case 9:
				{BltSurface(backsurf,surf36,xx-36,yy+27+timer*3,FALSE);break;}

				case 10:
				{BltSurface(backsurf,surf36,xx-44-(timer*3)*0.414,28+yy+(timer*3),FALSE);break;}

				case 11:
				{BltSurface(backsurf,surf36,xx-57-timer*1.8,yy+15+timer*1.8,FALSE);break;}

				case 12:
				{BltSurface(backsurf,surf36,xx-68-(timer*3),yy+7+(timer*3)*0.414,FALSE);break;}
				
				case 13:
				{BltSurface(backsurf,surf36,xx-70-timer*3,yy,FALSE);break;}
				
				case 14:
				{BltSurface(backsurf,surf36,xx-60-(timer*3),yy-12-(timer*3)*0.414,FALSE);break;}
				
				case 15:
				{BltSurface(backsurf,surf36,xx-56-timer*1.8,yy-16-timer*1.8,FALSE);break;}
				
				case 16:
				{BltSurface(backsurf,surf36,xx-48-(timer*3)*0.414,-28+yy-(timer*3),FALSE);break;}
		}
	
	}
	if(bVzr)
		{
			switch(ww)
			{
			case 1:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-37-48,yy-27-100*3-40,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 2:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-60+(100*3)*0.414,yy-(100*3)-106,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}

			case 3:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-18+60*3,yy-98-60*3,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 4:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-5+(100*3),yy-64-(100*3)*0.414,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 5:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx+100*3-48,yy-40,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 6:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-5+(100*3),yy-10+(100*3)*0.414,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 7:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-15+60*3,yy+15+60*3,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 8:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-56+(100*3)*0.414,28+yy+(100*3),TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 9:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-36-48,yy+27+100*3-40,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 10:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-94-(100*3)*0.414,28+yy+(100*3),TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 11:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-157-60*3,yy+15+60*3,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 12:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-138-(100*3),yy-14+(100*3)*0.414,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 13:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-118-100*3,yy-40,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 14:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-60-(100*3),yy-12-(100*3)*0.414,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 15:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-136-60*3,yy-98-60*3,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			case 16:
				{
					if(timer<=124)
					{
						BltSurface(backsurf,surf[timer/4],xx-106-(100*3)*0.414,yy-(100*3)-106,TRUE);break;
					}
					else
					{
						bVzr=false;
					}
				}
			}
		}
	if(bPul)
	{
		if(timer<=10)
		{
			xx1=105+t;
			yy1=76+w;
			ww1=y;
		}
	
		switch (ww1)
		{
			case 1:
				{BltSurface(backsurf,surf36,xx1-37,yy1-27-timer1*12,FALSE);break;}
			case 2:
				{BltSurface(backsurf,surf36,xx1-27+(timer1*12)*0.414,-28+yy1-(timer1*12),FALSE);break;}
			case 3:
				{BltSurface(backsurf,surf36,xx1-18+timer1*12,yy1-18-timer1*12,FALSE);break;}
				
			case 4:
				{BltSurface(backsurf,surf36,xx1-5+(timer1*12),yy1-14-(timer1*12)*0.414,FALSE);break;}

				case 5:
				{BltSurface(backsurf,surf36,xx1+timer1*12,yy1,FALSE);break;}

				case 6:
				{BltSurface(backsurf,surf36,xx1-5+(timer1*12),yy1+8+(timer1*12)*0.414,FALSE);break;}

				case 7:
				{BltSurface(backsurf,surf36,xx1-15+timer1*12,yy1+15+timer1*12,FALSE);break;}

				case 8:
				{BltSurface(backsurf,surf36,xx1-28+(timer1*12)*0.414,28+yy1+(timer1*12),FALSE);break;}
				case 9:
				{BltSurface(backsurf,surf36,xx1-36,yy1+27+timer1*12,FALSE);break;}

				case 10:
				{BltSurface(backsurf,surf36,xx1-44-(timer1*12)*0.414,28+yy1+(timer1*12),FALSE);break;}

				case 11:
				{BltSurface(backsurf,surf36,xx1-57-timer1*12,yy1+15+timer1*12,FALSE);break;}

				case 12:
				{BltSurface(backsurf,surf36,xx1-68-(timer1*12),yy1+7+(timer1*12)*0.414,FALSE);break;}
				
				case 13:
				{BltSurface(backsurf,surf36,xx1-70-timer1*12,yy1,FALSE);break;}
				
				case 14:
				{BltSurface(backsurf,surf36,xx1-60-(timer1*12),yy1-12-(timer1*12)*0.414,FALSE);break;}
				
				case 15:
				{BltSurface(backsurf,surf36,xx1-56-timer1*12,yy1-16-timer1*12,FALSE);break;}
				
				case 16:
				{BltSurface(backsurf,surf36,xx1-48-(timer1*12)*0.414,-28+yy1-(timer1*12),FALSE);break;}
		}
	}
///	primsurf->Flip( 0, DDFLIP_WAIT );
	CRect screenRect = client;
	ClientToScreen(screenRect);
	primsurf->Blt(&screenRect, backsurf, &client, DDBLT_WAIT, 0);
}

void TankWin::RestoreSurfaces()
{
if(surfaces.front()->IsLost()==FALSE)
return;
CString filename;
filename="tankback.bmp";
surfaces.front()->Restore();
LoadSurface(surfaces.front().get(),filename);

if(surf2->IsLost()==FALSE)
return;
filename="tankright1.bmp";
surf2->Restore();
LoadSurface(surf2,filename);

if(surf3->IsLost()==FALSE)
return;
filename="tankrightback.bmp";
surf3->Restore();
LoadSurface(surf3,filename);

if(surf4->IsLost()==FALSE)
return;
filename="tankright2.bmp";
surf4->Restore();
LoadSurface(surf4,filename);

if(surf5->IsLost()==FALSE)
return;
filename="tankright3.bmp";//
surf5->Restore();
LoadSurface(surf5,filename);

if(surf6->IsLost()==FALSE)
return;
filename="tankright4.bmp";//
surf6->Restore();
LoadSurface(surf6,filename);

if(surf7->IsLost()==FALSE)
return;
filename="tankright6.bmp";//
surf7->Restore();
LoadSurface(surf7,filename);

if(surf8->IsLost()==FALSE)
return;
filename="tankright7.bmp";//
surf8->Restore();
LoadSurface(surf8,filename);

if(surf9->IsLost()==FALSE)
return;
filename="tankfront.bmp";
surf9->Restore();
LoadSurface(surf9,filename);

if(surf10->IsLost()==FALSE)
return;
filename="tankleft4.bmp";
surf10->Restore();
LoadSurface(surf10,filename);

if(surf11->IsLost()==FALSE)
return;
filename="tankleftfront.bmp";
surf11->Restore();
LoadSurface(surf11,filename);

if(surf12->IsLost()==FALSE)
return;
filename="tankleft3.bmp";
surf12->Restore();
LoadSurface(surf12,filename);

if(surf13->IsLost()==FALSE)
return;
filename="tankleft.bmp";//
surf13->Restore();
LoadSurface(surf13,filename);

if(surf14->IsLost()==FALSE)
return;
filename="tankleft23.bmp";//
surf14->Restore();
LoadSurface(surf14,filename);

if(surf15->IsLost()==FALSE)
return;
filename="tankleftback.bmp";//
surf15->Restore();
LoadSurface(surf15,filename);

if(surf16->IsLost()==FALSE)
return;
filename="tankleft1.bmp";//
surf16->Restore();
LoadSurface(surf16,filename);

if(surf17->IsLost()==FALSE)
return;
filename="1.bmp";//
surf17->Restore();
LoadSurface(surf17,filename);

if(surf18->IsLost()==FALSE)
return;
filename="2.bmp";//
surf18->Restore();
LoadSurface(surf18,filename);

if(surf19->IsLost()==FALSE)
return;
filename="3.BMP";//
surf19->Restore();
LoadSurface(surf19,filename);

if(surf20->IsLost()==FALSE)
return;
filename="4.BMP";//
surf20->Restore();
LoadSurface(surf20,filename);

if(surf21->IsLost()==FALSE)
return;
filename="5.BMP";//
surf21->Restore();
LoadSurface(surf21,filename);

if(surf22->IsLost()==FALSE)
return;
filename="6.BMP";//
surf22->Restore();
LoadSurface(surf22,filename);

if(surf23->IsLost()==FALSE)
return;
filename="7.BMP";
surf23->Restore();
LoadSurface(surf23,filename);

if(surf24->IsLost()==FALSE)
return;
filename="8.BMP";
surf24->Restore();
LoadSurface(surf24,filename);

if(surf25->IsLost()==FALSE)
return;
filename="9.BMP";
surf25->Restore();
LoadSurface(surf25,filename);

if(surf26->IsLost()==FALSE)
return;
filename="10.BMP";
surf26->Restore();
LoadSurface(surf26,filename);

if(surf27->IsLost()==FALSE)
return;
filename="11.BMP";
surf27->Restore();
LoadSurface(surf27,filename);

if(surf28->IsLost()==FALSE)
return;
filename="12.BMP";
surf28->Restore();
LoadSurface(surf28,filename);

if(surf29->IsLost()==FALSE)
return;
filename="13.BMP";
surf29->Restore();
LoadSurface(surf29,filename);

if(surf30->IsLost()==FALSE)
return;
filename="14.BMP";
surf30->Restore();
LoadSurface(surf30,filename);

if(surf31->IsLost()==FALSE)
return;
filename="15.BMP";
surf31->Restore();
LoadSurface(surf31,filename);

if(surf32->IsLost()==FALSE)
return;
filename="16.BMP";
surf32->Restore();
LoadSurface(surf32,filename);

if(surf33->IsLost()==FALSE)
return;
filename="ter.BMP";
surf33->Restore();
LoadSurface(surf33,filename);

if(surf34->IsLost()==FALSE)
return;
filename="trackh.BMP";
surf34->Restore();
LoadSurface(surf34,filename);

if(surf35->IsLost()==FALSE)
return;
filename="trackv.BMP";
surf35->Restore();
LoadSurface(surf35,filename);

if(surf36->IsLost()==FALSE)
return;
filename="point.BMP";
surf36->Restore();
LoadSurface(surf36,filename);

	for(int i1=101; i1<=132; i1++)
	{
		if(surf[i1-101]->IsLost()==FALSE)
		return;
		filename.Format("%i1.bmp",i1);
			surf[i1-101]->Restore();
			LoadSurface(surf[i1-101],filename);
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
	DWORD w,h,d;

	if (curdepth!=desireddepth)
		ddraw2->SetDisplayMode( 2880, 1800, curdepth, 0, 0 );

	for (i=0;i<nummodes;i++)
	{
		GetDisplayModeDimensions( i, w, h, d );
		if (w==desiredwidth && h==desiredheight && d==desireddepth)
			return i;
	}

	for (i=0;i<nummodes;i++)
	{
		GetDisplayModeDimensions( i, w, h, d );
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

	if(nChar==VK_RETURN && bOgon==false)
	{
		timer=1;
		bOgon=true;
	}
	
	if (nChar==VK_ESCAPE)
	{
		PostMessage( WM_CLOSE );
	}
	
	if (nChar==VK_SPACE)
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

void TankWin::OnTimer(UINT nIDEvent) 
{
//	if(nIDEvent==1)
//	{	
//		MessageBox("asd");
//	}
//	if (right)
//	{
//		if(nIDEvent==1)
//		t++;
//	}
//TankWin::OnTimer(nIDEvent);
}

