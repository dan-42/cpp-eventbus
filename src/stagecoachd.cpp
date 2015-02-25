//
// stream_server.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdio>
#include <iostream>
#include <memory>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

using boost::asio::local::stream_protocol;

class session  : public boost::enable_shared_from_this<session> {
public:
  session(boost::asio::io_service& io_service)   : socket_(io_service)   {
  }

  stream_protocol::socket& socket()   {
    return socket_;
  }

  void start()  {
      std::cout << "session start()" << std::endl;
    socket_.async_read_some(boost::asio::buffer(data_), boost::bind(&session::handle_read,
                           this, boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error)  {


      std::cout << "session handle read " << std::endl;
      callback_(std::string(data_.begin(), data_.end()));
      
    socket_.async_read_some(boost::asio::buffer(data_), boost::bind(&session::handle_read, this, boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

    //  boost::asio::async_write(socket_,boost::asio::buffer(data_, bytes_transferred),        boost::bind(&session::handle_write, this ,boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error) {
    if (!error)  {

      socket_.async_read_some(boost::asio::buffer(data_), boost::bind(&session::handle_read,
            this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
  }

  void send(const std::string &msg){
      std::cout << "session send()" << std::endl;
    boost::asio::async_write(socket_,boost::asio::buffer(msg, msg.size()), 
                            boost::bind(&session::handle_dummy_write, this, boost::asio::placeholders::error));
  }


  void handle_dummy_write(const boost::system::error_code& error) {
    if(error){
    }
  }

  void async_read(boost::function<void (std::string )> callback) {
    callback_ =  callback;
  }
private:
  // The socket used to communicate with the client.
  stream_protocol::socket socket_;

  // Buffer used to store data received from the client.
  boost::array<char, 1024> data_;

  boost::function<void (std::string )> callback_;
};

typedef boost::shared_ptr<session> session_ptr;


class dispatcher {
  public:
    void add_session(session_ptr s){
      sessions_.push_back(s);
      s->async_read(boost::bind(&dispatcher::handle_session_async_read, this,  _1));


    }

    void handle_session_async_read(const std::string& msg){

      std::cout << "handle session asyc read" << std::endl;
      send_message(msg);
    }
  
    void send_message(const std::string &msg){
      for(auto s : sessions_){
        s->send(msg);
      }
    }

  private:
    std::vector<session_ptr > sessions_;

};

class server {
public:
  server(boost::asio::io_service& io_service, const std::string& file)
    : io_service_(io_service), acceptor_(io_service, stream_protocol::endpoint(file)) {

    session_ptr new_session(new session(io_service_));
    acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
  }

  void handle_accept(session_ptr new_session, const boost::system::error_code& error) {
    if (!error) {
      dispatcher_.add_session(new_session);
      new_session->start();
    }

    new_session.reset(new session(io_service_));
    acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session,  boost::asio::placeholders::error));
  }

private:
  boost::asio::io_service& io_service_;
  stream_protocol::acceptor acceptor_;
  dispatcher dispatcher_;
};

int main(int argc, char* argv[]) {
  try  {
    if (argc != 2)   {
      std::cerr << "Usage: stream_server <file>\n";
      std::cerr << "*** WARNING: existing file is removed ***\n";
      return 1;
    }

    boost::asio::io_service io_service;

    std::remove(argv[1]);
    server s(io_service, argv[1]);

    io_service.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
