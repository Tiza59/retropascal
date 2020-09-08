// basic types of the p-code CPU
// (C) 2013 Frank Seide

#pragma once

#include <stdexcept>
#include <stdarg.h>     // for varargs (debug message)
#include <typeinfo>     // for typeid (debug message)

void dprintf(const char * format, ...);

namespace psystem
{

typedef unsigned __int16 WORD;
typedef unsigned __int16 WORD2;
typedef __int16 INT;                // a WORD that is a signed int
typedef unsigned __int8 BYTE;
typedef __int8 SBYTE;               // a BYTE that is signed
typedef BYTE CODE;                  // a BYTE that is an instruction (or part of it)
typedef BYTE CHAR;                  // a BYTE that is a character
template<typename T> struct PTR     // a WORD that is an address
{
    WORD offset;                    // use: p = (RELPTR&) item;
    PTR(const T * elem, void * membottom) : offset(intptr_t(elem) - intptr_t(membottom)/*((const char *)(elem)) - (const char *) membottom*/) { }
    PTR() : offset(0/*NIL*/) { }
};
// a reference to a BYTE ("internal byte pointer")
// Note that doc states that these are one word, but LDB and STB obviously
// use 2 words, as can be seen from what's on the stack.
// Also note that these are also generated by p-code directly, other than through
// conversion functions.
struct BYTEPTR
{
    INT byte;                       // index of the byte (>1 possible). Note: I have seen negative values here
    PTR<BYTE> addr;                 // address of the word
};
template<typename T> struct RELPTR  // a WORD that is a self-referencing pointer
{
    WORD offset;                    // use: p = (RELPTR&) item;
    operator T*()       { return (T*) (intptr_t(&offset) - offset); }   // self-rel: p[0] = byte offset to subtract from p
    operator T*() const { return (T*) (intptr_t(&offset) - offset); }
};
typedef float REAL;                 // 4 bytes

// error codes
enum XEQERRWD
{
    INVNDX = 0x01,  // Invalid index
    NOPROC = 0x02,  // Non-existent segment
    NOEXIT = 0x03,  // Exitting procedure never called
    STKOVR = 0x04,  // stack overflow (note: need to prevent recursive overflow)
    INTOVR = 0x05,  // Integer overflow
    DIVZER = 0x06,  // Divide by zero
    BADMEM = 0x07,  // Bad memory access ("PDP-11 error only" vs. 'NIL pointer reference' in SYSTEM.TEXT)
    UBREAK = 0x08,  // User break
    SYIOER = 0x09,  // System IO error
    UIOERR = 0x0A,  // User IO error
    NOTIMP = 0x0B,  // Instruction not implemented (need to save IP)
    FPIERR = 0x0C,  // Floating point error
    S2LONG = 0x0D,  // String too long (need to save PC)
    HLT    = 0x0E,  // Unconditional halt (need to save PC)
    BPTHLT = 0x0F,  // Conditional halt or breakpoint
    // non-standard:
    invalidpcode = 100,         // p-code is invalid, e.g. a p-code operand is out of range
    invalidpcodelogic = 101     // p-code is invalid, e.g. calling a segment that was not explicitly loaded
};

// TODO: which one to return for a disk drive without a disk in it?
enum IORSLTWD
{
    INOERROR,               // error strings from SYSTEM PRINTERROR:
    IBADBLOCK,              // 'parity (CRC)';
    IBADUNIT,               // 'illegal unit #';
    IBADMODE,               // 'illegal IO request';
    ITIMEOUT,               // 'data-com timeout';
    ILOSTUNIT,              // 'vol went off-line';
    ILOSTFILE,              // 'file lost in dir';
    IBADTITLE,              // 'bad file name';
    INOROOM,                // 'no room on vol';
    INOUNIT,                // 'vol not found';
    INOFILE,                // 'file not found';
    IDUPFILE,               // 'dup dir entry';
    INOTCLOSED,             // 'file already open';
    INOTOPEN,               // 'file not open';
    IBADFORMAT,             // 'bad input format';
    ISTRGOVFL,              // 'ring buffer overflow';
    // these are emitted by the FILER; I make up the codes
    IDISKWRITEPROTECTED,    // 'disk write protected'
    IILLEGALBLOCKNO,        // 'illegal block #'
    IBADBYTECOUNT,          // 'bad byte count'
    IBADINITRECORD,         // 'bad init record'
    // our own special codes for bug fixes etc.
    INOERROR_return_2 = 50, // tells emulatesyste() to fix a bug in BLOCKREAD of TEXT from char device (EDITOR expects read #blocks to be 2)
    ICALLAGAIN,             // UREAD would block; need to call again
    // Apple codes
    deviceerror = 64,       // Apple TechNote #10
    XEQERRbase = 100        // Apple TechNote #10 states that halt codes are reported as IO results by adding 100
};

// exceptions
static void abouttothrow() { }  // call this right before a 'throw' so we can catch it with a break point here
class psystemexception : public std::exception { public: psystemexception(const char * s) : std::exception(s) { } };
// runtime errors
class xeqerror : public psystemexception
{
public:
    XEQERRWD xeqerr;
    xeqerror(const char * s, XEQERRWD x) : xeqerr(x), psystemexception(s)
    {
        const type_info & xx = typeid(this);
        dprintf("xeqerror (%s) caught: %s, %d\n", typeid(this).name(), s, x);
    }
};
// convenience definitions for xeqerror
// TODO: rename these into e.g. invndxerror (same name as error code)
// non-errors:
class breakpointhit: public xeqerror { public: breakpointhit() : xeqerror("BPT instruction", BPTHLT) { } };
class systemexit: public xeqerror { public: systemexit() : xeqerror("XIT instruction", HLT) { } };
// non-fatal?
class intoverflow : public xeqerror { public: intoverflow() : xeqerror("integer overflow", INTOVR) { } };
class outofbounds : public xeqerror { public: outofbounds() : xeqerror("out of bounds", INVNDX) { } };
__declspec(noinline) static void throwoutofbounds() { abouttothrow(); throw outofbounds(); }
__forceinline static void checkboundscondition(bool cond) { if (!cond) throwoutofbounds(); }
class divbyzero: public xeqerror { public: divbyzero() : xeqerror("division by zero", DIVZER) { } };
// fatal:
class stringoverflow : public xeqerror { public: stringoverflow() : xeqerror("string overflow", S2LONG) { } };
class stackoverflow : public xeqerror { public: stackoverflow() : xeqerror("cpu stack overflow", STKOVR) { } };
__declspec(noinline) static void throwstackoverflow() { abouttothrow(); throw stackoverflow(); }
class framestackoverflow : public xeqerror { public: framestackoverflow() : xeqerror("frame stack overflow", STKOVR) { } };
class heapoverflow : public xeqerror { public: heapoverflow() : xeqerror("heap overflow", STKOVR) { } };
// pcode errors:
class invalidpcodeerror: public psystemexception { public: invalidpcodeerror(const char * s) : psystemexception(s) { } };
class invalidpcodearg: public invalidpcodeerror { public: invalidpcodearg() : invalidpcodeerror("invalid p-code argument") { } };
__declspec(noinline) static void throwinvalidpcodearg() { abouttothrow(); throw invalidpcodearg(); }
class stackunderflow : public invalidpcodeerror { public: stackunderflow() : invalidpcodeerror("cpu stack underflow") { } };
__declspec(noinline) static void throwstackunderflow() { abouttothrow(); throw stackunderflow(); }
class heapunderflow : public invalidpcodeerror { public: heapunderflow() : invalidpcodeerror("heap underflow") { } };
class framestackunderflow : public invalidpcodeerror { public: framestackunderflow() : invalidpcodeerror("frame stack underflow") { } };
class badprocedureid : public invalidpcodeerror { public: badprocedureid() : invalidpcodeerror("bad procedure id") { } };
__declspec(noinline) static void throwbadprocedureid() { abouttothrow(); throw badprocedureid(); }
// ^^ TODO: badprocedureid is also used for bad segment id

// a reference to a packed field
struct PACKEDPTR
{
    WORD shift;                     // index of last bit (max 15)
    WORD fieldwidth;                // number of bits
    PTR<WORD> addr;                 // address of word
    void check() const
    { 
        if (shift + fieldwidth > 16)
            throwinvalidpcodearg();       // invalid code
    }
    size_t mask() const { return (1 << fieldwidth) - 1; }
    WORD get(WORD bits) { check(); return (bits >> shift) & mask(); }
    void insert(WORD & bits, WORD val)
    {
        check(); 
        //if (val > mask())                 // not correct--e.g. b[2]:=odd(3) relies on not checking this
        //    throw intoverflow();
        WORD shiftedmask = mask() << shift; // bit mask where value goes
        bits &= ~shiftedmask;               // remove current value
        val &= mask();                      // force in range
        bits |= (val << shift);             // note: it is guaranteed here that the value fits into the bits
    }
    PACKEDPTR(PTR<WORD> arr, size_t elementsperword, size_t fieldw, size_t index)   // constructor for ARRAY (not RECORD) access (IXP, S1P)
    {
        // TODO: isn't this redundant? We only need the field width, or not?
        if (fieldw * elementsperword > 16)
            throwinvalidpcodearg();
        fieldwidth = fieldw;
        size_t whichword = index / elementsperword;
        size_t whichelement = index % elementsperword;
        addr = arr;
        addr.offset += whichword;
        shift = fieldw * whichelement;
        check();
    }
};

template<int N> struct pfixedstring
{
private:
    void check(size_t k1) const { if (k1 == 0 || k1 > length) throw outofbounds(); }
public:
    BYTE length;
    CHAR chars[N];                          // characters stored right after the length byte
    size_t size() const { return length; }
    bool empty() const { return size() == 0; }
    CHAR *       begin()       { return chars; }
    const CHAR * begin() const { return chars; }
    const CHAR * end() const { return begin() + size(); }
    // Note: index here is 1-based, following Pascal conventions. That's why we don't call these operator[].
    CHAR &     at(size_t k1)       { check(k1); return begin()[k1-1]; }
    const CHAR at(size_t k1) const { check(k1); return begin()[k1-1]; }
    // assign to a string that has been allocated for max 'allocsize' characters
    template<class PSTRING>
    void assign(const PSTRING & other, size_t allocsize = N/*default: actual allocation*/)
    {
        if (other.size() > allocsize)
            throw stringoverflow();
        memcpy(this, &other, other.size()+1);
#ifdef _DEBUG   // 0-terminate it for easier reading in the debugger
        // nasty one! A STRING[7] is passed as a VAR STRING[40], so we set a terminator and overwrite the next variable after it...
        //if (size() < allocsize)
        //    chars[size()] = 0;
#endif
    }
    // compare a string
    // note: CHAR is unsigned, so cannot just use strcmp() or STL trivially
    template<class PSTRING>
    int compare(const PSTRING & other) const
    {
        for (size_t k1 = 1; k1 <= size() && k1 <= other.size(); k1++)
        {
            int diff = (int) at(k1) - (int) other.at(k1);
            if (diff != 0)
                return diff;
        }
        return (int) size() - (int) other.size();
    }
    // convert to STL string
    std::string tostring() const
    {
        return std::string((const char*)begin(), (const char*)end());
    }
};

// only use this as a pointer
typedef pfixedstring<1> pstring;

template<int N> struct pfixedbitset
{
    WORD allocsize;         // number of *words* (max. 255)
    WORD bits[(N+15)/16];   // bit storage
    // single-bit operations
    bool inrange(size_t i) const { return i / 16 < allocsize; }
    void check(size_t i) const { if (!inrange(i)) throw outofbounds(); }
    WORD & word(size_t i)             { check(i); return bits[i / 16]; }
    const WORD & word(size_t i) const { check(i); return bits[i / 16]; }
    size_t mask(size_t i) const { return 1 << (i % 16); }
    bool testbit(size_t i) const
    {
        return inrange(i) && ((word(i) & mask(i)) != 0);
    }
    void setbit(size_t i) { word(i) |= mask(i); }
    // set operations
    template<class PBITSET> void assign(const PBITSET & other, size_t targetsize)
    {
        // note: this function MUST allow for overlapping buffers, as in ADJ instruction
        size_t othersize = other.allocsize;                 // read out now as it may get overwritten
        memmove(bits, other.bits, min(othersize, targetsize) * sizeof(WORD));  // copy/move the raw data
        for (size_t k = othersize; k < targetsize; k++)     // pad with 0 words if it was too short
            bits[k] = 0;
        allocsize = targetsize;
    }
    // TODO: we can test buffer here by checking if a longer than b then b must follow a
    // ...
    // comparisons
    template<class PBITSET> bool isequalto(const PBITSET & other)
    {
        const auto & us = *this;
        for (size_t k = 0; k < allocsize || k < other.allocsize; k++)
        {
            int a = k < allocsize ? bits[k] : 0;
            int b = k < other.allocsize ? other.bits[k] : 0;
            if (a != b)
                return false;
        }
        return true;    // same
    }
    template<class PBITSET> bool issubsetof(const PBITSET & other)
    {
        // is this an improper subset of other
        // means is there no element in this that is not in other
        const auto & us = *this;
        for (size_t k = 0; k < allocsize; k++)
        {
            int thisbits = bits[k];
            int otherbits = k < other.allocsize ? other.bits[k] : 0;
            if (~otherbits & thisbits)  // any element in this that is not in other?
                return false;
        }
        return true;    // no element in this found that are outside other
    }
    void * next()       // get pointer to whatever immediately follows this set
    {
        return &bits[allocsize];
    }
    pfixedbitset & allocateinfront(size_t size)
    {
        // 'this' points to a bitset (or anything else); put a set right before it
        return *(pfixedbitset *) (((WORD*)this) - (1 + size));
    }
};

// only use this as a pointer
typedef pfixedbitset<1> pbitset;

struct pwordstruct          // head of structs that start with a word count
{
    WORD numwords;          // number of *words* (max. 255) in the deriving struct, but not counting the numwords variable itself
    const void * data() const { return 1 + &numwords; } // first data byte (=first byte in derived class) follows here
    // TODO: ensure struct alignment to 2 bytes
};

// we follow Apple III Technical Reference Manual
// one exception (?): TRM says for a BCD word, the "high byte contains the two less significant digits"
// This class is directly what could be on the stack.
// Note that this is not very efficient, with runtime checks, lazy initialization etc. We don't really care about long ints.
template<int N/*number of digits*/> struct pfixeddecint : public pwordstruct
{
    // pwordstruct::numwords = 1 + number of valid words in bcd[] (i.e. not counting the numwords variable itself)
    size_t capacity() const { return N; }           // number of digits we can hold
    WORD sign;              // 0 (positive) or 0x00ff (negative)
    bool positive() const { return sign == 0; }
    size_t bcdsize() const { return numwords-1; }   // BCD words
    WORD bcd[(N+3)/4];      // bit storage of 4 BCD digits each, low word first
    void clear(WORD s = 0) { numwords = 1; sign = s; }  // this is zero
    pfixeddecint() { clear(); }
private:
    size_t offset(size_t i) const { return i / 4; }         // word index
    size_t shift(size_t i) const { return (i % 4) * 4; }    // bit shift of digit
public:
    void normalize()        // make as short as possible
    {
        // BUGBUG: Apple III Tech Ref Manual says 1..9 words for bcd[], i.e. cannot go to 0
        while (bcdsize() > 0 && bcd[bcdsize()-1] == 0)
            numwords--;
        if (bcdsize() == 0)
            sign = 0;       // also normalize the sign, so we have no -0
    }
    void adjust(size_t targetnumwords)
    {
        normalize();                        // make it as short as we can
        if (numwords > targetnumwords)      // still too large: fail
            throw intoverflow();
        while (numwords < targetnumwords)   // bring it back up
            growbyone();
        // again, this is inefficient but quick to code and simple in logic
    }
    size_t effectivesize() const            // determine how many non-zero digits we have
    {
        size_t n = size();
        while (n > 0 && digit(n-1) == 0)
            n--;
        return n;                           // return number of non-zero digits
    }
    size_t size() const             // number of digits allocated
    {
        size_t n = bcdsize() * 4;
        if (n > N)
            throw outofbounds();    // memory corruption
        return n;
    }
    int digit(size_t i) const
    {
        size_t o = offset(i);
        if (o < bcdsize())
            return (bcd[o] >> shift(i)) & 0x0f;
        else
            return 0;   // higher digits can be accessed for convenience; they are just 0
    }
    void growbyone()
    {
        if (bcdsize() >= _countof(bcd))
            throw intoverflow();
        bcd[bcdsize()] = 0;
        numwords++;
    }
    void setdigit(int i, size_t digit)
    {
        unsigned char o = offset(i);
        while (o >= bcdsize())   // opening a new word: clear it
            growbyone();
        bcd[o] &= ~(0x0f << shift(i));
        bcd[o] |= digit << shift(i);
    }
private:
    // sum numbers of the same sign (sign of b is assumed the same as a)
    template<class A, class B> void addabs(const A & a, const B & b)
    {
        clear(a.sign);
        size_t na = a.effectivesize();
        size_t nb = b.effectivesize();
        int carry = 0;
        for (size_t k = 0; k < na || k < nb || carry > 0; k++)
        {
            int sum = a.digit(k) + b.digit(k) + carry;  // max 19
            carry = sum > 9 ? 1 : 0;
            if (carry)
                sum -= 10;
            setdigit(k, sum);
        }
        normalize();
    }
    // subtract numbers of the same sign (sign of b is assumed the same as a)
    template<class A, class B> void subabs(const A & a, const B & b)
    {
        clear(a.sign);
        size_t na = a.effectivesize();
        size_t nb = b.effectivesize();
        int carry = 0;
        size_t k;
        for (k = 0; k < na || k < nb; k++)
        {
            int sum = a.digit(k) - carry - b.digit(k);
            carry = sum < 0 ? 1 : 0;
            if (carry)
                sum += 10;
            setdigit(k, sum);
        }
        if (carry)
        {
            // 9's complement
            for (size_t i = 0; i < k; i++)
                setdigit(i, 9 - digit(i));
            // add one, gives 10's complement
            for (size_t i = 0; i < k && carry > 0; i++)
            {
                int sum = digit(i) + carry;  // max 10
                carry = sum > 9 ? 1 : 0;
                if (carry)
                    sum = 0;
                setdigit(i, sum);
            }
            // flip the sign flag
            negate();
        }
        normalize();
    }
public:
    // compare ops compare 'a' to 'b' (sgn(a-b))
    template<class A, class B> static int compar(const A & a, const B & b)
    {
        size_t na = a.size();       // note: we could use effectivesize() but it's not going to be cheaper
        size_t nb = b.size();
        int diff = b.sign - a.sign; // note: 0 is always positive
        for (size_t k = max(na,nb); diff == 0 && k > 0; k--)
            diff = a.digit(k-1) - b.digit(k-1);
        return diff;
    }
    // binary ops return their result in 'this'
    bool iszero() const { return effectivesize() == 0; }
    void negate() { if (!iszero())/*there's no -0*/ sign = sign ^ 0xff; }
    template<class A, class B> void sum(const A & a, const B & b)
    {
        if (a.sign == b.sign)
            addabs(a, b);
        else
            subabs(a, b);
    }
    template<class A, class B> void diff(const A & a, const B & b)
    {
        if (a.sign == b.sign)
            subabs(a, b);
        else
            addabs(a, b);
    }
    template<class A, class B> void mul(const A & a, const B & b)
    {
        clear(a.sign ^ b.sign);
        size_t na = a.effectivesize();
        size_t nb = b.effectivesize();
        for (size_t ia = 0; ia < na; ia++)
        {
            size_t digitia = a.digit(ia);
            // add b multiplied by digitia shifted by ia
            int carry = 0;
            for (size_t jb = 0; jb < nb || carry > 0; jb++)
            {
                int sum = digit(ia + jb) + digitia * b.digit(jb) + carry;  // max 9+81+9=99
                carry = sum / 10;
                sum = sum % 10;
                setdigit(ia + jb, sum);
            }
        }
        normalize();
    }
    template<class A, class B> void div(const A & a, const B & b)
    {
        // http://en.wikipedia.org/wiki/Long_division
        size_t na = a.effectivesize();
        size_t nb = b.effectivesize();
        if (b.iszero())
            throw divbyzero();
        clear(a.sign ^ b.sign);
        if (nb > na)                // a < b -> result is 0
            return;

        // computing q = a div b:
        // r = a
        // n = max number of digits of quotient
        // q = 0                    // quotient
        // while n >= 0
        //   d = lower bound how often b * 10^n fits in r (at least 1 though, we know that)
        //   if d = 0
        //     n--; continue;
        //   q[n] += d;             // count the digit
        //   r -= d * b * 10^n      // there will be no underflow since d is a lower bound
        // continue;

        A r(a);                                 // r = a

        // aaaaaaaaaaaaaaa   na digits
        // / bbbbbbbbbbbbb   nb digits
        //             rrr   n digits result has max na-nb+1 digits

        size_t nq = max(na - nb + 1, (size_t)0);        // max length of r
        for (int n = nq-1; n >= 0; /*n-- is done in the loop*/)
        {
            if (r.digit(n+nb+1) != 0)
                throw std::logic_error("div: unexpected non-zero head digit in residual");
            // estimate how often b * 10^n fits in r
            // We keep it simple (we don't really care about perf) and just go by the first digit. This would be the place to improve (the bound).
            // Turns out it never gets it right in the first place, so improvement here would definitely help.
            size_t d = (10 * r.digit(n+nb) + r.digit(n+nb-1)) / (1+b.digit(nb-1)); // 1+ gives us a lower bound
            if (d == 0)
            {
                // also triggers if b*10^n fits once, so need to test for that
                int diff = 0;                   // will become sign of r - b*10^n
                for (int j = nb-1; diff == 0 && j >= 0; j--)
                    diff = r.digit(n+j) - b.digit(j);
                if (diff < 0)                   // r < b * 10^n (does not fit)
                {
                    n--;                        // shift divisor by one digit
                    continue;
                }
                d = 1;                          // otherwise divisor fit once at least
            }
            // divisor fit 'd' times at least; so subtract b*10^n d times from r, and account for it by increasing digit[n]
            if (digit(n) + d > 9)
                throw std::logic_error("div: unexpected overflow in result digit");
            setdigit(n, digit(n) + d);
            int carry = 0;                      // r -= d * b * 10^n
            for (size_t j = 0; j <= nb; j++)
            {
                int sum = r.digit(n+j)/*min 0*/ + carry/*min -9*/ - d * b.digit(j)/*min -81*/;
                carry = (100 + sum) / 10 - 10;  // 0 or negative up to -9
                sum -= carry * 10;
                r.setdigit(n+j, sum);
            }
            if (carry)
                throw std::logic_error("div: unexpected overflow subtracting divisor from residual");
        }

        normalize();
    }
    // conversion ops
    pfixeddecint(INT val)           // construct from an INT
    {
        clear((val < 0) ? 0xff : 0);
        int ival = abs((int)val);        // use full 'int' to avoid overflow on -32768
        for (size_t k = 0; k < 5; k++)
        {
            setdigit(k, ival % 10);
            ival /= 10;
        }
    }
    INT toINT() const
    {
        size_t n = effectivesize();
        if (n > 5)
            throw intoverflow();
        int val = 0;
        for (size_t k = n; k > 0; k--)
        {
            auto dig = digit(k-1);
            val = 10 * val + dig;
        }
        if (sign)
            val = -val;
        if (val < -32768 || val > 32767)
            throw intoverflow();
        return val;
    }
    // we assume (not sure) that 'width' the number of digits or the allocation length of the string
    // WRITE passes 38, while the max length of a decint is 36 digits.
    void toSTR(pstring & ps, size_t width) const
    {
        CHAR * p = ps.begin();
        if (sign != 0)
            *p++ = '-';
        for (size_t k = max(effectivesize(),1U); k > 0; k--)
            *p++ = '0' + digit(k-1);
        size_t n = p - ps.begin();
        if (n > width)
            throw stringoverflow();
        ps.length = n;  // (no overflow since n limited to N <= 36)
    }
};

// only use this as a pointer
typedef pfixeddecint<36> pdecint;   // 36 is max number of digits

