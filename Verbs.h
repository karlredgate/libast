
/*
 * Copyright (c) 2012 Karl N. Redgate
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdint.h>
#include "tcl.h"
#include "crc32.h"

#ifndef _OBJECTPOLICY_H_
#define _OBJECTPOLICY_H_

namespace Service {
    // using namespace ObjectProcessing;

    class Verb {
    protected:
        Verb *next;
    public:
        Verb( Verb *_next ) : next(_next) { }
        virtual ~Verb() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context& ) = 0;
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
    };

    class NullVerb : public Verb {
    public:
        NullVerb() : Verb(0) { }
        virtual ~NullVerb() {}
        virtual void destroy(Pool *);
        virtual void operator() (Context&);
    };

    class IfVerb : public Verb {
        Predicate *predicate;
        Verb      *block;
    public:
        IfVerb( Predicate *predicate, Verb *block, Verb *next )
        : predicate(predicate), block(block), Verb(next) { }
        virtual ~IfVerb() {}
        virtual void destroy(Pool *);
        virtual void operator() (Context&);
    };

    class Selection {
    public:
        Predicate *predicate;
        Verb      *block;
        Selection *next;
    
        Selection( Predicate *predicate, Verb *block, Selection *next )
        : predicate(predicate), block(block), next(next) { }
        ~Selection() {}
        void destroy(Pool *);
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
    };

    class Cond : public Verb {
        Selection *selection;
    public:
        Cond( Selection *selection, Verb *next )
        : selection(selection), Verb(next) { }
        virtual ~Cond() {}
        virtual void destroy(Pool *);
        virtual void operator() (Context&);
    };

    class fpID : public Verb {
        uint16_t id;
    public:
        fpID( uint16_t id, Verb *next )
        : Verb(next), id(id) { }
        virtual ~fpID() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context & );
    };

    class tunnel : public Verb {
    public:
        tunnel( Verb *verb ) : Verb(verb) { }
        virtual ~tunnel() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context & );
    };

    class cookiePersist : public Verb {
    public:
        ObjectProcessing::Persistence::Policy *persistence;
        cookiePersist(
            ObjectProcessing::Persistence::Policy *persistence,
            Verb *verb
        ) : persistence(persistence), Verb(verb) { }
        virtual ~cookiePersist() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context & );
    };

    class dont_retry : public Verb {
    public:
        dont_retry( Verb *verb ) : Verb(verb) { }
        virtual ~dont_retry() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context & );
    };

    class closeOptim : public Verb {
    public:
        closeOptim( Verb *verb ) : Verb(verb) { }
        virtual ~closeOptim() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context & );
    };

    class stickyVariable : public Verb {
        IntegerCoercion *coercion;
    public:
        stickyVariable( IntegerCoercion *coercion, Verb *verb )
        : Verb(verb), coercion(coercion) { }
        virtual ~stickyVariable() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context& );
    };

    class cookieNOOP : public Verb {
    public:
        cookieNOOP( Verb *verb ) : Verb(verb) { }
        virtual ~cookieNOOP() {}
        virtual void destroy(Pool *);
        virtual void operator() ( Context &state ) { (*next)( state ); }
    };

    bool Initialize( Tcl_Interp *, OPE * );
}
#endif

/* vim: set autoindent expandtab sw=4 : */
