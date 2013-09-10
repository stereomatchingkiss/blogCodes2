#ifndef HISTBACKPROJECTUI_HPP
#define HISTBACKPROJECTUI_HPP

#include <QWidget>

#include <opencv2/core/core.hpp>

#include <histBackProjectNoUI.hpp>

namespace Ui {
class histBackProjectUI;
}

class histBackProjectUI : public QWidget
{
    Q_OBJECT
    
public:    
    explicit histBackProjectUI(QWidget *parent = nullptr);
    histBackProjectUI(histBackProjectUI const&) = delete;
    histBackProjectUI& operator=(histBackProjectUI const&) = delete;
    ~histBackProjectUI();
    
private slots:
    void load_input();
    void load_model();

    void run();

private:
    int get_morphological_type(QString const&) const;

    void load(cv::Mat &mat);

private:
    histBackProjectNoUI impl_;
    cv::Mat input_;

    cv::Mat model_;
    Ui::histBackProjectUI *ui;
};

#endif // HISTBACKPROJECTUI_HPP
