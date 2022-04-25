#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace mcache {

typedef const void loaderFunc(const std::string &key);
typedef std::unordered_map<std::string, const void> mloaderFunc(
    const std::vector<std::string> &keys);
typedef const void valptrFunc();
typedef const unsigned char *serializeFunc(const std::string &key);
typedef bool unserializeFunc(const unsigned char &value);

class options;
typedef options *option;

class options {
   public:
    std::string name;
    std::string type;
    int ttl;
    loaderFunc *loader;
    loaderFunc *_real_loader;
    mloaderFunc *mloader;
    mloaderFunc *_real_mloader;
    void *default_value;
    serializeFunc *serialize;
    unserializeFunc *unserialize;

    // instance used for the first time initialized with mcache.
    static options &instance() {
        // Guaranteed to be destroyed.
        // Instantiated on first use.
        static options instance;
        return instance;
    }

    option WithName(std::string _name) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->name.assign(_name, 0, _name.length());
        return _instance;
    }

    option WithCacheType(std::string _type) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->type.assign(_type, 0, _type.length());
        return _instance;
    }

    option WithTTL(std::size_t _ttl) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->ttl = _ttl;
        return _instance;
    }

    option WithLoader(loaderFunc *_loader) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->loader = _loader;
        return _instance;
    }

    option WithMLoader(mloaderFunc *_mloader) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->mloader = _mloader;
        return _instance;
    }

    option WithSerialize(serializeFunc *_serialize) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->serialize = _serialize;
        return _instance;
    }

    option WithUnserialize(unserializeFunc *_unserialize) {
        if (_instance == nullptr) {
            _instance = new options();
        }
        _instance->unserialize = _unserialize;
        return _instance;
    }

    ~options() { delete _instance; }

   protected:
    // Constructor? (the {} brackets) are needed here.
    options()
        : name(""),
          type(""),
          ttl(0),
          loader(nullptr),
          _real_loader(nullptr),
          mloader(nullptr),
          _real_mloader(nullptr),
          serialize(nullptr),
          unserialize(nullptr),
          _instance(nullptr) {}

    // C++ 11
    // =======
    // We can use the better technique of deleting the methods
    // we don't want.
   public:
    options(options const &) = delete;
    void operator=(options const &) = delete;

    //_instance used for user custom options
    option _instance;
};

option WithName(std::string _name) {
    return options::instance().WithName(_name);
}
option WithCacheType(std::string _type) {
    return options::instance().WithCacheType(_type);
}
option WithTTL(std::size_t _ttl) { return options::instance().WithTTL(_ttl); }
option WithLoader(loaderFunc *_loader) {
    return options::instance().WithLoader(_loader);
}
option WithMLoader(mloaderFunc *_mloader) {
    return options::instance().WithMLoader(_mloader);
}
option WithSerialize(serializeFunc *_serialize) {
    return options::instance().WithSerialize(_serialize);
}
option WithUnserialize(unserializeFunc *_unserialize) {
    return options::instance().WithUnserialize(_unserialize);
}

}  // namespace mcache
