// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/communitypage.h>
#include <qt/forms/ui_communitypage.h>

#include <qt/bitcoinunits.h>
#include <qt/clientmodel.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/platformstyle.h>

#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QUrl>

CommunityPage::CommunityPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommunityPage),
    clientModel(nullptr)
{
    ui->setupUi(this);

    // add shadow
    QGraphicsDropShadowEffect* shadow1 = new QGraphicsDropShadowEffect();
    shadow1->setOffset(QPointF(5, 5));
    shadow1->setBlurRadius(20.0);
    QGraphicsDropShadowEffect* shadow2 = new QGraphicsDropShadowEffect();
    shadow2->setOffset(QPointF(5, 5));
    shadow2->setBlurRadius(20.0);
    QGraphicsDropShadowEffect* shadow3 = new QGraphicsDropShadowEffect();
    shadow3->setOffset(QPointF(5, 5));
    shadow3->setBlurRadius(20.0);
    QGraphicsDropShadowEffect* shadow4 = new QGraphicsDropShadowEffect();
    shadow4->setOffset(QPointF(5, 5));
    shadow4->setBlurRadius(20.0);
    ui->explorerBox->setGraphicsEffect(shadow1);
    ui->twitterBox->setGraphicsEffect(shadow2);
    ui->chatBox->setGraphicsEffect(shadow3);
    ui->websiteBox->setGraphicsEffect(shadow4);

    // manage event
    ui->explorerBox->installEventFilter(this);
    ui->twitterBox->installEventFilter(this);
    ui->chatBox->installEventFilter(this);
    ui->websiteBox->installEventFilter(this);
}

CommunityPage::~CommunityPage()
{
    delete ui;
}

void CommunityPage::setClientModel(ClientModel *model)
{
    this->clientModel = model;
}

bool CommunityPage::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        openLink(object);
    }
    return QWidget::eventFilter(object, event);
}

void CommunityPage::openLink(QObject *object)
{
    if(object->objectName() == "explorerBox") {
        if( GUIUtil::IsVericoin() )
            QDesktopServices::openUrl(QUrl(COMMUNITY_VRC_EXPLORER_URL));
        else
            QDesktopServices::openUrl(QUrl(COMMUNITY_VRM_EXPLORER_URL));
    }
    else if(object->objectName() == "twitterBox")
        QDesktopServices::openUrl(QUrl(COMMUNITY_TWITTER_URL));
    else if(object->objectName() == "chatBox")
        QDesktopServices::openUrl(QUrl(COMMUNITY_CHAT_URL));
    else if(object->objectName() == "websiteBox")
        QDesktopServices::openUrl(QUrl(COMMUNITY_WEBSITE_URL));
}