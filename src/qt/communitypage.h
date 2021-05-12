// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_COMMUNITYPAGE_H
#define BITCOIN_QT_COMMUNITYPAGE_H

#include <QWidget>
#include <memory>

class ClientModel;
class PlatformStyle;
class WalletModel;

namespace Ui {
    class CommunityPage;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Overview ("home") page widget */
class CommunityPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommunityPage(const PlatformStyle *platformStyle, QWidget *parent = nullptr);
    ~CommunityPage();

    void setClientModel(ClientModel *clientModel);

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void openLink(QObject *object);

private:
    Ui::CommunityPage *ui;
    ClientModel *clientModel;
};

#endif // BITCOIN_QT_COMMUNITYPAGE_H
