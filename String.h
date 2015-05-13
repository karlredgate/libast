
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

#ifndef _OBJECT_PRIMITIVES_H_
#define _OBJECT_PRIMITIVES_H_

#include <stdint.h>
#include <cstdlib>
#include "tcl.h"
#include "crc32.h"

namespace Service {

    class String {
    public:
        union {
            __vector unsigned long v;
            struct {
                char    *start;
                uint32_t length;
                uint32_t value;
                uint16_t count;
                bool     allocated;
            };
        };
    
        uint32_t size( char *s ) {
            char *t = s;
            while ( *t ) t++;
            return (t - s);
        }
        char *allocate() {
            allocated = true;
            return (char *)malloc( length + 1 );
        }
        void allocate( uint32_t bytes ) {
            length = bytes;
            start = allocate();
            allocated = true;
        }
        void reallocate( uint32_t bytes ) {
            if ( allocated == false ) return;
            free( start );
            allocate( length + bytes );
        }
        void copy( char *s ) {
            char *d = start;
            while ( *d++ = *s++ );
        }
        void set( char *s ) {
            length = size(s);
            start  = allocate();
            copy( s );
        }
        uint32_t limit( String *that ) {
            if ( this->length < that->length )  return this->length;
            return that->length;
        }
    
        String() : start(0), length(0), count(0), allocated(false) {};
        String( char *s ) { set(s); }
        String( String &s) { set(s.start); }
        String( Tcl_Interp *, Tcl_Obj * );
        // virtual ~String() { if ( allocated ) free(start); }
        ~String();
        void * operator new ( std::size_t, Pool * );
        void destroy(Pool *);
    
        bool eq( String *that ) {
            if ( this->start == 0 ) return false;
            if ( this->length != that->length ) return false;
            register char *s = this->start;
            register char *t = that->start;
            register uint32_t remaining = this->length;
            while ( remaining > 0 ) {
                if ( *s++ != *t++ ) return false;
                remaining--;
            }
            return true;
        }
        bool ne( String *that ) {
            if ( this->start == 0 ) return false;
            if ( this->length != that->length ) return true;
            register char *s = this->start;
            register char *t = that->start;
            register uint32_t remaining = this->length;
            while ( remaining > 0 ) {
                if ( *s++ != *t++ ) return true;
                remaining--;
            }
            return false;
        }
        bool lt( String *that ) {
            if ( this->start == 0 ) return false;
            register uint32_t remaining = limit(that);
            register char *l = this->start;
            register char *r = that->start;
            while ( remaining-- ) {
                if ( *l > *r ) return false;
                if ( *l < *r ) return true;
                l++, r++;
            }
            return (this->length < that->length);
        }
        bool gt( String *that ) {
            if ( this->start == 0 ) return false;
            register uint32_t remaining = limit(that);
            register char *l = this->start;
            register char *r = that->start;
            while ( remaining-- ) {
                if ( *l < *r ) return false;
                if ( *l > *r ) return true;
                l++, r++;
            }
            return (this->length > that->length);
        }
        bool le( String *that ) {
            if ( this->start == 0 ) return false;
            register uint32_t remaining = limit(that);
            register char *l = this->start;
            register char *r = that->start;
            while ( remaining-- ) {
                if ( *l > *r ) return false;
                l++, r++;
            }
            return (this->length <= that->length);
        }
        bool ge( String *that ) {
            if ( this->start == 0 ) return false;
            register uint32_t remaining = limit(that);
            register char *l = this->start;
            register char *r = that->start;
            while ( remaining-- ) {
                if ( *l < *r ) return false;
                l++, r++;
            }
            return (this->length >= that->length);
        }
        
        // int compare( String *that ) { }
    };
    OStream& operator << (OStream& lhs, const String& rhs);
    namespace Parse {
        class Procedure;
        class Processor;
    }
    class Field : public String {
    public:
        union {
            __vector unsigned long w;
            struct {
                char *end;
                Field *parent;
                Parse::Procedure *parser;
                Parse::Processor *processor;
            };
        };
    
