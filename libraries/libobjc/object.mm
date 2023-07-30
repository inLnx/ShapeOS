/**
 * @file object.mm
 * @author Krisna Pranav
 * @brief Object
 * @version 1.0
 * @date 2023-07-30
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
*/

#import <objc/objc.h>
#include <sys/_types/_uintptr_t.h>
#import <stdio.h>
#import <ctype.h>
#import <malloc/malloc.h>
#import "runtime/runtime-private.h"
#import "runtime/objc-private.h"

extern "C" 
{
    extern id _Nullable
    objc_msgSend(id _Nullable self, SEL _Nonnull op, ...);
    extern id
    _objc_constructOrFree(id bytes, Class cls);

    extern void *objc_destructInstance(id obj);

    extern id object_dispose(id obj);
}

extern bool DisableNonpointerIsa;
static Class remapClass(Class cls);

#define fastpath(x) (__builtin_expect(bool(x), 1))
#define slowpath(x) (__builtin_expect(bool(x), 0))

#define TAG_COUNT 8 
#define TAG_SLOT_MASK 0xf

#define TAG_MASK 1
#define TAG_SLOT_SHIFT 0
#define TAG_PAYLOAD_LSHIFT 0
#define TAG_PAYLOAD_RSHIFT 4

SEL SEL_load = NULL;
SEL SEL_initialize = NULL;
SEL SEL_resolveInstanceMethod = NULL;
SEL SEL_resolveClassMethod = NULL;
SEL SEL_cxx_construct = NULL;
SEL SEL_cxx_destruct = NULL;
SEL SEL_retain = NULL;
SEL SEL_release = NULL;
SEL SEL_autorelease = NULL;
SEL SEL_retainCount = NULL;
SEL SEL_alloc = NULL;
SEL SEL_allocWithZone = NULL;
SEL SEL_dealloc = NULL;
SEL SEL_copy = NULL;
SEL SEL_new = NULL;
SEL SEL_finalize = NULL;
SEL SEL_forwardInvocation = NULL;
SEL SEL_tryRetain = NULL;
SEL SEL_isDeallocating = NULL;
SEL SEL_retainWeakReference = NULL;
SEL SEL_allowsWeakReference = NULL;

static inline uintptr_t addc(uintptr_t lhs, uintptr_t rhs, uintptr_t carryin, uintptr_t *carryout) {
    return __builtin_addcl(lhs, rhs, carryin, (unsigned long *)carryout);
}

static inline uintptr_t subc(uintptr_t lhs, uintptr_t rhs, uintptr_t carryin, uintptr_t *carryout) {
    return __builtin_subcl(lhs, rhs, carryin, (unsigned long *)carryout);
}

static inline uintptr_t LoadExclusive(uintptr_t *src) {
    return *src;
}

static inline bool StoreExclusive(uintptr_t *dst, uintptr_t oldvalue, uintptr_t value) {
    return __sync_bool_compare_and_swap((void**)dst, (void*)oldvalue, (void*)value);
}

static inline bool StoreReleaseExclusive(uintptr_t *dst, uintptr_t oldvalue, uintptr_t value) {
    return StoreExclusive(dst, oldvalue, value);
}

inline bool objc_object::isClass() {
    if (isTaggedPointer()) return false;
    if (!ISA()->isRealized()) return false;
    return ISA()->isMetaClass();
}

inline bool objc_object::isTaggedPointer() {
    return false;
}

inline Class objc_object::ISA() {
    return (Class)(isa.bits & ISA_MASK);
}

inline void objc_object::initIsa(Class cls) {
    initIsa(cls, false, false);
}

inline void objc_object::initClassIsa(Class cls) {
    if (DisableNonpointerIsa || cls->requiresRawIsa()) {
        initIsa(cls, false, false);
    } else {
        initIsa(cls, true, false);
    }
}

inline void objc_object::initProtocolIsa(Class cls) {
    return initClassIsa(cls);
}

inline void objc_object::initInstanceIsa(Class cls, bool hasCxxDtor) {
    assert(!cls->requiresRawIsa());
    assert(hasCxxDtor == cls->hasCxxDtor());

    initIsa(cls, true, hasCxxDtor);
}

inline void objc_object::initIsa(Class cls, bool nonpointer, bool hasCxxDtor) {
    assert(!isTaggedPointer());

    if (!nonpointer) {
        isa.cls = cls;
    } else {
        assert(!DisableNonpointerIsa);
        assert(!cls->requiresRawIsa());
        isa_t newisa(0);

        newisa.bits = ISA_MAGIC_VALUE;
        newisa.has_cxx_dtor = hasCxxDtor;
        newisa.shiftcls = (uintptr_t)cls >> 3;
        isa = newisa;
    }
}

