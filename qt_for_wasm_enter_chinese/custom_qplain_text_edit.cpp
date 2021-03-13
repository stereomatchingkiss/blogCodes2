#include "custom_qplain_text_edit.hpp"

#include <QDebug>
#include <QTimer>

#ifdef Q_OS_WASM

#include <emscripten.h>

namespace{

EM_JS(char*, global_text_process_result_is_valid, (), {
      return globalTextProcessResultIsValid();
})

EM_JS(char*, get_global_text_process_result, (), {
      return getGlobalTextProcessResult();
})

EM_JS(void, multi_lines_prompt, (char const *input_strings, char const *input_mode, char const *title), {
      multiLinesPrompt(UTF8ToString(input_strings), UTF8ToString(input_mode), UTF8ToString(title));
})

}

#endif

custom_qplain_text_edit::custom_qplain_text_edit(QWidget *parent) :
    QPlainTextEdit(parent)
{
#ifdef Q_OS_WASM
    timer_ = new QTimer(this);
    timer_->setInterval(100);

    connect(timer_, &QTimer::timeout, [this]()
    {
        if(global_text_process_result_is_valid()){
            char *msg = get_global_text_process_result();
            setPlainText(QString(msg));
            free(msg);
            timer_->stop();
        }
    });
#endif
}

void custom_qplain_text_edit::keyPressEvent(QKeyEvent *e)
{
#ifndef Q_OS_WASM
    QPlainTextEdit::keyPressEvent(e);
#endif
}

void custom_qplain_text_edit::mousePressEvent(QMouseEvent *e)
{
#ifdef Q_OS_WASM
    multi_lines_prompt(toPlainText().toUtf8().data(), "textarea", "Input plain text");
    timer_->start();
#else
    QPlainTextEdit::mousePressEvent(e);
#endif
}
