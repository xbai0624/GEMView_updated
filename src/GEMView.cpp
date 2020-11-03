#include "GEMView.h"
#include "stdio.h"
#include "TApplication.h"
#include "TAxis.h"
#include <QDesktopWidget>
#include <Qt>
#include <QSplitter>
#include <QGraphicsView>
#include <QPalette>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <QDialog>
#include "PRadHistCanvas.h"
#include "TMath.h"
using namespace std;
#define SEC *1e3
//=====================================================
//static variables to be used in the global scale
//PRadClient* GEMView::TheClient = new PRadClient();

QTextEdit & TheMessager() {
    static QTextEdit * fMessager = new QTextEdit();
    return *fMessager;
}

QTextCursor & TheTextCursor() {
    static QTextCursor * fTextCursor = new QTextCursor(TheMessager().textCursor());
    return *fTextCursor;
}

QTextBlockFormat & TheTextBlockFormat() {
    static QTextBlockFormat * fTextBlockFormat = new QTextBlockFormat();
    return *fTextBlockFormat;
}

static int TheCurrentLine = 0 ;

//==================================================
GEMView::GEMView():QMainWindow() {   
    //  fConfig.Init("./db/Config.cfg"); 
    //  fConfig.Init("./db/Config_GEM_TRD.cfg"); 
    //  fConfig.Init("./db/Config_crGEM_Sr90.cfg"); 
    //  fConfig.Init("./db/Config_crGEM_3800V_Xray_50uA.cfg"); 
    //  fConfig.Init("./db/Config_TRDGEM_20171014.cfg");
    //  fConfig.Init("./db/Config_crGEM_fineBins.cfg"); 
    //fConfig.Init("./db/ConfigECU.cfg");  // use default value
    fRunType = (TString) fConfig.GetRunType() ;
    fNbOfTimeSamples = fConfig.GetNbOfTimeSamples() ;
    printf(" = GEMView::GEMView() ==> Start the run of type = %s and nbTimeS = %d\n",  fRunType.Data(), fNbOfTimeSamples) ;
    //add reading map here
    fMapping = GEMMapping::GetInstance();
    fMapping->LoadMapping(fConfig.GetInputMapName());
    fMapping->PrintMapping() ;
    fNbADCBins = 50 ;
    printf(" = GEMView::GEMView() ==> Mapping is loaded \n") ;
    InitConfig();
    InitGUI();
    InitToolBox();
    InitRootGraphics();
    fCurrentEntry = 1; //start counting event from 1
    SetMonitor(fConfig.GetCycleWait() SEC);//second
    printf(" = GEMView::InitConfig() ==> All initialisation done \n\n") ;

}

//==================================================
GEMView::~GEMView() {  
}

//==================================================
void GEMView::InitConfig() {
    printf(" = GEMView::InitConfig() ==> Enter \n") ;

    if (fConfig.GetOnlineMode()) {
	fHandler = new GEMInputHandler(fConfig.GetETIPAddress(), fConfig.GetTCPPort(), fConfig.GetInputFileName());
	assert(fConfig.GetETStationName() != NULL);
	std::string astr(fConfig.GetETStationName());		
	fHandler->CreateStation(astr, fConfig.GetETMode());
	fHandler->AttachStation();
	printf(" = GEMView::InitConfig() ==> ONLINE fHandler is initialised \n") ;
    } 
    else { 
	fHandler = new GEMInputHandler() ;
	printf(" = GEMView::InitConfig() ==> OFFLINE fHandler is initialised \n") ;
    }

    // Run config Parameters
    fHandler->SetZeroSupCut(fConfig.GetZeroSupCut()) ;;
    fHandler->SetCommonModeCut(fConfig.GetComModeCut()) ;
    fHandler->SetHitPeakOrSumADCs(fConfig.GetHitPeakOrSumADCs()) ;
    fHandler->SetCentralOrAllStripsADCs(fConfig.GetCentralOrAllStripsADCs()) ;
    fHandler->SetMinADCvalue(fConfig.GetMinADCvalue()) ;
    fHandler->SetMinClusterSize(fConfig.GetMinClusterSize()) ;
    fHandler->SetMaxClusterSize(fConfig.GetMaxClusterSize()) ;
    fHandler->SetMaxClusterMult(fConfig.GetMaxClusterMult()) ;
    fHandler->SetNbOfTimeSamples(fConfig.GetNbOfTimeSamples()) ;
    fHandler->SetStopTimeSample(fConfig.GetStopTimeSamples()) ;
    fHandler->SetStartTimeSample(fConfig.GetStartTimeSamples()) ;
    fRootFileName = fConfig.GetOutputFileName() ;

    // Zero suppression mode
    if ( ( fRunType == "SINGLEEVENT") || ( fRunType == "MULTIEVENTS") || ( fRunType == "MULTIFILES") || ( fRunType == "SEARCHEVENTS") ) {
	InitHistForZeroSup() ;
	fPedestal = new GEMPedestal(fConfig.GetPedFileName(), fConfig.GetNbOfTimeSamples());
	fHandler->InitPedestal(fPedestal); 
	printf(" = GEMView::InitConfig() ==> Initialisation for a %s Run \n",  fRunType.Data()) ;
    }

    // Pedestal mode
    else if ( fRunType == "PEDESTAL") {
	InitHistForRawData() ;
	fPedestal = new GEMPedestal(fConfig.GetPedFileName(), fConfig.GetNbOfTimeSamples());
	fPedestal->BookHistos() ;
	fHandler->InitPedestal(fPedestal); 
	printf(" = GEMView::InitConfig() ==> Initialisation for a %s Run \n",  fRunType.Data()) ;
    }

    // Raw data mode
    else if ( fRunType == "RAWDATA") { 
	InitHistForRawData() ;
	printf(" = GEMView::InitConfig() ==> Initialisation for a %s Run \n",  fRunType.Data()) ;
    }
    else printf(" = GEMView::InitConfig() ==> No Run specified \n") ;
    printf(" = GEMView::InitConfig() ==> Done \n") ;
}

//=====================================================
void GEMView::InitGUI() {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,16,0)
    // Make sure the ROOT graphical layer is initialised.
    //static struct needgraph {   needgraph () {  TApplication::NeedGraphicsLibs() ;  gApplication->InitializeGraphics();} }  needgraph;
