#ifndef POOL_HPP
#define POOL_HPP

#include <forward_list>
#include "SpinLock.hpp"

/**
 * Generic object pool pattern
 **/
template <class Element>
class Pool {
    typedef std::forward_list<Element> List;
    List used;
    List ready;

    SpinLock listLock;
    typedef SpinLock::Guard Lock;

public:
    typedef typename List::iterator Handle;

    static Element& fetch(const Handle& iterator) { return *std::next(iterator); }

    Handle acquire() {
        Lock lock{listLock};

        if (ready.empty()) {
            used.emplace_front();
        }
        else {
            used.splice_after(used.before_begin(), ready, ready.before_begin());
        }

        return used.before_begin();
    }

    //return the used element to the ready list
    void release(Handle&& element) {
        if (element != ready.end()) {
            Lock lock{listLock};

            ready.splice_after(ready.before_begin(), used, element);
            element = ready.end();
        }
    }

};

#endif
