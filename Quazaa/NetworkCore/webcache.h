/*
** webcache.h
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

#ifndef WEBCACHE_H
#define WEBCACHE_H

#include <QList>
#include <QUrl>
#include <QNetworkAccessManager>

#include <time.h>

class QNetworkReply;

// Przeniesc w lepsze miejsce
const quint32 RequeryTime = 3600;

class CWebCacheHost
{
public:
	QUrl    m_sUrl;
	quint32 m_tLastQuery;

	CWebCacheHost(QUrl url)
	{
		m_sUrl = url;
		m_tLastQuery = 0;
	}
	CWebCacheHost(QUrl url, quint32 ts)
	{
		m_sUrl = url;
		m_tLastQuery = ts;
	}

	inline bool CanQuery()
	{
		if(time(0) - m_tLastQuery > RequeryTime)
		{
			return true;
		}

		return false;
	}
};

class CWebCache: public QObject
{
	Q_OBJECT

protected:
	QList<CWebCacheHost>    m_lCaches;
	bool                    m_bRequesting;
	QNetworkAccessManager* m_pRequest;
	QNetworkReply*          m_pReply;
public:
	CWebCache();
	~CWebCache();
	void RequestRandom();
	void CancelRequests();

	inline bool isRequesting()
	{
		return m_bRequesting;
	}

public slots:
	void OnRequestComplete(QNetworkReply* pReply);
};

extern CWebCache WebCache;
#endif // WEBCACHE_H
