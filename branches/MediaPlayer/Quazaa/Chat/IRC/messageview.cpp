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

#include "messageview.h"
#include "session.h"
#include "completer.h"
#include "settingswizard.h"
#include <QStringListModel>
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>
#include <irccommand.h>
#include <ircutil.h>
#include <irc.h>

#include "quazaasettings.h"

QStringListModel* MessageView::MessageViewData::commandModel = 0;

MessageView::MessageView(IrcSession* session, QWidget* parent) :
    QWidget(parent)
{
	d.setupUi(this);

	d.m_bIsStatusChannel = false;

	closeButton = new QToolButton(this);
	closeButton->setIcon(QIcon(":/Resource/Generic/Exit.png"));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(part()));

    setFocusProxy(d.lineEditor);
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

	QFont font("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	d.textBrowser->setFont(font);

    d.session = session;
    d.formatter.setHightlights(QStringList(session->nickName()));
    connect(&d.parser, SIGNAL(customCommand(QString,QStringList)), this, SLOT(onCustomCommand(QString,QStringList)));

    d.userModel = new QStringListModel(this);

    if (!d.commandModel)
    {
        CommandParser::addCustomCommand("CONNECT", "(<host> <port>)");
        CommandParser::addCustomCommand("QUERY", "<user>");
        CommandParser::addCustomCommand("SETTINGS", "");

        QStringList prefixedCommands;
        foreach (const QString& command, CommandParser::availableCommands())
            prefixedCommands += "/" + command;

        d.commandModel = new QStringListModel(qApp);
        d.commandModel->setStringList(prefixedCommands);
    }

    d.lineEditor->completer()->setDefaultModel(d.userModel);
    d.lineEditor->completer()->setSlashModel(d.commandModel);

    connect(d.lineEditor, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
    connect(d.lineEditor, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

    d.helpLabel->hide();
	d.findFrame->setTextEdit(d.textBrowser);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

	connect(&quazaaSettings, SIGNAL(chatSettingsChanged()), this, SLOT(applySettings()));
	applySettings();
}

MessageView::~MessageView()
{
}

QString MessageView::receiver() const
{
    return d.receiver;
}

void MessageView::setReceiver(const QString& receiver)
{
    d.receiver = receiver;
}

void MessageView::setStatusChannel(bool statusChannel)
{
	d.m_bIsStatusChannel = statusChannel;
}

bool MessageView::isChannelView() const
{
    if (d.receiver.isEmpty())
        return false;

    switch (d.receiver.at(0).unicode())
    {
        case '#':
        case '&':
        case '!':
        case '+':
            return true;
        default:
			return false;
    }
}

bool MessageView::isStatusChannel() const
{
	return d.m_bIsStatusChannel;
}

void MessageView::showHelp(const QString& text, bool error)
{
    QString syntax;
    if (text == "/")
    {
        QStringList commands = CommandParser::availableCommands();
        syntax = commands.join(" ");
    }
    else if (text.startsWith('/'))
    {
        QStringList words = text.mid(1).split(' ');
        QString command = words.value(0);
        QStringList suggestions = CommandParser::suggestedCommands(command, words.mid(1));
        if (suggestions.count() == 1)
            syntax = CommandParser::syntax(suggestions.first());
        else
            syntax = suggestions.join(" ");

        if (syntax.isEmpty() && error)
            syntax = tr("Unknown command '%1'").arg(command.toUpper());
    }

    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error)
        pal.setColor(QPalette::WindowText, Qt::red);
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void MessageView::appendMessage(const QString& message)
{
    if (!message.isEmpty())
    {
        // workaround the link activation merge char format bug
        QString copy = message;
        if (copy.endsWith("</a>"))
            copy += " ";

        d.textBrowser->append(copy);
    }
}

bool MessageView::eventFilter(QObject* receiver, QEvent* event)
{
    Q_UNUSED(receiver);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // for example:
        // - Ctrl+C goes to the browser
        // - Ctrl+V goes to the line edit
        // - Shift+7 ("/") goes to the line edit
        switch (keyEvent->key())
        {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
                break;
            default:
                if (!keyEvent->matches(QKeySequence::Copy))
                {
                    QApplication::sendEvent(d.lineEditor, keyEvent);
                    d.lineEditor->setFocus();
                    return true;
                }
                break;
        }
    }
    return false;
}

void MessageView::onEscPressed()
{
    d.helpLabel->hide();
    d.findFrame->hide();
    setFocus(Qt::OtherFocusReason);
}

void MessageView::onSend(const QString& text)
{
    IrcCommand* cmd = d.parser.parseCommand(d.receiver, text);
    if (cmd)
    {
        d.session->sendCommand(cmd);

        if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction)
        {
            IrcMessage* msg = IrcMessage::fromCommand(d.session->nickName(), cmd);
            receiveMessage(msg);
            delete msg;
        }
        else if (cmd->type() == IrcCommand::Quit)
        {
            emit aboutToQuit();
        }
    }
    else if (d.parser.hasError())
    {
        showHelp(text, true);
    }
}

