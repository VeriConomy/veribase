// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/overviewpage.h>
#include <qt/forms/ui_overviewpage.h>

#include <interfaces/node.h>

#include <qt/bitcoinunits.h>
#include <qt/clientmodel.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/transactionfilterproxy.h>
#include <qt/transactiontablemodel.h>
#include <qt/walletmodel.h>


#include <QAbstractItemDelegate>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

#define DECORATION_SIZE 35
#define ICON_SIZE 16
#define MARGIN_SIZE 6
#define NUM_ITEMS 7

Q_DECLARE_METATYPE(interfaces::WalletBalances)

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit TxViewDelegate(const PlatformStyle *_platformStyle, QObject *parent=nullptr):
        QAbstractItemDelegate(parent), unit(BitcoinUnits::BTC),
        platformStyle(_platformStyle)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(TransactionTableModel::RawDecorationRole));
        QRect mainRect = option.rect;

        // Anti aliasing + border
        painter->setRenderHint(QPainter::Antialiasing);
        QPen pen(QColor(217, 217, 217), 1);
        painter->setPen(pen);

        // create rounded rectangle that contain the TX
        QPainterPath mainPath;
        QRect txRect(mainRect.left() + MARGIN_SIZE, mainRect.top() + MARGIN_SIZE, mainRect.width() - MARGIN_SIZE*2, DECORATION_SIZE);
        mainPath.addRoundedRect(txRect, 5, 5);
        painter->fillPath(mainPath, QColor(230, 230, 230));
        painter->drawPath(mainPath);

        // Add icon
        QRect iconRect(txRect.left() + 5, txRect.top() + ((DECORATION_SIZE - ICON_SIZE)/2), ICON_SIZE, ICON_SIZE );
        icon.paint(painter, iconRect);

        // Create Text Rect
        QRect textRect(iconRect.right() + 5, txRect.top(), txRect.width() - iconRect.right() - 5, DECORATION_SIZE);

        // Set default font
        QFont font("Lato", 9);
        painter->setPen(QColor(102, 102, 102));
        painter->setFont(font);

        // Write Date & Address
        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        painter->drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, QString("%1  -  %2").arg(GUIUtil::dateTimeStr(date)).arg(address));

        // Write amount
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();

        QColor foreground = COLOR_POSITIVE;
        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = COLOR_UNCONFIRMED;
        }

        painter->setPen(foreground);
        QFont fontHeavy("Lato", 9, QFont::ExtraBold);
        painter->setFont(fontHeavy);

        QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true, BitcoinUnits::separatorAlways);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(textRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE+MARGIN_SIZE*2);
    }

    int unit;
    const PlatformStyle *platformStyle;

};


#include <qt/overviewpage.moc>

