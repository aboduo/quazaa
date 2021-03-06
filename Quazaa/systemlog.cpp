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

#include "systemlog.h"
#include <QMetaType>
#include <QtCore>

#ifdef _DEBUG
#include "debug_new.h"
#endif

SystemLog systemLog;

SystemLog::SystemLog()
{
	qRegisterMetaType<LogSeverity::Severity>("LogSeverity::Severity");
}

void SystemLog::postLog(LogSeverity::Severity severity, QString message)
{
	static LogSeverity::Severity lastSeverity = LogSeverity::Information;
	static QString lastMessage;
	static int suppressed = 0;
	static bool bCheck = true;

	if( bCheck )
	{
		if(severity == lastSeverity && message == lastMessage)
		{
			suppressed++;
			return;
		}
		else
		{
			if(suppressed > 0)
			{
				bCheck = false;
				postLog(lastSeverity, tr("Suppressed %n identical message(s).", 0, suppressed));
				bCheck = true;
			}
			lastMessage = message;
			lastSeverity = severity;
			suppressed = 0;
		}
	}

	switch(severity)
	{
		case LogSeverity::Debug:
			qDebug() << qPrintable(message);
			break;
		case LogSeverity::Warning:
			qWarning() << qPrintable(message);
			break;
		case LogSeverity::Critical:
			qCritical() << qPrintable(message);
			break;
		default:
			break;
	}
	emit logPosted(message, severity);
}
void SystemLog::postLog(LogSeverity::Severity severity, const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	QString message = QString().vsprintf(format, argList);
	postLog(severity, message);
	va_end(argList);
}

