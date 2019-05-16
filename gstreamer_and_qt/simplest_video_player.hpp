#ifndef SIMPLEST_VIDEO_PLAYER_HPP
#define SIMPLEST_VIDEO_PLAYER_HPP

#include <QWidget>

#include <memory>

class simplest_video_player : public QWidget
{
public:
    simplest_video_player();
    ~simplest_video_player();

    void play(QString const &command);

private:
    struct pimpl;

    std::unique_ptr<pimpl> impl_;
};

#endif // SIMPLEST_VIDEO_PLAYER_HPP