OverviewPage::OverviewPage(interfaces::Node& node, const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    m_node(node),
    ui(new Ui::OverviewPage),
    clientModel(nullptr),
    walletModel(nullptr),
    txdelegate(new TxViewDelegate(platformStyle, this))
{
    ui->setupUi(this);

    // Handle Mining / Staking part
    updateStatsTimer = new QTimer(this);
    connect(updateStatsTimer, &QTimer::timeout, this, &OverviewPage::updateStats);
    updateStats();
    updateStatsTimer->start(5000);

    for (int i = 0; i < ui->topBoxes->count(); ++i)
    {
        QWidget *box = ui->topBoxes->itemAt(i)->widget();
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setOffset(QPointF(5, 5));
        shadow->setBlurRadius(20.0);
        box->setGraphicsEffect(shadow);
    }


    m_balances.balance = -1;

    // Recent transactions
    ui->lastTransactionsContent->setItemDelegate(txdelegate);
    ui->lastTransactionsContent->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE + (MARGIN_SIZE * 2)));
    ui->lastTransactionsContent->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + (MARGIN_SIZE * 2)));
    ui->lastTransactionsContent->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(ui->lastTransactionsContent, &QListView::clicked, this, &OverviewPage::handleTransactionClicked);

    // start with displaying the "out of sync" warnings
    showOutOfSyncWarning(true);
    connect(ui->availableWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->unconfirmedWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->totalWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->immatureWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->minerHashOrInterestWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->estNextRewardOrInflationWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);
    connect(ui->blockRewardOrNetworkStakingWarn, &QPushButton::clicked, this, &OverviewPage::handleOutOfSyncWarningClicks);


    if( GUIUtil::IsVericoin()) {
        ui->immatureTitle->setText(tr("Staking"));
        ui->blockRewardOrNetworkStakingTitle->setText(tr("Network Coins Stake"));
        ui->minerHashOrInterestTitle->setText(tr("Interest Rate"));
        ui->estNextRewardOrInflationTitle->setText(tr("Inflation Rate"));
        ui->miningOrStakingProcessTitle->setText(tr("Staking Process"));
    }
    else {
        ui->immatureTitle->setText(tr("Block Reward"));
        ui->immatureTitle->setText(tr("Immature"));
        ui->minerHashOrInterestTitle->setText(tr("Miner Hashrate"));
        ui->estNextRewardOrInflationTitle->setText(tr("Est. Next Reward"));
        ui->miningOrStakingProcessTitle->setText(tr("Mining Process"));
    }

    // manage receive/send button
    ui->receiveBox->installEventFilter(this);
    ui->sendBox->installEventFilter(this);
}

bool OverviewPage::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if(object->objectName() == ui->receiveBox->objectName())
            Q_EMIT receiveClicked();
        else if(object->objectName() == ui->sendBox->objectName())
            Q_EMIT sendClicked();
    }
    return QWidget::eventFilter(object, event);
}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        Q_EMIT transactionClicked(filter->mapToSource(index));
}

void OverviewPage::handleOutOfSyncWarningClicks()
{
    Q_EMIT outOfSyncWarningClicked();
}

OverviewPage::~OverviewPage()
{
    delete ui;
}

void OverviewPage::setBalance(const interfaces::WalletBalances& balances)
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    m_balances = balances;
    if (walletModel->wallet().privateKeysDisabled()) {
        ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balances.watch_only_balance, false, BitcoinUnits::separatorAlways));
        ui->labelUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, balances.unconfirmed_watch_only_balance, false, BitcoinUnits::separatorAlways));

        if( GUIUtil::IsVericoin() ) {
            ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, balances.stake, false, BitcoinUnits::separatorAlways));
            ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balances.watch_only_balance + balances.unconfirmed_watch_only_balance + balances.immature_watch_only_balance + balances.stake, false, BitcoinUnits::separatorAlways));
        }
        else {
            ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, balances.immature_watch_only_balance, false, BitcoinUnits::separatorAlways));
            ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balances.watch_only_balance + balances.unconfirmed_watch_only_balance + balances.immature_watch_only_balance, false, BitcoinUnits::separatorAlways));
        }

    } else {
        ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balances.balance, false, BitcoinUnits::separatorAlways));
        ui->labelUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, balances.unconfirmed_balance, false, BitcoinUnits::separatorAlways));
        ui->labelWatchAvailable->setText(BitcoinUnits::formatWithUnit(unit, balances.watch_only_balance, false, BitcoinUnits::separatorAlways));
        ui->labelWatchUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, balances.unconfirmed_watch_only_balance, false, BitcoinUnits::separatorAlways));
        ui->labelWatchTotal->setText(BitcoinUnits::formatWithUnit(unit, balances.watch_only_balance + balances.unconfirmed_watch_only_balance + balances.immature_watch_only_balance, false, BitcoinUnits::separatorAlways));


        if( GUIUtil::IsVericoin()) {
            ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, balances.stake, false, BitcoinUnits::separatorAlways));
            ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balances.balance + balances.unconfirmed_balance + balances.immature_balance + balances.stake, false, BitcoinUnits::separatorAlways));
        }
        else {
            ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, balances.immature_balance, false, BitcoinUnits::separatorAlways));
            ui->labelWatchImmature->setText(BitcoinUnits::formatWithUnit(unit, balances.immature_watch_only_balance, false, BitcoinUnits::separatorAlways));
            ui->labelTotal->setText(BitcoinUnits::formatWithUnit(unit, balances.balance + balances.unconfirmed_balance + balances.immature_balance, false, BitcoinUnits::separatorAlways));
        }
    }
}

