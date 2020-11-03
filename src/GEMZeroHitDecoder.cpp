#include "GEMZeroHitDecoder.h"

//_______________________________________________________________________________
GEMZeroHitDecoder::GEMZeroHitDecoder(uint32_t *rbuf, int Size, Int_t nbOfTimeSamples) {
  fIsHitMaxOrTotalADCs = "signalPeak"; 
  fTimeSample = nbOfTimeSamples;
  buf = rbuf;
  fSize = Size;
  cout<<"GEMZeroHitDecoder:: event size:  "<<fSize<<endl;
  fMapping = GEMMapping::GetInstance();

  fFECID = 0;
  fADCChannel = 0;
  fAPVID = 0;
  fAPVKey = 0;
  fZeroSupCut = 5;

  fListOfHitsZero.clear();
  fListOfHitsZeroFromPlane.clear();
  //  fMinADCvalue = minADCs ; 
  //=======================//
  // cluster information   //
  //=======================//
  fMinClusterSize = 1;
  fMaxClusterSize = 20;
  fIsClusterMaxOrTotalADCs = "totalADCs";
  fIsGoodClusterEvent = kFALSE;
  fListOfClustersZeroFromPlane.clear();
  fListOfClustersZeroFromPlane.clear();

  //---------
  ProcessEvent();
  //ped->Delete();
  //
}

//_______________________________________________________________________________
GEMZeroHitDecoder::~GEMZeroHitDecoder() {
  //clear hits
  map<int, GEMHit *>::iterator it = fListOfHitsZero.begin();
  for(;it!=fListOfHitsZero.end();++it) delete (it->second);

  //clear clusters
  if( fListOfClustersZeroFromPlane.size() > 0)   {
    map<TString, list<GEMCluster*> >::iterator itt = fListOfClustersZeroFromPlane.begin();
    for(;itt!=fListOfClustersZeroFromPlane.end();++itt)     {
      list<GEMCluster*>::iterator itc = (itt->second).begin();
      for(;itc!=(itt->second).end();++itc) 
        delete *itc;
      (itt->second).clear();
    }
  }
  fListOfClustersZeroFromPlane.clear();
  fListOfHitsZero.clear();
  fListOfHitsZeroFromPlane.clear();
  DeleteClustersInPlaneMap();
}

//_______________________________________________________________________________
void GEMZeroHitDecoder::ProcessEvent( )
{
  EventHandler( );
}

//_______________________________________________________________________________
void GEMZeroHitDecoder::EventHandler( ) {
  //cout<<"GEMHitDecoder::EventHandler begin..."<<endl;

  /* hit structure
   * det: 1 bit
   * plane: 1 bit
   * fec: 4 bit
   * adcchannel: 4 bit
   * strip: 7 bit
   * time sample: 3 bit
   * polarity: 1 bit
   * val: 11 bit
   */

  //Fill Hits
  int pol = 0;
  int adc = 0;
  int TS = 0;
  int chNo = 0;

  for(int i=0;i<fSize;i++)  {
    fFECID = ( (buf[i]>>26) & 0xf);
    fADCChannel = ( (buf[i]>>22) & 0xf);
    chNo = ( (buf[i]>>15) & 0x7f );
    TS = ( (buf[i]>>12) & 0x7 );
    pol = ( (buf[i]>>11) & 0x1);
    adc = ( (buf[i]) & 0x7ff );

    if(pol == 1) adc = -adc;
    fAPVID = (fFECID<<4)|fADCChannel;
    fAPVKey = fMapping->GetAPVNoFromID(fAPVID);
    int hitID = (fAPVKey << 8) | chNo ;
    //debug
    cout<<"fec: "<<fFECID<<" adc: "<<fADCChannel<<" strip: "<<chNo<<" timebin: "<<TS<<" adc: "<<adc<<endl;
    if( !fListOfHitsZero[hitID]) {
      Int_t maxTimeSample = fTimeSample - 1 ;
      GEMHit * hit = new GEMHit(hitID, fAPVID, chNo, fZeroSupCut, fIsHitMaxOrTotalADCs, fTimeSample, 0, maxTimeSample);
      fListOfHitsZero[hitID] = hit;
    }
    fListOfHitsZero[hitID] -> AddTimeBinADCs(TS, adc, 0.0);
  }
  GetListOfHitsZeroFromPlanes();
  //========================================//
  //               cluster computing        //
  //========================================//
  ComputeClusters();
}