        Field() : parent(0), parser(0), processor(0) {}
        // virtual ~Field();
        ~Field();
    };
    OStream& operator << (OStream& lhs, const Field& rhs);
    inline void ClearFields( uint32_t count, Field *f ) {
        register uint32_t n = (count + 31) >> 5;
    #ifdef __ALTIVEC__
    UINFO( 5, "AltiVec: Clear fields" << endl );
    __vector unsigned long zero = vec_splat_u32(0);
    switch ( count & 0x1f ) {
    case  0: do { f->v = zero; f++;
    case 31:      f->v = zero; f++;
    case 30:      f->v = zero; f++;
    case 29:      f->v = zero; f++;
    case 28:      f->v = zero; f++;
    case 27:      f->v = zero; f++;
    case 26:      f->v = zero; f++;
    case 25:      f->v = zero; f++;
    case 24:      f->v = zero; f++;
    case 23:      f->v = zero; f++;
    case 22:      f->v = zero; f++;
    case 21:      f->v = zero; f++;
    case 20:      f->v = zero; f++;
    case 19:      f->v = zero; f++;
    case 18:      f->v = zero; f++;
    case 17:      f->v = zero; f++;
    case 16:      f->v = zero; f++;
    case 15:      f->v = zero; f++;
    case 14:      f->v = zero; f++;
    case 13:      f->v = zero; f++;
    case 12:      f->v = zero; f++;
    case 11:      f->v = zero; f++;
    case 10:      f->v = zero; f++;
    case  9:      f->v = zero; f++;
    case  8:      f->v = zero; f++;
    case  7:      f->v = zero; f++;
    case  6:      f->v = zero; f++;
    case  5:      f->v = zero; f++;
    case  4:      f->v = zero; f++;
    case  3:      f->v = zero; f++;
    case  2:      f->v = zero; f++;
    case  1:      f->v = zero; f++;
             } while ( --n > 0 );
    }
    
    #else
    #warning Disabling AltiVec
    switch ( count & 0x1f ) {
    case  0: do { f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 31:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 30:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 29:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 28:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 27:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 26:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 25:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 24:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 23:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 22:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 21:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 20:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 19:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 18:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 17:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 16:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 15:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 14:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 13:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 12:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 11:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case 10:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  9:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  8:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  7:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  6:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  5:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  4:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  3:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  2:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
    case  1:      f->start = 0; f->length = 0; f->count = 0; f->value = 0; f++;
             } while ( --n > 0 );
    }
    
    #endif
    }
    
    class Predicate {
    public:
        virtual ~Predicate() {}
        virtual bool operator () ( Context * ) = 0;
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
        virtual void destroy(Pool *);
    };
    class AnnotatedPredicate {
    public:
        struct Annotation {
            bool evaluated : 1;
            bool result    : 1;
        } annotation;
        virtual ~AnnotatedPredicate() {}
        virtual bool operator () ( Context * ) = 0;
    };
    
    class Glob {
    protected:
        class String {
            String() {}
        public:
            char *s;
            int length;
        
            String( char *s, int length ) : s(s), length(length) { }
            bool     empty() const { return (length <= 0); }
            bool not_empty() const { return (length > 0); }
        
            void advance() { s++; length--; }
            char pop() {
                char c = *s;
                advance();
                return c;
            }
        };
        
        class Predicate {
        public:
            virtual ~Predicate() {}
            virtual bool operator () ( String& subject ) = 0;
            void * operator new ( std::size_t, Pool * );
            void operator delete ( void * ) {}
            virtual void destroy(Pool *);
        };
        class isEmpty : public Predicate {
        public:
            isEmpty() { }
            ~isEmpty() {}
            virtual bool operator () ( String& subject ) {
                return subject.empty();
            }
        };
        class Skip : public Predicate {
        public:
            Skip() { }
            ~Skip() {}
            virtual bool operator () ( String& subject ) {
                if ( subject.empty() ) return false;
                subject.advance();
                return true;
            }
        };
        class Matches : public Predicate {
            char a;
        public:
            Matches( char a ) : a(a) { }
            ~Matches() {}
            virtual bool operator () ( String& subject ) {
                if ( subject.empty() )  return false;
                return ( subject.pop() == a );
            }
        };
        
        class State {
        protected:
        public:
            Predicate *predicate;
            State     *match;
            State     *otherwise;
            bool      isKleene;
        
            State() : predicate(0), match(0), otherwise(0), isKleene(false) { }
            virtual ~State() {}
            void * operator new ( std::size_t, Pool * );
            void operator delete ( void * ) {}
            virtual void destroy(Pool *);
        
            State *next( String& subject ) {
                if ( (*predicate)(subject) )  return match;
                return otherwise;
            }
        
