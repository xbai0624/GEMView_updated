#ifndef GEMGRAPHICSSCENE_H
#define GEMGRAPHICSSCENE_H

#include "TPaveText.h"
#include "GEMAPV.h"
#include "hardcode.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QGraphicsTextItem>
#include <vector>
#include "GEMMapping.h"
using namespace std;

class GEMAPV;
class GEMGraphicsScene : public QWidget
{
   //Q_OBJECT

 public:
  GEMGraphicsScene(int width, int height);
  ~GEMGraphicsScene();
  GEMAPV *getGEMAPV(int n);
  void drawGEMAPV();
	void DrawGEMHit(vector<float> *x1, vector<float> *y1, vector<float> *x2, vector<float> *y2);
	void RemoveGEMHit();
	void AccumulateHits(bool is ) { fAccumulateHits = is; }
	bool IsAccumulating() const { return fAccumulateHits; }
	void GEMToLabFrame(float x, float y, float *labX, float *labY, int type);
	
	protected:
  void wheelEvent(QWheelEvent *event);
 
 private:
  void drawGEMFrame();
  void drawGEMCoordinate();
  void calcAPVLocation(int i, double &x, double &y, double &width, double &height);
   //GEMAPV *getGEMAPV(int n);

  GEMMapping *fMapping;
  double fYGEMSize;
  double fXGEMSize;
  double fHeight;
  double fWidth;
  double fYmargin;
  double fXmargin;
  double fXinterval;
  double fYinterval;
  double fXScaletoGEM;
  double fYScaletoGEM;
  double fHitRadius;
  int    fNline;
  bool   fAccumulateHits;
  QHBoxLayout    *fGEMLayout;
  QGraphicsScene *fGEMScene;
  QGraphicsView  *fGEMView;
  vector<GEMAPV*> fAPV;
  QGraphicsTextItem *fText[4];
  std::vector<QGraphicsItem*> fHitStorageVector;
  
};



#endif
