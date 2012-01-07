/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2012.
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
#include <QListView>

#ifdef _DEBUG
#include "debug_new.h"
#endif

DialogAddRule::DialogAddRule(WidgetSecurity* parent, CSecureRule* pRule) :
	QDialog( parent ),
	m_ui( new Ui::DialogAddRule ),
	m_pParent( parent )
{
	m_ui->setupUi( this );
	m_ui->comboBoxAction->setView( new QListView() );
	m_ui->comboBoxExpire->setView( new QListView() );
	m_ui->comboBoxRuleType->setView( new QListView() );

	if ( pRule )
		m_pRule = pRule->getCopy();
	else
		m_pRule = new CIPRule();

	switch ( m_pRule->type() )
	{
	case security::CSecureRule::srContentAddressRange:
		m_ui->comboBoxRuleType->setCurrentIndex( 1 );
		m_ui->stackedWidgetType->setCurrentIndex( 1 );
		break;
	case security::CSecureRule::srContentCountry:
		m_ui->comboBoxRuleType->setCurrentIndex( 2 );
		m_ui->stackedWidgetType->setCurrentIndex( 2 );
		m_ui->lineEditCountry->setText( m_pRule->getContentString() );
		break;
	case security::CSecureRule::srContentHash:
		m_ui->comboBoxRuleType->setCurrentIndex( 3 );
		m_ui->stackedWidgetType->setCurrentIndex( 3 );
		break;
	case security::CSecureRule::srContentText:
		m_ui->comboBoxRuleType->setCurrentIndex( 4 );
		m_ui->stackedWidgetType->setCurrentIndex( 4 );
		m_ui->lineEditContent->setText( m_pRule->getContentString() );
		break;
	case security::CSecureRule::srContentRegExp:
		m_ui->comboBoxRuleType->setCurrentIndex( 5 );
		m_ui->stackedWidgetType->setCurrentIndex( 5 );
		m_ui->lineEditRegularExpression->setText( m_pRule->getContentString() );
		break;
	case security::CSecureRule::srContentUserAgent:
		m_ui->comboBoxRuleType->setCurrentIndex( 6 );
		m_ui->stackedWidgetType->setCurrentIndex( 6 );
		m_ui->checkBoxUserAgent->setChecked( ((CUserAgentRule*)m_pRule)->getRegExp() );
		m_ui->lineEditUserAgent->setText( m_pRule->getContentString() );
		break;
	case security::CSecureRule::srContentAddress:
	default:
		m_ui->comboBoxRuleType->setCurrentIndex( 0 );
		m_ui->stackedWidgetType->setCurrentIndex( 0 );
		m_ui->lineEditIP->setText( ((CIPRule*)m_pRule)->getContentString() );
		break;
	}
}

DialogAddRule::~DialogAddRule()
{
	delete m_ui;
	delete m_pRule;
}

void DialogAddRule::changeEvent(QEvent* e)
{
	QDialog::changeEvent( e );
	switch ( e->type() )
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi( this );
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

	switch ( m_ui->comboBoxRuleType->currentIndex() )
	{
	case 0:
		pRule = new CIPRule();
		sTmp = m_ui->lineEditIP->text();
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
		sTmp = m_ui->lineEditCountry->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 3:
		pRule = new CHashRule();
		sTmp = m_ui->lineEditHash->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 4:
		pRule = new CContentRule();
		sTmp = m_ui->lineEditContent->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		((CContentRule*)pRule)->setAll( m_ui->radioButtonMatchAll->isChecked() );
		break;
	case 5:
		pRule = new CRegExpRule();
		sTmp = m_ui->lineEditRegularExpression->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		break;
	case 6:
		pRule = new CUserAgentRule();
		sTmp = m_ui->lineEditUserAgent->text();
		if ( !pRule->parseContent( sTmp ) )
		{
			delete pRule;
			pRule = NULL;
		}
		((CUserAgentRule*)pRule)->setRegExp( m_ui->checkBoxUserAgent->isChecked() );
		break;
	default:
		Q_ASSERT( false );
	}

	if ( pRule )
	{
		quint32 tExpire = m_ui->comboBoxExpire->currentIndex();
		if ( tExpire == 2 )
		{
			tExpire = 0;
			tExpire += m_ui->lineEditMinutes->text().toUShort() * 60;
			tExpire += m_ui->lineEditHours->text().toUShort() * 3600;
			tExpire += m_ui->lineEditDays->text().toUShort() * 216000;
			tExpire += static_cast< quint32 >( time( NULL ) );
		}
		pRule->m_tExpire = tExpire;

		pRule->m_sComment = m_ui->lineEditComment->text();
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
		m_ui->lineEditDays->setEnabled( true );
		m_ui->lineEditHours->setEnabled( true );
		m_ui->lineEditMinutes->setEnabled( true );
	}
	else
	{
		m_ui->lineEditDays->setEnabled( false );
		m_ui->lineEditHours->setEnabled( false );
		m_ui->lineEditMinutes->setEnabled( false );
	}
}

void DialogAddRule::on_lineEditMinutes_lostFocus()
{
	quint64 nMinutes = m_ui->lineEditMinutes->text().toULong();
	quint64 nHours = m_ui->lineEditHours->text().toULong();
	quint64 nDays = m_ui->lineEditDays->text().toULong();

	m_ui->lineEditMinutes->setText( QString::number( nMinutes % 60 ) );
	nHours += nMinutes / 60;
	m_ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	m_ui->lineEditDays->setText( QString::number( nDays ) );
}

void DialogAddRule::on_lineEditHours_lostFocus()
{
	quint64 nHours = m_ui->lineEditHours->text().toULong();
	quint64 nDays = m_ui->lineEditDays->text().toULong();

	m_ui->lineEditHours->setText( QString::number( nHours % 24 ) );
	nDays += nHours / 24;
	m_ui->lineEditDays->setText( QString::number( nDays ) );
}

void DialogAddRule::on_lineEditDays_lostFocus()
{
	quint64 nDays = m_ui->lineEditDays->text().toULong();

	m_ui->lineEditDays->setText( QString::number( nDays ) );
}

