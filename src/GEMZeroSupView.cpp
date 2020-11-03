#include "GEMZeroSupView.h"

#include <QDesktopWidget>
#include <TCanvas.h>

GEMZeroSupView::GEMZeroSupView(int onlineMode) {
  fIsOnlineMode = onlineMode;
  fMapping = GEMMapping::GetInstance();
  printf("GEMZeroSupView::GEMZeroSupView() ==> All initialisation done\n") ;
}

//===========================================
GEMZeroSupView::~GEMZeroSupView() {
}

//==============================================
void GEMZeroSupView::InitZeroSupHist() {
  Int_t nbOfPlanes = fMapping->GetNbOfPlane() ;
  printf(" GEMZeroSupView::InitGUI() ==> i=%d \n",nbOfPlanes) ;

  fZeroSupDisplay              = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fRawDataDisplay              = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fADCDisplay                  = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fHitCountDisplay             = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fHitUniformityDisplay        = new PRadHistCanvas *[fMapping->GetNbOfPlane()];    
  fClusterCountDisplay         = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fClusterUniformityDisplay    = new PRadHistCanvas *[fMapping->GetNbOfPlane()];    
  fClusterMultiplicityDisplay  = new PRadHistCanvas *[fMapping->GetNbOfPlane()];
  fClusterSizeDisplay          = new PRadHistCanvas *[fMapping->GetNbOfPlane()];    

  for (int i=0; i<nbOfPlanes; i++){
    fZeroSupDisplay[i]             = new PRadHistCanvas();
    fRawDataDisplay[i]             = new PRadHistCanvas();
    fADCDisplay[i]                 = new PRadHistCanvas();
    fHitCountDisplay[i]            = new PRadHistCanvas();
    fHitUniformityDisplay[i]       = new PRadHistCanvas();    
    fClusterCountDisplay[i]        = new PRadHistCanvas();
    fClusterUniformityDisplay[i]   = new PRadHistCanvas();    
    fClusterSizeDisplay[i]         = new PRadHistCanvas();    
    fClusterMultiplicityDisplay[i] = new PRadHistCanvas();    
    DisableMouseEvent(fZeroSupDisplay[i]);
  }  
}

