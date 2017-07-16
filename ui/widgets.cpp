
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>

#include "widgets.h"

CheckBox::CheckBox(const QString &text, QWidget *parent) :
        QCheckBox(text, parent)
{
    setFont(QFont("Helvetica", 10));
}

void CheckBox::stateChanged(int state)
{

}

FileDialog::FileDialog(QWidget *parent) : QFileDialog(parent)
{

}

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{

}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    this->chests =         new CheckBox("Shuffle Chests", this);
//    this->towns =          new CheckBox("Randomize Towns", this);
    this->shops =          new CheckBox("Randomize Weapon Shops", this);
    this->death_necklace = new CheckBox("Enable Death Necklace", this);
    this->speed_hacks =    new CheckBox("Enable Speed Hacks", this);
    this->growth =         new CheckBox("Randomize Growth", this);
    this->spells =         new CheckBox("Randomize Spell Learning", this);
    this->attack =         new CheckBox("Randomize Enemy Attacks", this);
    this->zones =          new CheckBox("Randomize Zones", this);
    this->music_shuffle =  new CheckBox("Shuffle Music", this);
    this->music_disable =  new CheckBox("Disable Music", this);
    this->copy_checksum =  new CheckBox("Copy Checksum to Clipboard", this);

    QVBoxLayout *vbox = new QVBoxLayout();
    QGridLayout *grid = new QGridLayout();
    vbox->addLayout(grid);
    grid->addWidget(chests,         0, 0, 0);
//    grid->addWidget(towns,          1, 0, 0);
    grid->addWidget(shops,          2, 0, 0);
    grid->addWidget(zones,          3, 0, 0);

    grid->addWidget(growth,         0, 1, 0);
    grid->addWidget(spells,         1, 1, 0);
    grid->addWidget(attack,         2, 1, 0);
    grid->addWidget(music_shuffle,  3, 1, 0);

    grid->addWidget(death_necklace, 0, 2, 0);
    grid->addWidget(speed_hacks,    1, 2, 0);
    grid->addWidget(copy_checksum,  2, 2, 0);
    grid->addWidget(music_disable,  3, 2, 0);
    this->setLayout(vbox);
}

