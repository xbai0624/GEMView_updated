////////////////////////////////////////////////////////////////////////////////
//
//  Embed a ROOT TCanvas into a QT widget, following the instructions in:
//  https://root-forum.cern.ch/t/root-canvas-in-qt5-revisited/34485/11
//
////////////////////////////////////////////////////////////////////////////////

#ifndef QROOTCANVAS_H
#define QROOTCANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTimer>

#include <TCanvas.h>

class QRootCanvas : public QWidget
{
    Q_OBJECT

public:
    QRootCanvas(QWidget *parent = 0);
    virtual ~QRootCanvas(){};
    void Refresh() {fCanvas->Modified(); fCanvas->Update();}
    TCanvas* GetCanvas() {return fCanvas;}

protected:
    TCanvas *fCanvas;

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent* e);
};

#endif
