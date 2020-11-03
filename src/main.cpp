#include "GEMView.h"

#include <QApplication>
#include <TApplication.h>

using namespace std;

int main(int argc, char* argv[])
{
    TApplication rootapp("gem view", &argc, argv);
    QApplication app(argc, argv);

    GEMView *gem_view = new GEMView();
    gem_view->show();

    QObject::connect(gem_view, SIGNAL(destroyed()), &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    return app.exec();
}
