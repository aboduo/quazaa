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

#include "dialogaddrule.h"
#include "ui_dialogaddrule.h"
#include "skinsettings.h"

#include <QListView>

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogAddRule::DialogAddRule(WidgetSecurity* parent, CSecureRule* pRule) :
	QDialog( parent ),
	ui( new Ui::DialogAddRule ),
	m_pParent( parent )
{
	ui->setupUi( this );
	ui->comboBoxAction->setView( new QListView() );
	ui->comboBoxExpire->setView( new QListView() );
	ui->comboBoxRuleType->setView( new QListView() );

	if ( pRule )
	{
		m_pRule = pRule->getCopy();
		ui->lineEditComment->setText( m_pRule->m_sComment );
	}
	else
		m_pRule = new CIPRule();

	switch ( m_pRule->type() )
	{
	case Security::CSecureRule::srContentAddressRange:
		ui->comboBoxRuleType->setCurrentIndex( 1 );
		ui->stackedWidgetType->setCurrentIndex( 1 );
		break;
	case Security::CSecureRule::srContentCountry:
		ui->comboBoxRuleType->setCurrentIndex( 2 );
		ui->stackedWidgetType->setCurrentIndex( 2 );
		ui->lineEditCountry->setText( m_pRule->getContentString() );
		break;
	case Security::CSecureRule::srContentHash:
		ui->comboBoxRuleType->setCurrentIndex( 3 );
		ui->stackedWidgetType->setCurrentIndex( 3 );
		break;
	case Security::CSecureRule::srContentText:
		ui->comboBoxRuleType->setCurrentIndex( 4 );
		ui->stackedWidgetType->setCurrentIndex( 4 );
		ui->lineEditContent->setText( m_pRule->getContentString() );
		break;
	case Security::CSecureRule::srContentRegExp:
		ui->comboBoxRuleType->setCurrentIndex( 5 );
		ui->stackedWidgetType->setCurrentIndex( 5 );
		ui->lineEditRegularExpression->setText( m_pRule->getContentString() );
		break;
	case Security::CSecureRule::srContentUserAgent:
		ui->comboBoxRuleType->setCurrentIndex( 6 );
		ui->stackedWidgetType->setCurrentIndex( 6 );
		ui->checkBoxUserAgent->setChecked( ((CUserAgentRule*)m_pRule)->getRegExp() );
		ui->lineEditUserAgent->setText( m_pRule->getContentString() );
		break;
	case Security::CSecureRule::srContentAddress:
	default:
		ui->comboBoxRuleType->setCurrentIndex( 0 );
		ui->stackedWidgetType->setCurrentIndex( 0 );
		ui->lineEditIP->setText( ((CIPRule*)m_pRule)->getContentString() );
		break;
	}
	setSkin();
}

DialogAddRule::~DialogAddRule()
{
	delete ui;
	delete m_pRule;
}

void DialogAddRule::changeEvent(QEvent* e)
{
	QDialog::changeEvent( e );
	switch ( e->type() )
	{
		case QEvent::LanguageChange:
			ui->retranslateUi( this );
			break;
		default:
			break;
	}
}

// TODO: change user interface for IP ranges and hashes.
void DialogAddRule::on_pushButtonOK_clicked()
{
	CSecureRule* pRule = NULL;
	QString sTmp;

	switch ( ui->comboBoxRuleType->currentIndex() )
	{
	case 0:
		pRule = new CIPRule();
		sTmp = ui->lineEditIP->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 1:
		pRule = new CIPRangeRule();

		break;
	case 2:
		pRule = new CCountryRule();
		sTmp = ui->lineEditCountry->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 3:
		pRule = new CHashRule();
		sTmp = ui->lineEditURI->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 4:
		pRule = new CContentRule();
		sTmp = ui->lineEditContent->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		((CContentRule*)pRule)->setAll( ui->radioButtonMatchAll->isChecked() );
		break;
	case 5:
		pRule = new CRegExpRule();
		sTmp = ui->lineEditRegularExpression->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 6:
		pRule = new CUserAgentRule();
		sTmp = ui->lineEditUserAgent->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		((CUserAgentRule*)pRule)->setRegExp( ui->checkBoxUserAgent->isChecked() );
		break;
	default:
		Q_ASSERT( false );
	}

	if ( pRule )
	{
		quint32 tExpire = ui->comboBoxExpire->currentIndex();
		if ( tExpire == 2 )
		{
			tExpire = 0;
			tExpire += ui->lineEditMinutes->text().toUShort() * 60;
			tExpire += ui->lineEditHours->text().toUShort() * 3600;
			tExpire += ui->lineEditDays->text().toUShort() * 216000;
			tExpire += static_cast< quint32 >( time( NULL ) );
		}
		pRule->m_tExpire = tExpire;

		pRule->m_sComment = ui->lineEditComment->text();
		pRule->m_oUUID = m_pRule->m_oUUID;

		if ( *pRule != *m_pRule )
		{
			securityManager.remove( m_pRule );
			securityManager.add( pRule );
		}
	}

	emit dataUpdated();
	emit closed();
	close();
}

void DialogAddRule::on_pushButtonCancel_clicked()
{
	emit closed();
	close();
}

void DialogAddRule::on_comboBoxExpire_currentIndexChanged(int index)
{
	if ( index == 2 )
	{
		ui->lineEditDays->setEnabled( true );
		ui->lineEditHours->setEnabled( true );
		ui->lineEditMinutes->setEnabled( true );
	}
	else
	{
		ui->lineEditDays->setEnabled( false );
		ui->lineEditHours->setEnabled( false );
		ui->lineEditMinutes->setEnabled( false );
	}
}

void DialogAddRule::on_lineEditMinutes_lostFocus()
{
	quint64 nMinutes = ui->lineEditMinutes->text().toULong();
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditMinutes->setText( QString::number( nMinutes % 60 ) );
	nHours += nMinutes / 60;
	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}

void DialogAddRule::on_lineEditHours_lostFocus()
{
	quint64 nHours = ui->lineEditHours->text().toULong();
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	ui->lineEditDays->setText( QString::number( nDays ) );
}

void DialogAddRule::on_lineEditDays_lostFocus()
{
	quint64 nDays = ui->lineEditDays->text().toULong();

	ui->lineEditDays->setText( QString::number( nDays ) );
}

void DialogAddRule::setSkin()
{

}