            virtual bool accept() { return false; }
            virtual bool reject() { return false; }
        };
        class Accept : public State {
        public:
            Accept() : State() { }
            virtual bool accept() { return true; }
        };
        class Reject : public State {
        public:
            Reject() : State() { }
            virtual bool reject() { return true; }
        };
        
        State *start;
    public:
        Glob(Pool *, const char *);
        ~Glob() {}
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
        void destroy( Pool * );
        bool match( char *, uint32_t );
    };
    
    class IntegerCoercion {
    public:
        virtual ~IntegerCoercion() {}
        virtual uint32_t operator() ( Context * ) = 0;
        virtual void destroy(Pool *);
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
    };
    class StringCoercion {
    public:
        virtual String * operator() ( Context * ) = 0;
        virtual ~StringCoercion() {}
        void * operator new ( std::size_t, Pool * );
        void operator delete ( void * ) {}
        virtual void destroy(Pool *);
    };
    
    class IntegerIdentity : public IntegerCoercion {
        uint32_t value;
    public:
        IntegerIdentity( uint32_t value ) : value(value) { }
        virtual ~IntegerIdentity() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() ( Context *context ) {
            return value;
        }
    };
    class StringLength : public IntegerCoercion {
        StringCoercion *coercion;
    public:
        StringLength( StringCoercion *coercion )
        : coercion(coercion) { }
        virtual ~StringLength() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() (Context *);
    };
    class FieldLength : public IntegerCoercion {
        Field *dr;
    public:
        FieldLength( Field *dr ) : dr(dr) { }
        virtual ~FieldLength() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() ( Context *context ) {
            return dr->length;
        }
    };
    class FieldValue : public IntegerCoercion {
        Field *dr;
    public:
        FieldValue( Field *dr ) : dr(dr) { }
        virtual ~FieldValue() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() ( Context *context ) {
            if ( dr->count == 0 ) return 0;
            return dr->value;
        }
    };
    class FieldCRC32 : public IntegerCoercion {
        StringCoercion *input;
        bool annotated;
        uint32_t value;
    public:
        FieldCRC32( StringCoercion *input ) : input(input) { }
        virtual ~FieldCRC32() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() (Context *);
    };
    class IntegerFork : public IntegerCoercion {
        Predicate *predicate;
        IntegerCoercion *trueClause;
        IntegerCoercion *falseClause;
    public:
        IntegerFork(
            Predicate *predicate,
            IntegerCoercion *trueClause,
            IntegerCoercion *falseClause
        )
        : predicate(predicate),
          trueClause(trueClause),
          falseClause(falseClause) { }
    
        virtual ~IntegerFork() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() (Context *);
    };
    class StringIdentity : public StringCoercion {
        String *value;
    public:
        StringIdentity( String *value ) : value(value) {}
        virtual ~StringIdentity() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    };
    class FieldString : public StringCoercion {
        Field *dr;
    public:
        FieldString( Field *dr ) : dr(dr) { }
        virtual ~FieldString() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    };
    class hexdecode : public StringCoercion {
        StringCoercion *input;
        bool annotated;
        String annotation;
        static const char nybble[];
    public:
        hexdecode( StringCoercion *input )
        : input(input), annotated(false) {
            annotation.allocate( 1024 );
        }
        virtual ~hexdecode() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    };
    class s_lowercase : public StringCoercion {
        StringCoercion *input;
        bool annotated;
        String annotation;
    public:
        s_lowercase( StringCoercion *input )
        : input(input), annotated(false) {
            annotation.allocate( 1024 );
        }
        virtual ~s_lowercase() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    };
    class s_date : public StringCoercion {
        uint32_t delta;
        bool annotated;
        String annotation;
    public:
        s_date( uint32_t delta )
        : delta(delta), annotated(false) {
            // "Wdy, DD-Mon-YYYY HH:MM:SS UTC"
            annotation.allocate( 30 );
            annotation.count = 1;
        }
        virtual ~s_date() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    };
    class StringFork : public StringCoercion {
        Predicate *predicate;
        StringCoercion *trueClause;
        StringCoercion *falseClause;
    public:
        StringFork(
            Predicate *predicate,
            StringCoercion *trueClause,
            StringCoercion *falseClause
        )
        : predicate(predicate),
          trueClause(trueClause),
          falseClause(falseClause) { }
    
