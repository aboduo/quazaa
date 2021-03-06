/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public 
** License version 3.0 requirements will be met: 
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version 
** 3.0 along with Quazaa; if not, write to the Free Software Foundation, 
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "widgettransfers.h"
#include "ui_widgettransfers.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

#include <QDebug>

WidgetTransfers::WidgetTransfers(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::WidgetTransfers)
{
	ui->setupUi(this);
	ui->splitterTransfers->restoreState(quazaaSettings.WinMain.TransfersSplitter);
	panelDownloads = new WidgetDownloads();
	ui->verticalLayoutDownloads->addWidget(panelDownloads);
	panelUploads = new WidgetUploads();
	ui->verticalLayoutUploads->addWidget(panelUploads);
	ui->splitterDownloads->restoreState(quazaaSettings.WinMain.DownloadsSplitter);
	ui->splitterUploads->restoreState(quazaaSettings.WinMain.UploadsSplitter);
	ui->stackedWidgetTransfers->setCurrentIndex(0);
	ui->tabWidgetDownloadDetails->setCurrentIndex(0);
	setSkin();
}

WidgetTransfers::~WidgetTransfers()
{
	delete ui;
}

void WidgetTransfers::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetTransfers::saveWidget()
{
	quazaaSettings.WinMain.TransfersSplitter = ui->splitterTransfers->saveState();
	quazaaSettings.WinMain.DownloadsSplitter = ui->splitterDownloads->saveState();
	quazaaSettings.WinMain.UploadsSplitter = ui->splitterUploads->saveState();
	panelDownloads->saveWidget();
	panelUploads->saveWidget();
}

void WidgetTransfers::on_splitterTransfers_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterTransfers->handle(1)->underMouse())
	{
		if(ui->splitterTransfers->sizes()[0] > 0)
		{
			quazaaSettings.WinMain.TransfersSplitterRestoreLeft = ui->splitterTransfers->sizes()[0];
			quazaaSettings.WinMain.TransfersSplitterRestoreRight = ui->splitterTransfers->sizes()[1];
			QList<int> newSizes;
			newSizes.append(0);
			newSizes.append(ui->splitterTransfers->sizes()[0] + ui->splitterTransfers->sizes()[1]);
			ui->splitterTransfers->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreLeft);
			sizesList.append(quazaaSettings.WinMain.TransfersSplitterRestoreRight);
			ui->splitterTransfers->setSizes(sizesList);
		}
	}
}

void WidgetTransfers::setSkin()
{

}

void WidgetTransfers::on_splitterUploads_customContextMenuRequested(const QPoint &pos)
{
	Q_UNUSED(pos);

	if(ui->splitterUploads->handle(1)->underMouse())
	{
		if(ui->splitterUploads->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.UploadsSplitterRestoreTop = ui->splitterUploads->sizes()[0];
			quazaaSettings.WinMain.UploadsSplitterRestoreBottom = ui->splitterUploads->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterUploads->sizes()[0] + ui->splitterUploads->sizes()[1]);
			newSizes.append(0);
			ui->splitterUploads->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.UploadsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.UploadsSplitterRestoreBottom);
			ui->splitterUploads->setSizes(sizesList);
		}
	}
}

void WidgetTransfers::on_splitterDownloads_customContextMenuRequested(const QPoint &pos)
{
	Q_UNUSED(pos);

	if(ui->splitterDownloads->handle(1)->underMouse())
	{
		if(ui->splitterDownloads->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.DownloadsSplitterRestoreTop = ui->splitterDownloads->sizes()[0];
			quazaaSettings.WinMain.DownloadsSplitterRestoreBottom = ui->splitterDownloads->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterDownloads->sizes()[0] + ui->splitterDownloads->sizes()[1]);
			newSizes.append(0);
			ui->splitterDownloads->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.DownloadsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.DownloadsSplitterRestoreBottom);
			ui->splitterDownloads->setSizes(sizesList);
		}
	}
}

void WidgetTransfers::on_treeWidgetTransfers_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous);

	if(current->statusTip(0) == "Downloads")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(0);
	}

	if(current->statusTip(0) == "CompletedDownloads")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(0);
	}

	if(current->statusTip(0) == "Paused")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(0);
	}

	if(current->statusTip(0) == "Active")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(0);
	}

	if(current->statusTip(0) == "Inactive")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(0);
	}

	if(current->statusTip(0) == "Uploads")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(1);
	}

	if(current->statusTip(0) == "CompletedUploads")
	{
		ui->stackedWidgetTransfers->setCurrentIndex(1);
	}
}
