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
class QListWidget;
class QFontComboBox;
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

    /*!
     * \brief Get the font after configured by user.
     * \return The user configured font.
     */
    QFont resultFont() const;

    /*!
     * \brief Set the initial font, which is going to be tweak.
     * \param font The initial font.
     */
    void setInitialFont(const QFont &font);

    /*!
     * \brief Executes a modal font dialog and returns a font.\n
     * If the user clicks OK, the selected font is returned. If the user clicks
     * Cancel, the initial font is returned.\n
     * The dialog is constructed with the given parent and the options specified
     * in options. \n
     * \param parent The parent widget.
     * \param title The font dialog title.
     * \param initialFont The initial font.
     * \return
     */
    static QFont getFont(QWidget *parent=0,
                         const QString &title=QString(),
                         const QFont &initialFont=QFont());

signals:

public slots:

private slots:
    void onActionOk(const bool &checked);
    void onActionCancel(const bool &checked);

    void onActionSearchFont(const QString &filterText);
    void onActionStyleStatusChange(const int &statusIndex);

private:
    inline void syncFontSize(qreal pointSize,
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

    QFontComboBox *m_fontComboBox;
    QListView *m_fontFamilyList;
    QListWidget *m_sizeListWidget;
    QLineEdit *m_sizeEditor, *m_previewer;
    QSortFilterProxyModel *m_fontFamilyFilter;
    QSlider *m_sizeSlider;
    QFont m_resultFont;
    QStringList m_pointSizeList;
};

#endif // KNFONTDIALOG_H