//_____________________________________________________________________________________________
map < TString, list <GEMHit * > > GEMZeroHitDecoder::GetListOfHitsZeroFromPlanes() {
  // printf(" Enter  GEMHitDecoder::GetListOfHitsFromPlanes()\n") ;
  map < Int_t, GEMHit * >::const_iterator hit_itr ;
  for(hit_itr = fListOfHitsZero.begin(); hit_itr != fListOfHitsZero.end(); ++hit_itr) { 
    GEMHit * hit = (* hit_itr).second ;
    TString planename = hit->GetPlane() ;
    fListOfHitsZeroFromPlane[planename].push_back(hit) ;
  }
  return fListOfHitsZeroFromPlane ;
}

//____________________________________________________________________________________________
TH1F* GEMZeroHitDecoder::GetZeroHit(TString str) {
  TH1F * h1;

  int nbDetector = fMapping->GetNbOfDetectors();
  for(int i=0;i<nbDetector;i++) {
    TString detectorName = fMapping->GetDetectorFromID(i);
    list<TString> planeList = fMapping->GetPlaneListFromDetector(detectorName);
    list<TString>::iterator it;
    for(it=planeList.begin();it!=planeList.end();++it) {
      if(*it == str) {
        TString hh = detectorName+"_"+(*it)+"_hit_distribution_zero_suppression";
	h1 = new TH1F(hh, hh, 2000, -fMapping->GetPlaneSize(*it)/2-100, fMapping->GetPlaneSize(*it)/2+100 );
        list< GEMHit* > hitList = fListOfHitsZeroFromPlane[ *it  ];
        list< GEMHit* >::iterator hit_it;
        for(hit_it=hitList.begin(); hit_it!=hitList.end();++hit_it) {
          Float_t pos = (*hit_it) -> GetStripPosition();
	  Float_t adc = (*hit_it) -> GetHitADCs(); 
	  h1 -> Fill(pos, adc);
        }
      }
    }
  }
  return h1;
}

/*****************************************************************************
 * **                    Compute Clusters Information                     ** *
 *****************************************************************************/

//============================================================================================
static Bool_t CompareStripNo( TObject *obj1, TObject *obj2) {
  Bool_t compare ;
  if ( ( (GEMHit*) obj1 )->GetStripNo() < ( ( GEMHit*) obj2 )->GetStripNo() ) compare = kTRUE ;
  else compare = kFALSE ;
  return compare ;
}

//============================================================================================
static Bool_t CompareClusterADCs( TObject *obj1, TObject *obj2) {
  Bool_t compare ;
  if ( ( (GEMCluster*) obj1 )->GetClusterADCs() > ( ( GEMCluster*) obj2 )->GetClusterADCs()) compare = kTRUE ;
  else compare = kFALSE ;
  return compare ;
}

//============================================================================================
void GEMZeroHitDecoder::ComputeClusters()  {
  //  printf("==GEMZeroHitDecoder::ComputeClustersInPlane() \n") ;
  map < TString, list <GEMHit*> >::const_iterator  hitsFromPlane_itr ;
  for (hitsFromPlane_itr = fListOfHitsZeroFromPlane.begin(); hitsFromPlane_itr != fListOfHitsZeroFromPlane.end(); ++hitsFromPlane_itr) {
    TString plane =  (*hitsFromPlane_itr).first ;
    list <GEMHit*> hitsFromPlane = (*hitsFromPlane_itr).second ; 
    hitsFromPlane.sort(CompareStripNo) ;
    Int_t listSize = hitsFromPlane.size() ;

    if (listSize < fMinClusterSize) {
      fIsGoodClusterEvent = kFALSE ;
      continue ;
    }

    Int_t previousStrip = -2 ;
    Int_t clusterNo = -1 ;
    map<Int_t, GEMCluster *> clustersMap ;
    list <GEMHit *>::const_iterator hit_itr ;

    for (hit_itr = hitsFromPlane.begin(); hit_itr != hitsFromPlane.end(); hit_itr++) {
      GEMHit * hit =  * hit_itr ; 
      Int_t currentStrip = hit->GetStripNo() ;
      if(currentStrip < 0) {cout<<"Warning: Found cluster outside GEM detector..."<<endl;continue; }// for special 0~16 strips, just remove them.
      if(currentStrip != (previousStrip + 1)) {
	clusterNo++ ;
      }
      if(!clustersMap[clusterNo]) {
	clustersMap[clusterNo] = new GEMCluster(0, clusterNo, fMinClusterSize, fMaxClusterSize, fIsHitMaxOrTotalADCs, fMinADCvalue) ;
	//	clustersMap[clusterNo]->SetNbAPVsFromPlane(hit->GetNbAPVsFromPlane());
	//	clustersMap[clusterNo]->SetAPVIndexOnPlane(hit->GetAPVIndexOnPlane());
	//	clustersMap[clusterNo]->SetPlaneSize(hit->GetPlaneSize());
	//	clustersMap[clusterNo]->SetPlane(hit->GetPlane());
      }
      clustersMap[clusterNo]->AddHit(hit) ;
      previousStrip = currentStrip;
    }

    map<Int_t, GEMCluster *>::const_iterator  cluster_itr ;
    for (cluster_itr = clustersMap.begin(); cluster_itr != clustersMap.end(); cluster_itr++) {
      GEMCluster * cluster = ( * cluster_itr ).second ;
      if (!cluster->IsGoodCluster()) {
	delete cluster ;
	continue ;
      }
      cluster->ComputeCluster() ;
      //      cluster->ComputeClusterPosition() ;
      fListOfClustersZeroFromPlane[plane].push_back(cluster) ;
    }

    fListOfClustersZeroFromPlane[plane].sort(CompareClusterADCs) ;
    hitsFromPlane.clear() ;
    clustersMap.clear() ;
  }

}

