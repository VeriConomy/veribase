#ifndef BITCOIN_QT_LOGINOVERLAY_H
#define BITCOIN_QT_LOGINOVERLAY_H

#include <QWidget>

namespace Ui {
    class LoginOverlay;
}

/** Login overlay to access wallet */
class LoginOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit LoginOverlay(QWidget *parent);
    ~LoginOverlay();

public Q_SLOTS:
    void toggleVisibility();
    // will show or hide the modal layer
    void showHide(bool hide = false, bool userRequested = false);
    bool isLayerVisible() const { return layerIsVisible; }

protected:
    bool eventFilter(QObject * obj, QEvent * ev);
    bool event(QEvent* ev);

private:
    Ui::LoginOverlay *ui;
    bool layerIsVisible;
    bool userClosed;
};

#endif // BITCOIN_QT_LOGINOVERLAY_H
