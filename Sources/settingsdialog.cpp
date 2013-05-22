#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QCheckBox>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
     :   QDialog(parent)
     ,   reverseYAxisCheckBox(new QCheckBox(tr("Reverse Y-Axis"), this))
{
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);

    // Populate checkboxes
    reverseYAxisCheckBox->setChecked(false);
 
    // Add widgets to layout
    dialogLayout->addWidget(reverseYAxisCheckBox);

    // Add standard buttons to layout
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttonBox);

    // Connect standard buttons
    connect((QObject*)buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
                    this, SLOT(accept()));
    connect((QObject*)buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
                    this, SLOT(reject()));

    setLayout(dialogLayout);
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::exec(bool yAxisReversed)
{
    reverseYAxisCheckBox->setChecked(yAxisReversed);
    QDialog::exec();
}

bool SettingsDialog::isYAxisReversed() const
{
    return reverseYAxisCheckBox->isChecked();
}
