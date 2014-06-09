#include "asioAndSignal2.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

namespace{
/// @brief io_service dedicated to gui.
boost::asio::io_service gui_service;

/// @brief io_service dedicated to worker.
boost::asio::io_service worker_service;

/// @brief work to keep gui_service from stopping prematurely.
boost::optional<boost::asio::io_service::work> gui_work;

/// @brief hello slot.
void hello(int x)
{
  std::cout << "hello with " << x << " from thread " <<
               boost::this_thread::get_id() << std::endl;
}

/// @brief world slot.
void world(int x)
{
  std::cout << "world with " << x << " from thread " <<
               boost::this_thread::get_id() << std::endl;
}

/// @brief Type for signals.
using SignalType = boost::signals2::signal<void (int)>;

void emit_then_notify_gui(SignalType& signal, unsigned int x);

/// @brief Emit signals then message worker.
void emit_then_notify_worker(SignalType& signal, unsigned int x)
{
  // Emit signal, causing registered slots to run within this thread.
  signal(x);

  // If x has been exhausted, then cause gui service to run out of work.
  if (!x)
  {
    gui_work = boost::none;
  }
  // Otherwise, post work into worker service.
  else
  {
    std::cout << "GUI thread: " << boost::this_thread::get_id() <<
                 " scheduling other thread to emit signals" << std::endl;
    worker_service.post(boost::bind(
        &emit_then_notify_gui,
        boost::ref(signal), --x));
  }
}

/// @brief Emit signals then message worker.
void emit_then_notify_gui(SignalType& signal, unsigned int x)
{
  // Emit signal, causing registered slots to run within this thread.
  signal(x);

  // If x has been exhausted, then cause gui service to run out of work.
  if (!x)
  {
    gui_work = boost::none;
  }
  // Otherwise, post more work into gui.
  else
  {
    std::cout << "Worker thread: " << boost::this_thread::get_id() <<
                 " scheduling other thread to emit signals" << std::endl;
    gui_service.post(boost::bind(
        &emit_then_notify_worker,
        boost::ref(signal), --x));
  }
}

void worker_main()
{
  std::cout << "Worker thread: " << boost::this_thread::get_id() << std::endl;
  worker_service.run();
}

}

void asio_and_signal2()
{
  SignalType signal;

  // Connect slots to signal.
  signal.connect(&hello);
  signal.connect(&world);

  boost::optional<boost::asio::io_service::work> worker_work(
     boost::ref(worker_service));
  gui_work = boost::in_place(boost::ref(gui_service));

  std::cout << "GUI thread: " << boost::this_thread::get_id() << std::endl;

  // Spawn off worker thread.
  boost::thread worker_thread(&worker_main);

  // Add work to worker.
  worker_service.post(boost::bind(
      &emit_then_notify_gui,
      boost::ref(signal), 3));

  // Mocked up GUI main loop.
  while (!gui_service.stopped())
  {
    // Do other GUI actions.

    // Perform message processing.
    gui_service.poll_one();
  }

  // Cleanup.
  worker_work = boost::none;
  worker_thread.join();
}
