#include "confessionalpage.h"

#ifndef _WINDOWS
#include "ui_confessionalpage.h"
#endif

#include "clientmodel.h"
#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "key.h"
#include "wallet.h"

#include <QAbstractItemDelegate>
#include <QPainter>

#ifndef _WINDOWS
#include <QtWebKit>
#include <QInputDialog>
#endif

ConfessionalPage::ConfessionalPage(QWidget *parent) :
    QWidget(parent)
#ifndef _WINDOWS
    ,ui(new Ui::ConfessionalPage)
#endif
{
#ifndef _WINDOWS
    ui->setupUi(this);
#endif
}

ConfessionalPage::~ConfessionalPage()
{
#ifndef _WINDOWS
    delete ui;
#endif
}

void ConfessionalPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}

#ifndef _WINDOWS
void ConfessionalPage::updateConfessions()
{
    ui->confessionalView->load(QUrl("http://confessioncoin.com"));
}

void ConfessionalPage::generateTipAddress()
{
    std::string newAddress = walletModel->getNewWalletAddress();

    QWebPage *page = ui->confessionalView->page();
    QWebFrame *frame = page->mainFrame();
    QWebElement document = frame->documentElement();
    QWebElement tip_address = frame->findFirstElement("#confession-tip-address");

    tip_address.setAttribute("value", newAddress.c_str());

    QString js = "this.value='" + QString(newAddress.c_str()) + "'";
    tip_address.evaluateJavaScript(js);
}

void ConfessionalPage::capturePenance()
{
    QWebPage *page = ui->confessionalView->page();
    QWebFrame *frame = page->mainFrame();
    QWebElement document = frame->documentElement();
    QWebElement amount = frame->findFirstElement("#penance-amount");
    QWebElement address = frame->findFirstElement("#penance-address");

    QString penance_amount = amount.toPlainText();
    QString penance_address = address.toPlainText();

    if (penance_amount.isEmpty() || penance_address.isEmpty() ||
        (penance_amount.length() == 0) || (penance_address.length() == 0))
    {
        QMessageBox::information(NULL, "Penance Capture Error", "Please post a confession first, and while the dialog is displaying the penance amount and address, try clicking this button again to facilitate the transaction.");
    }
    else
    {
        QString msg = "The Confessional requires a penance of " + penance_amount + " CFN sent to " + penance_address + " in order to post this confession.\n\n";

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this, "Penance Capture Complete",
            msg + "Would you like to automatically pay this penance now?",
            QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            SendCoinsRecipient confession;
            confession.address = penance_address;
            confession.label = "Confession";
            confession.amount = (penance_amount.toInt() * 100000000);

            QList<SendCoinsRecipient> l;
            l.append(confession);

            WalletModel::UnlockContext ctx(walletModel->requestUnlock());
            if(ctx.isValid())
            {
                walletModel->sendCoins(l, NULL);
            }
        }

        QWebElement okButton = document.findFirst("button[class=ui-button]");
        okButton.evaluateJavaScript("this.click()");

        okButton = document.findFirst("span[class=ui-button-text]");
        okButton.evaluateJavaScript("this.click()");
    }
}

void ConfessionalPage::captureTipAddress()
{
    QWebPage *page = ui->confessionalView->page();
    QWebFrame *frame = page->mainFrame();
    QWebElement document = frame->documentElement();
    QWebElement tip_address_elem = frame->findFirstElement("#current-tip-address");

    QString tip_address = tip_address_elem.toPlainText();

    if (tip_address.isEmpty() || (tip_address.length() == 0))
    {
        QMessageBox::information(NULL, "Tip Address Capture Error", "Please click on the \"tip\" link by the confession to where you'd like to send some coins before clicking this.");
    }
    else
    {
        bool ok = false;
        double tip_amount = QInputDialog::getDouble(this, "Enter Tip Amount", "How many ConfessionCoins would you like to send to " + tip_address, 5, -10000, 10000, 2, &ok);
        if (ok)
        {
            SendCoinsRecipient confession;
            confession.address = tip_address;
            confession.label = "Confession Tip";
            confession.amount = (tip_amount * 100000000);

            QList<SendCoinsRecipient> l;
            l.append(confession);

            WalletModel::UnlockContext ctx(walletModel->requestUnlock());
            if(ctx.isValid())
            {
                walletModel->sendCoins(l, NULL);
            }
        }
    }
    QWebElement okButton = document.findFirst("button[class=ui-button]");
    okButton.evaluateJavaScript("this.click()");

    okButton = document.findFirst("span[class=ui-button-text]");
    okButton.evaluateJavaScript("this.click()");
}
#endif
