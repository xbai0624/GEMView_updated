#include "GEMPedestal.h"
#include "GEMRawDecoder.h"
#include "GEMMapping.h"
#include "GEMRawPedestal.h"
#include <stdio.h>
#include <TCanvas.h>
#include <iostream>

using namespace std;

//=========================================================================================================================
GEMPedestal::GEMPedestal(TString pedFileName, int nbOfTimeSamples) {
  printf("   GEMPedestal::GEMPedestal() ==> Start init \n" ) ;
  fPedFileName = pedFileName ;
  printf("   GEMPedestal::GEMPedestal() ==> pedestal file %s\n",fPedFileName.Data() ) ;

  fRawPedestal = new GEMRawPedestal(nbOfTimeSamples);
  mapping = GEMMapping::GetInstance();
  NCH = 128;
  nNbofAPVs = mapping->GetNbOfAPVs();
  FECs.clear();
  FECs = mapping->GetBankIDSet();
  printf("   GEMPedestal::GEMPedestal() ==> End init \n" ) ;

}

//=========================================================================================================================
GEMPedestal::~GEMPedestal() {
  FECs.clear();
}

//=========================================================================================================================
void GEMPedestal::Delete() {
  int N = vStripOffsetHistos.size();
  for(int i=0;i<N;i++) {
    delete vStripOffsetHistos[i];
    delete vStripNoiseHistos[i];
  }
  int M = vApvPedestalOffset.size();
  for(int i=0;i<M;i++) {
    delete vApvPedestalOffset[i];
    delete vApvPedestalNoise[i];
  }
}

//=========================================================================================================================
void GEMPedestal::BookHistos() {
   printf("   GEMPedestal::BookHistos() ==> Enter \n" ) ;
  //book histograms for each strip
  for(int chNo = 0; chNo < NCH; chNo++) {
    for(int apvKey = 0; apvKey < nNbofAPVs; apvKey++) {
      stringstream out;
      stringstream apv;
      apv << apvKey;
      out << chNo;
      TString chNoStr = out.str();
      TString apvStr = apv.str();
      TString noise = "hNoise_" + apvStr+ "_" + chNoStr;
      TString offset = "hOffset_" +apvStr+ "_"+ chNoStr;
      vStripNoiseHistos.push_back( new TH1F(noise, noise, 8097, -4048, 4048) ); 
      vStripOffsetHistos.push_back( new TH1F(offset, offset, 8097, -4048, 4048)  );
    }
  }

  // book histograms for each APV
  for(int apvKey = 0; apvKey < nNbofAPVs; apvKey++)  {;
    stringstream out;
    out << apvKey;
    TString outStr = out.str(); 
    vApvPedestalOffset.push_back(new TH1F(GetHistoName(apvKey, "offset", ""),  GetHistoName(apvKey, "offset", ""), 128, -0.5, 127.5));
    vApvPedestalNoise.push_back(new TH1F(GetHistoName(apvKey, "noise", ""),  GetHistoName(apvKey, "noise", ""), 128, -0.5, 127.5));
  }

  // book histograms for overall distribution
  hAllStripNoise = new TH1F("hAllStripNoise", "Overall Noise Distribution", 100, 0, 10);
  hAllXStripNoise = new TH1F("hAllXStripNoise", "Overall X Direction Noise Distribution", 100, 0, 10);
  hAllYStripNoise = new TH1F("hAllYStripNoise", "Overall Y Direction Noise Distribution", 100, 0, 10);
  printf("   GEMPedestal::BookHistos() ==> Exit \n" ) ;
}

//=========================================================================================================================
TString GEMPedestal::GetHistoName(Int_t apvKey, TString dataType, TString dataNb) {
  Int_t apvID = mapping->GetAPVIDFromAPVNo(apvKey);
  TString apvName = mapping->GetAPVFromID(apvID);
  TString histoName = dataType + dataNb + "_" + apvName ;
  return histoName;
}


