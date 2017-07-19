//
// Created by mcgrew on 7/17/17.
//

#ifndef DWRANDOMIZER_MAIN_WINDOW_H
#define DWRANDOMIZER_MAIN_WINDOW_H

#include "widgets.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

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
    void handleComboBox(int index);
    void handleFlags();
    void handleButton();

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
    DirEntry *outputDir;
    SeedEntry *seed;
    FlagEntry *flags;
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
    LevelComboBox *levelSpeed;
    QPushButton *goButton;
};


#endif //DWRANDOMIZER_MAIN_WINDOW_H