        virtual ~StringFork() {}
        virtual void destroy(Pool *);
        virtual String * operator() (Context *);
    
    };
    class ClientAddress : public IntegerCoercion {
    public:
        virtual ~ClientAddress() {}
        virtual void destroy(Pool *);
        virtual uint32_t operator() ( Context * );
    };
    
    class T : public Predicate {
    public:
        T() { }
        virtual ~T() {}
        virtual void destroy(Pool *pool) {}
        virtual bool
        operator () ( Context *context ) { return true; }
    };
    class F : public Predicate {
    public:
        F() { }
        virtual ~F() {}
        virtual void destroy(Pool *pool) {}
        virtual bool
        operator () ( Context *context ) { return false; }
    };
    class Matches : public Predicate {
        StringCoercion *ff;
        Glob *speglob;
    public:
        Matches( StringCoercion *ff, Glob *glob )
        : ff(ff), speglob(glob) {}
        virtual ~Matches() {}
        virtual bool operator() ( Context * );
        virtual void destroy(Pool *);
    };
    class present : public Predicate {
        StringCoercion *coercion;
    public:
        present( StringCoercion *coercion ) : coercion(coercion) {}
        virtual ~present() { }
        virtual bool operator() ( Context * ) ;
        virtual void destroy( Pool * );
    };
    class absent : public Predicate {
        StringCoercion *coercion;
    public:
        absent( StringCoercion *coercion ) : coercion(coercion) {}
        virtual ~absent() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context * );
    };
    class i_eq_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_eq_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_eq_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_eq_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) == (*rhs)(context) );
        }
    };
    class i_eq_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_eq_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_eq_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_eq_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) == rhs );
        }
    };
    
    class i_ne_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_ne_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_ne_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_ne_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) != (*rhs)(context) );
        }
    };
    class i_ne_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_ne_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_ne_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_ne_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) != rhs );
        }
    };
    
    class i_lt_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_lt_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_lt_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_lt_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) < (*rhs)(context) );
        }
    };
    class i_lt_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_lt_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_lt_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_lt_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) < rhs );
        }
    };
    
    class i_gt_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_gt_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_gt_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_gt_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) > (*rhs)(context) );
        }
    };
    class i_gt_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_gt_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_gt_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_gt_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) > rhs );
        }
    };
    
    class i_le_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_le_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_le_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_le_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) <= (*rhs)(context) );
        }
    };
    class i_le_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_le_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_le_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_le_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) <= rhs );
        }
    };
    
    class i_ge_r_r : public Predicate {
        IntegerCoercion *lhs;
        IntegerCoercion *rhs;
    public:
        i_ge_r_r( IntegerCoercion *lhs, IntegerCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_ge_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::i_ge_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) >= (*rhs)(context) );
        }
    };
    class i_ge_r_i : public Predicate {
        IntegerCoercion *lhs;
        uint32_t rhs;
    public:
        i_ge_r_i( IntegerCoercion *lhs, uint32_t rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~i_ge_r_i() { }
        virtual void destroy(Pool *pool) {
            UINFO( 8, "Predicate::i_ge_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            return ( (*lhs)(context) >= rhs );
        }
    };
    
    class s_eq_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_eq_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_eq_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_eq_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->eq(b) );
        }
    };
    class s_eq_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_eq_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_eq_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_eq_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->eq(rhs) );
        }
    };
    
    class s_ne_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_ne_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_ne_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_ne_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->ne(b) );
        }
    };
    class s_ne_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_ne_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_ne_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_ne_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->ne(rhs) );
        }
    };
    
    class s_lt_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_lt_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_lt_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_lt_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->lt(b) );
        }
    };
    class s_lt_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_lt_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_lt_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_lt_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->lt(rhs) );
        }
    };
    
    class s_gt_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_gt_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_gt_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_gt_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->gt(b) );
        }
    };
    class s_gt_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_gt_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_gt_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_gt_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->gt(rhs) );
        }
    };
    
    class s_le_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_le_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_le_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_le_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->le(b) );
        }
    };
    class s_le_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_le_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_le_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_le_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->le(rhs) );
        }
    };
    
    class s_ge_r_r : public Predicate {
        StringCoercion *lhs;
        StringCoercion *rhs;
    public:
        s_ge_r_r( StringCoercion *lhs, StringCoercion *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_ge_r_r() {}
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_ge_r_r: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            String *b = (*rhs)(context);
            return ( a->ge(b) );
        }
    };
    class s_ge_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_ge_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_ge_r_i() { }
        virtual void destroy( Pool *pool ) {
            UINFO( 8, "Predicate::s_ge_r_i: destroy" << endl );
            if ( lhs ) lhs->destroy( pool );
            if ( rhs ) rhs->destroy( pool );
            Predicate::destroy( pool );
        }
        virtual bool operator() ( Context *context ) {
            String *a = (*lhs)(context);
            return ( a->ge(rhs) );
        }
    };
    
    class s_prefix_r_i : public Predicate {
        StringCoercion *lhs;
        String *rhs;
    public:
        s_prefix_r_i( StringCoercion *lhs, String *rhs )
        : lhs(lhs), rhs(rhs)  {}
        virtual ~s_prefix_r_i() {}
        virtual void destroy( Pool * );
        virtual bool operator() ( Context * );
    };
    class Contains : public Predicate {
        IntegerCoercion *coercion;
        uint32_t mask;
    public:
        Contains( IntegerCoercion *coercion, uint32_t mask )
        : coercion(coercion), mask(mask) {}
        virtual ~Contains() {}
        virtual void destroy( Pool * );
        virtual bool operator() ( Context * );
    };
    class BinaryLogic : public Predicate {
    protected:
        Predicate *lhs;
        Predicate *rhs;
    public:
        BinaryLogic( Predicate *lhs, Predicate *rhs )
        : lhs(lhs), rhs(rhs) {}
        virtual ~BinaryLogic() {}
        virtual void destroy(Pool *);
    };
    class OR : public BinaryLogic {
    public:
        OR( Predicate *lhs, Predicate *rhs )
        : BinaryLogic(lhs, rhs) {}
        virtual bool operator() ( Context * );
        virtual void destroy(Pool *);
    };
    class NOR : public BinaryLogic {
    public:
        NOR( Predicate *lhs, Predicate *rhs )
        : BinaryLogic(lhs, rhs) {}
        virtual void destroy(Pool *);
        virtual bool operator() ( Context * );
    };
    class AND : public BinaryLogic {
    public:
        AND( Predicate *lhs, Predicate *rhs )
        : BinaryLogic(lhs, rhs) {}
        virtual void destroy(Pool *);
        virtual bool operator() ( Context * );
    };
    class NAND : public BinaryLogic {
    public:
        NAND( Predicate *lhs, Predicate *rhs )
        : BinaryLogic(lhs, rhs) {}
        virtual void destroy(Pool *);
        virtual bool operator() ( Context * );
    };
    class NOT : public Predicate {
        Predicate *operand;
    public:
        NOT( Predicate *operand ) : operand(operand) { }
        virtual ~NOT() {}
        virtual bool operator() ( Context * );
        virtual void destroy(Pool *);
    };
    class AddressMatches : public Predicate {
        uint32_t matchAddress;
        uint32_t mask;
    public:
        AddressMatches( uint32_t addr, uint32_t bits ) {
            mask = 0xffffffff << (32-bits);
            matchAddress = (addr & mask);
        }
        virtual ~AddressMatches() {}
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class LocationMatchAllPorts : public Predicate {
        Field *field;
    public:
        LocationMatchAllPorts( Field *field ) : field(field) { }
        virtual ~LocationMatchAllPorts() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class LocationMatchOnePortNeeded : public Predicate {
        Field *field;
    public:
        LocationMatchOnePortNeeded( Field *field ) : field(field) { }
        virtual ~LocationMatchOnePortNeeded() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class ConnectionDelete : public Predicate {
    public:
        ConnectionDelete() { }
        virtual ~ConnectionDelete() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class ConnectionInsert : public Predicate {
    public:
        ConnectionInsert() { }
        virtual ~ConnectionInsert() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class SetCookieInsert : public Predicate {
    public:
        SetCookieInsert() { }
        virtual ~SetCookieInsert() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class IsPassive : public Predicate {
    public:
        IsPassive() { }
        virtual ~IsPassive() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    class SSLCipherInsert : public Predicate {
    public:
        SSLCipherInsert() { }
        virtual ~SSLCipherInsert() { }
        virtual void destroy(Pool *);
        virtual bool operator() ( Context *context );
    };
    
    bool Initialize( Tcl_Interp *, OPE * );
}
#endif

/* vim: set autoindent expandtab sw=4 : */
