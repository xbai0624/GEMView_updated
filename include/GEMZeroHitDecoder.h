#ifndef _GEMZEROHITDECODER_H__
#define _GEMZEROHITDECODER_H__

#include "GEMMapping.h"
#include "GEMHit.h"
#include "GEMCluster.h"

#include <stdint.h>

#include "TString.h"
#include "TObject.h"
#include <TStyle.h>
#include <TSystem.h>
#include <TROOT.h>
#include "TH1F.h"
#include "TList.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMath.h"

#include <map>
#include <list>
#include <vector>
#include <numeric>


class GEMZeroHitDecoder
{
public:
  GEMZeroHitDecoder(uint32_t *buf, int Size, Int_t nbOfTimeSamples);
  //  GEMZeroHitDecoder(uint32_t *buf, int Size, Int_t nbOfTimeSamples, Float_t minADCs);
  ~GEMZeroHitDecoder();

public:
  void ProcessEvent();
  void EventHandler();

  map< TString, list<GEMHit*> > GetListOfHitsZeroFromPlanes();
  TH1F* GetZeroHit(TString plane);

private:
  uint32_t *buf;
  int fSize;
  Int_t NCH;

private:
  int fZeroSupCut;
  int fFECID;
  int fADCChannel;
  int fAPVID;
  int fAPVKey;
  TString fIsHitMaxOrTotalADCs;
  GEMMapping * fMapping;

private:
  map<Int_t, GEMHit*> fListOfHitsZero;
  map<TString, list<GEMHit*> > fListOfHitsZeroFromPlane;

// compute cluster information 
public:
  void ComputeClusters();
  void DeleteClustersInPlaneMap() ;
  map < TString, list <GEMCluster * > > GetListOfClustersFromPlanes() { return  fListOfClustersZeroFromPlane;  }
  TH1F* GetCluster(TString str);
  void GetCluster2DPosition(vector<float> &x1, vector<float> &y1, vector<float> &x2, vector<float> &y2);

private:
  TString fIsClusterMaxOrTotalADCs;
  Bool_t fIsGoodClusterEvent;
  Int_t fMinClusterSize, fMaxClusterSize, fTimeSample;
  Float_t fMinADCvalue ;

  map<TString, list<GEMCluster*> > fListOfClustersZeroFromPlane;
};


#endif
