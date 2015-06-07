/*
 * Copyright (C) Kreogist Dev Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QListView>
#include <QListWidget>
#include <QScrollBar>
#include <QSignalMapper>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QPushButton>
#include <QSlider>
#include <QScopedPointer>
#include <QSortFilterProxyModel>

#include "knsearchbox.h"
#include "knfontdialog.h"

#include <QDebug>

KNFontDialog::KNFontDialog(QWidget *parent) :
    QDialog(parent),
    m_fontComboBox(new QFontComboBox(this)),
    m_fontFamilyList(new QListView(this)),
    m_sizeListWidget(new QListWidget(this)),
    m_sizeEditor(new QLineEdit(this)),
    m_previewer(new QLineEdit(this)),
    m_fontFamilyFilter(new QSortFilterProxyModel(this)),
    m_sizeSlider(new QSlider(Qt::Vertical, this))
{
    //Configure the point size list.
    m_pointSizeList << "6"
                    << "8"
                    << "9"
                    << "10"
                    << "11"
                    << "12"
                    << "13"
                    << "14"
                    << "18"
                    << "24"
                    << "36"
                    << "48"
                    << "64"
                    << "72"
                    << "96"
                    << "144"
                    << "288";

    //Initial layout.
    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                          this);
    setLayout(mainLayout);

    //Font selector.
    //Configure view.
    m_fontFamilyList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_fontFamilyList->setSelectionMode(QAbstractItemView::SingleSelection);
    //Configure filter model.
    m_fontFamilyFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    //Generate a font combo box, and we can get the font model.
    m_fontComboBox->hide();
    m_fontFamilyFilter->setSourceModel(m_fontComboBox->model());
    m_fontFamilyList->setModel(m_fontFamilyFilter);
    m_fontFamilyList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fontFamilyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_fontFamilyList->selectionModel(),
            &QItemSelectionModel::currentChanged,
            [=](const QModelIndex &current)
            {
                //Sync the current font style to the editor.
                if(current.isValid())
                {
                    QFont previewFont=m_previewer->font();
                    previewFont.setFamily(
                                current.data(Qt::DisplayRole).toString());
                    m_previewer->setFont(previewFont);
                }
            });
    //Generate the font selector keyword input box.
    QLineEdit *fontSearcher=new KNSearchBox(this);
    fontSearcher->setPlaceholderText(tr("Search Font"));
    //Link the searcher to the filter model.
    connect(fontSearcher, &KNSearchBox::textChanged,
            this, &KNFontDialog::onActionSearchFont);
    //Generate the font selector layout.
    QBoxLayout *fontFamilyLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                                mainLayout->widget());
    fontFamilyLayout->addWidget(new QLabel(tr("Font"), this), 0, Qt::AlignLeft);
    fontFamilyLayout->addWidget(m_fontFamilyList, 1);
    fontFamilyLayout->addWidget(fontSearcher);
    mainLayout->addLayout(fontFamilyLayout, 1);

    //Font options.
    //Configure the size editor.
    QValidator *realValidator=new QRegExpValidator(QRegExp("[0-9\\.]+$"), this);
    m_sizeEditor->setValidator(realValidator);
    connect(m_sizeEditor, &QLineEdit::textChanged,
            [=](const QString &sizeText)
            {
                syncFontSize(sizeText.toDouble(), false);
            });
    //Configure the size view and model.
    m_sizeEditor->setMaximumWidth(50);
    m_sizeListWidget->setMaximumWidth(50);
    m_sizeListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    //Add font sizes to list widget.
    for(QString fontSizeText:m_pointSizeList)
    {
        m_sizeListWidget->addItem(fontSizeText);
    }
    //Link the current change signal.
    connect(m_sizeListWidget, &QListWidget::currentRowChanged,
            [=](const int &currentRow)
            {
                syncFontSize(m_pointSizeList.at(currentRow).toDouble());
            });
    //Configure the slider.
    connect(m_sizeSlider, &QSlider::valueChanged,
            [=](const int &sliderFontSize)
            {
                syncFontSize(sliderFontSize);
            });
    //Font size manager.
    QGridLayout *sizeLayout=new QGridLayout(mainLayout->widget());
    sizeLayout->addWidget(new QLabel(tr("Size"), this), 0, 0, 1, 2);
    sizeLayout->addWidget(m_sizeEditor, 1, 0, 1, 1);
    sizeLayout->addWidget(m_sizeListWidget, 2, 0, 1, 1);
    sizeLayout->addWidget(m_sizeSlider, 1, 1, 2, 1);
    //Font style manager.
    QBoxLayout *styleLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                           mainLayout->widget());
    styleLayout->addWidget(new QLabel(tr("Style"), this));
    //Generate style checked signal mapper.
    QSignalMapper *fontStyleMapper=new QSignalMapper(this);
    connect(fontStyleMapper,
            static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
            this,
            &KNFontDialog::onActionStyleStatusChange);
    //Generate style tweak check boxes.
    QString fontStyleCaptions[FontStylesCount];
    fontStyleCaptions[Bold]=tr("Bold");
    fontStyleCaptions[Italic]=tr("Italic");
    fontStyleCaptions[Underline]=tr("Underline");
    fontStyleCaptions[StrikeOut]=tr("Strike out");
    fontStyleCaptions[Kerning]=tr("Kerning");
    for(int i=0; i<FontStylesCount; i++)
    {
        //Generate style check box.
        m_fontStyles[i]=new QCheckBox(fontStyleCaptions[i], this);
        //Link the style check box to signal mapper.
        connect(m_fontStyles[i], SIGNAL(stateChanged(int)),
                fontStyleMapper, SLOT(map()));
        fontStyleMapper->setMapping(m_fontStyles[i], i);
        //Add the check box to layout.
        styleLayout->addWidget(m_fontStyles[i]);
    }
    styleLayout->addStretch();

    //Ok and Cancel button.
    QPushButton *ok=new QPushButton(tr("Ok"), this),
                *cancel=new QPushButton(tr("Cancel"), this);
    //Configure the buttons.
    ok->setDefault(true);
    //Link ok and cancel button.
    connect(ok,
            static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
            this, &KNFontDialog::onActionOk);
    connect(cancel,
            static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
            this, &KNFontDialog::onActionCancel);
    QBoxLayout *buttonLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                            mainLayout->widget());
    buttonLayout->addWidget(ok);
    buttonLayout->addWidget(cancel);
    buttonLayout->addStretch();

    //Add size manager and style manager to options layout.
    QBoxLayout *fontOptionsLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                                 mainLayout->widget());
    fontOptionsLayout->addLayout(sizeLayout);
    fontOptionsLayout->addLayout(styleLayout);
    fontOptionsLayout->addLayout(buttonLayout);

    //Previewer.
    //Configure the initial text of the previewer.
    m_previewer->setText("AaBbXxYy");
    m_previewer->setFixedSize(249, 100);
    m_previewer->setAlignment(Qt::AlignCenter);
    //We will get the font here.
    QBoxLayout *centerLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                             mainLayout->widget());
    centerLayout->addLayout(fontOptionsLayout, 1);
    centerLayout->addWidget(new QLabel(tr("Preview"), this));
    centerLayout->addWidget(m_previewer);
    mainLayout->addLayout(centerLayout);
}

QFont KNFontDialog::resultFont() const
{
    return m_resultFont;
}

void KNFontDialog::setInitialFont(const QFont &font)
{
    //Save the initial font as the result font.
    m_resultFont=font;
    //Set the font to previewer.
    m_previewer->setFont(font);
    //Sync the font size.
    syncFontSize(font.pointSizeF());
    //Sync the font family.
    //Clear the filter of proxy model first.
    m_fontFamilyFilter->setFilterFixedString("");
    //Find and select the font family.
    m_fontComboBox->setCurrentFont(font);
    QModelIndex fontProxyIndex=
            m_fontFamilyFilter->mapFromSource(
                m_fontComboBox->model()->index(m_fontComboBox->currentIndex(),
                                               0));
    m_fontFamilyList->setCurrentIndex(fontProxyIndex);
    m_fontFamilyList->scrollTo(fontProxyIndex);
    //Sync the font style.
    m_fontStyles[Bold]->setChecked(font.bold());
    m_fontStyles[Italic]->setChecked(font.italic());
    m_fontStyles[Underline]->setChecked(font.underline());
    m_fontStyles[StrikeOut]->setChecked(font.strikeOut());
    m_fontStyles[Kerning]->setChecked(font.kerning());
}

QFont KNFontDialog::getFont(QWidget *parent,
                            const QString &title,
                            const QFont &initialFont)
{
    //Generate a font dialog.
    QScopedPointer<KNFontDialog> fontDialog(new KNFontDialog(parent));
    //Set the title and initial font.
    fontDialog->setWindowTitle(title.isEmpty()?tr("Font"):title);
    //Set the initial font.
    fontDialog->setInitialFont(initialFont);
    //Launch the font dialog. Return the result font if accept the tweak.
    if(QDialog::Accepted==fontDialog->exec())
    {
        return fontDialog->resultFont();
    }
    return initialFont;
}

void KNFontDialog::onActionOk(const bool &checked)
{
    Q_UNUSED(checked);
    //Set the result font.
    m_resultFont=m_previewer->font();
    //Set accept flag.
    done(QDialog::Accepted);
}

void KNFontDialog::onActionCancel(const bool &checked)
{
    Q_UNUSED(checked);
    done(QDialog::Rejected);
}

void KNFontDialog::onActionSearchFont(const QString &filterText)
{
    //Set the filter text.
    m_fontFamilyFilter->setFilterFixedString(filterText);
    //Check if there's any filter's row.
    if(m_fontFamilyFilter->rowCount()>0)
    {
        //Select the first font in the filter list.
        m_fontFamilyList->setCurrentIndex(m_fontFamilyFilter->index(0,0));
    }
}

void KNFontDialog::onActionStyleStatusChange(const int &statusIndex)
{
    //Get the previewer font.
    QFont previewFont=m_previewer->font();
    //Change the font status.
    switch(statusIndex)
    {
    case Bold:
        previewFont.setBold(m_fontStyles[Bold]->isChecked());
        break;
    case Italic:
        previewFont.setItalic(m_fontStyles[Italic]->isChecked());
        break;
    case Underline:
        previewFont.setUnderline(m_fontStyles[Underline]->isChecked());
        break;
    case StrikeOut:
        previewFont.setStrikeOut(m_fontStyles[StrikeOut]->isChecked());
        break;
    case Kerning:
        previewFont.setKerning(m_fontStyles[Kerning]->isChecked());
        break;
    }
    //Set the new font to previewer.
    m_previewer->setFont(previewFont);
}

void KNFontDialog::syncFontSize(qreal pointSize, bool changeLineEdit)
{
    //Block the signal senders.
    m_sizeListWidget->blockSignals(true);
    m_sizeEditor->blockSignals(true);
    m_sizeSlider->blockSignals(true);
    //Set the data.
    if(pointSize<=0.0000)
    {
        pointSize=1.0000;
    }
    QString textPointSize=QString::number(pointSize);
    //Sync editor data to the point size.
    if(changeLineEdit)
    {
        m_sizeEditor->setText(textPointSize);
    }
    //Sync the list to the point size.
    int sizeIndex=m_pointSizeList.indexOf(textPointSize);
    //If we find the size in the list,
    if(sizeIndex==-1)
    {
        //Select nothing.
        m_sizeListWidget->clearSelection();
    }
    else
    {
        //Select the size item.
        m_sizeListWidget->setCurrentRow(sizeIndex);
    }
    //Sync the slider.
    m_sizeSlider->setValue(pointSize);
    //Sync to the editor.
    QFont previewFont=m_previewer->font();
    previewFont.setPointSizeF(pointSize);
    m_previewer->setFont(previewFont);
    //Unblock the signal senders.
    m_sizeListWidget->blockSignals(false);
    m_sizeEditor->blockSignals(false);
    m_sizeSlider->blockSignals(false);
}