// show/hide watch-only labels
void OverviewPage::updateWatchOnlyLabels(bool showWatchOnly)
{
    ui->watchAvailableBox->setVisible(showWatchOnly);
    ui->watchUnconfirmedBox->setVisible(showWatchOnly);
    ui->watchImmatureBox->setVisible(showWatchOnly);

    if( GUIUtil::IsVericoin())
        ui->watchImmatureBox->setVisible(false);

    ui->watchTotalBox->setVisible(showWatchOnly);
}

void OverviewPage::setVericoinInfo()
{
    ui->labelMinerHashrateOrInterest->setText(QString("%1 %").arg(QString::number( m_node.getCurrentInterestRate(), 'f', 2)));
    ui->labelEstNextRewardOrInflation->setText(QString("%1 %").arg(QString::number( m_node.getCurrentInflationRate(), 'f', 2)));
    ui->labelblockRewardOrNetworkStaking->setText(QString("%1 %").arg(QString::number(((m_node.getNetworkStakeWeight()/2)/30000000)*100,'f',2)));
}

void OverviewPage::setVeriumInfo()
{
    int unit = walletModel->getOptionsModel()->getDisplayUnit();
    ui->labelblockRewardOrNetworkStaking->setText(BitcoinUnits::formatWithUnit(unit, m_node.getBlockReward(), false, BitcoinUnits::separatorAlways));
}

void OverviewPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if (model) {
        // Show warning, for example if this is a prerelease version
        connect(model, &ClientModel::alertsChanged, this, &OverviewPage::updateAlerts);
        updateAlerts(model->getStatusBarWarnings());

        // On Vericoin, update interest / inflation / ... every block update
        if( GUIUtil::IsVericoin() ) {
            setVericoinInfo();
            connect(model, &ClientModel::numBlocksChanged, this, &OverviewPage::setVericoinInfo);
        } else {
            setVeriumInfo();
            connect(model, &ClientModel::numBlocksChanged, this, &OverviewPage::setVeriumInfo);
        }
    }
}

void OverviewPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter.reset(new TransactionFilterProxy());
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->setShowInactive(false);
        filter->sort(TransactionTableModel::Date, Qt::DescendingOrder);

        ui->lastTransactionsContent->setModel(filter.get());
        ui->lastTransactionsContent->setModelColumn(TransactionTableModel::ToAddress);

        // Keep up to date with wallet
        interfaces::Wallet& wallet = model->wallet();
        interfaces::WalletBalances balances = wallet.getBalances();
        setBalance(balances);
        connect(model, &WalletModel::balanceChanged, this, &OverviewPage::setBalance);

        connect(model->getOptionsModel(), &OptionsModel::displayUnitChanged, this, &OverviewPage::updateDisplayUnit);

        updateWatchOnlyLabels(wallet.haveWatchOnly() && !model->wallet().privateKeysDisabled());
        connect(model, &WalletModel::notifyWatchonlyChanged, [this](bool showWatchOnly) {
            updateWatchOnlyLabels(showWatchOnly && !walletModel->wallet().privateKeysDisabled());
        });
    }

    // update the display unit, to not use the default ("VRM")
    updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
    if(walletModel && walletModel->getOptionsModel())
    {
        if (m_balances.balance != -1) {
            setBalance(m_balances);
        }

        // Update txdelegate->unit with the current unit
        txdelegate->unit = walletModel->getOptionsModel()->getDisplayUnit();

        ui->lastTransactionsContent->update();
    }
}

