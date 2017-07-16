/*
 * Created by mcgrew
 */


#include "dwr.h"
#include "widgets.h"


int main(int argc, char *argv[])
{

    QApplication dwr_ui(argc, argv);

    MainWindow window;

    window.setWindowTitle("DWRandomizer " VERSION);
    window.show();

    return dwr_ui.exec();
}
