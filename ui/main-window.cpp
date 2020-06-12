
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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTabWidget>

#define __STDC_FORMAT_MACROS
#include <cinttypes>

#include "dwr.h"
#include "sprites.h"
#include "main-window.h"

#define PANE_COLUMNS 2
#define PANE_ROWS    7

const char* const tab_names[] {
    "Gameplay",
    "Features",
    "Monsters",
    "Shortcuts",
    "Challenge",
    "Goals",
    "Cosmetic"
};

enum tabs {
    GAMEPLAY,
    FEATURES,
    MONSTERS,
    SHORTCUTS,
    CHALLENGE,
    GOALS,
    COSMETIC
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setMinimumWidth(650);
    this->mainWidget = new QWidget();
    this->gameplayWidget = new QWidget();
    this->funWidget = new QWidget();
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
    this->goButton = new QPushButton("Randomize!", this);
    this->spriteSelect = new QComboBox(this);
    for (size_t i=0; i < sprite_count(); ++i) {
       spriteSelect->addItem(dwr_sprite_names[i]);
    }
    this->tabWidget = new QTabWidget(this);

    for (int i=0; i < TAB_COUNT; i++) {
      this->tabContents[i] = new QWidget(this);
    }
}

void MainWindow::initSlots()
{
    connect(this->flags, SIGNAL(textEdited(QString)), this, SLOT(handleFlags()));
    connect(this->levelSpeed, SIGNAL(activated(int)),
            this, SLOT(handleCheckBox()));
    connect(this->goButton,     SIGNAL(clicked()), this, SLOT(handleButton()));
}

void MainWindow::layout()
{
    QVBoxLayout *vbox;
    QGridLayout *grid;
    QGridLayout *goLayout;

    for (int i=0; i < TAB_COUNT; i++) {
      this->optionGrids[i] = new QGridLayout();
    }

    vbox = new QVBoxLayout();
    grid = new QGridLayout();
    goLayout = new QGridLayout();

    vbox->addLayout(grid);
    vbox->addWidget(tabWidget);
    vbox->addLayout(goLayout);

    grid->addWidget(this->romFile,   0, 0, 0);
    grid->addWidget(this->outputDir, 0, 1, 0);
    grid->addWidget(this->seed,      1, 0, 0);
    grid->addWidget(this->flags,     1, 1, 0);

    /* Add tab names and initialize layouts */
    for (int i=0; i < TAB_COUNT; i++) {
      this->tabContents[i]->setLayout(this->optionGrids[i]);
      tabWidget->addTab(tabContents[i], tab_names[i]);
      for (int c=0; c < PANE_COLUMNS; c++) {
        this->addLabel("", i, 0, c);
      }
      /* add empty labels for padding */
      for (int r=1; r < PANE_ROWS; r++) {
        this->addLabel("", i, r, 0);
      }
    }

    /* Gameplay Options */
    this->addOption('C', "Shuffle Chests && Searches",         GAMEPLAY,  0, 0);
    this->addOption('G', "Random Growth",                      GAMEPLAY,  1, 0);
    this->addOption('M', "Random Map",                         GAMEPLAY,  2, 0);
    this->addOption('S', "Random Spell Learning",              GAMEPLAY,  3, 0);
    this->addOption('Y', "Heal/Hurt before \"More\"", "S", "", GAMEPLAY,  4, 0);
    this->addOption('W', "Random Weapon Shops",                GAMEPLAY,  0, 1);
    this->addOption('w', "Random Weapon Prices",               GAMEPLAY,  1, 1);
    this->addOption('x', "Random XP Requirements",             GAMEPLAY,  2, 1);

    /* Features */
    this->addOption('R', "Enable Menu Wrapping",               FEATURES,  0, 0);
    this->addOption('D', "Enable Death Necklace",              FEATURES,  1, 0);
    this->addOption('T', "Enable Torches In Battle",           FEATURES,  2, 0);
    this->addOption('b', "Big Swamp",                 "M", "", FEATURES,  0, 1);
    this->addOption('r', "Repel in dungeons",                  FEATURES,  1, 1);
    this->addOption('p', "Permanent repel",                    FEATURES,  2, 1);
    this->addOption('y', "Permanent torch",                    FEATURES,  3, 1);

    /* Monster flags */
    this->addOption('P', "Random Monster Abilities",           MONSTERS,  0, 0);
    this->addOption('Z', "Random Monster Zones",               MONSTERS,  1, 0);
    this->addOption('e', "Random Monster Stats",      "Z", "", MONSTERS,  2, 0);
    this->addOption('d', "Random Monster XP && Gold",          MONSTERS,  3, 0);
    this->addOption('g', "Make Stats Consistent",     "e", "", MONSTERS,  4, 0);
    this->addOption('l', "Scared Metal Slimes",       "", "e", MONSTERS,  0, 1);

    /* Shortcuts */
    this->addOption('t', "Fast Text",                          SHORTCUTS, 0, 0);
    this->addOption('H', "Speed Hacks",                        SHORTCUTS, 1, 0);
    this->addOption('o', "Open Charlock",                      SHORTCUTS, 2, 0);
    this->addOption('s', "Short Charlock",                     SHORTCUTS, 3, 0);
    this->addOption('k', "Don't Require Magic Keys",           SHORTCUTS, 4, 0);
    this->addLabel("Leveling Speed",                           SHORTCUTS, 0, 1);
    this->placeWidget(this->levelSpeed,                        SHORTCUTS, 1, 1);

    /* Challenge */
    this->addOption('u', "No Hurtmore",                        CHALLENGE, 0, 0);
    this->addOption('n', "No Numbers",                         CHALLENGE, 1, 0);
    this->addOption('i', "Invisible Hero",                     CHALLENGE, 2, 0);
    this->addOption('q', "Invisible NPCs",                     CHALLENGE, 3, 0);

    /* Goals */
    this->addOption('c', "Cursed Princess",                    GOALS,     0, 0);
    this->addOption('h', "Three's Company",                    GOALS,     1, 0);

    /* Cosmetic Options */
    this->addOption('K', "Shuffle Music",             "", "Q", COSMETIC,  0, 0);
    this->addOption('Q', "Disable Music",             "", "K", COSMETIC,  1, 0);
    this->addOption('m', "Modern Spell Names",                 COSMETIC,  2, 0);
    this->addOption('j', "Noir Mode",                          COSMETIC,  3, 0);
    this->addLabel("Player Sprite",                            COSMETIC,  0, 1);
    this->placeWidget(this->spriteSelect,                      COSMETIC,  1, 1);

    goLayout->addWidget(new QLabel("", this), 0, 0, 0);
    goLayout->addWidget(new QLabel("", this), 0, 1, 0);
    goLayout->addWidget(this->goButton, 0, 2, 0);

    this->mainWidget->setLayout(vbox);
}

