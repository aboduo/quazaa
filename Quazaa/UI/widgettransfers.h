/*
** widgettransfers.h
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

#ifndef WIDGETTRANSFERS_H
#define WIDGETTRANSFERS_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "widgetdownloads.h"
#include "widgetuploads.h"

namespace Ui
{
	class WidgetTransfers;
}

class WidgetTransfers : public QWidget
{
	Q_OBJECT
public:
	WidgetTransfers(QWidget* parent = 0);
	~WidgetTransfers();
	WidgetDownloads* panelDownloads;
	WidgetUploads* panelUploads;
	void saveWidget();

protected:
	void changeEvent(QEvent* e);

private:
	Ui::WidgetTransfers* ui;

private slots:
	void on_splitterTransfers_customContextMenuRequested(QPoint pos);
	void setSkin();
	void on_splitterUploads_customContextMenuRequested(const QPoint &pos);
	void on_splitterDownloads_customContextMenuRequested(const QPoint &pos);
	void on_treeWidgetTransfers_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif // WIDGETTRANSFERS_H
