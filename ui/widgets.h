
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>


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
    bool updateState(QString flags);

private:
    char flag;

};

class LevelComboBox : public QComboBox {

public:
    LevelComboBox(QWidget *parent = 0);
    bool updateState(QString flags);
    char getFlag();

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
    QPushButton *button;
    QLabel *label;
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
    void random();

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
