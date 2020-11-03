#ifndef GEMVIEW_H
#define GEMVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QSplitter>
#include <Qt>
#include <QVBoxLayout>
#include <QTimer>
#include <map>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <vector>
#include <QPushButton>
#include <QTextEdit>
#include <QtGui>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QGridLayout>
#include <QProgressBar>
#include "hardcode.h"

#include "TCanvas.h"
#include "TStyle.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TList.h"
#include "PRadHistCanvas.h"
#include "TAxis.h"
#include "TPaveText.h"
#include "TRandom3.h"

#include "PRadHistCanvas.h"
#include "TFile.h"

#include "GEMHistContainer.h"
#include "GEMConfiguration.h"
#include "GEMRawDecoder.h"
#include "GEMInputHandler.h"
#include "GEMMapping.h"
#include "GEMPedestal.h"
#include "GEMFEC.h"

class TObject;
class TCanvas;
class GEMHistContainer;
class GEMFEC;

class GEMView : public QMainWindow//, public Ui::DynamicSlice
{
    Q_OBJECT

public:
    GEMView();
    ~GEMView();

    static void PrintTextOnMessager(QString qString, int level);
    int IsOnlineMode() const { return fConfig.GetOnlineMode(); }

private slots:
    int GEMMonitorLoop();
    void SpinCurrentEvent(int i);
    void ChangeTimerInterval(int i);
    void ChangeAutoMode();
    int  AutoGEMMonitorLoop();
    void ParserInnerLoop();
    void WriteRootFile(TString outputRootFile);
    void SearchEvents();
    void AnalyzeMultiEvents();
    void AnalyzeMultipleFiles(TString inputDataFile, TString outputRootFile);
    void ProducePedestals() ;

protected:
    //void DynamicSlice();

public slots:
    //void DynamicExec(TObject *select, unsigned int event, TCanvas *); 

private:
    Int_t fNbADCBins ;

    void Clear();
    void ResetMultipleFilesHistos();
    void fillRawDataHisto(); 
    void DrawSingleEventDataHisto() ;
    void DrawMultipleEventDataHisto() ;

    void SetMonitor(int sec);
    void InitConfig();
    void InitGUI();
    void InitToolBox();
    void InitRootGraphics();
    void InitHistForZeroSup();
    void InitHistForRawData();
    void InitHistForMultipleFiles() ;

    void DrawSingleEvent1DHistos(int planeId, TH1F *hist, Int_t  nbOfStrips) ;
    void DrawSingleEvent1DHistos(int planeId, TH1F *hist, Int_t  nbOfStrips, Float_t size) ;

    void DrawSingleEvent2DHistos(int planeId, TH2F *hist, TString plotStyle, Int_t  nbOfStrips) ;
    void DrawSingleEvent2DHistos(int planeId, TH2F *hist, TString plotStyle, Float_t size, Int_t nbOfStrips) ;

    void DrawMultiEventsADCHistos(int id, TH1F* adcHist) ;
    void DrawMultiEvents1DHistos(int id, TH1F* hitHist, TH1F* clusterHist, TH1F* clusterInfoHist, TH2F* adcTimeBinPosHist) ;
    void DrawMultiEvents2DHistos(int detId, TH2F * pos2DHist) ;
    void DrawMultiEvents2DHistos(int detId, TH2F *chargeSharingHist, TH1F* chRatioHist) ;

    void DeleteHistForMultipleFiles() ;

    void ClearDisplay(PRadHistCanvas *display) ;
    void DisableMouseEvent(PRadHistCanvas* theWidget);

    QWidget     *fMainWidget;
    QTabWidget  *fTabWidget;
    QWidget     *fAllFECWidget;
    QGridLayout *fLayoutForAllAPV;
    QVBoxLayout *fLeftSplitter;
    QHBoxLayout *fLayout;

    //  int fWidth, fHeight; //width and height of the QMainWindow
    int fWidth, fHeight, fCurrentEntry, fNbOfTimeSamples; 
    TString fRunType, fRootFileName ;

    GEMFEC            **fFEC;
    GEMHistContainer  **fRootWidget;
    GEMConfiguration    fConfig;
    GEMPedestal        *fPedestal;
    GEMMapping         *fMapping;
    GEMInputHandler    *fHandler;

    UInt_t *fBuffer;
    QTimer *timer;
    QTimer *innerTimer;
    QHBoxLayout *fToolBox;
    QSpinBox *fEventSpin;

    QPushButton *fAutoMode, *fSearchEvents, *fMultipleEvents, *fEnableRootFile, *fProcessPedestal;

    PRadHistCanvas *fCornerWidget;
    QProgressBar* fProgressBar;

    TH1F **f1DSingleEventHist, **fADCHist, **fHitHist, **fClusterHist, **fClusterInfoHist, **fChargeRatioHist ;
    TH2F  **fTimeBinPosHist, **fADCTimeBinPosHist, **f2DPlotsHist, **f2DSingleEventHist, **fChargeSharingHist; 

    QWidget *fADCTimeBinPosWidget, *f2DSingleEventWidget, *f1DSingleEventWidget, *fADCWidget;
    QWidget *fHitWidget, *fClusterWidget, *fClusterInfoWidget, *f2DPlotsWidget, *fChargeSharingWidget;

    QGridLayout  *fADCTimeBinPosLayout, *f2DSingleEventLayout, *f1DSingleEventLayout, *fADCLayout;
    QGridLayout  *fHitLayout, *fClusterLayout, *fClusterInfoLayout, *f2DPlotsLayout, *fChargeSharingLayout;

    PRadHistCanvas **fADCTimeBinPosDisplay, **f2DSingleEventDisplay, **f1DSingleEventDisplay, **fADCDisplay ; 
    PRadHistCanvas **fHitDisplay, **fClusterDisplay, **fClusterInfoDisplay, **f2DPlotsDisplay, **fChargeSharingDisplay ;
};

#endif 
