#ifndef CONFESSIONALPAGE_H
#define CONFESSIONALPAGE_H

#include <QWidget>

#ifndef _WINDOWS
#include <QWebView>
#else
class QWebView { };
#endif

#ifndef _WINDOWS
namespace Ui {
    class ConfessionalPage;
}
#endif
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
#ifndef _WINDOWS
    Ui::ConfessionalPage *ui;
#endif
    WalletModel *walletModel;

private slots:
#ifndef _WINDOWS
    void updateConfessions();
    void generateTipAddress();
    void capturePenance();
    void captureTipAddress();
#endif
};

#endif // CONFESSIONALPAGE_H
