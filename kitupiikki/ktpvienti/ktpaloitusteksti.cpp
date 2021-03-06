/*
   Copyright (C) 2017 Arto Hyvättinen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ktpaloitusteksti.h"
#include "ui_ktpaloitusteksti.h"
#include "db/kirjanpito.h"

KtpAloitusTeksti::KtpAloitusTeksti() :
    ui(new Ui::KtpAloitusTeksti)
{
    ui->setupUi(this);
    setTitle("Tilikartan ohje");

    registerField("ohje", ui->introEdit);
    ui->introEdit->setText(kp()->asetukset()->asetus("TilikarttaOhje"));
}

KtpAloitusTeksti::~KtpAloitusTeksti()
{
    delete ui;
}

bool KtpAloitusTeksti::validatePage()
{
    QString ohje = ui->introEdit->toHtml();
    setField("ohje", ohje );
    return true;
}
