//
// Created by mcgrew on 7/17/17.
//

#ifndef DWRANDOMIZER_MAIN_WINDOW_H
#define DWRANDOMIZER_MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>

#include "widgets.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

struct configuration {
    char input_file[1024];
    char output_dir[1024];
    char flags[64];
};

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
    void initStatus();
    QString getOptions();
    void setOptions(QString flags);
    QString getFlags();
    void setFlags(QString flags);
    bool saveConfig();
    bool loadConfig();
    void addOption(char flag, QString text, int x, int y);

private:
    QWidget *mainWidget;
    FileEntry *romFile;
    DirEntry *outputDir;
    SeedEntry *seed;
    FlagEntry *flags;
    QList<CheckBox*> options;
    LevelComboBox *levelSpeed;
    QComboBox *spriteSelect;
    QPushButton *goButton;
    QGridLayout *optionGrid;
};


#endif //DWRANDOMIZER_MAIN_WINDOW_H
