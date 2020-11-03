#include <cassert>
#include <iostream>

#include "hardcode.h"
#include "GEMHistContainer.h"


using namespace std;

//====================================================================
GEMHistContainer::GEMHistContainer(GEMView* const p, int id) : 
    fParent(p), fModuleID(id), fIsActive(false)
{

    //   printf(" GEMHistContainer::GEMHistContainer() => %d \n", id) ;
    this->setObjectName(QString::fromUtf8(Form("APV_%d", id)));
    fCanvas = new PRadHistCanvas(this);
    fCanvas -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(fCanvas);

    //EnableSignalEvents(kMousePressEvent);
    fAPV = nullptr;
    fRootHist = nullptr;
    TCanvas *c1 = fCanvas->GetCanvas();
    c1->SetTopMargin(0.12);
}

//====================================================================
GEMHistContainer::~GEMHistContainer() {}

//====================================================================
void GEMHistContainer::connectAPV(GEMAPV *apv) {
    if (fIsActive) fAPV = apv;
}

//_______________________________________________________________
void GEMHistContainer::RegisterHist(TH1I *hist) {
    if (fIsActive) fRootHist = hist;
}

//====================================================================
TH1I * GEMHistContainer::getHist() {
    if (fIsActive) return fRootHist;
    else return 0;
}

//====================================================================
void GEMHistContainer::drawHist() {
    //std::cout<<"GEMHistContainter::drawHist"<<std::endl;
    if(!fRootHist) std::cout<<"histogram not connected."<<std::endl;
    gStyle->SetOptStat(0);
    if (fIsActive)
    {
	TCanvas *c1 = fCanvas->GetCanvas();
	c1->cd();
	c1->SetFillColor(10);
	//gPad->SetTopMargin(0.01);
	//gPad->SetBottomMargin(0.01);
	//gPad->SetLeftMargin(0.01);
	//gPad->SetRightMargin(0.01);
	fRootHist->Draw();
	c1->Modified();
	c1->Update();
	fCanvas->Refresh();
    }
}

//====================================================================
void GEMHistContainer::enterEvent(QEvent *event) {
    if (fIsActive){
	if (fAPV != nullptr)  fAPV->turnOnModuleColor(true);
	TCanvas *c1 = fCanvas->GetCanvas();
	//    c1->SetFillColor(5);
	c1->Modified();
	c1->Update();

    }
    //TQtWidget::enterEvent(event);
    QWidget::enterEvent(event);
}

//====================================================================
void GEMHistContainer::leaveEvent(QEvent *event) {
    if (fIsActive) {
	if (fAPV != nullptr) fAPV->turnOnModuleColor(false);
	TCanvas *c1 = fCanvas->GetCanvas();
	//    c1->SetFillColor(10);
	c1->Modified();
	c1->Update();
    }
    //TQtWidget::leaveEvent(event);
    QWidget::leaveEvent(event);
}

//====================================================================
void GEMHistContainer::mousePressEvent(QMouseEvent* e) {
    if (fIsActive) {
	if (e->button() == Qt::LeftButton) printf(" GEMHistContainer::mousePressEvent(0 => DrawCornerHisto(fRootHist) has been disbled for now \n") ;
    }
    //TQtWidget::mousePressEvent(e);
    QWidget::mousePressEvent(e);
}

//====================================================================_
void GEMHistContainer::SetBasicInfo(TString plane, int fecid, int apv_index, int apv_id) {
    fFECID = fecid;
    fPlane = plane;
    fIndex = apv_index;
    fAPVID = apv_id;
}

//====================================================================_
void GEMHistContainer::RefreshGraphics() {
    TCanvas *c1 = fCanvas->GetCanvas();
    c1->Modified();
    c1->Update();
    fCanvas->Refresh();
}
