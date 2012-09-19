/*
** hash.h
**
** Copyright © Quazaa Development Team, 2009-2011.
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

#ifndef HASH_H
#define HASH_H

#include "types.h"

struct invalid_hash_exception{};

class CHash
{

public:
	enum Algorithm {SHA1, MD5, MD4};

protected:
	void*				m_pContext;
	bool				m_bFinalized;
	CHash::Algorithm	m_nHashAlgorithm;
	QByteArray			m_baRawValue;

public:
	CHash(const CHash& rhs);
	CHash(CHash::Algorithm algo);
	CHash(QByteArray baRaw, CHash::Algorithm algo);
	~CHash();

	static int	ByteCount(CHash::Algorithm algo);

	static CHash* FromURN(QString sURN);
	static CHash* FromRaw(QByteArray& baRaw, CHash::Algorithm algo);

	QString ToURN();
	QString ToString();
	QByteArray RawValue();

	void AddData(const char* pData, quint32 nLength);
	void AddData(QByteArray& baData);

	QString GetFamilyName();
	void Finalize()
	{
		RawValue();
	}
};

#endif // HASH_H