#ifndef POOL_HPP
#define POOL_HPP

#include <forward_list>
#include <mutex>

/**
 * Generic object pool pattern
 **/
template <class Element>
class Pool {
    std::mutex list_mutex;
    typedef std::lock_guard<decltype(list_mutex)> Lock;

    typedef std::forward_list<Element> List;
    List used;
    List ready;

public:
    typedef typename List::iterator _t;

    static Element& fetch(const _t& that) { return *std::next(that); }

    _t acquire() {
        Lock lock{list_mutex};

        if (ready.empty()) {
            used.emplace_front();
        }
        else {
            used.splice_after(used.before_begin(), ready, ready.before_begin());
        }

        return used.before_begin();
    }

    //return the used element to the ready list
    void release(_t&& element) {
        if (element != ready.end()) {
            Lock lock{list_mutex};

            ready.splice_after(ready.before_begin(), used, element);
            element = ready.end();
        }
    }

};

#endif
