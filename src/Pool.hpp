#ifndef POOL_HPP
#define POOL_HPP

#include <list>
#include <mutex>

/**
 * Generic object pool pattern
 **/
template <class Element>
class Pool {
    typedef std::list<Element> list;

    list ready_list; //elements ready for use
    list used_list; //used elements
    std::mutex pool_mutex;

public:
    typedef typename list::iterator element_type;

    element_type empty() { return std::end(used_list); }

    element_type acquire() {
        std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};

        if (ready_list.empty()) {
            //create a new element
            used_list.emplace_front();
        }
        else {
            //reuse an element from the ready list
            used_list.splice(used_list.begin(), ready_list, ready_list.begin());
        }
        return used_list.begin();
    }

    //return the used element to the ready list
    void release(element_type&& element) {
        std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};
        ready_list.splice(ready_list.begin(), used_list, element);
    }

};

#endif
