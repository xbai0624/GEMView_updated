//============================================================================//
// A class contains a few root canvas                                         //
//                                                                            //
// Chao Peng                                                                  //
// 02/27/2016                                                                 //
//============================================================================//

#include <QLayout>
#include <QTimer>
#include <QResizeEvent>

#include "TApplication.h"
#include "TVirtualX.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TFormula.h"
#include "TF1.h"
#include "TH1.h"
#include "TFrame.h"
#include "TTimer.h"
#include "TRandom.h"
#include "TAxis.h"
#include "TList.h"

#include "PRadHistCanvas.h"
#include "QRootCanvas.h"


PRadHistCanvas::PRadHistCanvas(QWidget *parent) : QWidget(parent)
{
    // add canvas in vertical layout
    canvas1 = new QRootCanvas(this);
    //canvas1 -> resize(width(), height());
    canvas1 -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(canvas1);

    // root timer to process root events
    rootTimer = new QTimer(this);
    QObject::connect( rootTimer, SIGNAL(timeout()), this, SLOT(rootInnerLoop()) );
    rootTimer->start(50);

    // global settings for root
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetStatFontSize(0.08);
}

void PRadHistCanvas::rootInnerLoop()
{
   //call the inner loop of ROOT
   gSystem->ProcessEvents();
}

// show the histogram in second slot
void PRadHistCanvas::UpdateHist(TObject *hist)
{
    TCanvas *c2 = canvas1->GetCanvas();
    c2->cd();
    c2->SetGrid();
    gPad->SetLogy();

    TH1I *energyHist = (TH1I*)hist;

    energyHist->GetXaxis()->SetLabelSize(0.08);
    energyHist->GetYaxis()->SetLabelSize(0.08);
    energyHist->SetFillColor(46);
    energyHist->Draw();
    canvas1->Refresh();
}