void OverviewPage::updateAlerts(const QString &warnings)
{
    this->ui->labelAlerts->setVisible(!warnings.isEmpty());
    this->ui->labelAlerts->setText(warnings);
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    ui->availableWarn->setVisible(fShow);
    ui->unconfirmedWarn->setVisible(fShow);
    ui->immatureWarn->setVisible(fShow);
    ui->totalWarn->setVisible(fShow);
    if( GUIUtil::IsVericoin() ) {
        ui->minerHashOrInterestWarn->setVisible(fShow);
        ui->estNextRewardOrInflationWarn->setVisible(fShow);
        ui->blockRewardOrNetworkStakingWarn->setVisible(fShow);
    }
    else {
        ui->minerHashOrInterestWarn->setVisible(false);
        ui->estNextRewardOrInflationWarn->setVisible(false);
        ui->blockRewardOrNetworkStakingWarn->setVisible(false);
    }
}

void OverviewPage::updateStats()
{
    if( GUIUtil::IsVericoin() ) {
        if( m_node.isStaking() ) {
            ui->mineButton->setIcon(QIcon(":/icons/stakingon"));
            ui->labelMinerButton->setText(tr("Click to stop:"));
            if ( walletModel )
            {
                uint64_t staketime = walletModel->wallet().getTimeToStake();
                int stakerate = 1;
                if (staketime > 3600){
                    stakerate = staketime/(60*60);
                }
                ui->mineButton->setToolTip(tr("<html><head/><body><p>Next reward estimated in %1 hour</p></body></html>").arg(stakerate));
            }
        } else {
            ui->mineButton->setToolTip(tr("<html><head/><body><p>Click to start/stop</p></body></html>"));
            ui->mineButton->setIcon(QIcon(":/icons/stakingoff"));
            ui->labelMinerButton->setText(tr("Click to start:"));
        }
    } else {
        if( m_node.isMining() ) {
            ui->mineButton->setIcon(QIcon(":/icons/miningon"));
            ui->labelMinerButton->setText(tr("Click to stop:"));
            // XXX: Add Verium Stat here
        } else {
            ui->labelMinerHashrateOrInterest->setText("--- H/m");
            ui->labelEstNextRewardOrInflation->setText("--- Day(s)");
            ui->mineButton->setIcon(QIcon(":/icons/miningoff"));
            ui->labelMinerButton->setText(tr("Click to start:"));
        }
    }
    if( ui->minerBox->height() - 15 > 50) {
        if( (ui->minerBox->height() - 50) > (ui->minerBox->width() - 15) ) {
            ui->mineButton->setIconSize(QSize(ui->minerBox->width() - 15,ui->minerBox->width() - 15));
        }
        else {
            ui->mineButton->setIconSize(QSize(ui->minerBox->height() - 50, ui->minerBox->height() - 50));
        }
    }
}


void OverviewPage::on_mineButton_clicked()
{
    // check client is in sync
    QDateTime lastBlockDate = QDateTime::fromTime_t(clientModel->getHeaderTipTime());
    int secs = lastBlockDate.secsTo(QDateTime::currentDateTime());
    int count = clientModel->getHeaderTipHeight();
    int nTotalBlocks = clientModel->getHeaderTipHeight();
    int peers = clientModel->getNumConnections();

    bool miningOrStakingState = m_node.isStaking();
    if( ! GUIUtil::IsVericoin() )
        miningOrStakingState = m_node.isMining();

    if((secs > 90*60 && count < nTotalBlocks && !miningOrStakingState) || (peers < 1 && !miningOrStakingState))
    {
        QMessageBox::warning(this, tr("Process"),
            tr("Please wait until fully in sync with network."),
            QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if ( ! walletModel )
      return;

    if( ! miningOrStakingState )
        walletModel->manageProcess(true, 0);
    else
        walletModel->manageProcess(false, 0);

    updateStats();
}