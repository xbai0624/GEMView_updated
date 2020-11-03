#include "GEMFEC.h"

#include <TCanvas.h>
#include <vector>
#include <cassert>
#include <TSystem.h>
#include <TStyle.h>
#include <TAxis.h>

//====================================================================
GEMFEC::GEMFEC() {}

//====================================================================
GEMFEC::GEMFEC(GEMView* const p, int FECid, int nChannel, list<int> * APV_id_list):
    fFECid(FECid), fNChannel(nChannel), fNBins(2500) 
{

    //  printf("  GEMFEC::GEMFEC(), FECId=%d, numberChannels=%d \n", FECid, fNChannel) ;

    fMapping = GEMMapping::GetInstance();
    //EnableSignalEvents(kMousePressEvent);
    this->setObjectName(QString::fromUtf8(Form("FEC %i", fFECid)));

    // root canvas
    fCanvas = new PRadHistCanvas(this);
    //fCanvas -> resize(width(), height());
    fCanvas -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *layout= new QVBoxLayout(this);
    layout->addWidget(fCanvas);

    fChannelHist = new TH1I * [fNChannel];
    for (int i=0; i<fNChannel; i++) {
	//    printf("  GEMFEC::GEMFEC(), FECId=%d, Channel=%d \n", FECid, i) ;
	fChannelIsActive.push_back(0);
	fChannelHist[i] = new TH1I (Form("APV_%d_FEC_%d", i, fFECid), Form("APV_%d_FEC_%d_INACTIVE", i, fFECid), fNBins, 0, fNBins);
	fChannelHist[i] -> SetLabelSize(0.05, "X");
	fChannelHist[i] -> SetLabelSize(0.05, "Y");
	gStyle -> SetTitleFontSize(0.08);
	fChannelHist[i]->SetStats(0);
    }

    //set the active channel according to the list
    for (std::list<int>::iterator it = (*APV_id_list).begin(); it != (*APV_id_list).end(); it++){
	TString thisPlane = (fMapping->GetPlaneFromAPVID(*it)).Data();
	Int_t thisIndex = fMapping->GetAPVIndexOnPlane(*it);
	fAPVID.push_back(*it);
	// int apvCh = *it % fNChannel; // old version
	// XB update:
	int apvCh =((*it)&0xF) % fNChannel; // (*it) is actually apv_index, composed by: (fec_id<<4)|adc_ch

	assert(apvCh<fNChannel);
	fChannelIsActive.at(apvCh) = 1;
	fChannelHist[apvCh]->SetTitle(Form("APV_%d_FEC_%d_%s_%d", apvCh, fFECid, thisPlane.Data(), thisIndex));	
    }
}
//====================================================================
GEMFEC::~GEMFEC()
{
    for (int i=0; i<fNChannel; i++){
	delete fChannelHist[i];	
    } 
}

//====================================================================
void GEMFEC::ClearHist() {
    for (int i=0; i<fNChannel; i++) fChannelHist[i]->Reset("M");
    TCanvas *c1 = fCanvas->GetCanvas();
    c1->GetListOfPrimitives()->Clear();
    c1->Modified();
    c1->Update();
}

//====================================================================
void GEMFEC::DrawHist() {
    fCanvas->resize(width(), height());
    TCanvas *c1 = fCanvas->GetCanvas();
    //  c1->SetFillColor(10);
    c1->Divide(4,4);
    for (int i=0; i<fNChannel; i++) {
	c1->cd(i+1);
	TPad* pad = (TPad*)c1->GetPad(i+1);
	pad->SetLeftMargin(0.13);
	fChannelHist[i]->Draw();
    }
    c1->Modified();
    c1->Update();
    fCanvas->Refresh();
}

//====================================================================
int GEMFEC::GetAPVIDFromThisFEC(int i) {
    if (i > (int)fAPVID.size() - 1){
	cout<<"FEC "<<fFECid<<" does not has that many active channels"<<endl;
	return -1;
    }else{
	return fAPVID.at(i);
    }
}

//====================================================================
void GEMFEC::RefreshGraphics() {
    TCanvas *c1 = fCanvas->GetCanvas();
    c1->Modified();
    c1->Update();
    fCanvas->Refresh();
}
