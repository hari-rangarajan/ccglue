// lookup.hpp
// Copyright (c) 2009-2011 Ben Hanson (http://www.benhanson.net/)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_LOOKUP_HPP
#define LEXERTL_LOOKUP_HPP

#include <assert.h>
#include "bool.hpp"
#include "char_traits.hpp"
#include <iterator>
#include <stack>
#include "state_machine.hpp"

namespace lexertl
{
template<typename iter, typename id_type = std::size_t>
struct basic_match_results
{
    typedef iter iter_type;
    typedef typename std::iterator_traits<iter_type>::value_type char_type;
    typedef typename basic_char_traits<char_type>::index_type index_type;

    id_type id;
    id_type user_id;
    iter_type start;
    iter_type end;
    iter_type eoi;
    bool bol;
    id_type state;

    basic_match_results () :
        id (0),
        user_id (npos ()),
        start (iter_type ()),
        end (iter_type ()),
        eoi (iter_type ()),
        bol (true),
        state (0)
    {
    }

    basic_match_results (const iter_type &start_, const iter_type &end_) :
        id (0),
        user_id (npos ()),
        start (start_),
        end (start_),
        eoi (end_),
        bol (true),
        state (0)
    {
    }

    virtual ~basic_match_results ()
    {
    }

    virtual void clear ()
    {
        id  = 0;
        user_id = npos ();
        start = eoi;
        end = eoi;
        bol = true;
        state = 0;
    }

    virtual void reset (iter_type &start_, iter_type &end_)
    {
        id  = 0;
        user_id = npos ();
        start = start_;
        end  = start_;
        eoi = end_;
        bol = true;
        state = 0;
    }

    static id_type npos ()
    {
        return static_cast<id_type>(~0);
    }

    static id_type skip ()
    {
        return static_cast<id_type>(~1);
    }
};

template<typename iter, typename id_type = std::size_t>
struct basic_push_match_results : public basic_match_results<iter, id_type>
{
    typedef std::pair<id_type, id_type> id_type_pair;
    std::stack<id_type_pair> stack;

    basic_push_match_results () :
        basic_match_results<iter, id_type> ()
    {
    }

    basic_push_match_results (const iter &start_, const iter &end_) :
        basic_match_results<iter, id_type> (start_, end_)
    {
    }

    virtual ~basic_push_match_results ()
    {
    }

    virtual void clear ()
    {
        basic_match_results<iter, id_type>::clear ();

        while (!stack.empty()) stack.pop ();
    }

    virtual void reset (iter &start_, iter &end_)
    {
        basic_match_results<iter, id_type>::reset (start_, end_);

        while (!stack.empty()) stack.pop ();
    }
};

typedef basic_match_results<std::string::const_iterator, std::size_t>
    match_results;
typedef basic_match_results<std::wstring::const_iterator, std::size_t>
    wmatch_results;

typedef basic_push_match_results<std::string::const_iterator, std::size_t>
    push_match_results;
typedef basic_push_match_results<std::wstring::const_iterator, std::size_t>
    wpush_match_results;

namespace detail
{
template<typename iter_type, typename id_type>
void next (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_match_results<iter_type, id_type> &results_, const false_ &)
{
    typedef basic_match_results<iter_type, id_type> results;
    const lexertl::detail::basic_internals<id_type> &internals_ = sm_.data ();
    typename results::iter_type end_token_ = results_.end;

skip:
    typename results::iter_type curr_ = results_.end;

    results_.start = curr_;

again:
    // wchar_t input, char state machine
    if (curr_ == results_.eoi)
    {
        results_.id = internals_._eoi;
        results_.user_id = results::npos ();
        return;
    }

    bool bol_ = results_.bol;
    const id_type *lookup_ = &internals_._lookup[results_.state]->
        front ();
    const id_type dfa_alphabet_ = internals_._dfa_alphabet[results_.state];
    const id_type *dfa_ = &internals_._dfa[results_.state]->front ();
    const id_type *ptr_ = dfa_ + dfa_alphabet_;
    bool end_state_ = *ptr_ != 0;
    id_type id_ = *(ptr_ + id_index);
    id_type uid_ = *(ptr_ + user_id_index);
    id_type start_state_ = results_.state;
    bool end_bol_ = bol_;
    id_type EOL_state_ = 0;

    if (bol_)
    {
        const id_type state_ = *dfa_;

        if (state_)
        {
            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }
    }

    while (curr_ != results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && *curr_ == '\n')
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
        }
        else
        {
            const std::size_t bytes_ =
                sizeof (typename results::char_type) < 3 ?
                sizeof (typename results::char_type) : 3;
            const std::size_t shift_[] = {0, 8, 16};
            typename results::char_type prev_char_ = *curr_++;

            bol_ = prev_char_ == '\n';

            for (std::size_t i_ = 0; i_ < bytes_; ++i_)
            {
                const id_type state_ = ptr_[lookup_[static_cast
                    <unsigned char>((prev_char_ >>
                    shift_[bytes_ - 1 - i_]) & 0xff)]];

                if (state_ == 0)
                {
                    EOL_state_ = 0;
                    goto quit;
                }

                ptr_ = &dfa_[state_ * dfa_alphabet_];
            }
        }

