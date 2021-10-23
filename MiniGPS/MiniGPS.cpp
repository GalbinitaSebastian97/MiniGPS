#include "GPS.h"

BEGIN_MESSAGE_MAP(GPS, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_NGRAPH, ResetareGraf)
END_MESSAGE_MAP()

GPS::GPS(){
	home = CPoint(30, 100); 
	end = CPoint(500, 700);
	Create(NULL, L"GPS", WS_OVERLAPPEDWINDOW, CRect(0, 0, 1000, 800));//creaza fereastra 
	bgGraf.Create(L"Graf nou", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, CRect(CPoint(home.x+600, home.y +550), CSize(180, 40)), this, IDC_NGRAPH);//buton
	AfisareSetup(); 
	ResetareGraf();
}

void GPS::ResetareGraf(){
	CString s[] = { "Autobuz","Pornire","Destinatie","Cost","Statii","Drum"};
	nAutobuz = 0; 
	Initializare(); 
	Invalidate();
	tabel.DestroyWindow();
	tabel.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER, CRect(CPoint(end.x + 30, home.y-10), CSize(430, 550)), this, IDC_TABLE);
	for (int i = 0; i <= 5; i++) tabel.InsertColumn(i, s[i], LVCFMT_LEFT, ((i == 5) ? 250 : 60));
}

void GPS::AfisareSetup(){
	cColor = 1;
	int Color[] = { RGB(150,150,150),
	RGB(0,0,200),RGB(200,0,0),
	RGB(0,200,0),RGB(200,200,0),
	RGB(0,200,200),RGB(200,0,200),
	RGB(200,50,150),RGB(100,100,255),
	RGB(250,50,255),RGB(100,200,50) };
	for (int i = 0; i <= 10; i++) pAutobuz[i].CreatePen(PS_SOLID, 2, Color[i]);
	fHelvetica.CreatePointFont(100, L"Helvetica");
	fArial.CreatePointFont(60, L"Arial");
}

void GPS::Initializare(){
	int i, j, u, w;
	double distance;
	srand(time(0));
	bFlag = 0;
	if (nAutobuz == 0){
		for (i = 1; i <= Noduri; i++){
			//calculeaza valori random pentru pentu punctele din graf
			v[i].home.x = home.x + 20 + rand() % (end.x - home.x - 50);
			v[i].home.y = home.y + 20 + rand() % (end.y - home.y - 50);
			v[i].rct = CRect(v[i].home.x, v[i].home.y, v[i].home.x + 25, v[i].home.y + 25);//coordonatele dreptunghiului 
		}
		for (i = 1; i <= Noduri; i++)
			for (j = i; j <= Noduri; j++){
				v[i].cost[j] = 99;
				v1[i].cost[j] = 99;
				distance = sqrt(pow((double)(v[i].home.x - v[j].home.x), 2) + pow((double)(v[i].home.y - v[j].home.y), 2));
				if (distance < LinkRange) {
					v[i].cost[j] = 1 + rand() % 9;//costul drumului 
					v1[i].cost[j] = v[i].cost[j];
				}
				v[j].cost[i] = v[i].cost[j];
				v1[j].cost[i] = v1[i].cost[j];
			}
	} else {
		u = Autobuz[nAutobuz].drum[1];
		for (i = 1; i <= Autobuz[nAutobuz].nNod - 1; i++){
			w = Autobuz[nAutobuz].drum[i + 1];
			v[u].cost[w] = 99; 
			v[w].cost[u] = 99;
			u = w;
		}
		
	}
}

void GPS::TraseazaNod(int u){
	CClientDC dc(this);
	CString s;
	CPoint punct;
	CPen pBlack(PS_SOLID, 2, RGB(0, 0, 0));
	CPen pRed(PS_SOLID, 3, RGB(200, 0, 0));
	if (u == Sursa || u == Destinatie) dc.SelectObject(pRed);
	 else dc.SelectObject(pBlack);
	punct = v[u].home;
	dc.Ellipse(v[u].rct);
	s.Format(L"%d", u); dc.TextOut(punct.x + 4, punct.y + 4, s);
}
//nu are nici o treaba
void GPS::OnPaint(){
	CPaintDC dc(this);
	CString s;
	CRect rct;
	CPen rPen(PS_SOLID, 3, RGB(0, 0, 0));
	CPen mPen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen qPen(PS_SOLID, 1, RGB(100, 100, 100));
	CPoint mPoint, pt;
	int i, j;
	dc.SelectObject(rPen);
	dc.Rectangle(home.x - 10, home.y - 10, end.x + 10, end.y + 10);
	dc.SelectObject(qPen); 
	dc.SelectObject(fArial);
	for (i = 1; i <= Noduri; i++)
		for (j = 1; j <= Noduri; j++)
			if (v[i].cost[j] != 99) {
				dc.MoveTo(CPoint(v[i].home));
				dc.LineTo(CPoint(v[j].home));
				mPoint = CPoint((v[i].home.x + v[j].home.x) / 2, (v[i].home.y + v[j].home.y) / 2);
				s.Format(L"%d", v[i].cost[j]);
				dc.TextOut(mPoint.x, mPoint.y, s);
			}
	dc.SelectObject(fHelvetica); dc.SelectObject(mPen);
	for (i = 1; i <= Noduri; i++) TraseazaNod(i);
}

