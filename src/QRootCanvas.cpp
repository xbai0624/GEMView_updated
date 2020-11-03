#include "QRootCanvas.h"

#include <iostream>


////////////////////////////////////////////////////////////////////////////////
// QRootCanvas Constructor

QRootCanvas::QRootCanvas(QWidget *parent) : QWidget(parent, 0), fCanvas(nullptr)
{
    // set options needed to properly update the canvas when resizing the widget
    // and to properly handle context menus and mouse move events
    setAttribute(Qt::WA_PaintOnScreen, false);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NativeWindow, true);
    setUpdatesEnabled(kFALSE);
    setMouseTracking(kTRUE);
    setMinimumSize(60, 60);

    // register the QWidget in TVirtualX, giving its native window id
    int wid = gVirtualX->AddWindow((ULong_t)winId(), width(), height());
    // create the ROOT TCanvas, giving as argument the QWidget registered id
    fCanvas = new TCanvas("Root Canvas", width(), height(), wid);

    //TQObject::Connect("TGPopupMenu", "PoppedDown()", "TCanvas", fCanvas, "Update()");
}


////////////////////////////////////////////////////////////////////////////////
// handle mouse move event

void QRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (fCanvas) {
	if (e->buttons() & Qt::LeftButton) {
	    fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
	} else if (e->buttons() & Qt::MidButton) {
	    fCanvas->HandleInput(kButton2Motion, e->x(), e->y());
	} else if (e->buttons() & Qt::RightButton) {
	    fCanvas->HandleInput(kButton3Motion, e->x(), e->y());
	} else {
	    fCanvas->HandleInput(kMouseMotion, e->x(), e->y());
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
// handle mouse press event

void QRootCanvas::mousePressEvent(QMouseEvent *e)
{
    if (fCanvas) {
	switch (e->button()) {
	    case Qt::LeftButton :
		fCanvas->HandleInput(kButton1Down, e->x(), e->y());
		break;
	    case Qt::MidButton :
		fCanvas->HandleInput(kButton2Down, e->x(), e->y());
		break;
	    case Qt::RightButton :
		// does not work properly on Linux...
		// ...adding setAttribute(Qt::WA_PaintOnScreen, true) 
		// seems to cure the problem
		fCanvas->HandleInput(kButton3Down, e->x(), e->y());
		break;
	    default:
		break;
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
// handle mouse release event

void QRootCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (fCanvas) {
	switch (e->button()) {
	    case Qt::LeftButton :
		fCanvas->HandleInput(kButton1Up, e->x(), e->y());
		break;
	    case Qt::MidButton :
		fCanvas->HandleInput(kButton2Up, e->x(), e->y());
		break;
	    case Qt::RightButton :
		// does not work properly on Linux...
		// ...adding setAttribute(Qt::WA_PaintOnScreen, true) 
		// seems to cure the problem
		fCanvas->HandleInput(kButton3Up, e->x(), e->y());
		break;
	    default:
		break;
	}
    }
}


////////////////////////////////////////////////////////////////////////////////
// handle resize event

void QRootCanvas::resizeEvent(QResizeEvent *e)
{
    if (fCanvas) {
	fCanvas->SetCanvasSize(e->size().width(), e->size().height());
	fCanvas->Resize();
	fCanvas->Update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// handle paint event

void QRootCanvas::paintEvent(QPaintEvent *)
{
    if (fCanvas) {
	fCanvas->Resize();
	fCanvas->Update();
    }
}
