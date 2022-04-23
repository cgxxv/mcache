#pragma once
#include <string>
#include <unordered_map>

using namespace std;

namespace mcache {

#define TYPE_SIMPLE "simple"
#define TYPE_LRU "lru"
#define TYPE_LFU "lfu"
#define TYPE_ARC "arc"

typedef const void loaderFunc(const string &key);
typedef unordered_map<string, const void> mloaderFunc(
    const vector<string> &keys);
typedef const void valptrFunc();
typedef const unsigned char *serializeFunc(const string &key);
typedef bool unserializeFunc(const unsigned char &value);

struct options {
    string name;
    string type;
    int ttl;
    loaderFunc *loader;
    loaderFunc *real_loader;
    mloaderFunc *mloader;
    mloaderFunc *real_mloader;
    void *default_value;
    serializeFunc serialize;
    unserializeFunc unserialize;
};

typedef void option(options *);

auto withName(string name) {
    return [&](options &o) { o.name = name; };
}

auto withCacheType(string type) {
    return [&](options &o) { o.type = type; };
}

auto withTTL(int ttl) {
    return [&](options &o) { o.ttl = ttl; };
}

auto withLoader(loaderFunc *loader) {
    return [&](options &o) { o.loader = loader; };
}

auto withMLoader(mloaderFunc *mloader) {
    return [&](options &o) { o.mloader = mloader; };
}

}  // namespace mcache