void MainWindow::addOption(char flag, QString text, int tab, int x, int y)
{
    CheckBox *option = new CheckBox(flag, text, this);
    connect(option, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    this->options.append(option);
    this->optionGrids[tab]->addWidget(option, x, y, 0);
}

void MainWindow::addOption(char flag, QString text, QString requires,
        QString excluded_by, int tab, int x, int y)
{
    CheckBox *option = new CheckBox(flag, text, requires, excluded_by, this);
    connect(option, SIGNAL(clicked()), this, SLOT(handleCheckBox()));
    this->options.append(option);
    this->optionGrids[tab]->addWidget(option, x, y, 0);
}

void MainWindow::addLabel(QString text, int tab, int x, int y)
{
    this->optionGrids[tab]->addWidget(new QLabel(text, this), x, y, 0);
}

void MainWindow::placeWidget(QWidget *widget, int tab, int x, int y)
{
    this->optionGrids[tab]->addWidget(widget, x, y, 0);
}

QString MainWindow::getOptions()
{
    QList<CheckBox*>::const_iterator i;
    bool conflicts = false;

    std::string flags = std::string() + this->levelSpeed->getFlag();
    for (i = this->options.begin(); i != this->options.end(); ++i) {
        flags += (*i)->getFlag();
    }

    /* check for flag conflicts */
    for (i = this->options.begin(); i != this->options.end(); ++i) {
        conflicts |= (*i)->updateConflicts(QString(flags.c_str()));
    }

    if (conflicts) { /* there were conflicts, so update flags */
        flags.clear();
        flags += this->levelSpeed->getFlag();
        for (i = this->options.begin(); i != this->options.end(); ++i) {
            flags += (*i)->getFlag();
        }
    }

    std::sort(flags.begin(), flags.end());
    std::replace(flags.begin(), flags.end(), NO_FLAG, '\0');
    return QString(flags.c_str());
}

void MainWindow::setOptions(QString flags)
{
    QList<CheckBox*>::const_iterator i;

    for (i = this->options.begin(); i != this->options.end(); ++i) {
        if ((*i)->updateConflicts(flags)) {
            flags += (*i)->updateState(flags);
        }
    }

    this->levelSpeed->updateState(flags);
    this->setFlags(this->getOptions());
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
    strncpy(flags, flagStr.toLatin1().constData(), 63);

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
    // this->seed->random();
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