//===============================================
void GEMZeroSupView::InitGUI() {
  printf(" GEMZeroSupView::InitGUI() ==> Start \n") ;
  resize(QDesktopWidget().availableGeometry(this).size() * 0.85);
  fWidth = width();
  fHeight = height();

  fLayout       = new QHBoxLayout;  
  fLeftSplitter = new QVBoxLayout();  
  fTabWidget = new QTabWidget;
  fTabWidget->setMinimumWidth(fWidth);
  
  fZeroSupWidget = new QWidget();
  fLayoutForZeroSup = new QGridLayout();
  fZeroSupWidget->setLayout(fLayoutForZeroSup);
  fTabWidget->addTab(fZeroSupWidget, "Pre zero supppresion Vs Post ero suppression");

  fRawDataWidget = new QWidget();
  fLayoutForRawData = new QGridLayout();
  fRawDataWidget->setLayout(fLayoutForRawData);
  fTabWidget->addTab(fRawDataWidget, "Pre zero suppresion");

  if (!fIsOnlineMode) {
    fHitCountWidget = new QWidget();
    fLayoutForHitCount = new QGridLayout();
    fHitCountWidget->setLayout(fLayoutForHitCount);
    fTabWidget->addTab(fHitCountWidget, "Strip Hit Count Distribution");

    fHitUniformityWidget = new QWidget();
    fLayoutForHitUniformity = new QGridLayout();
    fHitUniformityWidget->setLayout(fLayoutForHitUniformity);
    fTabWidget->addTab(fHitUniformityWidget, "Strip Hit Uniformity Distribution");
 
    fClusterCountWidget = new QWidget();
    fLayoutForClusterCount = new QGridLayout();
    fClusterCountWidget->setLayout(fLayoutForClusterCount);
    fTabWidget->addTab(fClusterCountWidget, "Cluster Hit Count Distribution");

    fClusterUniformityWidget = new QWidget();
    fLayoutForClusterUniformity = new QGridLayout();
    fClusterUniformityWidget->setLayout(fLayoutForClusterUniformity);
    fTabWidget->addTab(fClusterUniformityWidget, "Cluster Hit Uniformity Distribution");

    fClusterSizeWidget = new QWidget();
    fLayoutForClusterSize = new QGridLayout();
    fClusterSizeWidget->setLayout(fLayoutForClusterSize);
    fTabWidget->addTab(fClusterSizeWidget, "Cluster size distribution");

    fClusterMultiplicityWidget = new QWidget();
    fLayoutForClusterMultiplicity = new QGridLayout();
    fClusterMultiplicityWidget->setLayout(fLayoutForClusterMultiplicity);
    fTabWidget->addTab(fClusterUniformityWidget, "Cluster multiplicity distribution");

    fADCWidget = new QWidget();
    fLayoutForADC = new QGridLayout();
    fADCWidget->setLayout(fLayoutForADC);
    fTabWidget->addTab(fADCWidget, "Cluster ADC Distribution");
  }

  map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
  map <TString, Int_t>::const_iterator plane_itr ;

  for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
    Int_t planeId = (*plane_itr).second ;
    Int_t detId = fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first ) ) ;
    Int_t i = 2 * detId + planeId ;
    fLayoutForZeroSup->addWidget(fZeroSupDisplay[i], detId, planeId) ;
    fLayoutForRawData->addWidget(fRawDataDisplay[i], detId, planeId) ;
    if (!fIsOnlineMode) {
      fLayoutForHitCount->addWidget(fHitCountDisplay[i], detId, planeId);
      fLayoutForHitUniformity->addWidget(fHitUniformityDisplay[i], detId, planeId);
      fLayoutForClusterCount->addWidget(fClusterCountDisplay[i], detId, planeId);
      fLayoutForClusterUniformity->addWidget(fClusterUniformityDisplay[i], detId, planeId);
      fLayoutForClusterSize->addWidget(fClusterSizeDisplay[i], detId, planeId);
      fLayoutForClusterMultiplicity->addWidget(fClusterMultiplicityDisplay[i], detId, planeId);
      fLayoutForADC->addWidget(fADCDisplay[i], detId, planeId);
    }
  }

  fLeftSplitter->addWidget(fTabWidget);  
  fLayout->addLayout(fLeftSplitter);
  
  fMainWidget = new QWidget;
  fMainWidget->setLayout(fLayout);
  setCentralWidget(fMainWidget);
  setWindowTitle(tr("GEM Hits Monitor"));
  printf(" GEMZeroSupView::InitGUI() ==> done \n") ;

}

