//
//

#include <src/application/Registry.h>
#include <src/util/StringUtils.h>
#include <src/common/Optional.h>
#include <strings.h>

namespace riner {

    //try to get element in map with key, not case sensitive, return nullptr if key doesn't match anything
    template <class Map>
    optional<const typename Map::mapped_type &> map_at_case_insensitive(Map &smap, const std::string &key) {
        for (auto &pair : smap)
            if (toLower(pair.first) == toLower(key))
                return {pair.second};
        return nullopt;
    }

    bool Registry::algoImplExists(std::string name) const {
        return _algoWithName.count(name);
    }

    bool Registry::poolImplExists(std::string name) const {
        return _poolWithName.count(name);
    }

    bool Registry::gpuApiExists(std::string name) const {
        return _gpuApiWithName.count(name);
    }

    unique_ptr<Algorithm> Registry::makeAlgo(const std::string &name, AlgoConstructionArgs args) const {
        if (auto e = map_at_case_insensitive(_algoWithName, name))
            return e->makeFunc(std::move(args)); //initFunc declared in Registry::addAlgoImpl<T>
        return nullptr;
    }

    //may return nullptr
    shared_ptr<Pool> Registry::makePool(const std::string &name, PoolConstructionArgs args) const {
        if (auto e = map_at_case_insensitive(_poolWithName, name))
            return e->makeFunc(std::move(args)); //initFunc declared in Registry::addAlgoImpl<T>
        return nullptr;
    }

    unique_ptr<GpuApi> Registry::tryMakeGpuApi(const std::string &name, const GpuApiConstructionArgs &args) const {
        if (auto e = map_at_case_insensitive(_gpuApiWithName, name))
            return e->tryMakeFunc(args); //initFunc declared in Registry::addAlgoImpl<T>
        return nullptr;
    }

    std::vector<Registry::Listing> Registry::listAlgoImpls() const {
        std::vector<Listing> ret;

        for (auto &pair : _algoWithName) {
            ret.emplace_back();
            auto &r = ret.back();
            r.name = pair.first;
            r.powType = pair.second.powType;
        }
        return ret;
    }

    std::vector<Registry::Listing> Registry::listPoolImpls() const {
        std::vector<Listing> ret;

        for (auto &pair : _poolWithName) {
            ret.emplace_back();
            auto &r = ret.back();
            r.name = pair.first;
            r.powType = pair.second.powType;
            r.protocolType = pair.second.protocolType;
            r.protocolTypeAlias = pair.second.protocolTypeAlias;
        }
        return ret;
    }

    std::vector<std::string> Registry::listGpuApis() const {
        std::vector<std::string> ret;

        for (auto &pair : _gpuApiWithName) {
            ret.emplace_back(pair.first);
        }
        return ret;
    }

    std::string Registry::powTypeOfAlgoImpl(const std::string &algoImplName) const {
        if (auto e = map_at_case_insensitive(_algoWithName, algoImplName))
            return e->powType;
        return "";
    }

    std::string Registry::powTypeOfPoolImpl(const std::string &poolImplName) const {
        if (auto e = map_at_case_insensitive(_poolWithName, poolImplName))
            return e->powType;
        return "";
    }

    std::string Registry::poolImplForProtocolAndPowType(const std::string &protocolType, const std::string &powType) const {
        for (auto &pair : _poolWithName) {
            const std::string &poolImplName = pair.first;
            const EntryPool &e = pair.second;
            bool samePow = toLower(e.powType) == toLower(powType);

            bool sameProto = protocolType.empty(); // pick first protocol if powType is empty
            if (sameProto && samePow) {
                LOG(WARNING) << "no protocol specified for '" << powType << "'-pool - trying '" << e.protocolType << "'-protocol";
                return poolImplName;
            }
            sameProto |= toLower(protocolType) == toLower(e.protocolType);
            sameProto |= toLower(protocolType) == toLower(e.protocolTypeAlias);

            if (sameProto && samePow) {
                return poolImplName;
            }
        }
        return "";
    }
}
