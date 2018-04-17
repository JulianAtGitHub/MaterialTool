#ifndef MTSIDEBAR_H
#define MTSIDEBAR_H

#include <QFrame>
#include <QPushButton>

class MTSideBar : public QFrame {
    Q_OBJECT

public:
    explicit MTSideBar(QWidget* parent = Q_NULLPTR);
    virtual ~MTSideBar(void);

private slots:
    void onAddBoxClicked(void);
};

#endif // MTSIDEBAR_H
