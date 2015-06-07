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
#include <QLabel>

#include "knsearchbox.h"

KNSearchBox::KNSearchBox(QWidget *parent) :
    QLineEdit(parent)
{
    //Generate the search icon.
    QPixmap searchIcon(":/common-icon/res/SearchIcon.png");
    QLabel *searchIconWidget=new QLabel(this);
    searchIconWidget->setPixmap(searchIcon);

    //Change the content margins to insert a image.
    QMargins editMargins=contentsMargins();
    editMargins.setLeft(editMargins.left()+searchIcon.width());
    setTextMargins(editMargins);
}
