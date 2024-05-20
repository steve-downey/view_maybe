// smd/maybe.h                                               -*-C++-*-
#ifndef INCLUDED_SMD_MAYBE
#define INCLUDED_SMD_MAYBE


//@PURPOSE:
//
//@CLASSES:
//
//@AUTHOR: Steve Downey (sdowney)
//
//@DESCRIPTION:

namespace smd {
template <class T>
concept maybe = requires(const T t) {
    bool(t);
    *(t);
};

} // namespace smd
#endif
