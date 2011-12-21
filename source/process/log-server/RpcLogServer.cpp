#include "RpcLogServer.h"

#include <log-manager/LogServerRequest.h>

namespace sf1r
{

RpcLogServer::RpcLogServer(const std::string& host, uint16_t port, uint32_t threadNum)
: host_(host)
, port_(port)
, threadNum_(threadNum)
{
}

void RpcLogServer::start()
{
    instance.listen(host_, port_);
    instance.start(threadNum_);
}

void RpcLogServer::dispatch(msgpack::rpc::request req)
{
    try
    {
        std::string method;
        req.method().convert(&method);

        if (method == LogServerRequest::METHOD_UPDATE_UUID)
        {
            msgpack::type::tuple<UUID2DocIdList> params;
            req.params().convert(&params);
            UUID2DocIdList uuid2DocIdList = params.get<0>();

            // todo insert to DRUM?
            std::cout<<uuid2DocIdList.uuid_<<std::endl;
        }
        else
        {
            req.error(msgpack::rpc::NO_METHOD_ERROR);
        }
    }
    catch (msgpack::type_error& e)
    {
        req.error(msgpack::rpc::ARGUMENT_ERROR);
    }
    catch (std::exception& e)
    {
        req.error(std::string(e.what()));
    }
}

}
