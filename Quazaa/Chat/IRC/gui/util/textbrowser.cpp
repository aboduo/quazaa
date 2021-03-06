/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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

#include "textbrowser.h"
#include <QApplication>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent), ub(-1), bud(0)
{
    unreadLineBrush = qApp->palette().highlight();
}

QWidget* TextBrowser::buddy() const
{
    return bud;
}

void TextBrowser::setBuddy(QWidget* buddy)
{
    bud = buddy;
}

int TextBrowser::unseenBlock() const
{
    return ub;
}

void TextBrowser::setUnseenBlock(int block)
{
    ub = block;
}

QColor TextBrowser::unreadLineColor() const
{
    return unreadLineBrush.color();
}

void TextBrowser::setUnreadLineColor(const QColor &color)
{
    unreadLineBrush.setColor(color);
}

void TextBrowser::append(const QString &text)
{
    if (!text.isEmpty()) {
        if (buffer.isEmpty())
            QMetaObject::invokeMethod(this, "appendBuffer", Qt::QueuedConnection);

        buffer += text;

        if (!isVisible() && ub == -1)
            ub = document()->blockCount() - 1;
    }
}

void TextBrowser::keyPressEvent(QKeyEvent* event)
{
    // for example:
    // - Ctrl+C goes to the browser
    // - Ctrl+V goes to the buddy
    // - Shift+7 ("/") goes to the buddy
    switch (event->key()) {
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Meta:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
            break;
        default:
            if (!event->matches(QKeySequence::Copy) && !event->matches(QKeySequence::SelectAll)) {
                QApplication::sendEvent(bud, event);
                bud->setFocus();
                return;
            }
            break;
    }
    QTextBrowser::keyPressEvent(event);
}

void TextBrowser::resizeEvent(QResizeEvent* event)
{
    QTextBrowser::resizeEvent(event);

    // http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=240940
    QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

void TextBrowser::scrollToTop()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void TextBrowser::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void TextBrowser::scrollToNextPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void TextBrowser::scrollToPreviousPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void TextBrowser::paintEvent(QPaintEvent* event)
{
    QTextBrowser::paintEvent(event);

    QPainter painter(viewport());

    QTextBlock block;
    if (ub > 0)
        block = document()->findBlockByNumber(ub);

    if (block.isValid()) {
        painter.save();
        painter.setBrush(unreadLineBrush);
        painter.setPen(Qt::DashLine);
        painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());

        QRectF br = document()->documentLayout()->blockBoundingRect(block);
        painter.drawLine(br.topLeft(), br.topRight());
        painter.restore();
    }

    QLinearGradient gradient(0, 0, 0, 3);
    gradient.setColorAt(0.0, palette().color(QPalette::Dark));
    gradient.setColorAt(1.0, Qt::transparent);
    painter.fillRect(0, 0, width(), 3, gradient);
}

void TextBrowser::wheelEvent(QWheelEvent* event)
{
#ifdef Q_WS_MACX
    // disable cmd+wheel zooming on mac
    QAbstractScrollArea::wheelEvent(event);
#else
    QTextBrowser::wheelEvent(event);
#endif // Q_WS_MACX
}

void TextBrowser::appendBuffer()
{
    QScrollBar* vbar = verticalScrollBar();
    const bool atBottom = vbar->value() >= vbar->maximum();

    QTextDocument* doc = document();
    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::End);

    foreach (const QString& line, buffer) {
        cursor.beginEditBlock();

        if (!doc->isEmpty())
            cursor.insertBlock();

#if QT_VERSION >= 0x040800
        QTextBlockFormat format = cursor.blockFormat();
        format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
        cursor.setBlockFormat(format);
#endif // QT_VERSION

        cursor.insertHtml(line);
        cursor.endEditBlock();
    }
    buffer.clear();

    if (atBottom)
        vbar->setValue(vbar->maximum());
}
