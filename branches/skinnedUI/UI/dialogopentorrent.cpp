//
// dialogopentorrent.cpp
//
// Copyright � Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "dialogopentorrent.h"
#include "ui_dialogopentorrent.h"
#include "QSkinDialog/qskinsettings.h"

#include <QListView>

DialogOpenTorrent::DialogOpenTorrent(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogOpenTorrent)
{
	ui->setupUi(this);
	ui->comboBoxAllocationMode->setView(new QListView());
	connect(&skinSettings, SIGNAL(skinChanged()), this, SLOT(skinChangeEvent()));
	skinChangeEvent();
}

DialogOpenTorrent::~DialogOpenTorrent()
{
	delete ui;
}

void DialogOpenTorrent::changeEvent(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogOpenTorrent::on_pushButtonOK_clicked()
{
	emit closed();
	close();
}

void DialogOpenTorrent::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogOpenTorrent::skinChangeEvent()
{
	ui->frameCommonHeader->setStyleSheet(skinSettings.dialogHeader);
}