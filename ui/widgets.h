
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>

#ifndef DWRANDOMIZER_WIDGETS_H
#define DWRANDOMIZER_WIDGETS_H

class CheckBox : public QCheckBox {

public:
    CheckBox(const QString &text, QWidget *parent = 0);
    void stateChanged(int state);

};

class FileDialog : public QFileDialog {

public:
    FileDialog(QWidget *parent = 0);

};

class TextBox : public QLineEdit {

public:
    TextBox(QWidget *parent = 0);

};

class MainWindow : public QWidget {

public:
    MainWindow(QWidget *parent = 0);

private:
    TextBox rom_file;
    TextBox output_dir;
    TextBox seed;
    TextBox flags;
    CheckBox *chests;
    CheckBox *towns;
    CheckBox *shops;
    CheckBox *death_necklace;
    CheckBox *speed_hacks;
    CheckBox *growth;
    CheckBox *spells;
    CheckBox *attack;
    CheckBox *zones;
    CheckBox *music_shuffle;
    CheckBox *music_disable;
    CheckBox *copy_checksum;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif //DWRANDOMIZER_WIDGETS_H
