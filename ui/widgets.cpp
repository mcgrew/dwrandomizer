
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <iostream>

#include "widgets.h"

#define NO_FLAG '\255'

CheckBox::CheckBox(const char flag, const QString &text, QWidget *parent) :
        QCheckBox(text, parent)
{
    this->flag = flag;
    setFont(QFont("Helvetica", 10));
}

char CheckBox::getFlag()
{
    if (this->isChecked()) {
        return this->flag;
    }
    return NO_FLAG;
}

void CheckBox::stateChanged(int state)
{
    super::stateChanged(state);
}

bool CheckBox::update(std::string &flags)
{
    bool checked = flags.find(this->flag) != std::string::npos;
    this->setChecked(checked);
    return checked;
}

Button::Button(QWidget *parent) : QPushButton(parent)
{

}

FileDialog::FileDialog(QWidget *parent) : QFileDialog(parent)
{

}

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->mainWidget = new QWidget();
    this->setCentralWidget(this->mainWidget);

    this->initWidgets();
    this->layout();
    this->initSlots();
}

void MainWindow::initWidgets()
{
    flags = new TextBox(this);

    chests =        new CheckBox('C', "Shuffle Chests & Search Items", this);
    shops =         new CheckBox('W', "Randomize Weapon Shops", this);
    deathNecklace = new CheckBox('D', "Enable Death Necklace", this);
    speedHacks =    new CheckBox('H', "Enable Speed Hacks", this);
    growth =        new CheckBox('G', "Randomize Growth", this);
    spells =        new CheckBox('M', "Randomize Spell Learning", this);
    attack =        new CheckBox('P', "Randomize Enemy Attacks", this);
    zones =         new CheckBox('Z', "Randomize Zones", this);
    musicShuffle =  new CheckBox('K', "Shuffle Music", this);
    musicDisable =  new CheckBox('Q', "Disable Music", this);
    copyChecksum =  new CheckBox(NO_FLAG, "Copy Checksum to Clipboard", this);
}

void MainWindow::initSlots()
{
    connect(this->flags, SIGNAL(textEdited(QString)), this, SLOT(handleFlags()));

    connect(this->chests,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->shops,        SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->deathNecklace,SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->speedHacks,   SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->growth,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->spells,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->attack,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->zones,        SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->musicShuffle, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->musicDisable, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->copyChecksum, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
}

void MainWindow::layout()
{
    QVBoxLayout *vbox;
    QGridLayout *grid;

    vbox = new QVBoxLayout();
    grid = new QGridLayout();

    vbox->addWidget(this->flags);
    vbox->addLayout(grid);
    grid->addWidget(this->chests,         0, 0, 0);
    grid->addWidget(this->shops,          2, 0, 0);
    grid->addWidget(this->zones,          3, 0, 0);

    grid->addWidget(this->growth,         0, 1, 0);
    grid->addWidget(this->spells,         1, 1, 0);
    grid->addWidget(this->attack,         2, 1, 0);
    grid->addWidget(this->musicShuffle,  3, 1, 0);

    grid->addWidget(this->deathNecklace, 0, 2, 0);
    grid->addWidget(this->speedHacks,    1, 2, 0);
    grid->addWidget(this->copyChecksum,  2, 2, 0);
    grid->addWidget(this->musicDisable,  3, 2, 0);
    this->mainWidget->setLayout(vbox);
}

std::string MainWindow::getOptions()
{
    std::string flags = std::string() +
                        this->chests->getFlag() +
                        this->shops->getFlag() +
                        this->deathNecklace->getFlag() +
                        this->speedHacks->getFlag() +
                        this->growth->getFlag() +
                        this->spells->getFlag() +
                        this->attack->getFlag() +
                        this->zones->getFlag() +
                        this->musicShuffle->getFlag() +
                        this->musicDisable->getFlag() +
                        this->copyChecksum->getFlag();

    std::sort(flags.begin(), flags.end());
    std::replace(flags.begin(), flags.end(), NO_FLAG, '\0');
    return flags;
}

void MainWindow::setOptions(std::string &flags)
{
    this->chests->update(flags);
    this->shops->update(flags);
    this->deathNecklace->update(flags);
    this->speedHacks->update(flags);
    this->growth->update(flags);
    this->spells->update(flags);
    this->attack->update(flags);
    this->zones->update(flags);
    this->musicShuffle->update(flags);
    this->musicDisable->update(flags);
    this->copyChecksum->update(flags);
}

std::string MainWindow::getFlags()
{
    std::string flags = this->flags->text().toLatin1().constData();
    std::sort(flags.begin(), flags.end());
    return flags;
}

void MainWindow::setFlags(std::string &flags)
{
    this->flags->setText(QString::fromStdString(flags));
}

void MainWindow::handleCheckBox()
{
    std::string flags = this->getOptions();
    this->setFlags(flags);
}

void MainWindow::handleFlags()
{
    std::string flags = this->getFlags();
    this->setOptions(flags);
}
