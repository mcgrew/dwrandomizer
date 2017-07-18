//
// Created by mcgrew on 7/17/17.
//

#ifndef DWRANDOMIZER_MAIN_WINDOW_H
#define DWRANDOMIZER_MAIN_WINDOW_H

#include "widgets.h"
#include <QtWidgets/QMainWindow>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    typedef QWidget super;
    MainWindow(QWidget *parent = 0);

private slots:
    void handleCheckBox();
    void handleFlags();

private:
    void layout();
    void initSlots();
    void initWidgets();
    std::string getOptions();
    void setOptions(std::string &flags);
    std::string getFlags();
    void setFlags(std::string &flags);

private:
    QWidget *mainWidget;
    FileEntry *romFile;
    TextBox *outputDir;
    TextBox *seed;
    TextBox *flags;
    CheckBox *chests;
    CheckBox *shops;
    CheckBox *deathNecklace;
    CheckBox *speedHacks;
    CheckBox *growth;
    CheckBox *spells;
    CheckBox *attack;
    CheckBox *zones;
    CheckBox *musicShuffle;
    CheckBox *musicDisable;
    CheckBox *copyChecksum;
};


#endif //DWRANDOMIZER_MAIN_WINDOW_H
