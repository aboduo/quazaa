/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "messageswizardpage.h"

enum Columns
{
	Name,
	Message,
	Highlight
};

MessagesWizardPage::MessagesWizardPage(QWidget* parent) : QWizardPage(parent)
{
	ui.setupUi(this);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/oxygen/64x64/actions/bookmark.png"));
	ui.treeWidget->header()->setSectionResizeMode(Name, QHeaderView::Stretch);
	ui.treeWidget->header()->setSectionResizeMode(Message, QHeaderView::ResizeToContents);
	ui.treeWidget->header()->setSectionResizeMode(Highlight, QHeaderView::ResizeToContents);

	connect(ui.treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SIGNAL(settingsChanged()));
}

QHash<int, bool> MessagesWizardPage::messages() const
{
	QHash<int, bool> messages;
	for (int i = IrcMessageType::Joins; i <= IrcMessageType::Topics; ++i)
		messages[i] = ui.treeWidget->topLevelItem(i)->checkState(Message) == Qt::Checked;
	return messages;
}

void MessagesWizardPage::setMessages(const QHash<int, bool>& messages)
{
	QHashIterator<int, bool> it(messages);
	while (it.hasNext())
	{
		it.next();
		ui.treeWidget->topLevelItem(it.key())->setCheckState(Message, it.value() ? Qt::Checked : Qt::Unchecked);
	}
}

QHash<int, bool> MessagesWizardPage::highlights() const
{
	QHash<int, bool> highlights;
	for (int i = IrcMessageType::Joins; i <= IrcMessageType::Topics; ++i)
		 highlights[i] = ui.treeWidget->topLevelItem(i)->checkState(Highlight) == Qt::Checked;
	return highlights;
}

void MessagesWizardPage::setHighlights(const QHash<int, bool>& highlights)
{
	QHashIterator<int, bool> it(highlights);
	while (it.hasNext())
	{
		it.next();
		ui.treeWidget->topLevelItem(it.key())->setCheckState(Highlight, it.value() ? Qt::Checked : Qt::Unchecked);
	}
}