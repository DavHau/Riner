
#include "ApiServer.h"
#include <numeric>

namespace miner {

    ApiServer::ApiServer(uint16_t port, const LockGuarded<std::vector<optional<Device>>> &devicesInUse)
            : devicesInUse(devicesInUse)
            , jrpc(std::make_unique<JrpcServer>(port)) {
        LOG(INFO) << "started api server on port " << port;
        registerFunctions();
        jrpc->launch();
    }

    void ApiServer::registerFunctions() {
        //json exceptions will get caught by the caller of these functions

        jrpc->registerFunc("divide", [] (float a, float b) -> JrpcReturn {

            if (b == 0)
                return {JrpcError::invalid_params, "division by zero"};

            return a / b;

        }, "a", "b");

        jrpc->registerFunc("getPoolStats", [] () -> JrpcReturn {
            return 0;
        });

        jrpc->registerFunc("getGpuStats", [&] () -> JrpcReturn {

            nl::json result;

            auto devicesInUseLocked = devicesInUse.lock();

            for (const optional<Device> &deviceInUse : *devicesInUseLocked) {

                nl::json j;

                if (deviceInUse) {
                    const Device &d = deviceInUse.value();

                    j = {
                            {"index", "i"},
                            {"runs algorithm", true},
                            {"algoImpl", d.settings.algoImplName},
                    };
                } else  {
                    j = {
                            {"index", "i"},
                            {"runs algorithm", false},
                    };
                }

                result.push_back(j);
            }

            return result;
        });

    }

    ApiServer::~ApiServer() {
        // manually destroy jrpc first, so the registered functions don't access
        // destroyed members asynchronously by accident
        jrpc.reset();
    }

}