//=====================================================
void GEMZeroSupView::InitToolBox() {
  printf(" GEMZeroSupView::InitToolBox() ==> Start \n") ;
  fToolBox = new QHBoxLayout;
  fEventSpin = new QSpinBox;
  
  fEventSpin->setRange(0, 100000);
  QObject::connect(fEventSpin, SIGNAL(valueChanged(int)), this, SLOT(SpinCurrentEvent(int)));
  fAnalyzer = new QPushButton("Analyze file");
  fToolBox->addWidget(fEventSpin);
  QObject::connect(fAnalyzer, SIGNAL(clicked()), this, SLOT(AnalyzeFile()));
  fToolBox->addWidget(fAnalyzer);;

  /**
  if (fConfig.GetOnlineMode() == 0) {
    fEventSpin->setRange(0, 100000);
    QObject::connect(fEventSpin, SIGNAL(valueChanged(int)), this, SLOT(SpinCurrentEvent(int)));
  }else{
    fEventSpin->setRange(fConfig.GetCycleWait(), 30);
    QObject::connect(fEventSpin, SIGNAL(valueChanged(int)), this, SLOT(ChangeTimerInterval(int)));
  }
  fToolBox->addWidget(fEventSpin);
   
  if (fConfig.GetOnlineMode()) { 
    fAutoMode = new QPushButton("Auto Update Off");
    QObject::connect(fAutoMode, SIGNAL(clicked()), this, SLOT(ChangeAutoMode()));
    fToolBox->addWidget(fAutoMode);
  }else{
    fAnalyzer = new QPushButton("Analyze file");
    QObject::connect(fAnalyzer, SIGNAL(clicked()), this, SLOT(AnalyzeFile()));
    fToolBox->addWidget(fAnalyzer);
  }
  */

  fEnableRootFile = new QPushButton("Write to Root File");
  QObject::connect(fEnableRootFile, SIGNAL(clicked()), this, SLOT(WriteRootFile()));
  fToolBox->addWidget(fEnableRootFile);

  fLeftSplitter->addLayout(fToolBox);  
  printf(" GEMZeroSupView::InitToolBox() ==> Done \n") ;

}

//==================================================
/**
void GEMZeroSupView::InitRootGraphics() {
  printf(" GEMZeroSupView::InitRootGraphics() ==> Start \n") ;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,16,0)
  // Make sure the ROOT graphical layer is initialised.
  static struct needgraph {   needgraph () {  TApplication::NeedGraphicsLibs() ;  gApplication->InitializeGraphics();} }  needgraph;
#endif
  printf(" GEMZeroSupView::InitRootGraphics() ==> Done \n") ;
}
*/

//======================================================================
void GEMZeroSupView::Clear(int plane, int type) {
  if (type == 0) {
    TCanvas *thisCanvas = fZeroSupDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fZeroSupDisplay[plane]->Refresh(); 
    
    TCanvas *theCanvas = fRawDataDisplay[plane]->GetCanvas();
    theCanvas->GetListOfPrimitives()->Clear();
    theCanvas->Modified();
    theCanvas->Update();
    fRawDataDisplay[plane]->Refresh();     
  }

  else if (type == 1) {
    TCanvas *thisCanvas = fADCDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fADCDisplay[plane]->Refresh();
  } 
  else if (type == 2) {
    TCanvas *thisCanvas = fHitCountDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fHitCountDisplay[plane]->Refresh();
  }
  else if (type == 3) {
    TCanvas *thisCanvas = fHitUniformityDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fHitUniformityDisplay[plane]->Refresh();
  }
  else if (type == 4) {
    TCanvas *thisCanvas = fClusterCountDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterCountDisplay[plane]->Refresh();
  }
  else if (type == 5) {
    TCanvas *thisCanvas = fClusterUniformityDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterUniformityDisplay[plane]->Refresh();
  }

  else if (type == 6) {
    TCanvas *thisCanvas = fClusterSizeDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterSizeDisplay[plane]->Refresh();
  }

  else if (type == 7) {
    TCanvas *thisCanvas = fClusterMultiplicityDisplay[plane]->GetCanvas();
    thisCanvas->GetListOfPrimitives()->Clear();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterMultiplicityDisplay[plane]->Refresh();
  }

}

//======================================================================
void GEMZeroSupView::DrawZeroSupHist(int plane, TH1F* preZeroSupHist, TH1F *zeroSupHist) {
  Clear(plane, 0);
  TCanvas *thisCanvas = fZeroSupDisplay[plane]->GetCanvas();
  thisCanvas->Divide(1,2);
  thisCanvas->cd(1);
  preZeroSupHist->Draw();
  thisCanvas->cd(2);
  thisCanvas->Modified();
  thisCanvas->Update();
  fZeroSupDisplay[plane]->Refresh();
  
  TCanvas *theCanvas = fRawDataDisplay[plane]->GetCanvas();
  theCanvas->cd();
  theCanvas->SetFillColor(10);
  preZeroSupHist->Draw();
  theCanvas->Modified();
  theCanvas->Update();
  fRawDataDisplay[plane]->Refresh();
}