int GPS::CMAN() {
	int minDistance = 99;
	int cun;
	for (int i = 1; i <= Noduri; i++)
		if ((!stareVarf[i]) && (minDistance >= v[Sursa].sp[i])) {
			minDistance = v[Sursa].sp[i];
			cun = i;
		}
	return cun;
}
void GPS::Dijkstra(){
	int minDistance = 99;
	int cun;
	//Initializare
	for (int i = 1; i <= Noduri; i++) {
		stareVarf[i] = false;
		NodPrecedent[i] = -1;
		v[Sursa].sp[i] = 99;
	}
	v[Sursa].sp[Sursa] = 0;
	//compute
	int k = 0;
	while (k < Noduri) {
		cun = CMAN();
		stareVarf[cun] = true;
		for (int i = 1; i <= Noduri; i++)
			if ((!stareVarf[i]) && (v[cun].cost[i] > 0))
				if (v[Sursa].sp[i] > v[Sursa].sp[cun] + v[cun].cost[i])
				{
					v[Sursa].sp[i] = v[Sursa].sp[cun] + v[cun].cost[i];
					NodPrecedent[i] = cun;
				}
		k++;
	}
	nAutobuz++;
	Autobuz[nAutobuz].cTotal= v[Sursa].sp[Destinatie];
}

void GPS::TraseazaDrum(){
	CClientDC dc(this);
	int w, i, u, r, dummy[Noduri + 1];
	//afisam  Sursa
	dc.SelectObject(pAutobuz[1 + cColor % 10]); 
	cColor++;
	w = Destinatie; 
	Autobuz[nAutobuz].nNod = 0; 
	Autobuz[nAutobuz].drum[1] = Sursa;
	for (i = 1; i <= Noduri; i++){
		u = NodPrecedent[w];
		if (u != -1){
			r = ++Autobuz[nAutobuz].nNod; 
			dummy[r] = w;
			w = u;
		}
		dummy[r + 1] = Sursa;
	}
	r = ++Autobuz[nAutobuz].nNod; 
	Autobuz[nAutobuz].drum[r] = Destinatie;
	u = Sursa; 
	Autobuz[nAutobuz].drum[1] = u;
	dc.MoveTo(v[u].home);
	for (i = 1; i <= Autobuz[nAutobuz].nNod; i++){
		w = dummy[Autobuz[nAutobuz].nNod - i + 1];
		Autobuz[nAutobuz].drum[i] = w;
		dc.LineTo(v[w].home);
		u = w;
	}
	w = Destinatie; 
	Autobuz[nAutobuz].drum[i] = w;
	dc.LineTo(v[w].home);
}

void GPS::OnLButtonDown(UINT nFlags, CPoint pt){
	CClientDC dc(this);
	CString s;
	dc.SelectObject(pAutobuz[1]); 
	dc.SelectObject(fArial);
	for (int i = 1; i <= Noduri; i++)
		if (v[i].rct.PtInRect(pt)){
			bFlag++;
			if (bFlag == 1) //bFlag=1 este  Sursa
			{
				Sursa = i;
				TraseazaNod(Sursa);
			}
			if (bFlag == 2) //bFlag=2 este Destinatia
			{
				Destinatie = i; 
				TraseazaNod(Destinatie);
				for (int j = 1; j <= 3; j++)
				{
					Dijkstra();
					if (v[Sursa].sp[Destinatie] != 99)
					{
						TraseazaDrum(); 
						ArataTabel();
						Initializare();
					}
					else
					{
						bFlag = 0;
						dc.SelectObject(pAutobuz[0]);
						TraseazaNod(Sursa); TraseazaNod(Destinatie);
					}
				}

			}
			for (int i = 1; i <= Noduri; i++)
				for (int j = 1; j <= Noduri; j++) {
					v[i].cost[j] = v1[i].cost[j];
				}
		}

}

void GPS::ArataTabel(){
	int i, k, w, u;
	CString s, S;
	for (i = 1; i <= nAutobuz; i++){
		tabel.DeleteItem(i - 1);
		s.Format(L"%d", i); tabel.InsertItem(i - 1, s, 0);
		s.Format(L"%d", Autobuz[i].drum[1]); tabel.SetItemText(i - 1, 1, s);//Pornire
		w = Autobuz[i].nNod;//calculez destiatia
		s.Format(L"%d", Autobuz[i].drum[w]); tabel.SetItemText(i - 1, 2, s);//Destinatie
		u = Autobuz[i].drum[1]; w = Autobuz[i].drum[w];
		s.Format(L"%d", Autobuz[i].cTotal); tabel.SetItemText(i - 1, 3, s);//Costul total al drumului 
		s.Format(L"%d", Autobuz[i].nNod); tabel.SetItemText(i - 1, 4, s); //Numarul de statii prin care a trecut
		S = "";
		for (k = 1; k <= Autobuz[i].nNod; k++) { s.Format(L"%d ", Autobuz[i].drum[k]); S += s; }
		tabel.SetItemText(i - 1, 5, S);//Afisarea drumului 
	}
}