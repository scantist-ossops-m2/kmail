/*
    SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>
    SPDX-License-Identifier: GPL-2.0-only
*/
#pragma once
#include "historyclosedreaderinfo.h"
#include "kmail_private_export.h"

#include <QObject>

class KMAILTESTS_TESTS_EXPORT HistoryClosedReaderManager : public QObject
{
    Q_OBJECT
public:
    static HistoryClosedReaderManager *self();

    explicit HistoryClosedReaderManager(QObject *parent = nullptr);
    ~HistoryClosedReaderManager() override;

    void addInfo(const HistoryClosedReaderInfo &info);

    [[nodiscard]] HistoryClosedReaderInfo lastInfo() const;

    void clear();

    [[nodiscard]] bool isEmpty() const;

private:
    QList<HistoryClosedReaderInfo> mClosedReaderInfos;
};
