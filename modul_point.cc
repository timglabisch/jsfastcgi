#include <v8.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include "modul_point.h"
#include "node_object_wrap.h"

using namespace v8;

modul_point::modul_point() {};
modul_point::~modul_point() {};

Handle<Value> modul_point::New(const Arguments& args) {
	// this function is the constructor for a new javascript modul_point class. 
	// at first we create a new instance of the class module point and initialize the posX property.
	modul_point* obj = new modul_point();
	obj->posX = 0;

	// use the wrap function that node provides...
	obj->Wrap(args.This());

	return args.This();
}

Handle<Value> modul_point::getPosX(const Arguments& args) {
	// we need to create a scope for our return Value.
	// this makes sure that the GC can track these elementents 
	HandleScope scope;

	// the HandleScope::Close method copies the first argument and gives us a handle that can safely be returned.
	return scope.Close(Undefined());
}

Handle<Value> modul_point::setPosX(const Arguments& args) {
	// we need to create a scope for our return Value.
	// this makes sure that the GC can track these elementents 
	HandleScope scope;

	// the HandleScope::Close method copies the first argument and gives us a handle that can safely be returned.
	return scope.Close(Undefined());
}

template <typename target_t>
void SetPrototypeMethod(target_t target,
        const char* name, v8::InvocationCallback callback)
{
    v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(callback);
    target->PrototypeTemplate()->Set(v8::String::NewSymbol(name), templ);
}

void modul_point::addToV8Scope(Handle<ObjectTemplate>* scope, Isolate* isolate) {

	HandleScope handle_scope(isolate);

	// at first we need the create the main object, we call this javascript class "point"
	// in javascript there are no real classes, so we create a function.
	// read more about the function template here: http://fossies.org/dox/node-v0.10.5/classv8_1_1FunctionTemplate.html
	// Local means that we want to use the v8 GC to tidy up, you can read
	// more about Local and Persistent Handles here:  https://developers.google.com/v8/embed at "Handles and Garbage Collection"
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

	// now we have to give our class a name, everytime we deal with strings or other types we use the v8:: type, like V8::String, ...
	// we call out class "point"
	//tpl->SetClassName(String::NewSymbol("point"));

	// if you are familiar with javascript you know that everything except some scalar types and some internal functions are objects.
	// you can access this object instance using tpl->InstanceTemplate() and you'll get a Local<ObjectTemplate>
	// TODO: i am not 100% sure about the SetInternaalFieldCount, normal times it should be 1. 
	//tpl->InstanceTemplate()->SetInternalFieldCount(1);

	Persistent<FunctionTemplate> constructor = Persistent<FunctionTemplate>::New(tpl);
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(String::NewSymbol("Point"));

	// every function has a prototypeTemplate, if you are not familiar with prototypes in javascript
	// read this great article http://javascriptweblog.wordpress.com/2010/06/07/understanding-javascript-prototypes/
	// we can access the prototype in C++ using tpl->PrototypeTemplate()
	// now we can start to register all our functions in the prototype.
	//Local<String> s = String::NewSymbol("getPosX");
	//Local<FunctionTemplate> f = FunctionTemplate::New(&getPosX);
	//Local<Function> fi = f->GetFunction();
	//tpl->Set(s, fi); 
	//tpl->Set(String::NewSymbol("setPosX"), FunctionTemplate::New(setPosX)->GetFunction());
	// tpl->PrototypeTemplate()->Set(String::NewSymbol("incrementPosX"), FunctionTemplate::New(incrementPosX)->GetFunction());

	// now we create a persistent function, persisent because we dont want the gc to remove our constructor. 
	// Persistent<Function> constructor = Persistent<Function>::New(tpl);
	SetPrototypeMethod(tpl, "getPosX", getPosX);
    SetPrototypeMethod(tpl, "setPosX", setPosX);

	// now add our class to the scope
	(*scope)->Set(String::NewSymbol("Point"), constructor->GetFunction());
}

