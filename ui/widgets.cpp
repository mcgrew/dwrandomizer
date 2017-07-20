
#include <QtWidgets/QHBoxLayout>
#include <time.h>

#include "widgets.h"

CheckBox::CheckBox(const char flag, const QString &text, QWidget *parent) :
        QCheckBox(text, parent)
{
    this->flag = flag;
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

bool CheckBox::updateState(std::string &flags)
{
    bool checked = flags.find(this->flag) != std::string::npos;
    this->setChecked(checked);
    return checked;
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
           return 'f';
        case 2:
            return 'F';
        default:
            return NO_FLAG;
    }
}

bool LevelComboBox::updateState(std::string &flags)
{
    bool checked = flags.find('f') != std::string::npos;
    if (checked) {
        this->setCurrentIndex(0);
    } else {
        bool checked = flags.find('F') != std::string::npos;
        this->setCurrentIndex(1);
    }
    return checked;
}

Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent)
{
}

FileDialog::FileDialog(QWidget *parent) : QFileDialog(parent)
{
}

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{
}

ButtonEntry::ButtonEntry(QWidget *parent) :
        QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout();
    this->textBox = new TextBox(this);
    this->button = new Button("Click", this);
    hbox->addWidget(this->textBox);
    hbox->addWidget(this->button);
    this->setLayout(hbox);
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
}

void FileEntry::handleButton()
{
    QString filename = QFileDialog::getOpenFileName(
           this, tr("Choose the ROM file"), "./", tr("NES ROM Files (*.nes)"));
    this->textBox->setText(filename);
}

DirEntry::DirEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Browse...");
}

void DirEntry::handleButton()
{
    QString dirName = QFileDialog::getExistingDirectory(this,
            tr("Choose the output directory"), "./");
    this->textBox->setText(dirName);
}

SeedEntry::SeedEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Random");
    this->handleButton();
}

void SeedEntry::handleButton()
{
    uint64_t seed;
    char seedString[21];
    srand(time(NULL));
    seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    snprintf(seedString, 64, "%" PRIu64, seed);
    this->textBox->setText(QString(seedString));
}

uint64_t SeedEntry::getSeed()
{
    uint64_t seed;
    sscanf(this->textBox->text().toLatin1().constData(), "%"PRIu64, &seed);
    return seed;
}

FlagEntry::FlagEntry(QWidget *parent) : ButtonEntry(parent)
{
    this->button->setText("Defaults");
}

void FlagEntry::handleButton()
{
    this->setText("CDGMPRWZf");
    this->textBox->textEdited(this->text());
}

void FlagEntry::handleEdit(QString text)
{
    this->textEdited(text);
}

