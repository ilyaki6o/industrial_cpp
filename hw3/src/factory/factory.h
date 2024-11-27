#ifndef FACTORY_HEADER
#define FACTORY_HEADER

#include "../functions/func.h"
#include <memory>
#include <vector>
#include <string>


class TFactory {
    class TImpl;
    std::unique_ptr<const TImpl> Impl;

public:
    TFactory();
    ~TFactory();

    std::shared_ptr<TFunction> CreateObject(
        const std::string& name,
        const TOptions opts = TOptions()) const;
    
    std::vector<std::string> GetAvailableObjects() const;
};

#endif

