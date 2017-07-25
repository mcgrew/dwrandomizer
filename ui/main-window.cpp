
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QStatusBar>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QGuiApplication>
#include <QtGui/QClipboard>

#include "dwr.h"
#include "main-window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->mainWidget = new QWidget();
    this->setCentralWidget(this->mainWidget);

    this->initWidgets();
    this->initStatus();
    this->layout();
    this->initSlots();
    this->loadConfig();
}

void MainWindow::initStatus() {
    QStatusBar *status = this->statusBar();
    status->showMessage("Ready");
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::lightGray);
    palette.setColor(QPalette::Foreground, Qt::black);
    status->setPalette(palette);
    status->setAutoFillBackground(true);
}

void MainWindow::initWidgets()
{
    this->romFile = new FileEntry(this);
    this->outputDir = new DirEntry(this);
    this->seed = new SeedEntry(this);
    this->flags = new FlagEntry(this);

    chests =        new CheckBox('C', "Shuffle Chests && Search Items", this);
    shops =         new CheckBox('W', "Randomize Weapon Shops", this);
    noKey =         new CheckBox('k', "No Key Requirement", this);
    deathNecklace = new CheckBox('D', "Enable Death Necklace", this);
    speedHacks =    new CheckBox('H', "Enable Speed Hacks", this);
    speedHacksLow = new CheckBox('h', "Lesser Speed Hacks", this);
    rainbowBridge = new CheckBox('b', "Bridge Not Required", this);
    growth =        new CheckBox('G', "Randomize Growth", this);
    spells =        new CheckBox('M', "Randomize Spell Learning", this);
    attack =        new CheckBox('P', "Randomize Enemy Attacks", this);
    menuWrap =      new CheckBox('R', "Enable Menu Wrapping", this);
    zones =         new CheckBox('Z', "Randomize Zones", this);
    wrap =          new CheckBox('R', "Enable Menu Wrapping", this);
    musicShuffle =  new CheckBox('K', "Shuffle Music", this);
    musicDisable =  new CheckBox('Q', "Disable Music", this);
//    copyChecksum =  new CheckBox(NO_FLAG, "Copy Checksum to Clipboard", this);

    this->levelSpeed = new LevelComboBox(this);

    this->goButton = new QPushButton("Randomize!", this);
}

