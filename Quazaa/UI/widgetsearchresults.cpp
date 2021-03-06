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

#include "widgetsearchresults.h"
#include "ui_widgetsearchresults.h"
#include "dialogfiltersearch.h"

#include "quazaasettings.h"
#include "skinsettings.h"

#include "NetworkCore/query.h"
#include "queryhit.h"
#include "searchtreemodel.h"
#include "NetworkCore/managedsearch.h"
#include "downloads.h"

#include "systemlog.h"

#include <QMessageBox>

#ifdef _DEBUG
#include "debug_new.h"
#endif

WidgetSearchResults::WidgetSearchResults(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::WidgetSearchResults)
{
	ui->setupUi(this);
	labelFilter = new QLabel();
	labelFilter->setText("Filter: ");
	lineEditFilter = new QLineEdit();
	lineEditFilter->setMaximumWidth(150);
	ui->toolBarFilter->insertWidget(ui->actionFilterMore, labelFilter);
	ui->toolBarFilter->insertWidget(ui->actionFilterMore, lineEditFilter);
	restoreState(quazaaSettings.WinMain.SearchToolbar);
	WidgetSearchTemplate* tabSearch = new WidgetSearchTemplate();
	ui->tabWidgetSearch->addTab(tabSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(0);
	connect(tabSearch, SIGNAL(statsUpdated(WidgetSearchTemplate*)), this, SLOT(onStatsUpdated(WidgetSearchTemplate*)));
	ui->splitterSearchDetails->restoreState(quazaaSettings.WinMain.SearchDetailsSplitter);
	emit searchTabChanged(tabSearch);
	setSkin();
}

WidgetSearchResults::~WidgetSearchResults()
{
	delete ui;
}

void WidgetSearchResults::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch(e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WidgetSearchResults::saveWidget()
{
	quazaaSettings.WinMain.SearchToolbar = saveState();
	quazaaSettings.WinMain.SearchDetailsSplitter = ui->splitterSearchDetails->saveState();
}

void WidgetSearchResults::startSearch(QString searchString)
{
	if ( !searchString.isEmpty() )
	{
		WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>( ui->tabWidgetSearch->currentWidget() );
		if ( tabSearch )
		{
			if ( tabSearch->m_pSearch )
			{
				if ( tabSearch->m_sSearchString == searchString )
				{
					tabSearch->StartSearch( tabSearch->m_pSearch->m_pQuery );
					connect( tabSearch, SIGNAL( stateChanged() ), this, SIGNAL( stateChanged() ) );
					emit searchTabChanged( tabSearch );
					emit statsUpdated( tabSearch );
				}
				else
				{
					int result = QMessageBox::question( this, tr( "You have started a new search." ), tr("Would you like to start this search in a new tab?\n\nIf you perform this search in the current tab, you will mix the results of %1 search with the results of the new %2 search.").arg(tabSearch->m_sSearchString).arg(searchString), QMessageBox::Yes|QMessageBox::No);
					if ( result == QMessageBox::Yes )
					{
						addSearchTab();
						tabSearch = qobject_cast<WidgetSearchTemplate*>( ui->tabWidgetSearch->currentWidget() );
					}

					if ( tabSearch )
					{
						CQuery* pQuery = new CQuery();
						pQuery->SetDescriptiveName( searchString );
						tabSearch->StartSearch( pQuery );
						connect( tabSearch, SIGNAL( stateChanged() ), this, SIGNAL( stateChanged() ) );
						ui->tabWidgetSearch->setTabText( ui->tabWidgetSearch->currentIndex(), searchString );
						emit searchTabChanged( tabSearch );
						emit statsUpdated( tabSearch );
					}
				}
			}
			else
			{
				CQuery* pQuery = new CQuery();
				pQuery->SetDescriptiveName( searchString );
				tabSearch->StartSearch( pQuery );
				connect( tabSearch, SIGNAL( stateChanged() ), this, SIGNAL( stateChanged() ) );
				ui->tabWidgetSearch->setTabText( ui->tabWidgetSearch->currentIndex(), searchString );
			}
		}
	}
}

// Makes a new search tab and starts a search in it
void WidgetSearchResults::startNewSearch(QString* searchString)
{
	if(searchString != QString(""))
	{
		addSearchTab();
				WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
				if(tabSearch)
		{
			CQuery* pQuery = new CQuery();
			pQuery->SetDescriptiveName(QString(*searchString));
						tabSearch->StartSearch(pQuery);
						connect(tabSearch, SIGNAL(stateChanged()), this, SIGNAL(stateChanged()));
			ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->currentIndex(), QString(*searchString));
						emit searchTabChanged(tabSearch);
						emit statsUpdated(tabSearch);
		}
	}
}

void WidgetSearchResults::addSearchTab()
{
	WidgetSearchTemplate* tabSearch = new WidgetSearchTemplate();
	int nTab = ui->tabWidgetSearch->addTab(tabSearch, QIcon(":/Resource/Generic/Search.png"), tr("Search"));
	ui->tabWidgetSearch->setCurrentIndex(nTab);
	ui->tabWidgetSearch->setTabsClosable(true);
	connect(tabSearch, SIGNAL(statsUpdated(WidgetSearchTemplate*)), this, SLOT(onStatsUpdated(WidgetSearchTemplate*)));
	connect(tabSearch, SIGNAL(resultsDoubleClicked()), this, SLOT(on_actionSearchDownload_triggered()));
}

void WidgetSearchResults::on_tabWidgetSearch_tabCloseRequested(int index)
{
	WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->widget(index));
	ui->tabWidgetSearch->removeTab(index);
	delete tabSearch;
	if(ui->tabWidgetSearch->count() == 1)
	{
		ui->tabWidgetSearch->setTabsClosable(false);
	}
}