//=========================================================================================================================
void GEMPedestal::AccumulateEvent(int evtID, map<int, vector<int> > & mAPVRawTSs) {
  if (evtID % 250 == 0)  printf("   GEMPedestal::AccumulateEvent() ==> process event Id = %d\n", evtID) ;
  fRawPedestal->ComputeEventPedestal(mAPVRawTSs);
  for(auto &i: mAPVRawTSs) {
    int apvid = i.first;
    int apvKey = mapping->GetAPVNoFromID(apvid);
    for(int chNo=0;chNo<NCH;chNo++) {
      vStripNoiseHistos[apvKey*NCH + chNo]->Fill(fRawPedestal->GetStripNoise(apvid, chNo));
      vStripOffsetHistos[apvKey*NCH + chNo]->Fill(fRawPedestal->GetStripOffset(apvid, chNo));
    }
  }
}

//=========================================================================================================================
void GEMPedestal::ComputePedestal() {
  for(int apvKey = 0; apvKey < nNbofAPVs; apvKey++) {
    for(int chNo = 0; chNo < NCH; chNo++) {
      Float_t offset = vStripOffsetHistos[apvKey*NCH + chNo] -> GetMean();
      Float_t noise  = vStripNoiseHistos[apvKey*NCH + chNo] -> GetRMS();

      vApvPedestalOffset[apvKey]->SetBinContent(chNo, offset);
      vApvPedestalNoise[apvKey]->SetBinContent(chNo, noise);

      hAllStripNoise->Fill(noise);
      Int_t fAPVID = mapping->GetAPVIDFromAPVNo(apvKey);
      TString plane = mapping->GetPlaneFromAPVID(fAPVID);
      if(plane.Contains("X"))        hAllXStripNoise->Fill(noise);
      else if( plane.Contains("Y"))  hAllYStripNoise->Fill(noise);
      else cout<<"GEMPedestal::ComputePedestal: Error: Unrecongnized plane name..." <<endl;
    }
  }
}

//=========================================================================================================================
void GEMPedestal::SavePedestalFile() {
  printf("   GEMPedestal::SavePedestalFile() ==> save pedestal data in root file %s\n", fPedFileName.Data() ) ;
  TFile *file = new TFile(fPedFileName.Data(), "recreate");
  ComputePedestal();
  for(int apvKey=0;apvKey<nNbofAPVs;apvKey++) {
    vApvPedestalOffset[apvKey]->Write();
    vApvPedestalNoise[apvKey]->Write();
  }
  hAllStripNoise->Write();
  hAllXStripNoise->Write();
  hAllYStripNoise->Write();
  file->Write();
}

//=========================================================================================================================
void GEMPedestal::LoadPedestal() {
  printf("   GEMPedestal::LoadPedestal() ==> Loading pedestal file %s\n", fPedFileName.Data() ) ;
  TFile *_file = new TFile(fPedFileName.Data() , "READ" );
    if(_file->IsZombie() ) {
      cout<<"#### Cannot Load pedestal file... ####"<<endl;
      return;
    }
    Int_t nAPVs = mapping->GetNbOfAPVs();
    for(int i=0;i<nAPVs;i++) {
      stringstream out;
      out << i;
      TString outStr = out.str();
      vApvPedestalOffset.push_back( (TH1F*) _file->Get( GetHistoName(i, "offset", "") )  );
      vApvPedestalOffset[i]->SetDirectory(0);
      vApvPedestalNoise.push_back( (TH1F*) _file->Get( GetHistoName(i, "noise", "" ) ) ); 
      vApvPedestalNoise[i]->SetDirectory(0);
    }
    // Cannot close file while pedestal histograms are still being used...
    _file -> Close();
}

//=========================================================================================================================
vector<Float_t>  GEMPedestal::GetAPVNoises(Int_t apvid) {
  vector<Float_t> noises;
  Int_t apvNo = mapping->GetAPVNoFromID(apvid);
  for(int i=0;i<NCH;i++) noises.push_back( vApvPedestalNoise[apvNo]->GetBinContent(i)  );
  return noises;
}

//=========================================================================================================================
vector<Float_t>  GEMPedestal::GetAPVOffsets(Int_t apvid) {
  vector<Float_t> offsets;
  Int_t apvNo = mapping->GetAPVNoFromID(apvid);
  for(int i=0;i<NCH;i++) offsets.push_back( vApvPedestalOffset[apvNo]->GetBinContent(i)  );
  return offsets;
}