        if (*ptr_)
        {
            end_state_ = true;
            id_ = *(ptr_ + id_index);
            uid_ = *(ptr_ + user_id_index);
            start_state_ = *(ptr_ + next_dfa_index);
            end_bol_ = bol_;
            end_token_ = curr_;
        }
    }

quit:
    if (curr_ == results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                uid_ = *(ptr_ + user_id_index);
                start_state_ = *(ptr_ + next_dfa_index);
                end_bol_ = bol_;
                end_token_ = curr_;
            }
        }
    }

    if (end_state_)
    {
        // Return longest match
        results_.state = start_state_;
        results_.bol = end_bol_;
        results_.end = end_token_;

        if (id_ == sm_.skip ()) goto skip;

        if (id_ == internals_._eoi)
        {
            curr_ = end_token_;
            goto again;
        }
    }
    else
    {
        // No match causes char to be skipped
        results_.end = end_token_;
        results_.bol = *results_.end == '\n';
        results_.start = results_.end;
        ++results_.end;
        id_ = results::npos ();
        uid_ = results::npos ();
    }

    results_.id = id_;
    results_.user_id = uid_;
}

template<typename iter_type, typename id_type>
void next (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_match_results<iter_type, id_type> &results_, const true_ &)
{
    typedef basic_match_results<iter_type, id_type> results;
    typedef typename results::char_type char_type;
    const detail::basic_internals<id_type> &internals_ = sm_.data ();
    typename results::iter_type end_token_ = results_.end;

skip:
    typename results::iter_type curr_ = results_.end;

    results_.start = curr_;

again:
    // Input and state machine use same char type
    if (curr_ == results_.eoi)
    {
        results_.id = internals_._eoi;
        results_.user_id = results::npos ();
        return;
    }

    bool bol_ = results_.bol;
    const id_type *lookup_ = &internals_._lookup[results_.state]->
        front ();
    const id_type dfa_alphabet_ = internals_._dfa_alphabet[results_.state];
    const id_type *dfa_ = &internals_._dfa[results_.state]->front ();
    const id_type *ptr_ = dfa_ + dfa_alphabet_;
    bool end_state_ = *ptr_ != 0;
    id_type id_ = *(ptr_ + id_index);
    id_type uid_ = *(ptr_ + user_id_index);
    id_type start_state_ = results_.state;
    bool end_bol_ = bol_;
    id_type EOL_state_ = 0;

    if (bol_)
    {
        const id_type state_ = *dfa_;

        if (state_)
        {
            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }
    }

    while (curr_ != results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && *curr_ == '\n')
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
        }
        else
        {
            const typename results::char_type prev_char_ = *curr_++;
            const id_type state_ = ptr_[lookup_
                [static_cast<typename results::index_type>(prev_char_)]];

            bol_ = prev_char_ == '\n';

            if (state_ == 0)
            {
                EOL_state_ = 0;
                break;
            }

            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }

        if (*ptr_)
        {
            end_state_ = true;
            id_ = *(ptr_ + id_index);
            uid_ = *(ptr_ + user_id_index);
            start_state_ = *(ptr_ + next_dfa_index);
            end_bol_ = bol_;
            end_token_ = curr_;
        }
    }

    if (curr_ == results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                id_ = *(ptr_ + id_index);
                uid_ = *(ptr_ + user_id_index);
                start_state_ = *(ptr_ + next_dfa_index);
                end_bol_ = bol_;
                end_token_ = curr_;
            }
        }
    }

    if (end_state_)
    {
        // Return longest match
        results_.state = start_state_;
        results_.bol = end_bol_;
        results_.end = end_token_;

        if (id_ == sm_.skip ()) goto skip;

        if (id_ == internals_._eoi)
        {
            curr_ = end_token_;
            goto again;
        }
    }
    else
    {
        // No match causes char to be skipped
        results_.end = end_token_;
        results_.bol = *results_.end == '\n';
        results_.start = results_.end;
        ++results_.end;
        id_ = results::npos ();
        uid_ = results::npos ();
    }

    results_.id = id_;
    results_.user_id = uid_;
}

