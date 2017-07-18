
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#ifndef DWRANDOMIZER_WIDGETS_H
#define DWRANDOMIZER_WIDGETS_H

#define NO_FLAG '\255'

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
    Button(const QString &text, QWidget *parent = 0);

};

class FileEntry : public QWidget {
    Q_OBJECT
public:
    typedef QWidget super;
    FileEntry(QWidget *parent = 0);

private slots:
     void handleButton();

private:
    TextBox *textBox;
    Button *button;
};

#endif //DWRANDOMIZER_WIDGETS_H
