#include "confessionalpage.h"
#include "ui_confessionalpage.h"

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
#include <QtWebKit>

#include "confessionalpage.moc"

ConfessionalPage::ConfessionalPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfessionalPage)
{
    ui->setupUi(this);
}

ConfessionalPage::~ConfessionalPage()
{
    delete ui;
}

void ConfessionalPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
}

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

        QWebElement okButton = document.findFirst("span[class=ui-button-text]");
        okButton.evaluateJavaScript("this.click()");
        okButton = document.findFirst("button[class=ui-button]");
        okButton.evaluateJavaScript("this.click()");
    }
}

