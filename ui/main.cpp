/*
 * Created by mcgrew
 */

#include "dwr.h"
#include "widgets.h"
#include "main-window.h"


int main(int argc, char *argv[])
{

    QApplication dwr_ui(argc, argv);
    dwr_ui.setFont(QFont("Arial", 11));

    MainWindow window;

    window.setWindowTitle("DWRandomizer " VERSION);
    window.show();

    return dwr_ui.exec();
}
