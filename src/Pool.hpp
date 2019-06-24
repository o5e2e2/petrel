#ifndef POOL_HPP
#define POOL_HPP

#include <forward_list>
#include <mutex>

/**
 * Generic object pool pattern
 **/
template <class Element, class BasicLockable>
class Pool {
    typedef std::forward_list<Element> List;
    List used;
    List ready;

    BasicLockable listLock;
    typedef std::lock_guard<decltype(listLock)> Guard;

public:
    typedef typename List::iterator Iterator;

    static Element& fetch(const Iterator& iterator) { return *std::next(iterator); }

    Iterator acquire() {
        Guard g{listLock};

        if (ready.empty()) {
            used.emplace_front();
        }
        else {
            used.splice_after(used.before_begin(), ready, ready.before_begin());
        }

        return used.before_begin();
    }

    //return the used element to the ready list
    void release(Iterator&& element) {
        Guard g{listLock};

        if (element != ready.end()) {
            ready.splice_after(ready.before_begin(), used, element);
            element = ready.end();
        }
    }

};

#endif
