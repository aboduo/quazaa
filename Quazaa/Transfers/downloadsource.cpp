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

#include "downloadsource.h"
#include "queryhit.h"
#include "downloads.h"
#include "download.h"

#include "transfer.h"

#ifdef _DEBUG
#include "debug_new.h"
#endif

CDownloadSource::CDownloadSource(CDownload *pDownload, QObject *parent)
	: QObject(parent),
	  m_bPush(false),
	  m_nFailures(0),
	  m_pDownload(pDownload),
	  m_pTransfer(0),
	  m_lAvailableFrags(pDownload->m_nSize),
	  m_lDownloadedFrags(pDownload->m_nSize)
{
	m_tNextAccess = time(0);
}

CDownloadSource::CDownloadSource(CDownload *pDownload, CQueryHit *pHit, QObject *parent)
	: QObject(parent),
	  m_pDownload(pDownload),
	  m_pTransfer(0),
	  m_lAvailableFrags(pDownload->m_nSize),
	  m_lDownloadedFrags(pDownload->m_nSize)
{
	m_oAddress = pHit->m_pHitInfo->m_oNodeAddress;
	m_bPush = false; // todo
	m_oPushProxies << pHit->m_pHitInfo->m_lNeighbouringHubs;
	m_nProtocol = tpHTTP;
	m_nNetwork = dpG2;
	m_oGUID = pHit->m_pHitInfo->m_oNodeGUID;
	m_lHashes << pHit->m_lHashes;
	m_tNextAccess = time(0);
	m_nFailures = 0;
	m_sURL = pHit->m_sURL;
}

CDownloadSource::~CDownloadSource()
{
	closeTransfer();
	m_pDownload->removeSource(this);
}

CTransfer *CDownloadSource::createTransfer()
{
	ASSUME_LOCK(Downloads.m_pSection);

	CTransfer* pTransfer = 0;

	switch(m_nProtocol)
	{
		case tpHTTP:
			break;
		case tpBitTorrent:
			break;
		default:
			Q_ASSERT_X(0, "CDownloadSource::createTransfer()", "Invalid protocol id specified");
	}

	if( pTransfer )
		emit transferCreated();

	return pTransfer;
}

void CDownloadSource::closeTransfer()
{
	ASSUME_LOCK(Downloads.m_pSection);

	if( m_pTransfer )
	{
		delete m_pTransfer;
		m_pTransfer = 0;
		emit transferClosed();
	}
}

QDataStream& operator<<(QDataStream& s, const CDownloadSource& rhs)
{
	if( !rhs.m_bPush ) // do not store push sources (they may be useless after restart)
	{
		s << "download-source" << quint32(1); // version
		s << "address" << rhs.m_oAddress;
		s << "protocol" << rhs.m_nProtocol;
		s << "network" << rhs.m_nNetwork;
		s << "guid" << rhs.m_oGUID;
		s << "url" << rhs.m_sURL;
		s << "download-source-end";
	}

	return s;
}
QDataStream& operator>>(QDataStream& s, CDownloadSource& rhs)
{
	// download-source tag is handled elsewhere

	quint32 nVer = 0;
	s >> nVer;

	if( nVer == 1 )
	{
		QByteArray sTag;
		s >> sTag;
		sTag.chop(1);

		while( sTag != "download-source-end" && s.status() == QDataStream::Ok )
		{
			if( sTag == "address")
			{
				s >> rhs.m_oAddress;
			}
			else if( sTag == "protocol" )
			{
				// TODO: sanity check...
				int x;
				s >> x;
				rhs.m_nProtocol = static_cast<TransferProtocol>(x);
			}
			else if( sTag == "network" )
			{
				int x;
				s >> x;
				rhs.m_nNetwork = static_cast<DiscoveryProtocol>(x);
			}
			else if( sTag == "guid" )
			{
				s >> rhs.m_oGUID;
			}
			else if( sTag == "url" )
			{
				s >> rhs.m_sURL;
			}

			s >> sTag;
			sTag.chop(1);
		}
	}

	return s;
}
