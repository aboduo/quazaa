#ifndef METALINK4HANDLER_H
#define METALINK4HANDLER_H

#include <QFile>

#include "download.h"
#include "metalinkhandler.h"

class CMetalink4Handler : public CMetalinkHandler
{
public:
	explicit CMetalink4Handler(QFile& oFile = m_oEmptyQFile);

	CDownload* file(const unsigned int& ID) const;

private:
	bool parseFile(QList<MetaFile> &lFiles, quint16 ID);
};

#endif // METALINK4HANDLER_H
