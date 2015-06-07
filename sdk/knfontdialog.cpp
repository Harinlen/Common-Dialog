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
#include <QScrollBar>
#include <QLineEdit>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QPushButton>
#include <QSlider>
#include <QStringListModel>
#include <QSortFilterProxyModel>

#include "knfontdialog.h"

#include <QDebug>

KNFontDialog::KNFontDialog(QWidget *parent) :
    QDialog(parent),
    m_fontFamilyList(new QListView(this)),
    m_sizeListView(new QListView(this)),
    m_sizeEditor(new QLineEdit(this)),
    m_fontFamilyFilter(new QSortFilterProxyModel(this)),
    m_sizeListModel(new QStringListModel(this)),
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
    //Configure filter model.
    m_fontFamilyFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    //Generate a font combo box, and we can get the font model.
    QFontComboBox *fontComboBox=new QFontComboBox(this);
    fontComboBox->hide();
    m_fontFamilyFilter->setSourceModel(fontComboBox->model());
    m_fontFamilyList->setModel(m_fontFamilyFilter);
    m_fontFamilyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Generate the font selector keyword input box.
    QLineEdit *fontSearcher=new QLineEdit(this);
    //Link the searcher to the filter model.
    connect(fontSearcher, &QLineEdit::textChanged,
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
    m_sizeListModel->setStringList(m_pointSizeList);
    m_sizeListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_sizeListView->setModel(m_sizeListModel);
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
    //Link the current change signal.
    connect(m_sizeListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this, &KNFontDialog::onActionSizeViewCurrentChange);
    sizeLayout->addWidget(m_sizeListView, 2, 0, 1, 1);
    sizeLayout->addWidget(m_sizeSlider, 1, 1, 2, 1);
    //Font style manager.
    QBoxLayout *styleLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                           mainLayout->widget());
    styleLayout->addWidget(new QLabel(tr("Style"), this));
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
        //Add the check box to layout.
        styleLayout->addWidget(m_fontStyles[i]);
    }
    styleLayout->addStretch();
    //Add size manager and style manager to options layout.
    QBoxLayout *fontOptionsLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                                 mainLayout->widget());
    fontOptionsLayout->addLayout(sizeLayout);
    fontOptionsLayout->addLayout(styleLayout);
    mainLayout->addLayout(fontOptionsLayout, 1);

    //Previewer.
    //We will get the font here.

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
    mainLayout->addLayout(buttonLayout);
}

void KNFontDialog::onActionOk(const bool &checked)
{
    Q_UNUSED(checked);
    //Save the font.
    QFont resultFont;
    //Set the font parameters.
//    resultFont.setFamily();
    //Set the result font.
    m_resultFont=resultFont;
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

void KNFontDialog::onActionSizeViewCurrentChange(const QModelIndex &current,
                                                 const QModelIndex &previous)
{
    Q_UNUSED(previous);
    //Check if current index is vaild.
    if(current.isValid())
    {
        //Sync the font size data to the select item.
        syncFontSize(current.data(Qt::DisplayRole).toString().toDouble());
    }
}

void KNFontDialog::syncFontSize(const qreal &pointSize, bool changeLineEdit)
{
    //Block the signal senders.
    m_sizeListView->selectionModel()->blockSignals(true);
    m_sizeEditor->blockSignals(true);
    //Set the data.
    QString textPointSize=QString::number(pointSize);
    //Sync editor data to the point size.
    if(changeLineEdit)
    {
        m_sizeEditor->setText(textPointSize);
    }
    //Sync the list to the point size.
    int sizeIndex=m_pointSizeList.indexOf(textPointSize);
    //If we find the size in the list,
    if(sizeIndex!=-1)
    {
        //Select the size item.
        m_sizeListView->setCurrentIndex(m_sizeListModel->index(sizeIndex,
                                                               0));
        //Update the list view.
        m_sizeListView->verticalScrollBar()->setValue(m_sizeListView->verticalScrollBar()->value());
    }
    //Sync the slider.
    m_sizeSlider->setValue(pointSize);
    //Unblock the signal senders.
    m_sizeListView->selectionModel()->blockSignals(false);
    m_sizeEditor->blockSignals(false);
}
