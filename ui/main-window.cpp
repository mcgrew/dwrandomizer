
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>

#include "dwr.h"
#include "main-window.h"

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
    this->romFile = new FileEntry(this);
    this->outputDir = new DirEntry(this);
    this->seed = new SeedEntry(this);
    this->flags = new FlagEntry(this);

    chests =        new CheckBox('C', "Shuffle Chests && Search Items", this);
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

    this->levelSpeed = new LevelComboBox(this);

    this->goButton = new QPushButton("Randomize!", this);
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

    connect(this->levelSpeed, SIGNAL(activated(int)),
            this, SLOT(handleCheckBox()));

    connect(this->goButton,     SIGNAL(clicked()), this, SLOT(handleButton()));
}

void MainWindow::layout()
{
    QVBoxLayout *vbox;
    QGridLayout *grid;

    vbox = new QVBoxLayout();
    grid = new QGridLayout();

    grid->addWidget(this->romFile,   0, 0, 0);
    grid->addWidget(this->outputDir, 0, 1, 0);
    grid->addWidget(this->seed,      1, 0, 0);
    grid->addWidget(this->flags,     1, 1, 0);

    vbox->addLayout(grid);
    grid = new QGridLayout();
    vbox->addLayout(grid);

    grid->addWidget(this->chests,        0, 0, 0);
    grid->addWidget(this->shops,         1, 0, 0);
    grid->addWidget(this->zones,         2, 0, 0);

    grid->addWidget(this->growth,        0, 1, 0);
    grid->addWidget(this->spells,        1, 1, 0);
    grid->addWidget(this->attack,        2, 1, 0);
    grid->addWidget(this->copyChecksum,  3, 1, 0);

    grid->addWidget(this->deathNecklace, 0, 2, 0);
    grid->addWidget(this->speedHacks,    1, 2, 0);
    grid->addWidget(this->musicShuffle,  2, 2, 0);
    grid->addWidget(this->musicDisable,  3, 2, 0);

    grid->addWidget(new QLabel("Leveling Speed", this), 6, 0, 0);
    grid->addWidget(this->levelSpeed,    7, 0, 0);

    grid->addWidget(this->goButton,      8, 2, 0);

    this->mainWidget->setLayout(vbox);

    this->copyChecksum->hide();
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
                        this->copyChecksum->getFlag() +

                        this->levelSpeed->getFlag();

    std::sort(flags.begin(), flags.end());
    std::replace(flags.begin(), flags.end(), NO_FLAG, '\0');
    return std::string(flags.c_str());
}

void MainWindow::setOptions(std::string &flags)
{
    this->chests->updateState(flags);
    this->shops->updateState(flags);
    this->deathNecklace->updateState(flags);
    this->speedHacks->updateState(flags);
    this->growth->updateState(flags);
    this->spells->updateState(flags);
    this->attack->updateState(flags);
    this->zones->updateState(flags);
    this->musicShuffle->updateState(flags);
    this->musicDisable->updateState(flags);
    this->copyChecksum->updateState(flags);

    this->levelSpeed->updateState(flags);
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

void MainWindow::handleComboBox(int index)
{
    this->handleCheckBox();
}

void MainWindow::handleFlags()
{
    std::string flags = this->getFlags();
    this->setOptions(flags);
}

void MainWindow::handleButton()
{
    printf("Blorp");
    char flags[64];
    std::string flagStr = this->getFlags();
    strncpy(flags, flagStr.c_str(), 64);

    uint64_t seed = this->seed->getSeed();
    std::string inputFile = this->romFile->text().toLatin1().constData();
    std::string outputDir = this->outputDir->text().toLatin1().constData();
    if (dwr_randomize(inputFile.c_str(), seed, flags, outputDir.c_str())) {
        /* an error occurred */
    }
}