#endif

    printf(" = GEMView::InitGUI() ==> Enter Init GUI for a %s run\n",  fRunType.Data()) ;
    if (fRunType == "MULTIFILES") return ;

    //resize(QDesktopWidget().availableGeometry(this).size() * 0.975);
    fWidth = width();
    fHeight = height();

    fLayout       = new QHBoxLayout;
    fLeftSplitter = new QVBoxLayout();
    fTabWidget    = new QTabWidget;
    fTabWidget->setMinimumWidth(fWidth);

    if ( (fRunType == "SINGLEEVENT") || (fRunType == "SEARCHEVENTS") ) {
	// printf(" = GEMView::InitGUI() ==> Init GUI for a %s run\n",  fRunType.Data()) ;
	f1DSingleEventWidget  = new QWidget();
	f1DSingleEventLayout  = new QGridLayout();
	f1DSingleEventWidget->setLayout(f1DSingleEventLayout);
	fTabWidget->addTab(f1DSingleEventWidget, "1D Single Event Hit display");

	f2DSingleEventWidget  = new QWidget();
	f2DSingleEventLayout  = new QGridLayout();
	f2DSingleEventWidget->setLayout(f2DSingleEventLayout);
	fTabWidget->addTab(f2DSingleEventWidget, "2D Single Event Hit display");

	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    Int_t planeId = (*plane_itr).second ;
	    //     if (planeId == 1) continue ;
	    Int_t detId = fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first ) ) ;
	    Int_t i = 2 * detId + planeId ;
	    Int_t j = fMapping->GetNbOfPlane() + i ;
	    f1DSingleEventLayout->addWidget(f1DSingleEventDisplay[j], detId, 2*planeId) ;
	    f1DSingleEventLayout->addWidget(f1DSingleEventDisplay[i], detId,  2*planeId+1) ;
	    f2DSingleEventLayout->addWidget(f2DSingleEventDisplay[i], detId, 2*planeId) ;
	    f2DSingleEventLayout->addWidget(f2DSingleEventDisplay[j], detId,  2*planeId+1) ;
	    printf(" = GEMView::InitGUI() ==> Widget initialized for %s \n",  fRunType.Data()) ;
	}
    }

    else if (fRunType == "MULTIEVENTS") {
	// printf(" = GEMView::InitGUI() ==> Init GUI for a %s run\n",  fRunType.Data()) ;
	resize(QDesktopWidget().availableGeometry(this).size() * 0.25);

	// R/O planes
	fHitWidget = new QWidget();
	fHitLayout = new QGridLayout();
	fHitWidget->setLayout(fHitLayout);
	fTabWidget->addTab(fHitWidget, "Hit Position and ADCs");

	fADCWidget = new QWidget();
	fADCLayout = new QGridLayout();
	fADCWidget->setLayout(fADCLayout);
	fTabWidget->addTab(fADCWidget, "ADC Distribution") ;

	fClusterWidget = new QWidget();
	fClusterLayout = new QGridLayout();
	fClusterWidget->setLayout(fClusterLayout);
	fTabWidget->addTab(fClusterWidget, "Cluster Position and ADCs"); 

	fClusterInfoWidget = new QWidget();
	fClusterInfoLayout = new QGridLayout();
	fClusterInfoWidget->setLayout(fClusterInfoLayout);
	fTabWidget->addTab(fClusterInfoWidget, "Cluster Size and Multiplicity");

	fADCTimeBinPosWidget = new QWidget();
	fADCTimeBinPosLayout = new QGridLayout();
	fADCTimeBinPosWidget->setLayout(fADCTimeBinPosLayout);
	fTabWidget->addTab(fADCTimeBinPosWidget, "ADC vs. Position and timeBin");

	printf(" = GEMView::InitGUI() ==>  Widget \n") ;

	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    Int_t planeId = (*plane_itr).second ;
	    Int_t detId = fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first ) ) ;

	    Int_t i = 2 * detId + planeId ;
	    Int_t j = fMapping->GetNbOfPlane() + i ;
	    Int_t l = fMapping->GetNbOfPlane() + detId ;

	    fHitLayout->addWidget(fHitDisplay[i], detId, planeId);
	    fHitLayout->addWidget(fHitDisplay[j], l, planeId);

	    fClusterLayout->addWidget(fClusterDisplay[i], detId, planeId);
	    fClusterLayout->addWidget(fClusterDisplay[j], l, planeId);

	    fClusterInfoLayout->addWidget(fClusterInfoDisplay[i], detId, planeId);
	    fClusterInfoLayout->addWidget(fClusterInfoDisplay[j], l, planeId);

	    fADCLayout->addWidget(fADCDisplay[i], detId, planeId);
	    //      fADCLayout->addWidget(fADCDisplay[j], l, planeId);

	    fADCTimeBinPosLayout->addWidget(fADCTimeBinPosDisplay[i], detId, planeId);
	    fADCTimeBinPosLayout->addWidget(fADCTimeBinPosDisplay[j], l, planeId);
	}

	// Detector 2D infos
	f2DPlotsWidget    = new QWidget();
	f2DPlotsLayout = new QGridLayout();
	f2DPlotsWidget->setLayout(f2DPlotsLayout);
	fTabWidget->addTab(f2DPlotsWidget, "2D Cluster Position");

	fChargeSharingWidget    = new QWidget();
	fChargeSharingLayout = new QGridLayout();
	fChargeSharingWidget->setLayout(fChargeSharingLayout);
	fTabWidget->addTab(fChargeSharingWidget, "X-Y Charge correlation");

	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;

	    TString detector = (*det_itr).second ;
	    f2DPlotsLayout->addWidget(f2DPlotsDisplay[3*detId], 3*detId, 0);
	    f2DPlotsLayout->addWidget(f2DPlotsDisplay[3*detId+1], 3*detId, 1);
	    f2DPlotsLayout->addWidget(f2DPlotsDisplay[3*detId+2], 3*detId, 2);
	    fChargeSharingLayout->addWidget(fChargeSharingDisplay[2*detId], 2*detId, 0);
	    fChargeSharingLayout->addWidget(fChargeSharingDisplay[2*detId+1], 2*detId, 1); 
	}
    }

    else if ( fRunType == "RAWDATA") {
	// printf(" = GEMView::InitGUI() ==> Init GUI for a %s run\n",  fRunType.Data()) ;

        // tab for all APVs combined
	fAllFECWidget = new QWidget ;
	fTabWidget->addTab(fAllFECWidget, tr("All FECs"));
	fAllFECWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// tab for separate APVs
	QWidget **page = new QWidget*[fMapping->GetNbOfFECs()];
	QVBoxLayout **sub_fec_layout = new QVBoxLayout*[fMapping->GetNbOfFECs()];
	for (int i=0; i<fMapping->GetNbOfFECs(); i++) 
	{
	    char thisName[255];
	    sprintf(thisName, "FEC %d (IP: %s)", i, (fMapping->GetFECIPFromFECID(i)).Data());
	    page[i] = new QWidget();
	    sub_fec_layout[i] = new QVBoxLayout(page[i]);
	    sub_fec_layout[i]->addWidget(fFEC[i]);
	    page[i]->resize(sizeHint());
	    page[i]->setLayout(sub_fec_layout[i]);
	    fTabWidget->addTab(page[i], tr(thisName));

	    //fTabWidget->addTab(fFEC[i], tr(thisName));
	}   

	int count=0;
	fLayoutForAllAPV = new QGridLayout();   
	fLayoutForAllAPV -> setVerticalSpacing(0);
	for (int i=0; i<fMapping->GetNbOfAPVs(); i++){
	    if (fRootWidget[i]->IsActive()) { 
		if (count <fMapping->GetNbOfAPVs()) {         
		    div_t position  = div(count, fConfig.GetNCol()) ;
		    fRootWidget[i] -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		    fLayoutForAllAPV->addWidget(fRootWidget[i], position.quot, position.rem);
		}

		count++;
	    }
	}   
	fAllFECWidget->setLayout(fLayoutForAllAPV);       
    }

    else if ( fRunType == "PEDESTAL") {
	// printf(" = GEMView::InitGUI() ==> Init GUI for a %s run\n",  fRunType.Data()) ;
	fAllFECWidget = new QWidget();
	fTabWidget->addTab(fAllFECWidget, tr("All FECs"));

	for (int i=0; i<fMapping->GetNbOfFECs(); i++) 
	{
	    char thisName[255];
	    sprintf(thisName, "FEC %d (IP: %s)", i, (fMapping->GetFECIPFromFECID(i)).Data());
	    fTabWidget->addTab(fFEC[i], tr(thisName));
	} 

	int count=0;
	fLayoutForAllAPV = new QGridLayout();   
	for (int i=0; i<fMapping->GetNbOfAPVs(); i++){
	    if (fRootWidget[i]->IsActive()) { 
		if (count < fMapping->GetNbOfAPVs() ) {         
		    div_t position = div(count, fConfig.GetNCol());
		    fLayoutForAllAPV->addWidget(fRootWidget[i], position.quot, position.rem);
		}
		count++;
	    }
	}      
	fAllFECWidget->setLayout(fLayoutForAllAPV);      
    }

    else printf(" = GEMView::InitGUI() ==> No Run specified \n") ;

    fLeftSplitter->addWidget(fTabWidget);
    fLayout->addLayout(fLeftSplitter);
    fMainWidget = new QWidget;
    fMainWidget->setLayout(fLayout);
    setCentralWidget(fMainWidget);
    setWindowTitle(tr("SRS Data Monitor"));
    printf(" = GEMView::InitGUI() ==> Exit Init GUI for a %s run\n",  fRunType.Data()) ;
}

//=====================================================
void GEMView::InitToolBox() 
{
    printf(" = GEMView::InitToolBox() ==> Enter Init Tool Box for a %s run\n",  fRunType.Data()) ;
    fToolBox = new QHBoxLayout;

    if ( fRunType == "MULTIFILES") 
    {
	TString outputDir = "/home/srsdaq/Desktop/tmp/" ;

	//    TString inputDir = "/home/srsdaq/data/CrGEM_EIC/xRay_I_50mA_CrGEM_HV3800/" ;
	//    TString multifiles = inputDir + "multifilesXRay50mA.txt" ;

	TString inputDir = "/home/srsdaq/data/CrGEM_EIC/xRay_I_75mA_CrGEM_HV3950/" ;
	TString multifiles = inputDir + "multifilesXRay75mA.txt" ;

	ifstream index;
	TString inputfile, day; 
	TString datafile, rootfile ;

	index.open(multifiles.Data());
	while (index.good()) 
	{
	    index >> day >> inputfile ;
	    printf("\n = GEMView::InitToolBox() ==> day %s  inputfile %s\n", day.Data(), inputfile.Data()) ;

	    InitHistForMultipleFiles() ;
	    printf(" = GEMView::InitToolBox() ==>  InitHistForMultipleFiles\n") ;

	    //      if ( day == "lastline" ) { 
	    if ( day == "100000" ) { 
		printf(" = GEMView::InitToolBox() ==> day %s  inputfile %s \n", day.Data(), inputfile.Data()) ;
		return ;
	    }

	    datafile = inputDir + "" + inputfile + ".evio" ;
	    rootfile = outputDir  + "" + inputfile + ".root" ;

	    AnalyzeMultipleFiles(datafile, rootfile) ;
	    DeleteHistForMultipleFiles() ;
	    printf(" = GEMView::InitToolBox() ==>  DeleteHistForMultipleFiles\n") ;
	}
	return ;
    }

	if ( ( fRunType == "RAWDATA") || ( fRunType == "SINGLEEVENT") ) {
	    fEventSpin = new QSpinBox;  
	    if (fConfig.GetOnlineMode()) { 
		fAutoMode = new QPushButton("Auto Update Off");
		QObject::connect(fAutoMode, SIGNAL(clicked()), this, SLOT(ChangeAutoMode()));
		fToolBox->addWidget(fAutoMode);
		fEventSpin->setRange(fConfig.GetCycleWait(), 30);
		QObject::connect(fEventSpin, SIGNAL(valueChanged(int)), this, SLOT(ChangeTimerInterval(int)));
	    }
	    else {
		fEventSpin->setRange(1, 1000000) ;
		QObject::connect(fEventSpin, SIGNAL(valueChanged(int)), this, SLOT(SpinCurrentEvent(int)));
	    }
	    fToolBox->addWidget(fEventSpin);
	}
	else {
	    if ( fRunType == "PEDESTAL") {
		fProcessPedestal = new QPushButton("Process Pedestals") ;
		QObject::connect(fProcessPedestal, SIGNAL(clicked()), this, SLOT(ProducePedestals())) ;
		fToolBox->addWidget(fProcessPedestal);
	    }
	    else if ( fRunType == "MULTIEVENTS") {
		AnalyzeMultiEvents() ;
	    }
	    else if ( fRunType == "SEARCHEVENTS") {
		fSearchEvents  = new QPushButton("Search Events") ;
		QObject::connect(fSearchEvents, SIGNAL(clicked()), this, SLOT(SearchEvents()));
		fToolBox->addWidget(fSearchEvents);
	    }
	}

	/**
	  fEnableRootFile = new QPushButton("Write to Root File");
	  QObject::connect(fEnableRootFile, SIGNAL(clicked()), this, SLOT(WriteRootFile()));
	  fToolBox->addWidget(fEnableRootFile);
	  */

	fLeftSplitter->addLayout(fToolBox);  
	printf(" = GEMView::InitToolBox() ==> Exit Init Tool Box for a %s run\n",  fRunType.Data()) ;
    }

    //==================================================
    void GEMView::InitRootGraphics() {
	if ( fRunType == "MULTIFILES") return ;

	printf(" = GEMView::InitRootGraphics() ==> Enter Init ROOT Graphics for a %s run\n",  fRunType.Data()) ;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,16,0)
	// Make sure the ROOT graphical layer is initialised.
	static struct needgraph {   needgraph () {  TApplication::NeedGraphicsLibs() ;  gApplication->InitializeGraphics();} }  needgraph;