void MessageView::part()
{
	if(!isChannelView())
	{
		if(isStatusChannel())
		{
			onSend("/quit");
		} else {
			emit closeQuery(this);
		}
	} else {
		onSend("/part");
	}
}

void MessageView::applySettings()
{
	d.formatter.setTimeStamp(quazaaSettings.Chat.TimeStamp);
	d.textBrowser->document()->setMaximumBlockCount(quazaaSettings.Chat.MaxBlockCount);

	QString backgroundColor = quazaaSettings.Chat.Colors.value(IRCColorType::Background);
    d.textBrowser->setStyleSheet(QString("QTextBrowser { background-color: %1 }").arg(backgroundColor));

    d.textBrowser->document()->setDefaultStyleSheet(
        QString(
            ".highlight { color: %1 }"
            ".message   { color: %2 }"
            ".notice    { color: %3 }"
            ".action    { color: %4 }"
            ".event     { color: %5 }"
		).arg(quazaaSettings.Chat.Colors.value((IRCColorType::Highlight)))
		 .arg(quazaaSettings.Chat.Colors.value((IRCColorType::Message)))
		 .arg(quazaaSettings.Chat.Colors.value((IRCColorType::Notice)))
		 .arg(quazaaSettings.Chat.Colors.value((IRCColorType::Action)))
		 .arg(quazaaSettings.Chat.Colors.value((IRCColorType::Event))));
}

void MessageView::receiveMessage(IrcMessage* message)
{
    bool append = true;
    bool hilite = false;
    bool matches = false;

    switch (message->type())
    {
    case IrcMessage::Join:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Joins);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Joins);
        break;
    case IrcMessage::Kick:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Kicks);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Kicks);
        break;
    case IrcMessage::Mode:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Modes);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Modes);
        break;
    case IrcMessage::Nick:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Nicks);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Nicks);
        break;
    case IrcMessage::Notice:
        matches = static_cast<IrcNoticeMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Part:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Parts);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Parts);
        break;
    case IrcMessage::Private:
        matches = !isChannelView() || static_cast<IrcPrivateMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Quit:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Quits);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Quits);
        break;
    case IrcMessage::Topic:
		append = quazaaSettings.Chat.Messages.value(IRCMessageType::Topics);
		hilite = quazaaSettings.Chat.Highlights.value(IRCMessageType::Topics);
        break;
    case IrcMessage::Unknown:
        qWarning() << "unknown:" << message;
        append = false;
        break;
    case IrcMessage::Invite:
    case IrcMessage::Numeric:
    case IrcMessage::Ping:
    case IrcMessage::Pong:
    case IrcMessage::Error:
        break;
    }

    if (matches)
        emit alert(this, true);
    else if (hilite)
        emit highlight(this, true);
    if (append)
        appendMessage(d.formatter.formatMessage(message));
}

void MessageView::addUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    users.append(user);
    d.userModel->setStringList(users);
}

void MessageView::removeUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    if (users.removeOne(user))
        d.userModel->setStringList(users);
}

void MessageView::onCustomCommand(const QString& command, const QStringList& params)
{
    if (command == "QUERY")
        emit query(params.value(0));
    else if (command == "SETTINGS")
	{
		SettingsWizard wizard(qApp->activeWindow());
		wizard.exec();
	}
    else if (command == "CONNECT")
        QMetaObject::invokeMethod(window(), "connectTo", Q_ARG(QString, params.value(0)), params.count() > 1 ? Q_ARG(quint16, params.value(1).toInt()) : QGenericArgument());
}