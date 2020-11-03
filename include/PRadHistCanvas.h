
#ifndef PRAD_HIST_CANVAS_H
#define PRAD_HIST_CANVAS_H

#include <QWidget>
#include "QRootCanvas.h"

class QTimer;
class TObject;
class TCanvas;
class TF1;

class PRadHistCanvas : public QWidget
{
    Q_OBJECT

public:
    PRadHistCanvas( QWidget *parent = 0);
    virtual ~PRadHistCanvas() {}
    void UpdateHist(TObject *hist);

    TCanvas *GetCanvas(){return canvas1->GetCanvas();}
    void Refresh(){canvas1->Refresh();}

public slots:
    void rootInnerLoop();

protected:
    QRootCanvas *canvas1;
    QTimer *rootTimer;
};

#endif
