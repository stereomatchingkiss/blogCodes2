#ifndef GUITHREADDISPATCHER_HPP
#define GUITHREADDISPATCHER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>

template<typename FuncParam>
class guiThreadDispatcher
{
public:
    guiThreadDispatcher();

    void poll_one(boost::function<FuncParam> func);

private:


private:
    boost::function<FuncParam> gui_func_;

    boost::asio::io_service gui_service_;
    boost::asio::io_service worker_service_;

    boost::optional<boost::asio::io_service::work> gui_work_;
    boost::optional<boost::asio::io_service::work> worker_work_;
};

template<typename FuncParam>
guiThreadDispatcher<FuncParam>::guiThreadDispatcher()
{}

template<typename FuncParam>
void guiThreadDispatcher<FuncParam>::poll_one(boost::function<FuncParam> func)
{
    gui_work_ = boost::in_place(boost::ref(gui_service_));
    worker_work_ = boost::in_place(boost::ref(worker_service_));

    std::cout << "GUI thread: " << boost::this_thread::get_id() << std::endl;

    // Spawn off worker thread.
    boost::thread worker_thread(func);

    // Add work to worker.
    /*worker_service_.post(call_gui_thread_from_worker);

    gui_service_.run();
    std::cout<<"after gui service run"<<std::endl;
    worker_work = boost::none;

    worker_thread.join();*/
    //func();
}

#endif // GUITHREADDISPATCHER_HPP