template<typename iter_type, typename id_type>
void next (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_push_match_results<iter_type, id_type> &results_, const false_ &)
{
    typedef basic_push_match_results<iter_type, id_type> results;
    const lexertl::detail::basic_internals<id_type> &internals_ = sm_.data ();
    typename results::iter_type end_token_ = results_.end;

skip:
    typename results::iter_type curr_ = results_.end;

    results_.start = curr_;

again:
    // wchar_t input, char state machine
    if (curr_ == results_.eoi)
    {
        results_.id = internals_._eoi;
        results_.user_id = results::npos ();
        return;
    }

    bool bol_ = results_.bol;
    const id_type *lookup_ = &internals_._lookup[results_.state]->
        front ();
    const id_type dfa_alphabet_ = internals_._dfa_alphabet[results_.state];
    const id_type *dfa_ = &internals_._dfa[results_.state]->front ();
    const id_type *ptr_ = dfa_ + dfa_alphabet_;
    bool end_state_ = *ptr_ != 0;
    bool pop_ = (*ptr_ & pop_dfa_bit) = 0;
    id_type id_ = *(ptr_ + id_index);
    id_type uid_ = *(ptr_ + user_id_index);
    id_type push_dfa_ = *(ptr_ + push_dfa_index);
    id_type start_state_ = results_.state;
    bool end_bol_ = bol_;
    id_type EOL_state_ = 0;

    if (bol_)
    {
        const id_type state_ = *dfa_;

        if (state_)
        {
            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }
    }

    while (curr_ != results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && *curr_ == '\n')
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
        }
        else
        {
            const std::size_t bytes_ =
                sizeof (typename results::char_type) < 3 ?
                sizeof (typename results::char_type) : 3;
            const std::size_t shift_[] = {0, 8, 16};
            typename results::char_type prev_char_ = *curr_++;

            bol_ = prev_char_ == '\n';

            for (std::size_t i_ = 0; i_ <
                (sizeof (typename results::char_type) < 3 ?
                sizeof (typename results::char_type) : 3); ++i_)
            {
                const id_type state_ = ptr_[lookup_[static_cast
                    <unsigned char>((prev_char_ >>
                    shift_[bytes_ - 1 - i_]) & 0xff)]];

                if (state_ == 0)
                {
                    EOL_state_ = 0;
                    goto quit;
                }

                ptr_ = &dfa_[state_ * dfa_alphabet_];
            }
        }

        if (*ptr_)
        {
            end_state_ = true;
            pop_ = *ptr_ & pop_dfa_bit;
            id_ = *(ptr_ + id_index);
            uid_ = *(ptr_ + user_id_index);
            push_dfa_ = *(ptr_ + push_dfa_index);
            start_state_ = *(ptr_ + next_dfa_index);
            end_bol_ = bol_;
            end_token_ = curr_;
        }
    }

quit:
    if (curr_ == results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                pop_ = (*ptr_ & pop_dfa_bit) != 0;
                id_ = *(ptr_ + id_index);
                uid_ = *(ptr_ + user_id_index);
                push_dfa_ = *(ptr_ + push_dfa_index);
                start_state_ = *(ptr_ + next_dfa_index);
                end_bol_ = bol_;
                end_token_ = curr_;
            }
        }
    }

    if (end_state_)
    {
        // Return longest match
        if (pop_)
        {
            start_state_ = results_.stack.top ().first;
            results_.stack.pop ();
        }
        else if (push_dfa_ != results_.npos ())
        {
            results_.stack.push (typename results::id_type_pair
                (push_dfa_, id_));
        }

        results_.state = start_state_;
        results_.bol = end_bol_;
        results_.end = end_token_;

        if (id_ == sm_.skip ()) goto skip;

        if (id_ == internals_._eoi || (pop_ && !results_.stack.empty () &&
            results_.stack.top ().second == internals_._eoi))
        {
            curr_ = end_token_;
            goto again;
        }
    }
    else
    {
        // No match causes char to be skipped
        results_.end = end_token_;
        results_.bol = *results_.end == '\n';
        results_.start = results_.end;
        ++results_.end;
        id_ = results::npos ();
        uid_ = results::npos ();
    }

    results_.id = id_;
    results_.user_id = uid_;
}