void WidgetSearchResults::stopSearch()
{
	WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if(tabSearch)
	{
		if(tabSearch->m_searchState == SearchState::Searching)
		{
			tabSearch->PauseSearch();
		}
		else if(tabSearch->m_searchState == SearchState::Paused)
		{
			tabSearch->StopSearch();
		}
	}
}

bool WidgetSearchResults::clearSearch()
{
	WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	if(tabSearch)
	{
		//qDebug() << "Clear search captured in WidgetSearchResults.";
		tabSearch->ClearSearch();
		ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->currentIndex(), tr("Search"));
		return true;
	}
	return false;
}

void WidgetSearchResults::on_actionFilterMore_triggered()
{
	DialogFilterSearch* dlgFilterSearch = new DialogFilterSearch(this);
	dlgFilterSearch->show();
}

void WidgetSearchResults::on_splitterSearchDetails_customContextMenuRequested(QPoint pos)
{
	Q_UNUSED(pos);

	if(ui->splitterSearchDetails->handle(1)->underMouse())
	{
		if(ui->splitterSearchDetails->sizes()[1] > 0)
		{
			quazaaSettings.WinMain.SearchResultsSplitterRestoreTop = ui->splitterSearchDetails->sizes()[0];
			quazaaSettings.WinMain.SearchResultsSplitterRestoreBottom = ui->splitterSearchDetails->sizes()[1];
			QList<int> newSizes;
			newSizes.append(ui->splitterSearchDetails->sizes()[0] + ui->splitterSearchDetails->sizes()[1]);
			newSizes.append(0);
			ui->splitterSearchDetails->setSizes(newSizes);
		}
		else
		{
			QList<int> sizesList;
			sizesList.append(quazaaSettings.WinMain.SearchResultsSplitterRestoreTop);
			sizesList.append(quazaaSettings.WinMain.SearchResultsSplitterRestoreBottom);
			ui->splitterSearchDetails->setSizes(sizesList);
		}
	}
}

void WidgetSearchResults::on_tabWidgetSearch_currentChanged(int index)
{
	Q_UNUSED(index);

	WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());
	emit searchTabChanged(tabSearch);
	emit statsUpdated(tabSearch);
	tabSearch->loadHeaderState();
}

void WidgetSearchResults::onStatsUpdated(WidgetSearchTemplate* searchWidget)
{
	ui->tabWidgetSearch->setTabText(ui->tabWidgetSearch->indexOf(searchWidget), (QString("%1 [%2,%3]").arg(searchWidget->m_sSearchString).arg(searchWidget->m_nFiles).arg(searchWidget->m_nHits)));
	if((searchWidget = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget())))
	{
		emit statsUpdated(searchWidget);
	}
}

void WidgetSearchResults::on_actionSearchDownload_triggered()
{
	if( ui->tabWidgetSearch->currentIndex() != -1 )
	{
		WidgetSearchTemplate* tabSearch = qobject_cast<WidgetSearchTemplate*>(ui->tabWidgetSearch->currentWidget());

		if( tabSearch )
		{
			SearchTreeItem* itemSearch = tabSearch->m_pSearchModel->itemFromIndex(tabSearch->CurrentItem());

			if( itemSearch != NULL )
			{
				CQueryHit* pHits = 0;
				CQueryHit* pLast = 0;

				for(int i = 0; i < itemSearch->childCount(); ++i)
				{
					if( pLast )
					{
						pLast->m_pNext = new CQueryHit(itemSearch->child(i)->HitData.pQueryHit.data());
						pLast = pLast->m_pNext;
					}
					else
					{
						pHits = new CQueryHit(itemSearch->child(i)->HitData.pQueryHit.data());
						pLast = pHits;
					}
				}

				Downloads.m_pSection.lock();
				Downloads.add(pHits);
				Downloads.m_pSection.unlock();

				delete pHits;
			}
		}
	}
}

void WidgetSearchResults::setSkin()
{

}
