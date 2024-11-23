#include <iostream>

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include "async/async.h"

using boost::asio::ip::tcp;

class BulkSession : public std::enable_shared_from_this<BulkSession> 
{
public:
    BulkSession(tcp::socket socket, std::size_t bulk_size)
        : socket_(std::move(socket)), bulk_size_(bulk_size), data_(1024, '\0'), handle_(nullptr) 
    {}

    void start() 
    {
        Read();
    }
    void set_handle(async::handle_t handle) 
    {
        handle_ = handle;
    }

private:
    void Read() 
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) 
            {
                if (!ec) 
                {
                    if (handle_) 
                    {
                        async::receive(handle_, data_.data(), length); 
                    }
                    Read();
                }
                else 
                {
                    if (handle_) 
                    {
                        async::disconnect(handle_);
                    }
                }
            });
    }


    tcp::socket socket_;
    std::size_t bulk_size_;
    std::vector<char> data_;
    async::handle_t handle_;
};

class BulkServer {
public:
    BulkServer(boost::asio::io_context& io_context, std::uint16_t port, std::size_t bulk_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), bulk_size_(bulk_size) 
    {
        Accept();
    }

private:
    void Accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) 
            {
                if (!ec) {
                    auto session = std::make_shared<BulkSession>(std::move(socket), bulk_size_);
                    auto handle = async::connect(bulk_size_);
                    session->set_handle(handle);
                    session->start();   
                }
                Accept();
            });
    }

    tcp::acceptor acceptor_;
    std::size_t bulk_size_;
};

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
        return 1;
    }

    try 
    {
        std::uint16_t port = static_cast<std::uint16_t>(std::stoi(argv[1]));
        std::size_t bulk_size = std::stoul(argv[2]);

        boost::asio::io_context io_context;
        BulkServer server(io_context, port, bulk_size);
        io_context.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}