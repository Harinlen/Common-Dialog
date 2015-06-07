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

#ifndef KNFONTDIALOG_H
#define KNFONTDIALOG_H

#include <QDialog>

class QSlider;
class QCheckBox;
class QLineEdit;
class QListView;
class QStringListModel;
class QSortFilterProxyModel;
/*!
 * \brief The KNFontDialog is a dialog to select a font, and tweak the style or
 * size of the font.
 */
class KNFontDialog : public QDialog
{
    Q_OBJECT
public:
    /*!
     * \brief Construct a KNFontDialog.
     * \param parent The parent widget.
     */
    explicit KNFontDialog(QWidget *parent = 0);

signals:

public slots:

private slots:
    void onActionOk(const bool &checked);
    void onActionCancel(const bool &checked);

    void onActionSearchFont(const QString &filterText);
    void onActionSizeViewCurrentChange(const QModelIndex &current,
                                       const QModelIndex &previous);

private:
    inline void syncFontSize(const qreal &pointSize,
                             bool changeLineEdit=true);
    enum FontStyles
    {
        Bold,
        Italic,
        Underline,
        StrikeOut,
        Kerning,
        FontStylesCount
    };
    QCheckBox *m_fontStyles[FontStylesCount];

    QListView *m_fontFamilyList, *m_sizeListView;
    QLineEdit *m_sizeEditor;
    QSortFilterProxyModel *m_fontFamilyFilter;
    QStringListModel *m_sizeListModel;
    QSlider *m_sizeSlider;
    QFont m_resultFont;
    QStringList m_pointSizeList;
};

#endif // KNFONTDIALOG_H
