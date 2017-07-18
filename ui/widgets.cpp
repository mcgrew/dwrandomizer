
#include <QtWidgets/QHBoxLayout>

#include "widgets.h"

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

Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent)
{

}

FileDialog::FileDialog(QWidget *parent) : QFileDialog(parent)
{

}

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{

}

FileEntry::FileEntry(QWidget *parent) :
        QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout();
    this->textBox = new TextBox(this);
    this->button = new Button("Browse...", this);
    hbox->addWidget(this->textBox);
    hbox->addWidget(this->button);
    this->setLayout(hbox);
    connect(this->button, SIGNAL(clicked()), this, SLOT(handleButton()));
}

void FileEntry::handleButton()
{
    QString filename = QFileDialog::getOpenFileName(
           this,tr("Choose the ROM file"), "./", tr("NES ROM Files (*.nes)"));
    this->textBox->setText(filename);
}

