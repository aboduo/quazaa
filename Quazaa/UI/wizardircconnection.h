#ifndef WIZARDIRCCONNECTION_H
#define WIZARDIRCCONNECTION_H

#include <QWizard>
#include "completerlineedit.h"

#include "connectioninfo.h"

namespace Ui {
class WizardIrcConnection;
}

class WizardIrcConnection : public QWizard
{
	Q_OBJECT

public:
	explicit WizardIrcConnection(QWidget *parent = 0);
	~WizardIrcConnection();

	enum Page
	{
		UserPage,
		ServerPage,
		ConnectionPage
	};

	QString nickName() const;
	void setNickName(const QString& nickName);

	QString realName() const;
	void setRealName(const QString& realName);

	ConnectionInfo connection() const;
	void setConnection(const ConnectionInfo& connection);

	QString hostName() const;
	void setHostName(const QString& hostName);

	quint16 port() const;
	void setPort(quint16 port);

	bool isSecure() const;
	void setSecure(bool secure);

    QString userName() const;
    void setUserName(const QString& userName);

	QString password() const;
	void setPassword(const QString& password);

	QString connectionName() const;
	void setConnectionName(const QString& name);

	bool isComplete() const;

private:
	Ui::WizardIrcConnection *ui;
    CompleterLineEdit* lineEditNickName;
    CompleterLineEdit* lineEditRealName;
    CompleterLineEdit* lineEditHost;
    CompleterLineEdit* lineEditUserName;
    CompleterLineEdit* lineEditConnectionName;

private slots:
	void accept();
	void completeChanged();
};

#endif // WIZARDIRCCONNECTION_H
