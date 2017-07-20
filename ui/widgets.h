
#define __STDC_FORMAT_MACROS

#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <inttypes.h>


#ifndef DWRANDOMIZER_WIDGETS_H
#define DWRANDOMIZER_WIDGETS_H

#define NO_FLAG '~'

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

class CheckBox : public QCheckBox {

public:
    typedef QCheckBox super;
    CheckBox(const char flag, const QString &text, QWidget *parent = 0);
    void stateChanged(int state);
    char getFlag();
    bool updateState(std::string &flags);

private:
    char flag;

};

class LevelComboBox : public QComboBox {

public:
    LevelComboBox(QWidget *parent = 0);
    bool updateState(std::string &flags);
    char getFlag();

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
    Button(const QString &text, QWidget *parent = 0);

};

class ButtonEntry : public QWidget {
    Q_OBJECT
public:
    typedef QWidget super;
    ButtonEntry(QWidget *parent = 0);
    QString text();
    void setText(QString text);

signals:
    void textEdited(QString);

private slots:
    virtual void handleButton(){}
    virtual void handleEdit(QString text){}

protected:
    QLineEdit *textBox;
    Button *button;
};

class FileEntry : public ButtonEntry {
    Q_OBJECT
public:
    typedef ButtonEntry super;
    FileEntry(QWidget *parent = 0);
private slots:
    void handleButton();
};

class DirEntry : public ButtonEntry {
    Q_OBJECT

public:
    typedef ButtonEntry super;
    DirEntry(QWidget *parent = 0);

private slots:
    void handleButton();
};

class SeedEntry : public ButtonEntry {
    Q_OBJECT
public:
    typedef ButtonEntry super;
    SeedEntry(QWidget *parent = 0);
    uint64_t getSeed();

private slots:
    void handleButton();
};

class FlagEntry : public ButtonEntry {
    Q_OBJECT
public:
    typedef ButtonEntry super;
    FlagEntry(QWidget *parent = 0);

private slots:
    void handleButton();
    virtual void handleEdit(QString text);
};

#endif //DWRANDOMIZER_WIDGETS_H
