#pragma once
#include <functional>
#include <memory>
class TcpClient;
class Buffer;
namespace Pine{
    using clientPtr = std::shared_ptr<TcpClient>;
    using TcpClientReadCallBackFun = std::function<void(clientPtr,Buffer*)>;
}