//=======================================================================
void GEMZeroSupView::DrawADCHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode){
  Clear(plane, 1);
  TCanvas *thisCanvas = fADCDisplay[plane]->GetCanvas();
  //thisCanvas->SetFillColor(10);
  thisCanvas->cd();
  theHist->Draw();
  thisCanvas->Modified();
  thisCanvas->Update();
  fADCDisplay[plane]->Refresh();
  }
}

//==================================================================
void GEMZeroSupView::DrawHitCountHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode){
  Clear(plane, 2);
  TCanvas *thisCanvas = fHitCountDisplay[plane]->GetCanvas();
  //thisCanvas->SetFillColor(10);
  thisCanvas->cd();
  theHist->Draw();
  thisCanvas->Modified();
  thisCanvas->Update();
  fHitCountDisplay[plane]->Refresh();
  }
}

//==================================================================
void GEMZeroSupView::DrawHitUniformityHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode) {
    Clear(plane, 3);
    TCanvas *thisCanvas = fHitUniformityDisplay[plane]->GetCanvas();
    //thisCanvas->SetFillColor(10);
    thisCanvas->cd();
    theHist->Draw();
    thisCanvas->Modified();
    thisCanvas->Update();
    fHitUniformityDisplay[plane]->Refresh();
  }
}

//==================================================================
void GEMZeroSupView::DrawClusterCountHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode){
  Clear(plane, 4);
  TCanvas *thisCanvas = fClusterCountDisplay[plane]->GetCanvas();
  //thisCanvas->SetFillColor(10);
  thisCanvas->cd();
  theHist->Draw();
  thisCanvas->Modified();
  thisCanvas->Update();
  fClusterCountDisplay[plane]->Refresh();
  }
}

//==================================================================
void GEMZeroSupView::DrawClusterUniformityHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode) {
    Clear(plane, 5);
    TCanvas *thisCanvas = fClusterUniformityDisplay[plane]->GetCanvas();
    //thisCanvas->SetFillColor(10);
    thisCanvas->cd();
    theHist->Draw();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterUniformityDisplay[plane]->Refresh();
  }
}
//==================================================================
void GEMZeroSupView::DrawClusterSizeHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode) {
    Clear(plane, 6);
    TCanvas *thisCanvas = fClusterSizeDisplay[plane]->GetCanvas();
    //thisCanvas->SetFillColor(10);
    thisCanvas->cd();
    theHist->Draw();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterSizeDisplay[plane]->Refresh();
  }
}

//==================================================================
void GEMZeroSupView::DrawClusterMultiplicityHist(int plane, TH1F* theHist) {
  if (!fIsOnlineMode) {
    Clear(plane, 7);
    TCanvas *thisCanvas = fClusterMultiplicityDisplay[plane]->GetCanvas();
    //thisCanvas->SetFillColor(10);
    thisCanvas->cd();
    theHist->Draw();
    thisCanvas->Modified();
    thisCanvas->Update();
    fClusterMultiplicityDisplay[plane]->Refresh();
  }
}

//====================================================================
void GEMZeroSupView::DisableMouseEvent(PRadHistCanvas* theWidget) {
  //theWidget->DisableSignalEvents(kMousePressEvent);
  //theWidget->DisableSignalEvents(kMouseMoveEvent);
  //theWidget->DisableSignalEvents(kMouseReleaseEvent);
  //theWidget->DisableSignalEvents(kMouseDoubleClickEvent);
  //theWidget->DisableSignalEvents(kKeyPressEvent);
  //theWidget->DisableSignalEvents(kEnterEvent);
  //theWidget->DisableSignalEvents(kLeaveEvent);
}












