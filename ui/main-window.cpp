
#include <QtWidgets/QVBoxLayout>
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

#define __STDC_FORMAT_MACROS
#include <cinttypes>

#include "dwr.h"
#include "sprites.h"
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
    this->levelSpeed = new LevelComboBox(this);
    this->statgrowth = new StatComboBox(this);
    this->goButton = new QPushButton("Randomize!", this);
    this->spriteSelect = new QComboBox(this);
    for (int i=0; i < sizeof(dwr_sprite_names)/sizeof(char*); ++i) {
       spriteSelect->addItem(dwr_sprite_names[i]);
    }
}

void MainWindow::initSlots()
{
    connect(this->flags, SIGNAL(textEdited(QString)), this, SLOT(handleFlags()));
    connect(this->levelSpeed, SIGNAL(activated(int)),
            this, SLOT(handleCheckBox()));
    connect(this->statgrowth, SIGNAL(activated(int)),
            this, SLOT(handleCheckBox()));
    connect(this->goButton,     SIGNAL(clicked()), this, SLOT(handleButton()));
}

void MainWindow::layout()
{
    QVBoxLayout *vbox;
    QGridLayout *grid;
    this->optionGrid = new QGridLayout();

    vbox = new QVBoxLayout();
    grid = new QGridLayout();
    vbox->addLayout(grid);
    vbox->addLayout(this->optionGrid);

    grid->addWidget(this->romFile,   0, 0, 0);
    grid->addWidget(this->outputDir, 0, 1, 0);
    grid->addWidget(this->seed,      1, 0, 0);
    grid->addWidget(this->flags,     1, 1, 0);

    this->addOption('C', "Shuffle Chests && Search Items", 0, 0);
    this->addOption('W', "Randomize Weapon Shops",         1, 0);
    this->addOption('G', "Randomize Growth",               2, 0);
    this->addOption('M', "Randomize Spell Learning",       3, 0);
    this->addOption('w', "Randomize World Map",            4, 0);
    this->addOption('X', "Chaos Mode",                     5, 0);
    this->addOption('r', "Random%",                        6, 0);

    this->addOption('P', "Randomize Enemy Attacks",        0, 1);
    this->addOption('Z', "Randomize Enemy Zones",          1, 1);
    this->addOption('R', "Enable Menu Wrapping",           2, 1);
    this->addOption('D', "Enable Death Necklace",          3, 1);
    this->addOption('K', "Shuffle Music",                  4, 1);
    this->addOption('Q', "Disable Music",                  5, 1);

    this->addOption('b', "Big Swamp",                      0, 2);
    this->addOption('t', "Fast Text",                      1, 2);
    this->addOption('h', "Speed Hacks",                    2, 2);
    this->addOption('o', "Open Charlock",                  3, 2);
    this->addOption('s', "Short Charlock",                 4, 2);
    this->addOption('k', "Don't Require Magic Keys",       5, 2);
    this->addOption('e', "No Equipment",                   6, 2);


    /* Add an empty label for padding */
    this->optionGrid->addWidget(new QLabel("", this), 7, 1, 0);

    this->optionGrid->addWidget(new QLabel("Leveling Speed", this), 8, 0, 0);
    this->optionGrid->addWidget(this->levelSpeed,    9, 0, 0);
    this->optionGrid->addWidget(new QLabel("Player Sprite", this), 8, 1, 0);
    this->optionGrid->addWidget(this->spriteSelect,  9, 1, 0);
    this->optionGrid->addWidget(new QLabel("Stat Growth", this), 8, 2, 0);
    this->optionGrid->addWidget(this->statgrowth,    9, 2, 0);

    this->optionGrid->addWidget(this->goButton,      10, 2, 0);

    this->mainWidget->setLayout(vbox);
}

void MainWindow::addOption(char flag, QString text, int x, int y)
{
    CheckBox *option = new CheckBox(flag, text, this);
    connect(option, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    this->options.append(option);
    this->optionGrid->addWidget(option, x, y, 0);
}

QString MainWindow::getOptions()
{
    QList<CheckBox*>::const_iterator i;

    std::string flags = std::string() + this->levelSpeed->getFlag();
    flags += this->statgrowth->getFlag();
    for (i = this->options.begin(); i != this->options.end(); ++i) {
        flags += (*i)->getFlag();
    }

    if (QString(flags.c_str()).indexOf(QChar('r')) != -1) // May want to option this out or disable all of the relevant checkboxes.
    {
        std::replace(flags.begin(), flags.end(), 'C', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'W', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'G', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'M', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'w', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'X', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'P', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'Z', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'D', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'b', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'o', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 's', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'k', NO_FLAG);
        std::replace(flags.begin(), flags.end(), 'e', NO_FLAG);
        std::replace(flags.begin(), flags.end(), '0', NO_FLAG);
        std::replace(flags.begin(), flags.end(), '1', NO_FLAG);
        std::replace(flags.begin(), flags.end(), '2', NO_FLAG);
        std::replace(flags.begin(), flags.end(), '3', NO_FLAG);
        std::replace(flags.begin(), flags.end(), '4', NO_FLAG);
    }

    std::sort(flags.begin(), flags.end());
    std::replace(flags.begin(), flags.end(), NO_FLAG, '\0');
    return QString(flags.c_str());
}

void MainWindow::setOptions(QString flags)
{
    QList<CheckBox*>::const_iterator i;

    for (i = this->options.begin(); i != this->options.end(); ++i) {
        flags += (*i)->updateState(flags);
    }

    this->levelSpeed->updateState(flags);
    this->statgrowth->updateState(flags);
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
    std::string spriteName =
                this->spriteSelect->currentText().toLatin1().constData();
    uint64_t crc = dwr_randomize(inputFile.c_str(), seed, flags,
                                 spriteName.c_str(), outputDir.c_str());
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
    out << this->spriteSelect->currentIndex() << endl;

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
    if (read) {
        tmp[read - 1] = '\0';
        this->romFile->setText(tmp);
        if (configFile.atEnd()) {
            return false;
        }
    }

    read = configFile.readLine(tmp, 1024);
    if (read) {
        tmp[read - 1] = '\0';
        this->outputDir->setText(tmp);
        if (configFile.atEnd()) {
            return false;
        }
    }

    read = configFile.readLine(tmp, 1024);
    if (read) {
        tmp[read - 1] = '\0';
        this->setFlags(tmp);
        this->setOptions(tmp);
    }

    read = configFile.readLine(tmp, 1024);
    if (read) {
        int spriteIndex = atoi(tmp);
        this->spriteSelect->setCurrentIndex(spriteIndex);
    }

    return true;
}
