// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef object_wrap_h
#define object_wrap_h

#include "v8.h"
#include <assert.h>

// Explicitly instantiate some template classes, so we're sure they will be
// present in the binary / shared object. There isn't much doubt that they will
// be, but MSVC tends to complain about these things.
#ifdef _MSC_VER
  template class v8::Persistent<v8::Object>;
  template class v8::Persistent<v8::FunctionTemplate>;
#endif

extern v8::Isolate* isolate;


class ObjectWrap {
 public:
  ObjectWrap ( ) {
    refs_ = 0;
  }


  virtual ~ObjectWrap ( ) {
    if (!handle_.IsEmpty()) {
      assert(handle_.IsNearDeath(isolate));
      handle_.ClearWeak(isolate);
      handle_->SetAlignedPointerInInternalField(0, 0);
      handle_.Dispose(isolate);
      handle_.Clear();
    }
  }


  template <class T>
  static inline T* Unwrap (v8::Handle<v8::Object> handle) {
    assert(!handle.IsEmpty());
    assert(handle->InternalFieldCount() > 0);
    return static_cast<T*>(handle->GetAlignedPointerFromInternalField(0));
  }


  v8::Persistent<v8::Object> handle_; // ro

 protected:
  inline void Wrap (v8::Handle<v8::Object> handle) {
    assert(handle_.IsEmpty());
    assert(handle->InternalFieldCount() > 0);
    handle_ = v8::Persistent<v8::Object>::New(isolate, handle);
    handle_->SetAlignedPointerInInternalField(0, this);
    MakeWeak();
  }


  inline void MakeWeak (void) {
    handle_.MakeWeak(isolate, this, WeakCallback);
    handle_.MarkIndependent(isolate);
  }

  /* Ref() marks the object as being attached to an event loop.
   * Refed objects will not be garbage collected, even if
   * all references are lost.
   */
  virtual void Ref() {
    assert(!handle_.IsEmpty());
    refs_++;
    handle_.ClearWeak(isolate);
  }

  /* Unref() marks an object as detached from the event loop.  This is its
   * default state.  When an object with a "weak" reference changes from
   * attached to detached state it will be freed. Be careful not to access
   * the object after making this call as it might be gone!
   * (A "weak reference" means an object that only has a
   * persistant handle.)
   *
   * DO NOT CALL THIS FROM DESTRUCTOR
   */
  virtual void Unref() {
    assert(!handle_.IsEmpty());
    assert(!handle_.IsWeak(isolate));
    assert(refs_ > 0);
    if (--refs_ == 0) { MakeWeak(); }
  }


  int refs_; // ro


 private:
  static void WeakCallback(v8::Isolate* env,
                           v8::Persistent<v8::Value> value,
                           void* data) {
    v8::HandleScope scope(isolate);

    ObjectWrap *obj = static_cast<ObjectWrap*>(data);
    assert(value == obj->handle_);
    assert(!obj->refs_);
    assert(value.IsNearDeath(env));
    delete obj;
  }
};

#endif // object_wrap_h