template<typename iter_type, typename id_type>
void next (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_push_match_results<iter_type, id_type> &results_, const true_ &)
{
    typedef basic_push_match_results<iter_type, id_type> results;
    typedef typename results::char_type char_type;
    const detail::basic_internals<id_type> &internals_ = sm_.data ();
    typename results::iter_type end_token_ = results_.end;

skip:
    typename results::iter_type curr_ = results_.end;

    results_.start = curr_;

again:
    // Input and state machine use same char type
    if (curr_ == results_.eoi)
    {
        results_.id = internals_._eoi;
        results_.user_id = results::npos ();
        return;
    }

    bool bol_ = results_.bol;
    const id_type *lookup_ = &internals_._lookup[results_.state]->
        front ();
    const id_type dfa_alphabet_ = internals_._dfa_alphabet[results_.state];
    const id_type *dfa_ = &internals_._dfa[results_.state]->front ();
    const id_type *ptr_ = dfa_ + dfa_alphabet_;
    bool end_state_ = *ptr_ != 0;
    bool pop_ = (*ptr_ & pop_dfa_bit) != 0;
    id_type id_ = *(ptr_ + id_index);
    id_type uid_ = *(ptr_ + user_id_index);
    id_type push_dfa_ = *(ptr_ + push_dfa_index);
    id_type start_state_ = results_.state;
    bool end_bol_ = bol_;
    id_type EOL_state_ = 0;

    if (bol_)
    {
        const id_type state_ = *dfa_;

        if (state_)
        {
            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }
    }

    while (curr_ != results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_ && *curr_ == '\n')
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];
        }
        else
        {
            const typename results::char_type prev_char_ = *curr_++;
            const id_type state_ = ptr_[lookup_
                [static_cast<typename results::index_type>(prev_char_)]];

            bol_ = prev_char_ == '\n';

            if (state_ == 0)
            {
                EOL_state_ = 0;
                break;
            }

            ptr_ = &dfa_[state_ * dfa_alphabet_];
        }

        if (*ptr_)
        {
            end_state_ = true;
            pop_ = (*ptr_ & pop_dfa_bit) != 0;
            id_ = *(ptr_ + id_index);
            uid_ = *(ptr_ + user_id_index);
            push_dfa_ = *(ptr_ + push_dfa_index);
            start_state_ = *(ptr_ + next_dfa_index);
            end_bol_ = bol_;
            end_token_ = curr_;
        }
    }

    if (curr_ == results_.eoi)
    {
        EOL_state_ = ptr_[eol_index];

        if (EOL_state_)
        {
            ptr_ = &dfa_[EOL_state_ * dfa_alphabet_];

            if (*ptr_)
            {
                end_state_ = true;
                pop_ = (*ptr_ & pop_dfa_bit) != 0;
                id_ = *(ptr_ + id_index);
                uid_ = *(ptr_ + user_id_index);
                push_dfa_ = *(ptr_ + push_dfa_index);
                start_state_ = *(ptr_ + next_dfa_index);
                end_bol_ = bol_;
                end_token_ = curr_;
            }
        }
    }

    if (end_state_)
    {
        // Return longest match
        if (pop_)
        {
            start_state_ =  results_.stack.top ().first;
            results_.stack.pop ();
        }
        else if (push_dfa_ != results_.npos ())
        {
            results_.stack.push (typename results::id_type_pair
                (push_dfa_, id_));
        }

        results_.state = start_state_;
        results_.bol = end_bol_;
        results_.end = end_token_;

        if (id_ == sm_.skip ()) goto skip;

        if (id_ == internals_._eoi || (pop_ && !results_.stack.empty () &&
            results_.stack.top ().second == internals_._eoi))
        {
            curr_ = end_token_;
            goto again;
        }
    }
    else
    {
        // No match causes char to be skipped
        results_.end = end_token_;
        results_.bol = *results_.end == '\n';
        results_.start = results_.end;
        ++results_.end;
        id_ = results::npos ();
        uid_ = results::npos ();
    }

    results_.id = id_;
    results_.user_id = uid_;
}
}

template<typename iter_type, typename id_type>
void lookup (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_match_results<iter_type, id_type> &results_)
{
    // If this asserts, you should be using basic_push_match_results
    // instead of basic_match_results.
    assert(!(sm_.data()._features & lexertl::recursive_bit));
    detail::next<iter_type, id_type> (sm_, results_, bool_<sizeof
        (typename std::iterator_traits<iter_type>::value_type) == 1> ());
}

template<typename iter_type, typename id_type>
void lookup (const basic_state_machine<typename std::iterator_traits
    <iter_type>::value_type, id_type> &sm_,
    basic_push_match_results<iter_type, id_type> &results_)
{
    detail::next<iter_type, id_type> (sm_, results_, bool_<sizeof
        (typename std::iterator_traits<iter_type>::value_type) == 1> ());
}
}

#endif
