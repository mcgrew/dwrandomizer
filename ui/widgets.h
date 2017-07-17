
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMainWindow>

#ifndef DWRANDOMIZER_WIDGETS_H
#define DWRANDOMIZER_WIDGETS_H

class CheckBox : public QCheckBox {

public:
    typedef QCheckBox super;
    CheckBox(const char flag, const QString &text, QWidget *parent = 0);
    void stateChanged(int state);
    char getFlag();
    bool update(std::string &flags);

private:
    char flag;

};

class FileDialog : public QFileDialog {

public:
    typedef QFileDialog super;
    FileDialog(QWidget *parent = 0);

};

class TextBox : public QLineEdit {

public:
    typedef QLineEdit super;
    TextBox(QWidget *parent = 0);
};

class Button : public QPushButton {

public:
    typedef QPushButton super;
    Button(QWidget *parent = 0);

};

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
    TextBox *romFile;
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif //DWRANDOMIZER_WIDGETS_H
