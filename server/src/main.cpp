#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <asio.hpp>
#include <cstdint>

struct Message
{
    std::vector<uint8_t> data;
};

class TcpServer
{
public:
    TcpServer(asio::io_context &io_context, short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        auto socket = std::make_shared<asio::ip::tcp::socket>(acceptor_.get_executor());
        acceptor_.async_accept(*socket, [this, socket](std::error_code ec)
                               {
            if (!ec) {
                std::cout << "New client connected: " << socket->remote_endpoint() << std::endl;
                std::thread(&TcpServer::handle_client, this, socket).detach();
            }
            start_accept(); });
    }

    void handle_client(std::shared_ptr<asio::ip::tcp::socket> socket)
    {
        try
        {
            for (;;)
            {
                Message received_msg = read_message(socket);
                std::cout << "Received message of " << received_msg.data.size() << " bytes" << std::endl;

                // Echo back to client
                write_message(socket, received_msg);
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception in thread: " << e.what() << std::endl;
        }
    }

    Message read_message(std::shared_ptr<asio::ip::tcp::socket> socket)
    {
        // Read message header
        uint32_t message_size;
        asio::read(*socket, asio::buffer(&message_size, sizeof(message_size)));
        message_size = ntohl(message_size);

        // Read message body
        Message msg;
        msg.data.resize(message_size);
        asio::read(*socket, asio::buffer(msg.data.data(), message_size));

        return msg;
    }

    void write_message(std::shared_ptr<asio::ip::tcp::socket> socket, const Message &msg)
    {
        // Write message header
        uint32_t message_size = htonl(static_cast<uint32_t>(msg.data.size()));
        asio::write(*socket, asio::buffer(&message_size, sizeof(message_size)));

        // Write message body
        asio::write(*socket, asio::buffer(msg.data.data(), msg.data.size()));
    }

    asio::ip::tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        asio::io_context io_context;
        TcpServer server(io_context, 12345);
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