inline void objc_object::rootDealloc() {
    if (isTaggedPointer()) return;  
    
    if (fastpath(isa.indexed  &&
                 !isa.weakly_referenced  &&
                 !isa.has_assoc  &&
                 !isa.has_cxx_dtor  &&
                 !isa.has_sidetable_rc)) {
        free(this);
    }
    else {
        object_dispose((id)this);
    }
}

inline id objc_object::retain() {
    assert(!isTaggedPointer());

    if (!ISA()->hasCustomRR()) {
        return rootRetain();
    }

    return ((id(*)(objc_object *, SEL))objc_msgSend)(this, SEL_retain);
}

inline void objc_object::release() {
    assert(!isTaggedPointer());

    if (!ISA()->hasCustomRR()) {
        rootRelease();
        return;
    }

    ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_release);

}

inline id objc_object::autorelease() {
    if (isTaggedPointer()) {
        return (id)this;
    }

    if (fastpath(!ISA()->hasCustomRR())) return rootAutoRelease();
    return ((id(*)(objc_object *, SEL))objc_msgSend)(this, SEL_autorelease);
}

bool objc_object::overrelease_error() {
    return false;
}

inline id objc_object::rootRetain() {
    return rootRetain(false, false);
}

inline bool objc_object::rootTryRetain() {
    return rootRetain(true, false) ? true : false;
}

inline bool objc_object::rootRelease() {
    return rootRelease(true, false);
}

inline id objc_object::rootAutorelease() {
    if (isTaggedPointer()) return (id)this;

    return rootAutorelease();
}

id objc_object::rootRetain_overflow(bool tryRetain) {
    return rootRetain(tryRetain, true);
}

bool objc_object::rootRetain_underflow(bool performDealloc) {
    return rootRelease(performDealloc, true);
}

inline bool objc_object::rootRelease(bool performDealloc, bool handleUnderflow) {
    if (isTaggedPointer()) return false;
    
    bool sideTableLocked = false;
    
    isa_t oldisa;
    isa_t newisa;
    
retry:
    do {
        oldisa = LoadExclusive(&isa.bits);
        newisa = oldisa;
        if (!newisa.indexed) goto unindexed;
        uintptr_t carry;
        newisa.bits = subc(newisa.bits, RC_ONE, 0, &carry); 
        if (carry) goto underflow;
    } while (!StoreReleaseExclusive(&isa.bits, oldisa.bits, newisa.bits));
    
    if (sideTableLocked) sidetable_unlock();
    return false;
    
underflow:
    newisa = oldisa;
    
    if (newisa.has_sidetable_rc) {
        if (!handleUnderflow) {
            return rootRelease_underflow(performDealloc);
        }
        
        if (!sideTableLocked) {
            sidetable_lock();
            sideTableLocked = true;
            if (!isa.indexed) {
                goto unindexed;
            }
        }
        
        size_t borrowed = sidetable_subExtraRC_nolock(RC_HALF);
        
        if (borrowed > 0) {
            newisa.extra_rc = borrowed - 1;  
            bool stored = StoreExclusive(&isa.bits, oldisa.bits, newisa.bits);
            if (!stored) {
                isa_t oldisa2 = LoadExclusive(&isa.bits);
                isa_t newisa2 = oldisa2;
                if (newisa2.indexed) {
                    uintptr_t overflow;
                    newisa2.bits =
                    addc(newisa2.bits, RC_ONE * (borrowed-1), 0, &overflow);
                    if (!overflow) {
                        stored = StoreReleaseExclusive(&isa.bits, oldisa2.bits, newisa2.bits);
                    }
                }
            }
            
            if (!stored) {
                // Inline update failed.
                // Put the retains back in the side table.
                sidetable_addExtraRC_nolock(borrowed);
                goto retry;
            }
            
            sidetable_unlock();
            return false;
        }
        else {
        }
    }
    
    if (sideTableLocked) sidetable_unlock();
    
    if (newisa.deallocating) {
        return overrelease_error();
    }
    newisa.deallocating = true;
    if (!StoreExclusive(&isa.bits, oldisa.bits, newisa.bits)) goto retry;
    __sync_synchronize();
    if (performDealloc) {
        ((void(*)(objc_object *, SEL))objc_msgSend)(this, SEL_dealloc);
    }
    return true;
    
unindexed:
    if (sideTableLocked) sidetable_unlock();
    return sidetable_release(performDealloc);
}