 // this is often used inside the system
template<int N> struct CHARARRAY
{
    CHAR chars[N];
    CHAR * begin() { return &chars[0]; }
    const CHAR * begin() const { return &chars[0]; }
    const CHAR * end() const { return begin() + N; }
    size_t size() const { return _countof(chars); }
    CHAR &       at(size_t i)       { return chars[i-1]; }
    const CHAR & at(size_t i) const { return chars[i-1]; }
    int compare(const CHARARRAY<N> & other) { for (size_t k = 1; k <= size(); k++) { int diff = at(k) - other.at(k); if (diff) return diff; } return 0; }
    std::string tostring() const { return std::string((const char*)begin(), (const char*)end()); }
};
typedef CHARARRAY<8> ALPHA;

// system-procedure emulation proc numbers
// These are system routines the compiler uses (based on grepping for CXP), augmented from GLOBALS.TEXT for the missing ones:
// straight in order of declaration, i.e. PROCEDURE/FUNCTION keyword incl. FORWARDed ones
enum systemprocedures   // for emulation
{
    syspnUSERPROG = 0,  // special proc no for outer block (USERPROGRAM) for detecting return to shell
    syspnBEGINEND = 1,  // outer block
    syspnEXECERROR,
    syspnFINIT,         // (VAR F: FIB; WINDOW: WINDOWP; RECWORDS: INTEGER);
    syspnFRESET,        // (VAR F: FIB);
    syspnFOPEN,         // (VAR F: FIB; VAR FTITLE: STRING; FOPENOLD: BOOLEAN; JUNK: FIBP);
    syspnFCLOSE,        // (VAR F: FIB; FTYPE: CLOSETYPE);
    syspnFGET,          // (VAR F: FIB);
    syspnFPUT,          // (VAR F: FIB);
    syspnFSEEK,         // (VAR F: FIB; RECNUM: INTEGER)         Note: #9, Retro Pascal only
    syspnFEOF,          // (VAR F: FIB): BOOLEAN;
    syspnFEOLN,         // (VAR F: FIB): BOOLEAN;
    syspnFREADINT,      // (VAR F: FIB; VAR I: INTEGER);
    syspnFWRITEINT,     // (VAR F: FIB; I,RLENG: INTEGER);
    syspnFREADREAL,     // (VAR F: FIB; VAR X: REAL)             Note: #14, Retro Pascal only
    syspnFWRITEREAL,    // (VAR F: FIB; X: REAL; W, D: INTEGER)  Note: #15, Retro Pascal only
    syspnFREADCHAR,     // (VAR F: FIB; VAR CH: CHAR);
    syspnFWRITECHAR,    // (VAR F: FIB; CH: CHAR; RLENG: INTEGER);
    syspnFREADSTRING,   // (VAR F: FIB; VAR S: STRING; SLENG: INTEGER);
    syspnFWRITESTRING,  // (VAR F: FIB; VAR S: STRING; RLENG: INTEGER);
    syspnFWRITEBYTES,   // (VAR F: FIB; VAR A: WINDOW; RLENG,ALENG: INTEGER);
    syspnFREADLN,       // (VAR F: FIB);
    syspnFWRITELN,      // (VAR F: FIB);
    syspnSCONCAT,       // (VAR DEST,SRC: STRING; DESTLENG: INTEGER);
    syspnSINSERT,       // (VAR SRC,DEST: STRING; DESTLENG,INSINX: INTEGER);
    syspnSCOPY,         // (VAR SRC,DEST: STRING; SRCINX,COPYLENG: INTEGER);
    syspnSDELETE,       // (VAR DEST: STRING; DELINX,DELLENG: INTEGER);
    syspnSPOS,          // (VAR TARGET,SRC: STRING): INTEGER;
    syspnFBLOCKIO,      // (VAR F: FIB; VAR A: WINDOW; I: INTEGER; NBLOCKS,RBLOCK: INTEGER; DOREAD: BOOLEAN): INTEGER;
    syspnFGOTOXY,       // (X,Y: INTEGER);
    // (* NON FIXED FORWARD DECLARATIONS *)
    syspnVOLSEARCH,     // (VAR FVID: VID; LOOKHARD: BOOLEAN; VAR FDIR: DIRP): UNITNUM;
    syspnWRITEDIR,      // (FUNIT: UNITNUM; FDIR: DIRP);
    syspnDIRSEARCH,     // (VAR FTID: TID; FINDPERM: BOOLEAN; FDIR: DIRP): DIRRANGE;
    syspnSCANTITLE,     // (FTITLE: STRING; VAR FVID: VID; VAR FTID: TID; VAR FSEGS: INTEGER; VAR FKIND: FILEKIND): BOOLEAN;
    // (the following do not seem useful, avoid these)
    syspnDELENTRY,      // (FINX: DIRRANGE; FDIR: DIRP);
    syspnINSENTRY,      // (VAR FENTRY: DIRENTRY; FINX: DIRRANGE; FDIR: DIRP);
    syspnHOMECURSOR,
    syspnCLEARSCREEN,
    syspnCLEARLINE,
    syspnPROMPT,
    syspnSPACEWAIT,     // (FLUSH: BOOLEAN): BOOLEAN;
    syspnGETCHAR,       // (FLUSH: BOOLEAN): CHAR;
    syspnFETCHDIR,      // (FUNIT:UNITNUM) : BOOLEAN;
    syspnCOMMAND,
    // Retro Pascal only (to eliminate separate PASCALIO UNIT)
    syspnFREADDEC,      // (VAR F: FIB; VAR D: STUNT; L: INTEGER);  Note: #44
    syspnFWRITEDEC,     // (VAR F: FIB; D: DECMAX; RLENG: INTEGER); Note: #45
    syspnnum
};

};
