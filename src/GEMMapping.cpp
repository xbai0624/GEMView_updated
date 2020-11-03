#include "GEMMapping.h"

GEMMapping * GEMMapping::instance = 0 ;

//======================================================================================================================================
void GEMMapping::SetCartesianStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString planeX,  Float_t sizeX, Int_t connectorsX, Int_t orientX, TString planeY,  Float_t sizeY, Int_t connectorsY, Int_t orientY) {

    printf("   GEMMapping::SetCartesianStripsReadoutMap() => readout=%s, detType=%s, det=%s, detID=%d, planeX=%s, SizeX=%f, connectorsX=%d, planeY=%s, SizeY=%f, connectorsY=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, planeX.Data(), sizeX, connectorsX, planeY.Data(), sizeY, connectorsY) ;

    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ; 

    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;

    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;

    fPlaneIDFromPlaneMap[planeX] = 0 ;
    fPlaneIDFromPlaneMap[planeY] = 1 ;

    fDetectorFromPlaneMap[planeX] = detector ;
    fDetectorFromPlaneMap[planeY] = detector ;

    fPlaneListFromDetectorMap[detector].push_back(planeX) ;
    fPlaneListFromDetectorMap[detector].push_back(planeY) ;

    fCartesianPlaneMap[planeX].push_back(0) ;
    fCartesianPlaneMap[planeX].push_back(sizeX) ;
    fCartesianPlaneMap[planeX].push_back(connectorsX) ;
    fCartesianPlaneMap[planeX].push_back(orientX) ;

    fCartesianPlaneMap[planeY].push_back(1) ;
    fCartesianPlaneMap[planeY].push_back(sizeY) ;
    fCartesianPlaneMap[planeY].push_back(connectorsY) ;
    fCartesianPlaneMap[planeY].push_back(orientY) ;
}

//======================================================================================================================================
void GEMMapping::SetUVStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID,  Float_t length,  Float_t innerR ,  Float_t outerR , TString planeTop, Int_t connectTop, Int_t orientTop, TString planeBot, Int_t connectBot, Int_t orientBot) {

    printf("   GEMMapping::SetUVStripsReadoutMap() => readout=%s, detType=%s, det=%s,  planeTop=%s, nbAPV_Top=%d, planeBot=%s, nbAPV_Bot=%d, Length=%f, innerR=%f, outerR=%f \n", readoutBoard.Data(), detectorType.Data(), detector.Data(),  planeTop.Data(), connectTop, planeBot.Data(), connectBot, length, innerR, outerR) ;

    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;

    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;

    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;

    fPlaneIDFromPlaneMap[planeTop] = 0 ;
    fPlaneIDFromPlaneMap[planeBot] = 1 ;

    fDetectorFromPlaneMap[planeTop] = detector ;
    fDetectorFromPlaneMap[planeBot] = detector ;

    fPlaneListFromDetectorMap[detector].push_back(planeTop) ;
    fPlaneListFromDetectorMap[detector].push_back(planeBot) ;

    fCartesianPlaneMap[planeTop].push_back(0) ;
    fCartesianPlaneMap[planeBot].push_back(1) ;

    fUVangleReadoutMap[detector].push_back(length) ;
    fUVangleReadoutMap[detector].push_back(innerR) ;
    fUVangleReadoutMap[detector].push_back(outerR) ;

    fUVangleReadoutMap[planeTop].push_back(0) ;
    fUVangleReadoutMap[planeTop].push_back(connectTop) ;
    fUVangleReadoutMap[planeTop].push_back(orientTop) ;
    fUVangleReadoutMap[planeTop].push_back(length) ;

    fUVangleReadoutMap[planeBot].push_back(1) ;
    fUVangleReadoutMap[planeBot].push_back(connectBot) ;
    fUVangleReadoutMap[planeBot].push_back(orientBot) ;
    fUVangleReadoutMap[planeBot].push_back(outerR) ;
}
//======================================================================================================================================
void GEMMapping::Set1DStripsReadoutMap(TString readoutBoard, TString detectorType, TString detector, Int_t detID, TString plane,  Float_t size, Int_t connectors, Int_t orient) {

    printf("   GEMMapping::SetDetectorMap() => readout=%s, detType=%s, det=%s, detID=%d, plane=%s, Size=%f, connectors=%d, orientation=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, plane.Data(), size, connectors, orient) ;

    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;

    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;

    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;

    fPlaneIDFromPlaneMap[plane] = 0 ;
    fDetectorFromPlaneMap[plane] = detector ;
    fPlaneListFromDetectorMap[detector].push_back(plane) ;

    f1DStripsPlaneMap[plane].push_back(0) ;
    f1DStripsPlaneMap[plane].push_back(size) ;
    f1DStripsPlaneMap[plane].push_back(connectors) ;
    f1DStripsPlaneMap[plane].push_back(orient) ;
}

//======================================================================================================================================
void GEMMapping::SetCMSGEMReadoutMap(TString readoutBoard, TString detectorType,  TString detector, Int_t detID, TString EtaSector,  Float_t etaSectorPos, Float_t etaSectorSize, Float_t nbOfSectorConnectors, Int_t apvOrientOnEtaSector) {
    printf("   GEMMapping::SetDetectorMap() =>readout=%s, detType=%s, det=%s, detID=%d, EtaSector=%s, etaSectorSize=%f, nbOSectorfConnectors=%f, apvOrientOnEtaSector=%d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, EtaSector.Data(), etaSectorSize, nbOfSectorConnectors, apvOrientOnEtaSector) ;

    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;

    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;

    fDetectorFromPlaneMap[EtaSector] = detector ;
    fPlaneListFromDetectorMap[detector].push_back(EtaSector) ;

    fCMSGEMDetectorMap[EtaSector].push_back(etaSectorPos) ;
    fCMSGEMDetectorMap[EtaSector].push_back(etaSectorSize) ;
    fCMSGEMDetectorMap[EtaSector].push_back(nbOfSectorConnectors) ;
    fCMSGEMDetectorMap[EtaSector].push_back(apvOrientOnEtaSector) ;
}
//======================================================================================================================================
TString GEMMapping::GetAPV(TString detPlane, Int_t fecId, Int_t adcCh, Int_t apvNo, Int_t apvIndex) {
    stringstream out ;
    out << fecId ;
    TString fecIDStr = out.str();
    out.str("") ;
    out <<  adcCh;
    TString adcChStr = out.str();
    out.str("") ;
    out <<  apvNo ;
    TString apvNoStr = out.str();
    out.str("") ;
    out <<  apvIndex ;
    TString apvIndexStr = out.str();
    out.str("") ;
    TString apvName = "apv" + apvNoStr + "_adcCh" + adcChStr + "_FecId" + fecIDStr + "_" + detPlane ;
    return apvName ;
}

//======================================================================================================================================
void GEMMapping::SetPadsReadoutMap(TString readoutBoard, TString detectorType,  TString detector, Int_t detID, TString padPlane, Float_t padSizeX,  Float_t padSizeY, Float_t nbOfPadX, Float_t nbOfPadY, Float_t nbOfConnectors) {
    printf("   GEMMapping::SetDetectorMap() =>readout=%s, detType=%s, det=%s, detID=%d, padPlane=%s, nbOfPadX=%f, padSizeX=%f, nbOfPadY=%f, padSizeY=%f, nbOfConnectors=%f \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), detID, padPlane.Data(), nbOfPadX, padSizeX, nbOfPadY, padSizeY, nbOfConnectors) ;

    fDetectorFromIDMap[detID]     = detector ;
    fReadoutBoardFromIDMap[detID] = readoutBoard ;

    fDetectorIDFromDetectorMap[detector] = detID ;
    fReadoutBoardFromDetectorMap[detector] = readoutBoard ;
    fDetectorTypeFromDetectorMap[detector] = detectorType ;
    fDetectorListFromDetectorTypeMap[detectorType].push_back(detector) ;
    fDetectorListFromReadoutBoardMap[readoutBoard].push_back(detector) ;

    fDetectorFromPlaneMap[padPlane] = detector ;
    fPlaneListFromDetectorMap[detector].push_back(padPlane) ;
    fPlaneIDFromPlaneMap[padPlane]  = 0 ;

    fPadDetectorMap[detector].push_back(padSizeX) ;
    fPadDetectorMap[detector].push_back(padSizeY) ;
    fPadDetectorMap[detector].push_back(nbOfPadX) ;
    fPadDetectorMap[detector].push_back(nbOfPadY) ;
    fPadDetectorMap[detector].push_back(nbOfConnectors) ; 
}

//======================================================================================================================================
void  GEMMapping::SetAPVMap(TString detPlane, Int_t fecId, Int_t adcCh, Int_t apvNo, Int_t apvOrient, Int_t apvIndex, Int_t apvHdr, TString defaultAPV) {
    Int_t apvID = (fecId << 4) | adcCh ;
    printf("   GEMMapping::SetAPVMap()=> apv[%d]: apvId=%d, fecID=%d, adcCh=%d, plane=%s, orient=%d, index=%d, hdr=%d,  status=%s \n", apvNo, apvID, fecId, adcCh, detPlane.Data(), apvOrient, apvIndex, apvHdr, defaultAPV.Data()) ;
    TString apvName = GetAPV(detPlane, fecId, adcCh, apvNo, apvIndex) ;
    fAPVNoFromIDMap[apvID]           = apvNo ;
    fAPVIDFromAPVNoMap[apvNo]        = apvID ;
    fAPVFromIDMap[apvID]             = apvName ;
    fAPVstatusMap[apvID]             = defaultAPV ;
    fAPVHeaderLevelFromIDMap[apvID]  = apvHdr ;
    fAPVOrientationFromIDMap[apvID]  = apvOrient ;
    fAPVIndexOnPlaneFromIDMap[apvID] = apvIndex ;
    fAPVIDFromNameMap[apvName] = apvID ;
    fPlaneFromAPVIDMap[apvID]  = detPlane ;
    fAPVIDListFromFECIDMap[fecId].push_back(apvID);
    fFECIDListFromPlaneMap[detPlane].push_back(fecId);
    fAPVIDListFromPlaneMap[detPlane].push_back(apvID);
    fADCchannelsFromFECMap[fecId].push_back(adcCh) ;
    TString detector = GetDetectorFromPlane(detPlane) ;
    fAPVIDListFromDetectorMap[detector].push_back(apvID);
}

//======================================================================================================================================
void  GEMMapping::SetAPVtoPadMapping(Int_t fecId, Int_t adcCh, Int_t padId, Int_t apvCh) {
    Int_t apvID      = (fecId << 4) | adcCh ;
    Int_t apvChPadCh = (padId << 8) | apvCh ;
    fAPVToPadChannelMap[apvID].push_back(apvChPadCh) ;
}

//============================================================================================
void GEMMapping::LoadAPVtoPadMapping(const char * mappingCfgFilename) {
    //  Clear() ;
    printf("  GEMMapping::LoadAPVtoPadMapping() ==> Loading Mapping from %s \n", mappingCfgFilename) ;
    ifstream filestream (mappingCfgFilename, ifstream::in);
    TString line;
    while (line.ReadLine(filestream)) {

	line.Remove(TString::kLeading, ' ');   // strip leading spaces
	if (line.BeginsWith("#")) continue ;   // and skip comments
	//    printf("   GEMMapping::LoadAPVtoPadMapping ==> Scanning the mapping cfg file %s\n",line.Data()) ;

	//=== Create an array of the tokens separated by "," in the line;
	TObjArray * tokens = line.Tokenize(",");

	//=== iterator on the tokens array
	TIter myiter(tokens);
	while (TObjString * st = (TObjString*) myiter.Next()) {

	    //== Remove leading and trailer spaces
	    TString s = st->GetString().Remove(TString::kLeading, ' ' );
	    s.Remove(TString::kTrailing, ' ' );
	    if(s == "PAD") {
		Int_t apvCh = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		Int_t padId = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )) .Atoi();;
		Int_t fecId = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		Int_t adcCh = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		SetAPVtoPadMapping(fecId, adcCh, padId, apvCh) ;
	    }
	}
	tokens->Delete();
    }
    printf("\n  ======================================================================================================================\n") ;
}

//======================================================================================================================================
void GEMMapping::PrintMapping() {
    map<TString, list<TString> >::const_iterator det_itr ;
    for(det_itr = fPlaneListFromDetectorMap.begin(); det_itr != fPlaneListFromDetectorMap.end(); ++det_itr) {
	TString detector = det_itr->first ;
	printf("  ======================================================================================================================\n") ;
	printf("   GEMMapping::PrintMapping() ==> Detector = %s \n",detector.Data()) ;
	list<TString> detPlaneList = det_itr->second ;
	list<TString>::const_iterator plane_itr ;
	for(plane_itr = detPlaneList.begin(); plane_itr != detPlaneList.end(); ++plane_itr) {
	    TString detPlane = * plane_itr ;
	    list <Int_t> fecList = GetFECIDListFromPlane(detPlane) ;
	    list<Int_t>::const_iterator fec_itr ;
	    for(fec_itr = fecList.begin(); fec_itr != fecList.end(); ++fec_itr) { 
		Int_t fecId = * fec_itr ;
		printf("   GEMMapping::PrintMapping() ==> Plane=%s, FEC=%d \n", detPlane.Data(), fecId) ;
		list <Int_t> apvList = GetAPVIDListFromPlane(detPlane) ;
		list<Int_t>::const_iterator apv_itr ;
		for(apv_itr = apvList.begin(); apv_itr != apvList.end(); ++apv_itr) { 
		    Int_t apvID       = * apv_itr ;
		    Int_t apvNo       = GetAPVNoFromID(apvID);
		    Int_t apvIndex    = GetAPVIndexOnPlane(apvID);
		    Int_t apvOrient   = GetAPVOrientation(apvID);
		    Int_t fecID       = GetFECIDFromAPVID(apvID);
		    Int_t adcCh       = GetADCChannelFromAPVID(apvID);
		    Int_t apvHdrLevel = GetAPVHeaderLevelFromID(apvID);
		    TString  apvName  = GetAPVFromID(apvID) ;
		    TString defautAPV = GetAPVstatus(apvID);
		    if(fecID == fecId) printf("   GEMMapping::PrintMapping() ==> adcCh=%d, apv=%s, apvID=%d, apvNo=%d, index=%d, orient=%d, hdr=%d, status=%s\n", adcCh, apvName.Data(), apvID, apvNo, apvIndex, apvOrient, apvHdrLevel, defautAPV.Data()) ;
		}
	    }
	    printf("\n") ;
	} 
    }
    printf("======================================================================================================================\n") ;
    printf("   GEMMapping::PrintMapping() ==> Mapping of %d detectors, %d planes, %d FECs, %d APVs\n", GetNbOfDetectors(), GetNbOfPlane(), GetNbOfFECs(), GetNbOfAPVs());
    printf("======================================================================================================================\n") ;
}

//======================================================================================================================================
void GEMMapping::SaveMapping(const char * file) {
    printf("   GEMMapping::SaveMapping() ==> Saving PRD Mapping to file [%s],\n", file) ;
    FILE * f = fopen(file, "w");  
    fprintf(f,"#################################################################################################\n") ;
    fprintf(f,"         readoutType  Detector    Plane  DetNo   Plane   size (mm)  connectors  orientation\n");
    fprintf(f,"#################################################################################################\n") ;
    map<TString, list<TString> >::const_iterator det_itr ;
    for(det_itr = fPlaneListFromDetectorMap.begin(); det_itr != fPlaneListFromDetectorMap.end(); ++det_itr) {
	TString detector      = det_itr->first ;
	TString readoutBoard = GetReadoutBoardFromDetector(detector) ;
	TString detectorType = GetDetectorTypeFromDetector(detector) ;
	if ( (readoutBoard == "CARTESIAN") || (readoutBoard == "UV_ANGLE_OLD") ){
	    list<TString> detPlaneList = det_itr->second ;
	    TString planeX    = detPlaneList.front() ;
	    vector <Float_t> cartesianPlaneX = GetCartesianReadoutMap(planeX) ;
	    Float_t sizeX     = cartesianPlaneX[1] ;
	    Int_t connectorsX = (Int_t) (cartesianPlaneX[2]) ;
	    Int_t orientX     = (Int_t) (cartesianPlaneX[3]) ;

	    TString planeY    = detPlaneList.back() ;
	    vector <Float_t> cartesianPlaneY = GetCartesianReadoutMap(planeY) ;
	    Float_t sizeY     = cartesianPlaneY[1] ;
	    Int_t connectorsY = (Int_t) (cartesianPlaneY[2]) ;
	    Int_t orientY     = (Int_t) (cartesianPlaneY[3]) ;
	    fprintf(f,"DET,  %s,   %s,   %s,   %s,  %f,   %d,   %d,   %s,   %f,   %d,   %d \n", readoutBoard.Data(), detectorType.Data(), detector.Data(), planeX.Data(), sizeX, connectorsX, orientX, planeY.Data(), sizeY, connectorsY, orientY ) ;
	}
	else {
	    printf("   GEMMapping::SaveMapping() ==> detector readout board type %s is not yet implemented ==> PLEASE MOVE ON \n", readoutBoard.Data()) ;
	    continue ;
	}
    }

    fprintf(f,"###############################################################\n") ;
    fprintf(f,"#     fecId   adcCh   detPlane  apvOrient  apvIndex    apvHdr #\n");
    fprintf(f,"###############################################################\n") ;
    map<Int_t, TString>::const_iterator apv_itr;
    for(apv_itr = fAPVFromIDMap.begin(); apv_itr != fAPVFromIDMap.end(); ++apv_itr){
	Int_t apvID       = apv_itr->first;
	Int_t fecId       = GetFECIDFromAPVID(apvID);
	Int_t adcCh       = GetADCChannelFromAPVID(apvID);
	TString detPlane  = GetPlaneFromAPVID(apvID) ;
	Int_t apvOrient   = GetAPVOrientation(apvID);
	Int_t apvIndex    = GetAPVIndexOnPlane(apvID);
	Int_t apvHdr      = GetAPVHeaderLevelFromID(apvID);
	TString defautAPV = GetAPVstatus(apvID);
	fprintf(f,"APV,   %d,     %d,     %s,     %d,    %d,   %d,  %s, \n", fecId, adcCh, detPlane.Data(), apvOrient, apvIndex, apvHdr, defautAPV.Data());
    }
    fclose(f);
}

//============================================================================================
void GEMMapping::LoadMapping(const char * mappingCfgFilename) {

    Clear() ;
    printf("   GEMMapping::LoadDefaultMapping() ==> Loading Mapping from %s \n", mappingCfgFilename) ;
    Int_t apvNo = 0 ;
    Int_t detID = 0 ;
    ifstream filestream (mappingCfgFilename, ifstream::in); 
    TString line;
    while (line.ReadLine(filestream)) {

	line.Remove(TString::kLeading, ' ');   // strip leading spaces
	if (line.BeginsWith("#")) continue ;   // and skip comments
	//printf("   GEMMapping::LoadDefaultMapping() ==> Scanning the mapping cfg file %s\n",line.Data()) ;

	//=== Create an array of the tokens separated by "," in the line;
	TObjArray * tokens = line.Tokenize(","); 

	//=== iterator on the tokens array
	TIter myiter(tokens); 
	while (TObjString * st = (TObjString*) myiter.Next()) {

	    //== Remove leading and trailer spaces
	    TString s = st->GetString().Remove(TString::kLeading, ' ' );
	    s.Remove(TString::kTrailing, ' ' );                         

	    //      printf("    GEMMapping::LoadDefaultMapping() ==> Data ==> %s\n",s.Data()) ;
	    if(s == "DET") {
		TString readoutBoard = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		TString detectorType = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		TString detector     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		//	printf("    GEMMapping::LoadDefaultMapping() ==> Data ==> %s\n",s.Data()) ;

		if (readoutBoard == "CARTESIAN")  {
		    TString planeX           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Float_t sizeX            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Int_t   nbOfConnectorsX  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Int_t   orientationX     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();

		    TString planeY           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Float_t sizeY            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Int_t   nbOfConnectorsY  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Int_t   orientationY     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    SetCartesianStripsReadoutMap(readoutBoard, detectorType, detector, detID, planeX, sizeX, nbOfConnectorsX, orientationX, planeY, sizeY, nbOfConnectorsY, orientationY) ;
		}

		else if (readoutBoard == "1DSTRIPS")  {
		    TString plane           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Float_t size            = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Int_t   nbOfConnectors  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Int_t   orientation     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Set1DStripsReadoutMap(readoutBoard, detectorType, detector, detID, plane, size, nbOfConnectors, orientation);
		}

		else if (readoutBoard == "UV_ANGLE") {
		    Float_t length           = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t outerRadius      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t innerRadius      = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();

		    TString planeTop           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Int_t   nbOfConnectorsTop  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Int_t   orientationTop     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();

		    TString planeBot           = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Int_t   nbOfConnectorsBot  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    Int_t   orientationBot     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();

		    SetUVStripsReadoutMap(readoutBoard, detectorType, detector, detID, length, innerRadius, outerRadius, planeTop, nbOfConnectorsTop, orientationTop, planeBot, nbOfConnectorsBot, orientationBot) ;
		}

		else if (readoutBoard == "PADPLANE") {
		    TString padPlane     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Float_t padSizeX     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t nbPadX       = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t padSizeY     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t nbPadY       = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t nbConnectors = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    SetPadsReadoutMap(readoutBoard, detectorType, detector, detID, padPlane, padSizeX, padSizeY, nbPadX, nbPadY, nbConnectors) ;
		}

		else if (readoutBoard == "CMSGEM") {
		    TString etaSector     = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		    Float_t etaSectorPos = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t etaSectorSize = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Float_t nbConnectors        = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atof();
		    Int_t orientation     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		    SetCMSGEMReadoutMap(readoutBoard, detectorType, detector, detID, etaSector, etaSectorPos, etaSectorSize, nbConnectors, orientation) ;
		}

		else {
		    printf("XXXXXXX GEMMapping::LoadDefaultMapping()==> detector with this readout board type %s is not yet implemented ==> PLEASE MOVE ON XXXXXXXXXXX \n", readoutBoard.Data()) ;
		    continue ;
		}
		detID++ ;
	    }

	    if(s == "APV") {
		Int_t   fecId     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		Int_t   adcCh     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		TString detPlane  = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		Int_t   apvOrient = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		Int_t   apvIndex  = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		Int_t   apvheader = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		TString defautAPV = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );

		if (detPlane == "NULL") continue ;
		SetAPVMap(detPlane, fecId, adcCh, apvNo, apvOrient, apvIndex, apvheader, defautAPV) ;
		apvNo++ ;
	    }
	    if(s == "FEC") {
		Int_t   fecId     = (((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' )).Atoi();
		TString ip = ((TObjString*) myiter.Next())->GetString().Remove(TString::kLeading, ' ' );
		fFECIPFromFECID[fecId] = ip;
		fFECIDFromFECIP[ip] = fecId;
	    }
	}
	tokens->Delete();
    }
    ComputeActiveADCchannelsMap() ;
}

//============================================================================================
void GEMMapping::Clear() {
    printf("   GEMMapping::Clear() ==> Clearing Previous Mapping \n") ;

    fAPVIDFromAPVNoMap.clear() ; 
    fAPVIDFromNameMap.clear() ; 
    fAPVIDListFromDetectorMap.clear() ; 
    fAPVIDListFromPlaneMap.clear() ; 
    fAPVNoFromIDMap.clear() ; 
    fAPVFromIDMap.clear() ; 
    fAPVstatusMap.clear() ; 
    fAPVHeaderLevelFromIDMap.clear() ;

    fPlaneIDFromPlaneMap.clear() ;
    fDetectorIDFromDetectorMap.clear() ; 
    fDetectorFromIDMap.clear() ; 
    fDetectorFromAPVIDMap.clear() ;  
    fDetectorFromPlaneMap.clear() ;

    fPlaneFromAPVIDMap.clear() ;
    fReadoutBoardFromIDMap.clear() ;
    fReadoutBoardFromDetectorMap.clear() ;
    fNbOfAPVsFromDetectorMap.clear() ;
    fAPVOrientationFromIDMap.clear() ;
    fAPVIndexOnPlaneFromIDMap.clear() ;
    //  fADCchannelsFromFEC.clear() ;
    printf("   GEMMapping::Clear() ==> Previous Mapping cleared \n") ;
}

//============================================================================================
template <typename M> void ClearMapOfList( M & amap ) {
    for ( typename M::iterator it = amap.begin(); it != amap.end(); ++it ) {
	((*it).second).clear();
    }
    amap.clear() ;
}

//============================================================================================
void GEMMapping::ComputeActiveADCchannelsMap() {
    printf("   GEMMapping::ComputeActiveADCchannelsMap() ==> ComputeActiveADCchannelsMap  \n") ;
    fActiveADCchannelsMap.clear() ;
    map <Int_t, Int_t >::const_iterator adcChannel_itr ;
    for(adcChannel_itr = fAPVNoFromIDMap.begin(); adcChannel_itr != fAPVNoFromIDMap.end(); ++adcChannel_itr) {
	Int_t apvid = (* adcChannel_itr).first ;
	Int_t activeChannel = apvid & 0xF;
	Int_t fecId = (apvid >> 4 ) & 0xF;
	fActiveADCchannelsMap[fecId].push_back(activeChannel) ;
    }
}
//=====================================================
Int_t GEMMapping::CMSStripMapping(Int_t chNo) { 
    if((chNo%2)==1){
	chNo= 127 -((chNo-1)/2);
    }
    else{
	chNo = (chNo/2);
    }
    return chNo ;
}

//=====================================================
Int_t GEMMapping::StandardMapping(Int_t chNo) { 
    return chNo ;
}

//=====================================================
Int_t GEMMapping::EICStripMapping(Int_t chNo) { 
    if(chNo % 2 == 0) chNo = chNo / 2 ;
    else              chNo = 64 + (chNo - 1) / 2 ;
    return chNo ;
}

//=====================================================
Int_t GEMMapping::HMSStripMapping(Int_t chNo) { 
    if(chNo % 4 == 0)      chNo = chNo + 2 ;
    else if(chNo % 4 == 1) chNo = chNo - 1 ;
    else if(chNo % 4 == 2) chNo = chNo + 1 ;
    else if(chNo % 4 == 3) chNo = chNo - 2 ;
    else chNo = chNo ;
    return chNo ;
}

//=====================================================
Int_t GEMMapping::PRadStripMapping(Int_t apvID, Int_t chNo) {
    printf("   GEMMapping::GetPRadStripMapping(() ==> APVId=%d \n",apvID ) ;
    //------------ APV25 Internal Channel Mapping
    chNo = (32 * (chNo%4)) + (8 * (Int_t)(chNo/4)) - (31 * (Int_t)(chNo/16)) ;

    //------------ APV25 Channel to readout strip Mapping
    if ( (GetPlaneFromAPVID(apvID).Contains("X")) && (GetAPVIndexOnPlane(apvID)  == 11) ) {
	if (chNo % 2 == 0) 
	    chNo = ( chNo / 2) + 48 ;
	else
	    if (chNo < 96) 
		chNo = (95 - chNo) / 2 ;
	    else
		chNo = 127 + (97 - chNo) / 2 ;
    }
    else { // NON (fDetectorType == "PRADGEM") && (fPlane.Contains("Y")) && (fAPVIndex == 11)
	if (chNo % 2 == 0) 
	    chNo = ( chNo / 2) + 32 ;
	else 
	    if (chNo < 64) 
		chNo = (63 - chNo) / 2 ;
	    else
		chNo = 127 + (65 - chNo) / 2 ;
    }
    //   printf("PRDPedestal::PRadStripsFMapping ==>  APVID=%d, chNo=%d, stripNo=%d, \n",fAPVID, chno, chNo) ;
    return chNo ;
}

//=====================================================
Int_t GEMMapping::GetStripMapping(Int_t apvID, Int_t chNo) {
    chNo = APVchannelCorrection(chNo) ;
    TString detectorType = fDetectorTypeFromDetectorMap[fDetectorFromAPVIDMap[apvID]] ;
    if (detectorType == "CMSGEM")         return CMSStripMapping(chNo) ;
    else if (detectorType == "EICPROTO1") return EICStripMapping(chNo) ;
    else if (detectorType == "HMSGEM")    return HMSStripMapping(chNo) ;
    else if (detectorType == "PRADGEM")   return PRadStripMapping(apvID, chNo) ;
    else                                  return StandardMapping(chNo) ;
}

//=====================================================
Int_t  GEMMapping::APVchannelCorrection(Int_t chNo) { 
    return (32 * (chNo%4)) + (8 * (Int_t)(chNo/4)) - (31 * (Int_t)(chNo/16)) ;
}

//======================================================================================
// Get Bank IDs 
set<int> GEMMapping::GetBankIDSet(){
    set<int> fec;
    fec.clear();
    map<Int_t, list<Int_t> >::const_iterator it;
    for(it=fAPVIDListFromFECIDMap.begin(); it!=fAPVIDListFromFECIDMap.end(); ++it)
    {
	fec.insert(it->first);
    }
    return fec;
}

TString GEMMapping::GetFECIPFromFECID(Int_t id)
{
    return fFECIPFromFECID[id];
}

Int_t GEMMapping::GetFECIDFromFECIP(TString ip)
{
    string str1 = (const char *)ip;

    str1.erase( remove(str1.begin(), str1.end(), ' '), str1.end() );
    map<TString, Int_t>::iterator it = fFECIDFromFECIP.begin();
    for(;it!=fFECIDFromFECIP.end();++it)
    {
	string str2 = (const char *) it->first;
	str2.erase( remove(str2.begin(), str2.end(), ' '), str2.end() );
	if( str1.compare(str2) == 0 ) 
	{
	    return it->second;
	}
    }
    return -1;
}

//=====================================================
Int_t  GEMMapping::GetPlaneID(TString planeName) {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Int_t planeIDorEtaSector = (Int_t) (fCartesianPlaneMap[planeName])[0] ;
    if(readoutType == "1DSTRIPS") planeIDorEtaSector = (Int_t) (f1DStripsPlaneMap[planeName])[0];
    if(readoutType == "CMSGEM")   planeIDorEtaSector = (Int_t) (fCMSGEMDetectorMap[planeName])[0];
    if(readoutType == "UV_ANGLE") planeIDorEtaSector = (Int_t) (fUVangleReadoutMap[planeName])[0] ;
    return planeIDorEtaSector ;
}

//=====================================================
Int_t  GEMMapping::GetEtaSector(TString planeName) {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Int_t planeIDorEtaSector = (Int_t) (fCartesianPlaneMap[planeName])[0] ;
    if(readoutType == "1DSTRIPS") planeIDorEtaSector = (Int_t) (f1DStripsPlaneMap[planeName])[0];
    if(readoutType == "CMSGEM")   planeIDorEtaSector = (Int_t) (fCMSGEMDetectorMap[planeName])[0];
    if(readoutType == "UV_ANGLE") planeIDorEtaSector = (Int_t) (fUVangleReadoutMap[planeName])[0] ;
    return planeIDorEtaSector ;
}

//=====================================================
Float_t  GEMMapping::GetPlaneSize(TString planeName) {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Float_t planeSize   = (fCartesianPlaneMap[planeName])[1] ;
    if(readoutType == "1DSTRIPS") planeSize = (f1DStripsPlaneMap[planeName])[1] ;
    if(readoutType == "CMSGEM")   planeSize = (fCMSGEMDetectorMap[planeName])[1];
    if(readoutType == "UV_ANGLE") planeSize = (fUVangleReadoutMap[planeName])[3] ;
    return planeSize ;
}

//=====================================================
Int_t GEMMapping::GetNbOfAPVsOnPlane(TString planeName)  {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName)) ;
    Int_t nbOfAPVs = (Int_t) (fCartesianPlaneMap[planeName])[2] ;
    if(readoutType == "1DSTRIPS") nbOfAPVs = (Int_t) (f1DStripsPlaneMap[planeName])[2];
    if(readoutType == "CMSGEM")   nbOfAPVs = (Int_t) (fCMSGEMDetectorMap[planeName])[2];
    if(readoutType == "UV_ANGLE") nbOfAPVs =  (Int_t) (fUVangleReadoutMap[planeName])[1] ;
    return nbOfAPVs ;
}

//=====================================================
Int_t GEMMapping::GetPlaneOrientation(TString planeName)  {
    TString readoutType = GetReadoutBoardFromDetector(GetDetectorFromPlane(planeName))  ;
    Int_t orient = (Int_t) (fCartesianPlaneMap[planeName])[3] ;
    if(readoutType == "1DSTRIPS") orient = (Int_t) (f1DStripsPlaneMap[planeName])[3];
    if(readoutType == "CMSGEM")   orient = (Int_t) (fCMSGEMDetectorMap[planeName])[3];
    if(readoutType == "UV_ANGLE") orient = (Int_t) (fUVangleReadoutMap[planeName])[2];
    return orient ;
} 