//____________________________________________________________________________________________
TH1F* GEMZeroHitDecoder::GetCluster(TString str) {
  TH1F * hc1;
  int nbDetector = fMapping->GetNbOfDetectors();
  for(int i=0;i<nbDetector;i++)  {
    TString detectorName = fMapping->GetDetectorFromID(i);
    list<TString> planeList = fMapping->GetPlaneListFromDetector(detectorName);
    list<TString>::iterator it;
    for(it=planeList.begin();it!=planeList.end();++it) {
      if(*it == str)   {
        TString hh = detectorName+"_"+(*it)+"_Cluster_Distribution_zero_suppression";
	hc1 = new TH1F(hh, hh, 2000, -fMapping->GetPlaneSize(*it)/2-100, fMapping->GetPlaneSize(*it)/2+100 );
        list< GEMCluster* > clusterList = fListOfClustersZeroFromPlane[ *it  ];
        list< GEMCluster* >::iterator cluster_it;
        for(cluster_it=clusterList.begin(); cluster_it!=clusterList.end();++cluster_it) {
          Float_t pos = (*cluster_it) -> GetClusterPosition();
	  Float_t adc = (*cluster_it) -> GetClusterADCs(); 
	  hc1 -> Fill(pos, adc);
        }
      }
    }
  }
  return hc1;
}

//==================================================================================
void GEMZeroHitDecoder::DeleteClustersInPlaneMap() {
  map < TString, list <GEMCluster *> >::const_iterator itr ;
  for (itr = fListOfClustersZeroFromPlane.begin(); itr != fListOfClustersZeroFromPlane.end(); itr++) {
    list <GEMCluster *> listOfCluster = (*itr).second ;
    listOfCluster.clear() ;
  }
  fListOfClustersZeroFromPlane.clear() ;
}

//===================================================================================
void GEMZeroHitDecoder::GetCluster2DPosition(vector<float> &x1, vector<float> &y1, vector<float> &x2, vector<float> &y2)
{
  list<GEMCluster*> cluster_x1 = fListOfClustersZeroFromPlane["pRadGEM1X"];
  list<GEMCluster*> cluster_y1 = fListOfClustersZeroFromPlane["pRadGEM1Y"];
  list<GEMCluster*> cluster_x2 = fListOfClustersZeroFromPlane["pRadGEM2X"];
  list<GEMCluster*> cluster_y2 = fListOfClustersZeroFromPlane["pRadGEM2Y"];

  int s1 = cluster_x1.size();
  int s2 = cluster_y1.size();
  int nbCluster1 = (s1<s2)?s1:s2;
  s1 = cluster_x2.size();
  s2 = cluster_y2.size();
  int nbCluster2 = (s1<s2)?s1:s2;

  if(nbCluster1>0)
  {
    list<GEMCluster*>::iterator itx = cluster_x1.begin();
    list<GEMCluster*>::iterator ity = cluster_y1.begin();
    for(int i = 0;i<nbCluster1;i++)
    {
      x1.push_back( (*itx++)->GetClusterPosition()  );
      y1.push_back( (*ity++)->GetClusterPosition()  );
    }
  }

   if(nbCluster2>0)
  {
    list<GEMCluster*>::iterator itx2 = cluster_x2.begin();
    list<GEMCluster*>::iterator ity2 = cluster_y2.begin();
    for(int i = 0;i<nbCluster2;i++)
    {
      x2.push_back( (*itx2++)->GetClusterPosition()  );
      y2.push_back( (*ity2++)->GetClusterPosition()  );
    }
  }

}
