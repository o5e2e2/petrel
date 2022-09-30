#ifndef NODE_CONTROL_HPP
#define NODE_CONTROL_HPP

enum class NodeControl : bool {
    Continue = false,
    Abort = true
};

#define RETURN_IF_ABORT(control) { NodeControl c{control}; if (c != NodeControl::Continue) { return c; } } ((void)0)

#endif
