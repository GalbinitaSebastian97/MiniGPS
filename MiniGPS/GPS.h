#include <afxwin.h>
#include <afxcmn.h>
#include <iostream>

#define IDC_TABLE 500
#define IDC_NGRAPH 501
#define Noduri 30
#define LinkRange 150
using namespace std;

class GPS :public CFrameWnd {
	private: int Sursa,Destinatie, nAutobuz, bFlag;
		     CButton bgGraf;
		     CPen pAutobuz[Noduri + 1];
		     CPoint home, end;
		     CFont fHelvetica, fArial;
		     CListCtrl tabel;
			 int cColor;
			 int NodPrecedent[Noduri + 1];
			 bool stareVarf[Noduri + 1];
			 typedef struct{ CPoint home;
			   				 CRect rct;
							 int cost[Noduri + 1];
							 int sp[Noduri + 1];
							} NODE,NODE1;
			 NODE v[Noduri+ 1],v1[Noduri+1];
			 typedef struct {
				 int drum[Noduri + 1];
				 int nNod;
				 int cTotal;
			 } AUTOBUZ;
			 AUTOBUZ Autobuz[Noduri + 1];
	public: GPS();
			~GPS(){}
			int CMAN();
			void Dijkstra();
			void TraseazaDrum();
			void TraseazaNod(int);
			void ArataTabel();
			void AfisareSetup();
			void Initializare();
			afx_msg void OnPaint();
			afx_msg void OnLButtonDown(UINT, CPoint);
			afx_msg void ResetareGraf();
			DECLARE_MESSAGE_MAP()
};

class CMyWinApp :public CWinApp{
public:
	virtual BOOL InitInstance();
};

CMyWinApp MyApplication;
BOOL CMyWinApp::InitInstance()
{
	m_pMainWnd = new GPS;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	return TRUE;
}