void MainWindow::initSlots()
{
    connect(this->flags, SIGNAL(textEdited(QString)), this, SLOT(handleFlags()));

    connect(this->chests,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->shops,        SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->noKey,        SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->deathNecklace,SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->speedHacks,   SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->speedHacksLow,SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->rainbowBridge,SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->growth,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->spells,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->attack,       SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->menuWrap,     SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->zones,        SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->wrap,         SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->musicShuffle, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    connect(this->musicDisable, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
//    connect(this->copyChecksum, SIGNAL(clicked()), this, SLOT(handleCheckBox()));

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
    grid->addWidget(this->noKey,         3, 0, 0);

    grid->addWidget(this->growth,        0, 1, 0);
    grid->addWidget(this->spells,        1, 1, 0);
    grid->addWidget(this->attack,        2, 1, 0);

    grid->addWidget(this->wrap,          3, 1, 0);
//    grid->addWidget(this->copyChecksum,  3, 1, 0);

    grid->addWidget(this->deathNecklace, 0, 2, 0);
    grid->addWidget(this->speedHacks,    1, 2, 0);
    grid->addWidget(this->speedHacksLow, 2, 2, 0);
    grid->addWidget(this->rainbowBridge, 3, 2, 0);
    grid->addWidget(this->musicShuffle,  4, 2, 0);
    grid->addWidget(this->musicDisable,  5, 2, 0);

    grid->addWidget(new QLabel("Leveling Speed", this), 6, 0, 0);
    grid->addWidget(this->levelSpeed,    7, 0, 0);

    grid->addWidget(this->goButton,      8, 2, 0);

    this->mainWidget->setLayout(vbox);

//    this->copyChecksum->hide();
}

QString MainWindow::getOptions()
{
    std::string flags = std::string() +
                        this->chests->getFlag() +
                        this->shops->getFlag() +
                        this->noKey->getFlag() +
                        this->deathNecklace->getFlag() +
                        this->speedHacks->getFlag() +
                        this->speedHacksLow->getFlag() +
                        this->rainbowBridge->getFlag() +
                        this->growth->getFlag() +
                        this->spells->getFlag() +
                        this->attack->getFlag() +
                        this->menuWrap->getFlag() +
                        this->zones->getFlag() +
                        this->wrap->getFlag() +
                        this->musicShuffle->getFlag() +
                        this->musicDisable->getFlag() +

                        this->levelSpeed->getFlag();

    std::sort(flags.begin(), flags.end());
    std::replace(flags.begin(), flags.end(), NO_FLAG, '\0');
    return QString(flags.c_str());
}

void MainWindow::setOptions(QString flags)
{
    this->chests->updateState(flags);
    this->shops->updateState(flags);
    this->noKey->updateState(flags);
    this->deathNecklace->updateState(flags);
    this->speedHacks->updateState(flags);
    this->speedHacksLow->updateState(flags);
    this->rainbowBridge->updateState(flags);
    this->growth->updateState(flags);
    this->spells->updateState(flags);
    this->attack->updateState(flags);
    this->menuWrap->updateState(flags);
    this->zones->updateState(flags);
    this->wrap->updateState(flags);
    this->musicShuffle->updateState(flags);
    this->musicDisable->updateState(flags);
//    this->copyChecksum->updateState(flags);

    this->levelSpeed->updateState(flags);
}

QString MainWindow::getFlags()
{
    std::string flags = this->flags->text().toStdString();
    std::sort(flags.begin(), flags.end());
    return QString::fromStdString(flags);
}

void MainWindow::setFlags(QString flags)
{
    this->flags->setText(flags);
}

void MainWindow::handleCheckBox()
{
    QString flags = this->getOptions();
    this->setFlags(flags);
}

void MainWindow::handleComboBox(int index)
{
    this->handleCheckBox();
}

void MainWindow::handleFlags()
{
    QString flags = this->getFlags();
    this->setOptions(flags);
}

void MainWindow::handleButton()
{
    char flags[64], checksum[64];
    QString flagStr = this->getFlags();
    strncpy(flags, flagStr.toLatin1().constData(), 64);

    uint64_t seed = this->seed->getSeed();
    std::string inputFile = this->romFile->text().toLatin1().constData();
    std::string outputDir = this->outputDir->text().toLatin1().constData();
    uint64_t crc = dwr_randomize(inputFile.c_str(), seed,
                                 flags, outputDir.c_str());
    if (crc) {
        sprintf(checksum, "Checksum: %016" PRIx64, crc);
        QGuiApplication::clipboard()->setText(checksum);
        this->statusBar()->showMessage(
                QString("%1 (copied to clipboard)").arg(checksum));
        QMessageBox::information(this, "Success!",
                                 "The new ROM has been created.");
    } else {
        QMessageBox::critical(this, "Failed", "An error occurred and"
                "the ROM could not be created.");
    }
    this->saveConfig();
}

bool MainWindow::saveConfig()
{
    QDir configDir("");
    if (!configDir.exists(QDir::homePath() + "/.config/")){
        configDir.mkdir(QDir::homePath() + "/.config/");
    }

    QFile configFile(QDir::homePath() + "/.config/dwrandomizer2.conf");
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        printf("Failed to save configuration.\n");
        return false;
    }
    QTextStream out(&configFile);

    out << this->romFile->text() << endl;
    out << this->outputDir->text() << endl;
    out << this->getFlags() << endl;

    return true;
}

bool MainWindow::loadConfig()
{
    char tmp[1024];
    qint64 read;
    QFile configFile(QDir::homePath() + "/.config/dwrandomizer2.conf");
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("Failed to load configuration.\n");
        return false;
    }
    read = configFile.readLine(tmp, 1024);
    tmp[read - 1] = '\0';
    this->romFile->setText(tmp);
    if (configFile.atEnd()) {
        return false;
    }

    read = configFile.readLine(tmp, 1024);
    tmp[read - 1] = '\0';
    this->outputDir->setText(tmp);
    if (configFile.atEnd()) {
        return false;
    }

    read = configFile.readLine(tmp, 1024);
    tmp[read - 1] = '\0';
    this->setFlags(tmp);
    this->setOptions(tmp);

    return true;
}