#endif
	printf(" = GEMView::InitRootGraphics() ==> Exit Init ROOT Graphics for a %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::Clear() {
	for (int i=0; i<fMapping->GetNbOfFECs(); i++)    fFEC[i]->ClearHist();
    }

    //=====================================================
    void GEMView::DeleteHistForMultipleFiles() {
	// printf(" = GEMView::DeleteHistos() ==> Enter Init for zero sup analysis %s run\n",  fRunType.Data()) ;

	// PLANES
	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    TString plane = (*plane_itr).first ;
	    Int_t planeId = 2 * fMapping->GetDetectorID( fMapping->GetDetectorFromPlane(plane) ) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;
	    delete fHitHist[planeId];
	    delete fHitHist[id];
	    delete fClusterHist[planeId];
	    delete fClusterHist[id];
	    delete fClusterInfoHist[planeId];
	    delete fClusterInfoHist[id];
	    delete fADCHist[planeId];
	    delete fADCTimeBinPosHist[planeId];
	    delete fADCTimeBinPosHist[id];
	    delete fTimeBinPosHist[planeId];
	    delete fTimeBinPosHist[id];
	}

	delete[] fADCHist;          
	delete[] fHitHist;          
	delete[] fClusterHist;      
	delete[] fClusterInfoHist;  
	delete[] fTimeBinPosHist;   
	delete[] fADCTimeBinPosHist;

	// DETECTORS
	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;
	    TString detector =  (*det_itr).second ;
	    delete f2DPlotsHist[3*detId];
	    delete f2DPlotsHist[3*detId+1];
	    delete f2DPlotsHist[3*detId+2];
	    delete fChargeSharingHist[detId];
	    delete fChargeRatioHist[detId];
	}

	delete[] f2DPlotsHist;
	delete[] fChargeSharingHist;
	delete[] fChargeRatioHist;

	//  printf(" = GEMView::DeleteHistos ==> Exit Init for zero sup analysis %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::InitHistForMultipleFiles() {
	//  printf(" = GEMView::InitHistForMultipleFiles() ==> Enter Init for zero sup analysis %s run\n",  fRunType.Data()) ;

	Int_t nbOfPlaneHists = 2 * fMapping->GetNbOfPlane();
	Int_t driftTime = 25 * fNbOfTimeSamples ;

	// PLANES
	fADCHist           = new TH1F * [fMapping->GetNbOfPlane()] ;
	fHitHist           = new TH1F * [nbOfPlaneHists] ;
	fClusterHist       = new TH1F * [nbOfPlaneHists] ;
	fClusterInfoHist   = new TH1F * [nbOfPlaneHists] ;
	fTimeBinPosHist    = new TH2F * [nbOfPlaneHists] ;
	fADCTimeBinPosHist = new TH2F * [nbOfPlaneHists] ;

	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    TString plane = (*plane_itr).first  ;
	    Int_t planeId = 2 * fMapping->GetDetectorID( fMapping->GetDetectorFromPlane(plane) ) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;
	    Int_t nbOfStrips = 128 * fMapping->GetNbOfAPVsOnPlane((*plane_itr).first) ;
	    Float_t size = fMapping->GetPlaneSize((*plane_itr).first) ;

	    TString hitCount = (*plane_itr).first + "_stripHitCount" ;
	    fHitHist[planeId] = new TH1F(hitCount, hitCount, nbOfStrips, -1*size/2., size/2.);
	    fHitHist[planeId]->GetXaxis()->SetTitle("Strip hit position");
	    fHitHist[planeId]->GetYaxis()->SetTitle("Strip hit counts");
	    fHitHist[planeId]->SetLabelSize(0.04, "X");
	    fHitHist[planeId]->SetLabelSize(0.04, "Y");

	    TString hitUniformity = (*plane_itr).first + "_HitUniformity" ;
	    fHitHist[id] = new TH1F(hitUniformity, hitUniformity, nbOfStrips, -1*size/2., size/2.);
	    fHitHist[id]->GetXaxis()->SetTitle("Strip hit position");
	    fHitHist[id]->GetYaxis()->SetTitle("Average strip hit ADC");
	    fHitHist[id]->SetLabelSize(0.04, "X");
	    fHitHist[id]->SetLabelSize(0.04, "Y");

	    TString clusterCount = (*plane_itr).first + "_clusterHitCount" ;
	    fClusterHist[planeId] = new TH1F(clusterCount, clusterCount, nbOfStrips, -1*size/2., size/2.);
	    fClusterHist[planeId]->GetXaxis()->SetTitle("Cluster position");
	    fClusterHist[planeId]->GetYaxis()->SetTitle("Counts");
	    fClusterHist[planeId]->SetLabelSize(0.04, "X");
	    fClusterHist[planeId]->SetLabelSize(0.04, "Y");

	    TString clusterUniformity = (*plane_itr).first + "_clusterUniformity" ;
	    fClusterHist[id] = new TH1F(clusterUniformity, clusterUniformity, nbOfStrips, -1*size/2., size/2.);
	    fClusterHist[id]->GetXaxis()->SetTitle("Cluster's position");
	    fClusterHist[id]->GetYaxis()->SetTitle("Average ADC");
	    fClusterHist[id]->SetLabelSize(0.04, "X");
	    fClusterHist[id]->SetLabelSize(0.04, "Y");

	    TString clusterMult = (*plane_itr).first + "_clusterMultiplicity" ;
	    fClusterInfoHist[planeId] = new TH1F(clusterMult, clusterMult, 21, 0, 20.);
	    fClusterInfoHist[planeId]->GetXaxis()->SetTitle("Nb of clusters");
	    fClusterInfoHist[planeId]->GetYaxis()->SetTitle("Counts");
	    fClusterInfoHist[planeId]->SetLabelSize(0.04, "X");
	    fClusterInfoHist[planeId]->SetLabelSize(0.04, "Y");

	    TString clusterSize = (*plane_itr).first + "_clusterSize" ;
	    fClusterInfoHist[id] = new TH1F(clusterSize, clusterSize, 21, 0, 20.);
	    fClusterInfoHist[id]->GetXaxis()->SetTitle("Nb of hit per cluster");
	    fClusterInfoHist[id]->GetYaxis()->SetTitle("Counts");
	    fClusterInfoHist[id]->SetLabelSize(0.04, "X");
	    fClusterInfoHist[id]->SetLabelSize(0.04, "Y");

	    TString adcDist = (*plane_itr).first + "_adcDist" ;
	    fADCHist[planeId] = new TH1F(adcDist, adcDist, fNbADCBins, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue() );
	    fADCHist[planeId]->GetXaxis()->SetTitle("ADC");
	    fADCHist[planeId]->GetYaxis()->SetTitle("Counts");
	    fADCHist[planeId]->SetLabelSize(0.04, "X");
	    fADCHist[planeId]->SetLabelSize(0.04, "Y");

	    Int_t tsbin = (Int_t) (nbOfStrips / 8) ;

	    TString alltimeBin = (*plane_itr).first + "adc_vs_pos_allTimeBin" ;
	    fADCTimeBinPosHist[planeId] = new TH2F(alltimeBin, alltimeBin, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
	    fADCTimeBinPosHist[planeId]->GetXaxis()->SetTitle("drift time");
	    fADCTimeBinPosHist[planeId]->GetYaxis()->SetTitle("Position (mm)");
	    fADCTimeBinPosHist[planeId]->SetLabelSize(0.04, "X");
	    fADCTimeBinPosHist[planeId]->SetLabelSize(0.04, "Y");

	    TString timeBinPeak = (*plane_itr).first + "adc_vs_pos_timeBinPeak" ;
	    fADCTimeBinPosHist[id] = new TH2F(timeBinPeak, timeBinPeak,  fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
	    fADCTimeBinPosHist[id]->GetXaxis()->SetTitle("drift time");
	    fADCTimeBinPosHist[id]->GetYaxis()->SetTitle("Position (mm)");
	    fADCTimeBinPosHist[id]->SetLabelSize(0.04, "X");
	    fADCTimeBinPosHist[id]->SetLabelSize(0.04, "Y");

	    alltimeBin = (*plane_itr).first + "_pos_vs_allTimeBin" ;
	    fTimeBinPosHist[planeId] = new TH2F(alltimeBin, alltimeBin, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
	    fTimeBinPosHist[planeId]->GetXaxis()->SetTitle("drift time");
	    fTimeBinPosHist[planeId]->GetYaxis()->SetTitle("Position (mm)");
	    fTimeBinPosHist[planeId]->SetLabelSize(0.04, "X");
	    fTimeBinPosHist[planeId]->SetLabelSize(0.04, "Y");

	    timeBinPeak = (*plane_itr).first + "_pos_vs_timeBinPeak" ;
	    fTimeBinPosHist[id] = new TH2F(timeBinPeak, timeBinPeak, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
	    fTimeBinPosHist[id]->GetXaxis()->SetTitle("drift time");
	    fTimeBinPosHist[id]->GetYaxis()->SetTitle("Position (mm)");
	    fTimeBinPosHist[id]->SetLabelSize(0.04, "X");
	    fTimeBinPosHist[id]->SetLabelSize(0.04, "Y");
	    //    printf(" = GEMView::InitHistForMultipleFiles() ==> plane[%s] to histId[%d]  \n", plane.Data(), planeId); 
	}
	// DETECTORS
	f2DPlotsHist       = new TH2F * [3*fMapping->GetNbOfDetectors()];
	fChargeSharingHist = new TH2F * [fMapping->GetNbOfDetectors()];
	fChargeRatioHist   = new TH1F * [fMapping->GetNbOfDetectors()];

	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;
	    TString detector =  (*det_itr).second ;

	    Float_t size1 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).front());
	    Float_t size2 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).back());
	    //    Int_t nbin1 = 256;
	    //    Int_t nbin2 = 256; 
	    Int_t nbin1 = 64;
	    Int_t nbin2 = 64; 

	    /**
	      if (fMapping->GetReadoutBoardFromDetector(detector) == "UV_ANGLE") {
	      size1 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).front());
	      size2 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).back());
	      nbin1 = 25 ;
	      nbin2 = 25 ;  
	      }
	      */

	    TString hitMap    = detector + "_ClusterHitMap" ;
	    f2DPlotsHist[3*detId] = new TH2F(hitMap, hitMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
	    f2DPlotsHist[3*detId]->GetXaxis()->SetTitle("cluster x-position (mm)");
	    f2DPlotsHist[3*detId]->GetYaxis()->SetTitle("cluster y-position (mm)");
	    f2DPlotsHist[3*detId]->SetLabelSize(0.04, "X");
	    f2DPlotsHist[3*detId]->SetLabelSize(0.04, "Y");

	    TString adcMap    = detector + "_gain_Uniformity" ;
	    f2DPlotsHist[3*detId+1] = new TH2F(adcMap, adcMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
	    f2DPlotsHist[3*detId+1]->GetXaxis()->SetTitle("cluster x-position (mm)");
	    f2DPlotsHist[3*detId+1]->GetYaxis()->SetTitle("cluster y-position (mm)");
	    f2DPlotsHist[3*detId+1]->SetLabelSize(0.04, "X");
	    f2DPlotsHist[3*detId+1]->SetLabelSize(0.04, "Y");

	    TString timingMap = detector + "_timing_Uniformity" ;
	    f2DPlotsHist[3*detId+2] = new TH2F(timingMap, timingMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
	    f2DPlotsHist[3*detId+2]->GetXaxis()->SetTitle("cluster x-position (mm)");
	    f2DPlotsHist[3*detId+2]->GetYaxis()->SetTitle("cluster y-position (mm)");
	    f2DPlotsHist[3*detId+2]->SetLabelSize(0.04, "X");
	    f2DPlotsHist[3*detId+2]->SetLabelSize(0.04, "Y");

	    TString chSharing = detector + "_charge_Sharing" ;
	    fChargeSharingHist[detId] = new TH2F(chSharing, chSharing, 50, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue(), 50, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue());
	    fChargeSharingHist[detId]->GetXaxis()->SetTitle("ADC in X-strips");
	    fChargeSharingHist[detId]->GetYaxis()->SetTitle("ADC in Y-strips");
	    fChargeSharingHist[detId]->SetLabelSize(0.04, "X");
	    fChargeSharingHist[detId]->SetLabelSize(0.04, "Y");

	    TString chRatio   = detector + "_charge_Ratio" ;
	    fChargeRatioHist[detId] = new TH1F(chRatio, chRatio, 31, 0.0, 3.0);
	    fChargeRatioHist[detId]->GetXaxis()->SetTitle("ADC charge ratio x / Y");
	    fChargeRatioHist[detId]->GetYaxis()->SetTitle("ratio");
	    fChargeRatioHist[detId]->SetLabelSize(0.04, "X");
	    fChargeRatioHist[detId]->SetLabelSize(0.04, "Y");
	    //    printf(" = GEMView::InitHistForMultipleFiles() ==> detector[%s] to histId[%d]  \n", detector.Data(), detId) ;
	}
	//  printf(" = GEMView::InitHistForMultipleFiles() ==> Exit Init for zero sup analysis %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::InitHistForZeroSup() {
	printf(" = GEMView::InitHistForZeroSup() ==> Enter Init for zero sup analysis %s run\n",  fRunType.Data()) ;

	Int_t nbOfPlaneHists = 2 * fMapping->GetNbOfPlane();
	Int_t driftTime = 25 * fNbOfTimeSamples ;

	// PLANES
	if ( (fRunType == "SINGLEEVENT") || (fRunType == "SEARCHEVENTS") ) {
	    f1DSingleEventHist    = new TH1F * [nbOfPlaneHists] ;
	    f1DSingleEventDisplay = new PRadHistCanvas * [nbOfPlaneHists];

	    f2DSingleEventHist    = new TH2F * [nbOfPlaneHists] ;
	    f2DSingleEventDisplay = new PRadHistCanvas * [nbOfPlaneHists];

	    map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	    map <TString, Int_t>::const_iterator plane_itr ;
	    for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
		TString plane = (*plane_itr).first  ;
		Int_t planeId = 2 * fMapping->GetDetectorID( fMapping->GetDetectorFromPlane(plane) ) + (*plane_itr).second ;
		Int_t nbOfStrips = 128 * fMapping->GetNbOfAPVsOnPlane((*plane_itr).first) ;
		Float_t size = fMapping->GetPlaneSize((*plane_itr).first) ;
		Int_t id = fMapping->GetNbOfPlane() + planeId ;

		Int_t nbBins = nbOfStrips * fNbOfTimeSamples ;

		TString zeroSup       = (*plane_itr).first + "_zeroSup" ;
		f1DSingleEventHist[planeId] = new TH1F(zeroSup, zeroSup, nbBins, 0, (nbBins -1) );
		f1DSingleEventHist[planeId]->SetLabelSize(0.04, "X");
		f1DSingleEventHist[planeId]->SetLabelSize(0.04, "Y");
		f1DSingleEventDisplay[planeId] = new PRadHistCanvas();
		DisableMouseEvent(f1DSingleEventDisplay[planeId]);

		TString presZeroSup = (*plane_itr).first + "_pedSub" ;
		f1DSingleEventHist[id] = new TH1F(presZeroSup, presZeroSup, nbBins, 0, (nbBins -1) );
		f1DSingleEventHist[id]->SetLabelSize(0.04, "X");
		f1DSingleEventHist[id]->SetLabelSize(0.04, "Y");
		f1DSingleEventDisplay[id] = new PRadHistCanvas();
		DisableMouseEvent(f1DSingleEventDisplay[id]);

		zeroSup       = (*plane_itr).first + "_SingleHit_vs_timebin_2D" ;
		f2DSingleEventHist[planeId] = new TH2F(zeroSup, zeroSup, fNbOfTimeSamples, 0, driftTime, nbOfStrips, 0, (nbOfStrips-1) );
		f2DSingleEventHist[planeId]->GetXaxis()->SetTitle("drift time");
		f2DSingleEventHist[planeId]->GetYaxis()->SetTitle("Strip Nb");
		f2DSingleEventHist[planeId]->GetYaxis()->SetTitleSize(0.05);
		f2DSingleEventHist[planeId]->GetXaxis()->SetTitleSize(0.05);      
		f2DSingleEventHist[planeId]->GetXaxis()->SetTitleOffset(1.1);
		f2DSingleEventHist[planeId]->GetYaxis()->SetTitleOffset(1.1);

		f2DSingleEventHist[planeId]->SetLabelSize(0.04, "X");
		f2DSingleEventHist[planeId]->SetLabelSize(0.04, "Y");
		f2DSingleEventDisplay[planeId] = new PRadHistCanvas();
		DisableMouseEvent(f2DSingleEventDisplay[planeId]);
		printf(" = GEMView::InitHistForZeroSup() => Initialize single events histos: plane[%s], nbStrips=%d, nbTimeS=%d, size=%f, to histId[%d]\n", plane.Data(), nbOfStrips, fNbOfTimeSamples, size, planeId) ;

		presZeroSup = (*plane_itr).first + "Accu_Hits_vs_timebin_2D" ;
		f2DSingleEventHist[id] = new TH2F(presZeroSup, presZeroSup,  fNbOfTimeSamples, 0, driftTime,  nbOfStrips, 0, (nbOfStrips-1) );     
		f2DSingleEventHist[id]->GetXaxis()->SetTitle("drift time");
		f2DSingleEventHist[id]->GetYaxis()->SetTitle("Strip Nb");
		f2DSingleEventHist[id]->GetYaxis()->SetTitleSize(0.05);
		f2DSingleEventHist[id]->GetXaxis()->SetTitleSize(0.05);
		f2DSingleEventHist[id]->GetYaxis()->SetTitleOffset(1.1);
		f2DSingleEventHist[id]->GetXaxis()->SetTitleOffset(1.1);

		f2DSingleEventHist[id]->SetLabelSize(0.04, "X");
		f2DSingleEventHist[id]->SetLabelSize(0.04, "Y");
		f2DSingleEventDisplay[id] = new PRadHistCanvas();
		DisableMouseEvent(f2DSingleEventDisplay[id]);   
		printf(" = GEMView::InitHistForZeroSup() => Initialize histos: plane[%s], nbOfStrips=%d, nbOfTimeSamples=%d, size=%f, to histId[%d]\n", plane.Data(), nbOfStrips, fNbOfTimeSamples, size, id) ;
	    }
	}

	if (fRunType == "MULTIEVENTS") {
	    // PLANES
	    fADCHist           = new TH1F * [fMapping->GetNbOfPlane()] ;
	    fHitHist           = new TH1F * [nbOfPlaneHists] ;
	    fClusterHist       = new TH1F * [nbOfPlaneHists] ;
	    fClusterInfoHist   = new TH1F * [nbOfPlaneHists] ;
	    fTimeBinPosHist    = new TH2F * [nbOfPlaneHists] ;
	    fADCTimeBinPosHist = new TH2F * [nbOfPlaneHists] ;

	    fADCDisplay         = new PRadHistCanvas * [fMapping->GetNbOfPlane()];
	    fHitDisplay         = new PRadHistCanvas * [nbOfPlaneHists];
	    fClusterDisplay     = new PRadHistCanvas * [nbOfPlaneHists];
	    fClusterInfoDisplay = new PRadHistCanvas * [nbOfPlaneHists];    
	    fADCTimeBinPosDisplay  = new PRadHistCanvas * [nbOfPlaneHists];    

	    map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	    map <TString, Int_t>::const_iterator plane_itr ;
	    for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
		TString plane = (*plane_itr).first  ;
		Int_t planeId = 2 * fMapping->GetDetectorID( fMapping->GetDetectorFromPlane(plane) ) + (*plane_itr).second ;
		Int_t id = fMapping->GetNbOfPlane() + planeId ;
		Int_t nbOfStrips = 128 * fMapping->GetNbOfAPVsOnPlane((*plane_itr).first) ;
		Float_t size = fMapping->GetPlaneSize((*plane_itr).first) ;

		TString hitCount = (*plane_itr).first + "_stripHitCount" ;
		fHitHist[planeId] = new TH1F(hitCount, hitCount, nbOfStrips, -1*size/2., size/2.);
		fHitHist[planeId]->GetXaxis()->SetTitle("Strip hit position");
		fHitHist[planeId]->GetYaxis()->SetTitle("Strip hit counts");
		fHitHist[planeId]->SetLabelSize(0.04, "X");
		fHitHist[planeId]->SetLabelSize(0.04, "Y");
		fHitDisplay[planeId] = new PRadHistCanvas();   

		TString hitUniformity = (*plane_itr).first + "_HitUniformity" ;
		fHitHist[id] = new TH1F(hitUniformity, hitUniformity, nbOfStrips, -1*size/2., size/2.);
		fHitHist[id]->GetXaxis()->SetTitle("Strip hit position");
		fHitHist[id]->GetYaxis()->SetTitle("Average strip hit ADC");
		fHitHist[id]->SetLabelSize(0.04, "X");
		fHitHist[id]->SetLabelSize(0.04, "Y");
		fHitDisplay[id] = new PRadHistCanvas();   

		TString clusterCount = (*plane_itr).first + "_clusterHitCount" ;
		fClusterHist[planeId] = new TH1F(clusterCount, clusterCount, nbOfStrips, -1*size/2., size/2.);
		fClusterHist[planeId]->GetXaxis()->SetTitle("Cluster position");
		fClusterHist[planeId]->GetYaxis()->SetTitle("Counts");
		fClusterHist[planeId]->SetLabelSize(0.04, "X");
		fClusterHist[planeId]->SetLabelSize(0.04, "Y");
		fClusterDisplay[planeId] = new PRadHistCanvas();   

		TString clusterUniformity = (*plane_itr).first + "_clusterUniformity" ;
		fClusterHist[id] = new TH1F(clusterUniformity, clusterUniformity, nbOfStrips, -1*size/2., size/2.);
		fClusterHist[id]->GetXaxis()->SetTitle("Cluster's position");
		fClusterHist[id]->GetYaxis()->SetTitle("Average ADC");
		fClusterHist[id]->SetLabelSize(0.04, "X");
		fClusterHist[id]->SetLabelSize(0.04, "Y");
		fClusterDisplay[id] = new PRadHistCanvas();   

		TString clusterMult = (*plane_itr).first + "_clusterMultiplicity" ;
		fClusterInfoHist[planeId] = new TH1F(clusterMult, clusterMult, 21, 0, 20.);
		fClusterInfoHist[planeId]->GetXaxis()->SetTitle("Nb of clusters");
		fClusterInfoHist[planeId]->GetYaxis()->SetTitle("Counts");
		fClusterInfoHist[planeId]->SetLabelSize(0.04, "X");
		fClusterInfoHist[planeId]->SetLabelSize(0.04, "Y");
		fClusterInfoDisplay[planeId] = new PRadHistCanvas();   

		TString clusterSize = (*plane_itr).first + "_clusterSize" ;
		fClusterInfoHist[id] = new TH1F(clusterSize, clusterSize, 21, 0, 20.);
		fClusterInfoHist[id]->GetXaxis()->SetTitle("Nb of hit per cluster");
		fClusterInfoHist[id]->GetYaxis()->SetTitle("Counts");
		fClusterInfoHist[id]->SetLabelSize(0.04, "X");
		fClusterInfoHist[id]->SetLabelSize(0.04, "Y");
		fClusterInfoDisplay[id] = new PRadHistCanvas();   

		//      Int_t nbADCBins = 500 ;
		TString adcDist = (*plane_itr).first + "_adcDist" ;
		fADCHist[planeId] = new TH1F(adcDist, adcDist, fNbADCBins, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue() );
		fADCHist[planeId]->GetXaxis()->SetTitle("ADC");
		fADCHist[planeId]->GetYaxis()->SetTitle("Counts");
		fADCHist[planeId]->SetLabelSize(0.04, "X");
		fADCHist[planeId]->SetLabelSize(0.04, "Y");
		fADCDisplay[planeId] = new PRadHistCanvas();

		Int_t tsbin = (Int_t) (nbOfStrips / 8) ;

		TString alltimeBin = (*plane_itr).first + "adc_vs_pos_allTimeBin" ;
		fADCTimeBinPosHist[planeId] = new TH2F(alltimeBin, alltimeBin, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
		fADCTimeBinPosHist[planeId]->GetXaxis()->SetTitle("drift time");
		fADCTimeBinPosHist[planeId]->GetYaxis()->SetTitle("Position (mm)");
		fADCTimeBinPosHist[planeId]->SetLabelSize(0.04, "X");
		fADCTimeBinPosHist[planeId]->SetLabelSize(0.04, "Y");
		fADCTimeBinPosDisplay[planeId] = new PRadHistCanvas();   

		TString timeBinPeak = (*plane_itr).first + "adc_vs_pos_timeBinPeak" ;
		fADCTimeBinPosHist[id] = new TH2F(timeBinPeak, timeBinPeak,  fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
		fADCTimeBinPosHist[id]->GetXaxis()->SetTitle("drift time");
		fADCTimeBinPosHist[id]->GetYaxis()->SetTitle("Position (mm)");
		fADCTimeBinPosHist[id]->SetLabelSize(0.04, "X");
		fADCTimeBinPosHist[id]->SetLabelSize(0.04, "Y");
		fADCTimeBinPosDisplay[id] = new PRadHistCanvas();  

		alltimeBin = (*plane_itr).first + "_pos_vs_allTimeBin" ;
		fTimeBinPosHist[planeId] = new TH2F(alltimeBin, alltimeBin, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
		fTimeBinPosHist[planeId]->GetXaxis()->SetTitle("drift time");
		fTimeBinPosHist[planeId]->GetYaxis()->SetTitle("Position (mm)");
		fTimeBinPosHist[planeId]->SetLabelSize(0.04, "X");
		fTimeBinPosHist[planeId]->SetLabelSize(0.04, "Y");

		timeBinPeak = (*plane_itr).first + "_pos_vs_timeBinPeak" ;
		fTimeBinPosHist[id] = new TH2F(timeBinPeak, timeBinPeak, fNbOfTimeSamples, 0, driftTime, tsbin, 0, (nbOfStrips - 1));
		fTimeBinPosHist[id]->GetXaxis()->SetTitle("drift time");
		fTimeBinPosHist[id]->GetYaxis()->SetTitle("Position (mm)");
		fTimeBinPosHist[id]->SetLabelSize(0.04, "X");
		fTimeBinPosHist[id]->SetLabelSize(0.04, "Y");
		printf(" = GEMView::InitHistForZeroSup() ==> plane[%s] to histId[%d]  \n", plane.Data(), planeId);
	    }

	    // DETECTORS
	    f2DPlotsHist       = new TH2F * [3*fMapping->GetNbOfDetectors()];
	    fChargeSharingHist = new TH2F * [fMapping->GetNbOfDetectors()];
	    fChargeRatioHist   = new TH1F * [fMapping->GetNbOfDetectors()];

	    f2DPlotsDisplay       = new PRadHistCanvas *[3*fMapping->GetNbOfDetectors()];
	    fChargeSharingDisplay = new PRadHistCanvas *[2*fMapping->GetNbOfDetectors()]; 

	    map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	    map <Int_t, TString>::const_iterator det_itr ;
	    for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
		Int_t detId = (*det_itr).first ;
		TString detector =  (*det_itr).second ;

		Float_t size1 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).front());
		Float_t size2 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).back());
		//    Int_t nbin1 = 256;
		//      Int_t nbin2 = 256; 
		Int_t nbin1 = 64;
		Int_t nbin2 = 64; 

		/**
		  if (fMapping->GetReadoutBoardFromDetector(detector) == "UV_ANGLE") {
		  size1 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).front());
		  size2 = 0.5 * fMapping->GetPlaneSize(fMapping->GetPlaneListFromDetector(detector).back());
		  nbin1 = 25 ;
		  nbin2 = 25 ;  
		  }
		  */

		TString hitMap    = detector + "_ClusterHitMap" ;
		f2DPlotsHist[3*detId] = new TH2F(hitMap, hitMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
		f2DPlotsHist[3*detId]->GetXaxis()->SetTitle("cluster x-position (mm)");
		f2DPlotsHist[3*detId]->GetYaxis()->SetTitle("cluster y-position (mm)");
		f2DPlotsHist[3*detId]->SetLabelSize(0.04, "X");
		f2DPlotsHist[3*detId]->SetLabelSize(0.04, "Y");
		f2DPlotsDisplay[3*detId] = new PRadHistCanvas();

		TString adcMap    = detector + "_gain_Uniformity" ;
		f2DPlotsHist[3*detId+1] = new TH2F(adcMap, adcMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
		f2DPlotsHist[3*detId+1]->GetXaxis()->SetTitle("cluster x-position (mm)");
		f2DPlotsHist[3*detId+1]->GetYaxis()->SetTitle("cluster y-position (mm)");
		f2DPlotsHist[3*detId+1]->SetLabelSize(0.04, "X");
		f2DPlotsHist[3*detId+1]->SetLabelSize(0.04, "Y");
		f2DPlotsDisplay[3*detId+1] = new PRadHistCanvas();

		TString timingMap = detector + "_timing_Uniformity" ;
		f2DPlotsHist[3*detId+2] = new TH2F(timingMap, timingMap, nbin1, -1*size1, size1, nbin2, -1*size2, size2);
		f2DPlotsHist[3*detId+2]->GetXaxis()->SetTitle("cluster x-position (mm)");
		f2DPlotsHist[3*detId+2]->GetYaxis()->SetTitle("cluster y-position (mm)");
		f2DPlotsHist[3*detId+2]->SetLabelSize(0.04, "X");
		f2DPlotsHist[3*detId+2]->SetLabelSize(0.04, "Y");
		f2DPlotsDisplay[3*detId+2] = new PRadHistCanvas();    

		TString chSharing = detector + "_charge_Sharing" ;
		fChargeSharingHist[detId] = new TH2F(chSharing, chSharing, 50, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue(), 50, fConfig.GetMinADCvalue(), fConfig.GetMaxADCvalue());
		fChargeSharingHist[detId]->GetXaxis()->SetTitle("ADC in X-strips");
		fChargeSharingHist[detId]->GetYaxis()->SetTitle("ADC in Y-strips");
		fChargeSharingHist[detId]->SetLabelSize(0.04, "X");
		fChargeSharingHist[detId]->SetLabelSize(0.04, "Y");
		fChargeSharingDisplay[2*detId] = new PRadHistCanvas();    

		TString chRatio   = detector + "_charge_Ratio" ;
		fChargeRatioHist[detId] = new TH1F(chRatio, chRatio, 31, 0.0, 3.0);
		fChargeRatioHist[detId]->GetXaxis()->SetTitle("ADC charge ratio x / Y");
		fChargeRatioHist[detId]->GetYaxis()->SetTitle("ratio");
		fChargeRatioHist[detId]->SetLabelSize(0.04, "X");
		fChargeRatioHist[detId]->SetLabelSize(0.04, "Y");
		fChargeSharingDisplay[2*detId+1] = new PRadHistCanvas();    
		printf(" = GEMView::InitHistForZeroSup() ==> detector[%s] to histId[%d]  \n", detector.Data(), detId) ;
	    }
	}
	printf(" = GEMView::InitHistForZeroSup() ==> Exit Init for zero sup analysis %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::ResetMultipleFilesHistos() {
	//  printf(" = GEMView::ResetHistos() ==> Enter Init for zero sup analysis %s run\n",  fRunType.Data()) ;

	// PLANES
	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    TString plane = (*plane_itr).first  ;
	    Int_t planeId = 2 * fMapping->GetDetectorID( fMapping->GetDetectorFromPlane(plane) ) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;
	    fHitHist[planeId]->Reset();
	    fHitHist[id]->Reset();
	    fClusterHist[planeId]->Reset();
	    fClusterHist[id]->Reset();
	    fClusterInfoHist[planeId]->Reset();
	    fClusterInfoHist[id]->Reset();
	    fADCHist[planeId]->Reset();
	    fADCTimeBinPosHist[planeId]->Reset();
	    fADCTimeBinPosHist[id]->Reset();
	    fTimeBinPosHist[planeId]->Reset();
	    fTimeBinPosHist[id]->Reset();
	}

	// DETECTORS
	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;
	    TString detector =  (*det_itr).second ;
	    f2DPlotsHist[3*detId]->Reset();
	    f2DPlotsHist[3*detId+1]->Reset();
	    f2DPlotsHist[3*detId+2]->Reset();
	    fChargeSharingHist[detId]->Reset();
	    fChargeRatioHist[detId]->Reset();
	}
	//  printf(" = GEMView::ResetHistos ==> Exit Init for zero sup analysis %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::InitHistForRawData() {
	printf("XB = GEMView::InitHistForRawData() ==> Enter Init for raw APV data analysis %s run\n",  fRunType.Data()) ;

	fRootWidget = new GEMHistContainer *[fMapping->GetNbOfAPVs()];
	printf("XB = GEMView::InitHistForRawData() ==> fRootWidget is called\n") ;
	fFEC = new GEMFEC * [fMapping->GetNbOfFECs()];
	printf("XB = GEMView::InitHistForRawData() ==> fFEC is called \n") ;
	printf("XB = Nb of FECS: %d\n", fMapping->GetNbOfFECs());

	for (int i=0; i<fMapping->GetNbOfFECs(); i++) {
	    list<int> apv_list = fMapping->GetAPVIDListFromFECID(i);
	    fFEC[i] = new GEMFEC(this, i, fMapping->GetNChannelEachFEC(i), &(apv_list));
	    fFEC[i]->SetIPAddress(fMapping->GetFECIPFromFECID(i));
	}
	printf(" = GEMView::InitHistForRawData() ==> fFEC is initialised \n") ;

	for (int i=0; i<fMapping->GetNbOfAPVs(); i++) 
	{
	    fRootWidget[i] = new GEMHistContainer(this, i) ;
	}

	int globalIndex = 0;
	for (int i=0; i<fMapping->GetNbOfFECs(); i++) {
	    printf("number of active channels in fec %d: %d\n", i, fFEC[i]->GetNActiveChannel());
	    for (int j=0; j < fFEC[i]->GetNActiveChannel(); j++) 
	    {
		int currentAPVID = fFEC[i]->GetAPVIDFromThisFEC(j);
		TString currentPlane = (fMapping->GetPlaneFromAPVID(currentAPVID)).Data();
		fRootWidget[globalIndex]->Activate();
		// Int_t adc_no = fMapping->GetAPVNoFromID(currentAPVID) ; // old version
		// XB : update
		Int_t adc_no = currentAPVID & 0xf;
		fRootWidget[globalIndex]->RegisterHist(fFEC[i]->GetHist( adc_no));
		fRootWidget[globalIndex]->SetBasicInfo(currentPlane, i, globalIndex,  adc_no);
		globalIndex++;
	    }
	}
	printf(" = GEMView::InitHistForRawData() ==> fRootWidget is initialised \n") ;
	printf(" = GEMView::InitHistForRawData() ==> Exit Init for raw APV data analysis %s run\n",  fRunType.Data()) ;
    }

    //=====================================================
    void GEMView::fillRawDataHisto() 
    {
        std::cout<<"fill raw data histo..."<<std::endl;
	std::cout<<fHandler->fCurrentEvent.size()<<std::endl;
	//fill raw data 
	Clear();
	int fec_id=0;
	int adc_ch=0;
	map<int, map<int, vector<int> > >::iterator it;
	for(it = fHandler->fCurrentEvent.begin(); it!=fHandler->fCurrentEvent.end(); ++it)
	{
	    fec_id = it->first;
	    map<int, vector<int> > temp = it->second; 
	    //std::cout<<"fec id: "<<fec_id<<", number of adc ch: "<<temp.size()<<std::endl;
	    map<int, vector<int> >::iterator itt;
	    for(itt=temp.begin(); itt!=temp.end(); ++itt)
	    {
		adc_ch = itt->first;
		vector<int> adc_temp = itt->second;      
		Int_t apv_id = (fec_id << 4) | adc_ch ;
		Int_t adc_no = fMapping->GetAPVNoFromID(apv_id) ;
		//std::cout<<"apv id: "<<apv_id<<", adc_no: "<<adc_no<<std::endl;
		//std::cout<<"apv data size: "<<adc_temp.size()<<std::endl;
		//if(!fFEC[fec_id]) std::cout<<"not initialized..."<<std::endl;
		//std::cout<<"r1."<<adc_temp.size()<<std::endl;
		//std::cout<<"adc_no: "<<adc_no<<std::endl;
		// XB Update:
		adc_no = adc_ch;
		
		if (fFEC[fec_id]->IsChannelActive(adc_no) == 0) {
		    //std::cout<<"adc channel inactive."<<std::endl;
		    continue;
		}
		else if (adc_temp.size() > 0){
		    //std::cout<<"... hhh ..."<<std::endl;
		    fFEC[fec_id]->GetHist(adc_no)->SetBins(adc_temp.size(), 0, adc_temp.size());
		    //std::cout<<"... hhh end ..."<<std::endl;
		}
		int histMax = 0;
		int histMin = 1e6;
		//std::cout<<" ...000... "<<std::endl;
		for (unsigned int i=0; i<adc_temp.size(); i++){
		    fFEC[fec_id]->GetHist(adc_no)->SetBinContent(i, adc_temp.at(i));
		    if (adc_temp.at(i) < histMin) histMin = adc_temp.at(i);
		    if (adc_temp.at(i) > histMax) histMax = adc_temp.at(i);  
		}
		fFEC[fec_id]->GetHist(adc_no)->SetAxisRange(0.8*histMin, 1.05*histMax, "Y");
	    } 
	}
	std::cout<<"success!"<<std::endl;
	for (int i=0; i<fMapping->GetNbOfAPVs(); i++) 
	{
	    fRootWidget[i]->drawHist();  
	}
	std::cout<<"2<<"<<std::endl;
	for (int i=0; i<fMapping->GetNbOfFECs(); i++) fFEC[i]->DrawHist();
	std::cout<<"fill raw histo ended..."<<std::endl;
    }


    //===================================================== 
    void GEMView::DrawSingleEventDataHisto() {
	//  printf(" = GEMView::fillSingleEventDataHisto() ==> Start \n") ;
	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    Int_t planeId = 2 * fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first)) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;
	    Int_t nbOfStrips = 128 * fMapping->GetNbOfAPVsOnPlane((*plane_itr).first) ;
	    DrawSingleEvent1DHistos(planeId, f1DSingleEventHist[planeId], nbOfStrips) ;
	    DrawSingleEvent1DHistos(id, f1DSingleEventHist[id], nbOfStrips) ;
	    DrawSingleEvent2DHistos(planeId, f2DSingleEventHist[planeId], "lego2", nbOfStrips) ;
	    DrawSingleEvent2DHistos(id, f2DSingleEventHist[planeId], "colz", nbOfStrips) ;
	}
	//  printf(" = GEMView::fillSingleEventDataHisto() ==> End \n") ;
    }  

    //===============================================================================================
    void GEMView::DrawMultipleEventDataHisto() {
	printf(" = GEMView::fillMultipleEventDataHisto() ==> Start \n") ;
	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    Int_t planeId = 2 * fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first)) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;
	    DrawMultiEventsADCHistos(planeId, fADCHist[planeId]) ;
	    DrawMultiEvents1DHistos(planeId, fHitHist[planeId], fClusterHist[planeId], fClusterInfoHist[planeId], fADCTimeBinPosHist[planeId]) ;
	    DrawMultiEvents1DHistos(id, fHitHist[id], fClusterHist[id], fClusterInfoHist[id], fADCTimeBinPosHist[id]) ;
	}
	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;
	    DrawMultiEvents2DHistos(3*detId, f2DPlotsHist[3*detId]);
	    DrawMultiEvents2DHistos(3*detId+1, f2DPlotsHist[3*detId+1]);
	    DrawMultiEvents2DHistos(3*detId+2, f2DPlotsHist[3*detId+2]);
	    DrawMultiEvents2DHistos(detId, fChargeSharingHist[detId], fChargeRatioHist[detId]) ;
	}  
	printf(" = GEMView::fillMultipleEventDataHisto() ==> End \n") ;
    }


    //=====================================================
    int GEMView::GEMMonitorLoop() 
    { 
	int status = 0;
	map <TString, Int_t>  listOfPlanes  = fMapping-> GetPlaneIDFromPlaneMap() ;
	if (fConfig.GetOnlineMode())
	    status = fHandler->ProcessETEvent(f1DSingleEventHist, f2DSingleEventHist);
	else
	{
	    if( fRunType == "SINGLEEVENT")   
		status = fHandler->ProcessSingleEventFromFile(fConfig.GetInputFileName(), fCurrentEntry, f1DSingleEventHist, f2DSingleEventHist); 
	    else if ( fRunType == "RAWDATA") {
		status = fHandler->ProcessRawDataFromFile(fConfig.GetInputFileName(), fCurrentEntry); 
	    }
	}
	if (status == 1) {
	    if( fRunType == "SINGLEEVENT")   DrawSingleEventDataHisto() ;
	    else if ( fRunType == "RAWDATA") fillRawDataHisto();
	    return 1;
	}
	else{
	    PrintTextOnMessager(Form("Error reading event %d from file", fCurrentEntry), 1);
	    return 0;
	}
    }

    //=====================================================
    int GEMView::AutoGEMMonitorLoop() {
	if (!innerTimer->isActive() && !fHandler->IsEventFound()) innerTimer->start();
	int status = 0;
	if (fConfig.GetOnlineMode() && !innerTimer->isActive() && fHandler->IsEventFound()) {
	    status = fHandler->ProcessETEvent(f1DSingleEventHist, f2DSingleEventHist);
	}
	if (status == 1) {
	    if (fRunType == "SINGLEEVENT") DrawSingleEventDataHisto() ;
	    else                           fillRawDataHisto();
	    fHandler->Restart();
	    assert(!innerTimer->isActive());
	    return 1;
	}
	if (!fConfig.GetOnlineMode()) {  
	    status = fHandler->ProcessSingleEventFromFile(fConfig.GetInputFileName(), fCurrentEntry, f1DSingleEventHist, f2DSingleEventHist);
	    fCurrentEntry++;
	}
	if (status == 1) return 1;
	return 0;
    }

    //=====================================================
    void GEMView::ParserInnerLoop() {
	int status = fHandler->QuickCheckETEvent();
	if (status == 1) innerTimer->stop(); //found the event, let the rest process it
    }

    //=====================================================
    void GEMView::SetMonitor(int sec) {
	assert(sec > 0);
	timer = new QTimer(this);
	timer->setInterval(sec);
	timer->stop();  
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(AutoGEMMonitorLoop()));  
	innerTimer = new QTimer(this);
	innerTimer->setInterval(1);
	innerTimer->stop();  
	QObject::connect(innerTimer, SIGNAL(timeout()), this, SLOT(ParserInnerLoop()));  
	fProgressBar = new QProgressBar();
	fProgressBar->setMinimum(0);
	fProgressBar->setMaximum(100);
	fProgressBar->setTextVisible(true);
	fProgressBar->setUpdatesEnabled(true);
    }

    //=====================================================_
    void GEMView::SpinCurrentEvent(int i) 
    {
        //std::cout<<"XB: debug ..."<<std::endl;
	fCurrentEntry = i;
	GEMMonitorLoop();
    }

    //====================================================
    void GEMView::ChangeTimerInterval(int i) {
	bool wasTimerRunning = timer->isActive();
	timer->stop();
	if (i > 0) {
	    timer->setInterval(i SEC);
	    PrintTextOnMessager(Form("Setting time interval to %d second", i), 0);
	}
	if (wasTimerRunning) timer->start();
    }

    //====================================================
    void GEMView::ChangeAutoMode() {
	if (timer->isActive() == true) {
	    timer->stop();
	    PrintTextOnMessager("Time clock is stopped", 1);
	    fAutoMode->setText("Auto Update Off");
	}
	else{
	    timer->start();
	    PrintTextOnMessager("Time clock is started", 1);
	    fAutoMode->setText("Auto Update On");
	}
    }

    //=====================================================
    void GEMView::WriteRootFile(TString outputRootFile) {
	printf(" = GEMView::WriteRootFile() ==> Write Analysis Histograms in root files \n") ;

	PrintTextOnMessager(Form("Writing all current histograms to root file %s", outputRootFile.Data()), 0); 
	TFile *rootFile = new TFile(outputRootFile.Data(), "RECREATE");
	rootFile->cd();

	map <TString, Int_t> listOfPlanes = fMapping->GetPlaneIDFromPlaneMap() ;
	map <TString, Int_t>::const_iterator plane_itr ;
	for (plane_itr = listOfPlanes.begin(); plane_itr != listOfPlanes.end(); ++ plane_itr) {
	    Int_t planeId = 2 * fMapping->GetDetectorID(fMapping->GetDetectorFromPlane( (*plane_itr).first)) + (*plane_itr).second ;
	    Int_t id = fMapping->GetNbOfPlane() + planeId ;

	    fADCTimeBinPosHist[planeId]->Divide(fTimeBinPosHist[planeId]);
	    fADCHist[planeId]->Write();
	    fHitHist[planeId]->Write();
	    fClusterHist[planeId]->Write();
	    fClusterInfoHist[planeId]->Write();
	    fTimeBinPosHist[planeId]->Write();
	    fADCTimeBinPosHist[planeId]->Write();

	    fADCTimeBinPosHist[id]->Divide(fTimeBinPosHist[planeId]);
	    fHitHist[id]->Write();
	    fClusterHist[id]->Write();
	    fClusterInfoHist[id]->Write();
	    fTimeBinPosHist[id]->Write();
	    fADCTimeBinPosHist[id]->Write();
	}

	map <Int_t, TString> listOfDetectors = fMapping->GetDetectorFromIDMap() ;
	map <Int_t, TString>::const_iterator det_itr ;
	for (det_itr = listOfDetectors.begin(); det_itr != listOfDetectors.end(); ++ det_itr) {
	    Int_t detId = (*det_itr).first ;

	    Int_t nbOfBinsX = f2DPlotsHist[3*detId+1]->GetNbinsX() ;
	    Int_t nbOfBinsY = f2DPlotsHist[3*detId+1]->GetNbinsY() ;
	    for (Int_t ii = 0; ii < nbOfBinsX; ii++) {
		for (Int_t jj = 0; jj < nbOfBinsY; jj++) {
		    Int_t binx = ii + 1 ;
		    Int_t biny = jj + 1 ; 
		    Int_t binContent = ((Int_t ) f2DPlotsHist[3*detId]->GetBinContent(binx,biny)) ;
		    if (binContent < 1 )  {
			f2DPlotsHist[3*detId]->SetBinContent(binx,biny,0) ;
			f2DPlotsHist[3*detId+1]->SetBinContent(binx,biny,0) ;
			f2DPlotsHist[3*detId+2]->SetBinContent(binx,biny,0) ;
		    }
		}
	    }

	    f2DPlotsHist[3*detId+1]->Divide(f2DPlotsHist[3*detId]);
	    f2DPlotsHist[3*detId+2]->Divide(f2DPlotsHist[3*detId]);

	    Float_t norm =  (f2DPlotsHist[3*detId+1]->GetNbinsX()*f2DPlotsHist[3*detId+1]->GetNbinsY()) / f2DPlotsHist[3*detId+1]->Integral() ;
	    TString detector = (*det_itr).second ;
	    if (fMapping->GetReadoutBoardFromDetector(detector) == "UV_ANGLE") { 
		norm = 0.75 * norm ;
	    }

	    /**
	      f2DPlotsHist[3*detId+1]->Scale(norm) ;
	      f2DPlotsHist[3*detId+1]->SetMaximum(1.5);
	      f2DPlotsHist[3*detId+1]->SetMinimum(0.5);
	      */

	    f2DPlotsHist[3*detId]->Write();
	    f2DPlotsHist[3*detId+1]->Write();
	    f2DPlotsHist[3*detId+2]->Write();
	    fChargeSharingHist[detId]->Write();
	    fChargeRatioHist[detId]->Write();
	}
	rootFile->Close();
	delete rootFile;
    }

    //=====================================================_
    void GEMView::PrintTextOnMessager(QString qString, int level) {
	QString prefix = Form("[%d] ", TheCurrentLine);
	TheCurrentLine++;
	prefix.append(qString);
	prefix.append("<br>");
	QString alartHtml = "<font color=\"Red\">";
	QString normalHtml = "<font color=\"Black\">";

	switch(level) {
	    case 1: prefix = alartHtml % prefix;
		    break;
	    default:  prefix = normalHtml % prefix;
		      break;
	}
	TheTextCursor().insertHtml(prefix);
	TheTextCursor().movePosition(QTextCursor::End);
	TheMessager().ensureCursorVisible();
    }

    //==================================================
    void GEMView::AnalyzeMultiEvents() {
	/*
	   bool ok;
	   unsigned int minEntry = 0, maxEntry = 5000;
	   unsigned int minEntry = (unsigned int) (fConfig.GetFirstEvent()) ;
	   unsigned int maxEntry = (unsigned int) (fConfig.GetLastEvent());
	   QString eventRange;
	   fProgressBar->reset();
	   fProgressBar->setVisible(true);
	   fProgressBar->show();
	   eventRange = QInputDialog::getText(fMainWidget, tr("Event range (minEntry~maxEntry)"), tr("Event range: "), QLineEdit::Normal,"", &ok);   if (ok && !eventRange.isEmpty()) {
	   QStringList list = eventRange.split("~");
	   if (list.size() != 2) {
	   printf(" = GEMView::AnalyzeFile() => WARNING !!! Cannot understand the input formal, has to be minEntry~maxEntry. i.e 0~1000 \n") ;
	   return;
	   }
	   if ((list.at(0)).toInt(&ok, 10) < 0 || (list.at(1)).toInt(&ok, 10) < 0){
	   cout<<"both entry has to be larger than 0"<<endl;
	   return;
	   }
	   minEntry = (unsigned int)(list.at(0)).toInt(&ok, 10);
	   maxEntry = (unsigned int)(list.at(1)).toInt(&ok, 10);
	   if (!ok) return;
	   if (maxEntry<=minEntry){
	   printf(" = GEMView::AnalyzeFile() => WARNING !!! max entry has to be larger than min entry \n") ;
	   return;
	   }   
	   }
	   */

	fHandler->ProcessMultiEventsFromFile(fConfig.GetInputFileName(), fConfig.GetFirstEvent(), fConfig.GetLastEvent(), fADCHist, fHitHist, fClusterHist, fClusterInfoHist, f2DPlotsHist, fChargeSharingHist, fChargeRatioHist, fTimeBinPosHist, fADCTimeBinPosHist); 
	DrawMultipleEventDataHisto() ;

	//  TString rootFileName = fConfig.GetOutputFileName() ;
	//  WriteRootFile(rootFileName.Data()) ;
	WriteRootFile(fRootFileName.Data()) ;
    }

    //==================================================
    void GEMView::AnalyzeMultipleFiles(TString inputDataFile, TString outputRootFile) {
	fHandler->ProcessMultiEventsFromFile(inputDataFile.Data(), fConfig.GetFirstEvent(), fConfig.GetLastEvent(), fADCHist, fHitHist, fClusterHist, fClusterInfoHist, f2DPlotsHist, fChargeSharingHist, fChargeRatioHist, fTimeBinPosHist, fADCTimeBinPosHist); 
	WriteRootFile(outputRootFile.Data()) ;
	ResetMultipleFilesHistos() ;
    }

    //==================================================
    void GEMView::SearchEvents() {
	//  printf(" = GEMView::SearchEvents() \n") ;
	fHandler->ProcessSearchEventFromFile(fConfig.GetInputFileName(), f1DSingleEventHist, f2DSingleEventHist);
	DrawSingleEventDataHisto() ;
    }

    //=================================
    void GEMView::ProducePedestals() {
	if( fRunType != "PEDESTAL") {
	    printf(" = GEMView::ProducePedestals() => WARNING !!! Not a pedestal run \n") ;
	    return ;
	}
	fHandler->ProcessPedestals(fConfig.GetInputFileName(), fConfig.GetFirstEvent(), fConfig.GetLastEvent());
    }

    //======================================================================
    void GEMView::ClearDisplay(PRadHistCanvas *display) {
	TCanvas *thisCanvas = display->GetCanvas();
	thisCanvas->GetListOfPrimitives()->Clear();
	thisCanvas->Modified();
	thisCanvas->Update();
	display->Refresh();  
    }

    //===============================================================================================================
    void GEMView::DrawMultiEventsADCHistos(int id, TH1F* adcHist) {
	//  printf(" = GEMView::DrawMultiEvents1DHistos() ==> Enter \n") ;
	gStyle->SetOptStat(1111);
	ClearDisplay(fADCDisplay[id]);
	fADCDisplay[id]->GetCanvas()->cd() ;
	fADCDisplay[id]->GetCanvas()->SetFillColor(10);
	adcHist->Draw();
	fADCDisplay[id]->GetCanvas()->Modified();
	fADCDisplay[id]->GetCanvas()->Update();
	fADCDisplay[id]->Refresh();
    }

    //===============================================================================================================
    void GEMView::DrawMultiEvents1DHistos(int id, TH1F* hitHist, TH1F* clusterHist, TH1F* clusterInfoHist, TH2F* adcTimeBinPosHist) {
	//  printf(" = GEMView::DrawMultiEvents1DHistos() ==> Enter \n") ;
	gStyle->SetOptStat(1111);

	ClearDisplay(fHitDisplay[id]);
	fHitDisplay[id]->GetCanvas()->cd() ;
	fHitDisplay[id]->GetCanvas()->SetFillColor(10);
	hitHist->Draw();
	fHitDisplay[id]->GetCanvas()->Modified();
	fHitDisplay[id]->GetCanvas()->Update();
	fHitDisplay[id]->Refresh();

	ClearDisplay(fClusterDisplay[id]);
	fClusterDisplay[id]->GetCanvas()->cd() ;
	fClusterDisplay[id]->GetCanvas()->SetFillColor(10);
	clusterHist->Draw();
	fClusterDisplay[id]->GetCanvas()->Modified();
	fClusterDisplay[id]->GetCanvas()->Update();
	fClusterDisplay[id]->Refresh();

	ClearDisplay(fClusterInfoDisplay[id]);
	fClusterInfoDisplay[id]->GetCanvas()->cd() ;
	fClusterInfoDisplay[id]->GetCanvas()->SetFillColor(10);
	clusterInfoHist->Draw();
	fClusterInfoDisplay[id]->GetCanvas()->Modified();
	fClusterInfoDisplay[id]->GetCanvas()->Update();
	fClusterInfoDisplay[id]->Refresh();

	gStyle->SetOptStat(0);
	ClearDisplay(fADCTimeBinPosDisplay[id]);
	fADCTimeBinPosDisplay[id]->GetCanvas()->cd() ;
	fADCTimeBinPosDisplay[id]->GetCanvas()->SetFillColor(10);
	adcTimeBinPosHist->Draw("colz");
	fADCTimeBinPosDisplay[id]->GetCanvas()->Modified();
	fADCTimeBinPosDisplay[id]->GetCanvas()->Update();
	fADCTimeBinPosDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawSingleEvent1DHistos(int id, TH1F *hist, Int_t  nbOfStrips, Float_t size) {
	//  printf(" = GEMView::DrawSingleEvent1DHistos() ==> Enter \n") ;
	ClearDisplay(f1DSingleEventDisplay[id]);
	f1DSingleEventDisplay[id]->GetCanvas()->cd() ;
	f1DSingleEventDisplay[id]->GetCanvas()->SetFillColor(10);
	hist->SetBins(nbOfStrips, -1*size/2.0, size/2.0);
	hist->Draw();
	f1DSingleEventDisplay[id]->GetCanvas()->Modified();
	f1DSingleEventDisplay[id]->GetCanvas()->Update();
	f1DSingleEventDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawSingleEvent1DHistos(int id, TH1F *hist, Int_t  nbOfStrips) {
	//  printf(" = GEMView::DrawSingleEvent1DHistos() ==> Enter \n") ;
	ClearDisplay(f1DSingleEventDisplay[id]);
	f1DSingleEventDisplay[id]->GetCanvas()->cd() ;
	f1DSingleEventDisplay[id]->GetCanvas()->SetFillColor(10);
	Int_t nbBins = nbOfStrips ;
	//  Int_t nbBins = nbOfStrips*fNbOfTimeSamples ;
	hist->SetBins(nbBins, 0, (nbBins - 1) );
	hist->Draw();
	f1DSingleEventDisplay[id]->GetCanvas()->Modified();
	f1DSingleEventDisplay[id]->GetCanvas()->Update();
	f1DSingleEventDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawSingleEvent2DHistos(int id, TH2F *hist2d, TString plotStyle, Float_t size, Int_t  nbOfStrips) {
	ClearDisplay(f2DSingleEventDisplay[id]);
	Int_t driftTime = (fNbOfTimeSamples - 1) * 25 ;
	f2DSingleEventDisplay[id]->GetCanvas()->cd() ;
	f2DSingleEventDisplay[id]->GetCanvas()->SetFillColor(10);
	hist2d->SetBins(fNbOfTimeSamples, 0, driftTime, nbOfStrips, -1*size/2.0, size/2.0);
	hist2d->Draw(plotStyle);
	f2DSingleEventDisplay[id]->GetCanvas()->Modified();
	f2DSingleEventDisplay[id]->GetCanvas()->Update();
	f2DSingleEventDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawSingleEvent2DHistos(int id, TH2F *hist2d, TString plotStyle, Int_t  nbOfStrips) {
	ClearDisplay(f2DSingleEventDisplay[id]);
	Int_t driftTime = (fNbOfTimeSamples - 1) * 25 ;
	f2DSingleEventDisplay[id]->GetCanvas()->cd() ;
	f2DSingleEventDisplay[id]->GetCanvas()->SetFillColor(10);
	hist2d->SetBins(fNbOfTimeSamples, 0, driftTime, nbOfStrips, 0, (nbOfStrips - 1) );
	hist2d->Draw(plotStyle);
	f2DSingleEventDisplay[id]->GetCanvas()->Modified();
	f2DSingleEventDisplay[id]->GetCanvas()->Update();
	f2DSingleEventDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawMultiEvents2DHistos(int id, TH2F * pos2DHist) {
	ClearDisplay(f2DPlotsDisplay[id]) ;
	gStyle->SetOptStat(0);
	f2DPlotsDisplay[id]->GetCanvas()->cd();
	f2DPlotsDisplay[id]->GetCanvas()->SetFillColor(10);
	pos2DHist->Draw("colz");
	f2DPlotsDisplay[id]->GetCanvas()->Modified();
	f2DPlotsDisplay[id]->GetCanvas()->Update();
	f2DPlotsDisplay[id]->Refresh();
    }

    //================================================================================================================
    void GEMView::DrawMultiEvents2DHistos(int id, TH2F *chargeSharingHist, TH1F* chRatioHist) {
	ClearDisplay(fChargeSharingDisplay[2*id]) ;
	fChargeSharingDisplay[2*id]->GetCanvas()->cd();
	fChargeSharingDisplay[2*id]->GetCanvas()->SetFillColor(10);
	chargeSharingHist->Draw("colz");
	fChargeSharingDisplay[2*id]->GetCanvas()->Modified();
	fChargeSharingDisplay[2*id]->GetCanvas()->Update();
	fChargeSharingDisplay[2*id]->Refresh();

	ClearDisplay(fChargeSharingDisplay[2*id+1]) ;
	fChargeSharingDisplay[2*id+1]->GetCanvas()->cd();
	fChargeSharingDisplay[2*id+1]->GetCanvas()->SetFillColor(10);
	chRatioHist->Draw();
	fChargeSharingDisplay[2*id+1]->GetCanvas()->Modified();
	fChargeSharingDisplay[2*id+1]->GetCanvas()->Update();
	fChargeSharingDisplay[2*id+1]->Refresh();
    }

    //====================================================================
    void GEMView::DisableMouseEvent(PRadHistCanvas* theWidget) {
	//theWidget->DisableSignalEvents(kMousePressEvent);
	//theWidget->DisableSignalEvents(kMouseMoveEvent);
	//theWidget->DisableSignalEvents(kMouseReleaseEvent);
	//theWidget->DisableSignalEvents(kMouseDoubleClickEvent);
	//theWidget->DisableSignalEvents(kKeyPressEvent);
	//theWidget->DisableSignalEvents(kEnterEvent);
	//theWidget->DisableSignalEvents(kLeaveEvent);
    }
