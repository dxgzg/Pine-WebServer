#pragma once
#include<vector>
#include <unordered_map>
#include <memory>

class Channel;

class Poller{
public:
    virtual void poll(std::vector<Channel*>& artiveChannel) = 0;
    virtual ~Poller() = default;

protected:
    using PollerMap = std::unordered_map<int,Channel*>;
    PollerMap channels_;
private:

};