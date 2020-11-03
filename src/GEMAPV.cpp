#include "GEMAPV.h"

#include <TCanvas.h>

#include <QRectF>
#include <QPen>
#include <QBrush>
#include <QTextOption>

#include <iostream>
#include <cassert>

GEMAPV::GEMAPV() 
{
}

GEMAPV::GEMAPV(int id, double x, double y, double width, double height, QColor color) : QGraphicsRectItem(x, y, width, height), 
    fModuleID(id), fDefaultColor(color), fCurrentColor(Qt::white), fPlane(0), fFECID(0), fIndex(0), fIsActive(false)
{
    fHistContainer = NULL;
    assert(fDefaultColor != Qt::black &&  "black is used to highlight setected APV, please use other color for the default value.");
    //this is a pretty dumb way of adding text, but I am running out of time
    if (id < 24){
	fTextBox = new QRectF(x-2*width, y, 2*width, height);
	fIndex = id;
    }else if (id < 36 && id>= 24){
	fIndex = id -24;
	if ((id-24) < 6 || id == 35){
	    fTextBox = new QRectF(x, y-2*height, width, 2*height);
	}else{
	    fTextBox = new QRectF(x, y+height, width, 2*height);			
	}  
    }else if (id >= 36 && id < 60){
	fIndex = id - 36;
	fTextBox = new QRectF(x+1.5*width, y, 1.5*width, height);
    }else{
	fIndex = id -60;
	if ((id-60)<6 || id == 71){
	    fTextBox = new QRectF(x, y-2*height, width, 2*height);
	}else{
	    fTextBox = new QRectF(x, y+height, width, 2*height);
	}  
    }
    setAcceptHoverEvents(true);
    fFont = QFont("times", 10);
    fTransparent = 0;
}

GEMAPV::~GEMAPV() {}

//========================================================
void GEMAPV::connectCanvas(GEMHistContainer *c1) {
    fIsActive = true;
    SetBasicInfo( c1->GetPlane(), c1->GetFECID(), c1->GetAPVID());
    fHistContainer = c1;
    fCurrentColor = fDefaultColor;
    this->update();
}

//=====================================================================
QRectF GEMAPV::boundingRect() const {
    return this->rect();
}

//=====================================================================
void GEMAPV::turnOnModuleColor(bool on) {
    if (fIsActive) {
	if (on == true) {
	    fCurrentColor = Qt::black;
	    this->update();
	}
	else  {
	    fCurrentColor = fDefaultColor;
	    this->update();
	}
    }
}

//=====================================================================
void GEMAPV::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget) {
    QRectF rect = boundingRect();
    QPen Pen(Qt::black, 1);
    painter->setPen(Pen);

    painter->fillRect(rect, fCurrentColor);
    painter->drawRect(rect);

    painter->setPen(Qt::black);
    painter->setFont(fFont);
    if (fIsActive){
	fStringID = QString::number(fIndex);
	painter->drawText(*fTextBox, fStringID, QTextOption(Qt::AlignCenter));
    }
}

//==============================================================
void GEMAPV::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (fIsActive){
	turnOnModuleColor(true);
	TCanvas *fCanvas = fHistContainer->GetCanvas();
	//    fCanvas->SetFillColor(5);
	fCanvas->Modified();
	fCanvas->Update();
	QGraphicsRectItem::hoverEnterEvent(event);
    }
}

//==============================================================
void GEMAPV::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if (fIsActive){
	turnOnModuleColor(false);
	TCanvas *fCanvas = fHistContainer->GetCanvas();
	//    fCanvas->SetFillColor(10);
	fCanvas->Modified();
	fCanvas->Update();
	QGraphicsRectItem::hoverLeaveEvent(event);
    }
}

//=============================================================
void GEMAPV::mousePressEvent(QGraphicsSceneMouseEvent * event) {
    if (fIsActive){
	GEMView::PrintTextOnMessager(Form("the %d APV channel of FEC %d (APV %d on detector plane %s)", fAPVID, fFECID, fIndex, fPlane.Data()),0);
	QGraphicsRectItem::mousePressEvent(event);
    }
}

//==============================================================
void GEMAPV::SetBasicInfo(TString plane, int fec, int apv_id) {
    fPlane = plane;
    fFECID = fec;
    fAPVID = apv_id;
}
