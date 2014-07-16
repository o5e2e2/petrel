#ifndef POOL_HPP
#define POOL_HPP

#include <list>
#include <mutex>

/**
 * Generic object pool pattern
 **/
template <class Element>
class Pool {
public:
    typedef std::list<Element> list_t;
    typedef typename list_t::iterator element_type;

private:
    list_t list; //used elements
    element_type ready;

    std::mutex pool_mutex;

public:
    Pool () : ready(list.end()) {}

    element_type empty() { return list.end(); }

    element_type acquire() {
        std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};

        if (ready == empty()) {
            //create a new element
            list.emplace_front();
        }
        else {
            //reuse an element from the ready list
            list.splice(list.begin(), list, ready);
        }

        return list.begin();
    }

    //return the used element to the ready list
    void release(element_type&& element) {
        std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};
        list.splice(ready, list, element);
        ready = element;
    }

    index_t used() { return std::distance(list.begin(), ready); }
    index_t free() { return std::distance(ready, list.end()); }
    index_t total() { return std::distance(list.begin(), list.end()); }

};

#endif
