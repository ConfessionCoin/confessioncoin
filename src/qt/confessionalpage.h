#ifndef CONFESSIONALPAGE_H
#define CONFESSIONALPAGE_H

#include <QWidget>
#include <QWebView>

namespace Ui {
    class ConfessionalPage;
}
class WalletModel;

/** Confessional page widget */
class ConfessionalPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConfessionalPage(QWidget *parent = 0);
    ~ConfessionalPage();

    void setWalletModel(WalletModel *walletModel);

private:
    Ui::ConfessionalPage *ui;
    WalletModel *walletModel;

private slots:
    void updateConfessions();
    void generateTipAddress();
    void capturePenance();
    void captureTipAddress();
};

#endif // CONFESSIONALPAGE_H
