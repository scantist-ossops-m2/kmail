/*
   SPDX-FileCopyrightText: 2014-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "ui_sendlaterconfigurewidget.h"

#include <Akonadi/Item>

#include <KConfigGroup>
#include <QTreeWidgetItem>

namespace MessageComposer
{
class SendLaterInfo;
}

class SendLaterItem : public QTreeWidgetItem
{
public:
    explicit SendLaterItem(QTreeWidget *parent = nullptr);
    ~SendLaterItem() override;

    void setInfo(MessageComposer::SendLaterInfo *info);
    Q_REQUIRED_RESULT MessageComposer::SendLaterInfo *info() const;

private:
    MessageComposer::SendLaterInfo *mInfo = nullptr;
};

class SendLaterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SendLaterWidget(QWidget *parent = nullptr);
    ~SendLaterWidget() override;

    enum SendLaterColumn {
        To = 0,
        Subject,
        SendAround,
        Recursive,
        MessageId,
    };

    void save();
    void saveTreeWidgetHeader(KConfigGroup &group);
    void restoreTreeWidgetHeader(const QByteArray &group);
    void needToReload();
    Q_REQUIRED_RESULT QList<Akonadi::Item::Id> messagesToRemove() const;

Q_SIGNALS:
    void sendNow(Akonadi::Item::Id);

private:
    void slotDeleteItem();
    void slotModifyItem();
    void updateButtons();
    void slotCustomContextMenuRequested(const QPoint &);
    void slotSendNow();
    void createOrUpdateItem(MessageComposer::SendLaterInfo *info, SendLaterItem *item = nullptr);
    void load();
    QList<Akonadi::Item::Id> mListMessagesToRemove;
    bool mChanged = false;
    Ui::SendLaterConfigureWidget *mWidget = nullptr;
};
