
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <ctime>
#define __STDC_FORMAT_MACROS
#include <cinttypes>

#include "dwr.h"
#include "mt64.h"
#include "widgets.h"

CheckBox::CheckBox(const char flag, const QString &text, QWidget *parent) :
        QCheckBox(text, parent)
{
    this->flag = flag;
    this->conflicts = "";
    this->requires = "";
}

CheckBox::CheckBox(const char flag, const QString &text,
        const QString requires, const QString conflicts, QWidget *parent) :
        QCheckBox(text, parent)
{
    this->flag = flag;
    this->requires = requires;
    this->conflicts = conflicts;
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

bool CheckBox::updateState(const QString flags)
{
    bool checked = this->isEnabled() && flags.contains(this->flag);

    this->setChecked(checked);
    return checked;
}

bool CheckBox::updateConflicts(const QString flags)
{
    bool enabled = true;
    bool checked = flags.contains(this->flag);

    for (int i=0; i < this->requires.length(); i++) {
        if (!flags.contains(this->requires.at(i))) {
            enabled = false;
            checked = false;
            break;
        }
    }
    if (enabled) {
        for (int i=0; i < this->conflicts.length(); i++) {
            if (flags.contains(this->conflicts.at(i))) {
                enabled = false;
                checked = false;
                break;
            }
        }
    }
    this->setEnabled(enabled);
    this->setChecked(checked);
    return enabled;
}

LevelComboBox::LevelComboBox(QWidget *parent) : QComboBox(parent)
{
    this->addItem("Normal");
    this->addItem("Fast");
    this->addItem("Very Fast");
}

char LevelComboBox::getFlag()
{
    switch(this->currentIndex()) {
        case 1:
           return 'F';
        case 2:
            return 'V';
        default:
            return NO_FLAG;
    }
}

bool LevelComboBox::updateState(QString flags)
{
    if (flags.indexOf(QChar('F')) >= 0) {
        this->setCurrentIndex(1);
        return true;
    } else {
        if (flags.indexOf(QChar('V')) >= 0) {
            this->setCurrentIndex(2);
            return true;
        }
    }
    this->setCurrentIndex(0);
    return false;
}

ButtonEntry::ButtonEntry(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();
    this->textBox = new QLineEdit(this);
    this->button = new QPushButton("Click", this);
    this->label = new QLabel("", this);
    vbox->addWidget(this->label);
    hbox->addWidget(this->textBox);
    hbox->addWidget(this->button);
    vbox->addLayout(hbox);
    this->setLayout(vbox);
    connect(this->button, SIGNAL(clicked()), this, SLOT(handleButton()));
    connect(this->textBox, SIGNAL(textEdited(QString)), this, SLOT(handleEdit(QString)));
}


QString ButtonEntry::text()
{
    return this->textBox->text();
}

void ButtonEntry::setText(QString text)
{
    this->textBox->setText(text);
}

FileEntry::FileEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Browse...");
    this->label->setText("ROM File");
}

void FileEntry::handleButton()
{
    QString filename = QFileDialog::getOpenFileName(
           this, "Choose the ROM file", "./", "NES ROM Files (*.nes)");
    if (filename.size())
        this->textBox->setText(filename);
}

DirEntry::DirEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Browse...");
#ifdef _WIN32
    this->label->setText("Output Folder");
#else
    this->label->setText("Output Directory");
#endif
}

void DirEntry::handleButton()
{
    QString dirName = QFileDialog::getExistingDirectory(this,
#ifdef _WIN32
            "Choose the output folder",
#else
            "Choose the output directory",
#endif
            "./");
    if (dirName.size())
        this->textBox->setText(dirName);
}

SeedEntry::SeedEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Random");
    this->label->setText("Seed");
    this->handleButton();
}

void SeedEntry::random()
{
    uint64_t seed;
    char seedString[21];
    srand(time(NULL));
    seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    mt_init(seed);
    seed = mt_rand64();
    snprintf(seedString, 21, "%" PRIu64, seed);
    this->textBox->setText(QString(seedString));
}

void SeedEntry::handleButton()
{
    this->random();
}

uint64_t SeedEntry::getSeed()
{
    uint64_t seed;
    sscanf(this->textBox->text().toLatin1().constData(), "%" PRIu64, &seed);
    return seed;
}

FlagEntry::FlagEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Defaults");
    this->label->setText("Flags");
}

void FlagEntry::handleButton()
{
    this->setText(DEFAULT_FLAGS);
    this->textBox->textEdited(this->text());
}

void FlagEntry::handleEdit(QString text)
{
    this->textEdited(text);
}

