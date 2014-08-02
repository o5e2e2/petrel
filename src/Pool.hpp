#ifndef POOL_HPP
#define POOL_HPP

#include <forward_list>
#include <mutex>

/**
 * Generic object pool pattern
 **/
template <class Element>
class Pool {
    std::mutex pool_mutex;

    typedef std::forward_list<Element> list_t;
    list_t used;
    list_t ready;

public:
    typedef typename list_t::iterator element_type;

    Element& operator[] (const element_type& that) { return *std::next(that); }
    bool empty(const element_type& that) { return that == used.end(); }
    void clear(element_type& that) { that = used.end(); }

    element_type acquire() {
        std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};

        if (ready.empty()) {
            used.emplace_front();
        }
        else {
            used.splice_after(used.before_begin(), ready, ready.before_begin());
        }

        return used.before_begin();
    }

    //return the used element to the ready list
    void release(element_type&& element) {
        if (!empty(element)) {
            std::lock_guard<decltype(pool_mutex)> lock{pool_mutex};

            ready.splice_after(ready.before_begin(), used, element);
            clear(element);
        }
    }

};

#endif
