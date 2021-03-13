#ifndef CUSTOM_QPLAIN_TEXT_EDIT_HPP
#define CUSTOM_QPLAIN_TEXT_EDIT_HPP

#include <QPlainTextEdit>

class QTimer;

class custom_qplain_text_edit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit custom_qplain_text_edit(QWidget *parent);

signals:
    void mouse_pressed();

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
#ifdef Q_OS_WASM
    QTimer *timer_;
#endif
};

#endif // CUSTOM_QPLAIN_TEXT_EDIT_HPP
