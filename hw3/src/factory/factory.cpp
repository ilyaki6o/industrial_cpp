#include "factory.h"
#include <map>

class TFactory::TImpl {
    class ICreator {
    public:
        virtual ~ICreator(){}
        virtual std::shared_ptr<TFunction> Create(const TOptions opts) const = 0;
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;

    TRegisteredCreators RegisteredCreators;

public:
    template <class TCurrentObject>
    class TCreator : public ICreator{
        std::shared_ptr<TFunction> Create(const TOptions opts) const override{
            return std::make_shared<TCurrentObject>(opts);
        }
    };

    TImpl() { RegisterAll();}

    template <typename T>
    void RegisterCreator(const std::string& name) {
        RegisteredCreators[name] = std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
        RegisterCreator<IdentFunction>("ident");
        RegisterCreator<ConstFunction>("const");
        RegisterCreator<PowerFunction>("power");
        RegisterCreator<ExpFunction>("exp");
        RegisterCreator<PolinomialFunction>("polynomial");
    }

    std::shared_ptr<TFunction> CreateObject(
        const std::string& n,
        const TOptions opts
    ) const {
        auto creator = RegisteredCreators.find(n);

        if (creator == RegisteredCreators.end()) {
            return nullptr;
        }

        return creator->second->Create(opts);
    }

    std::vector<std::string> GetAvailableObjects () const {
        std::vector<std::string> result;

        for (const auto& creatorPair : RegisteredCreators) {
            result.push_back(creatorPair.first);
        }

        return result;
    }
};

std::shared_ptr<TFunction> TFactory::CreateObject(
    const std::string& n,
    const TOptions opts
) const {
    return Impl->CreateObject(n, opts);
}

TFactory::TFactory() : Impl(std::make_unique<TFactory::TImpl>()) {}
TFactory::~TFactory(){}

std::vector<std::string> TFactory::GetAvailableObjects() const {
    return Impl->GetAvailableObjects();
}
