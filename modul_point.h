#ifndef MODUL_POINT_H
#define MODUL_POINT_H

#include "node_object_wrap.h"
#include <v8.h>

class modul_point : public ObjectWrap {
	public:
		// we call this function using the v8 scope, this function then 
		// registers all functions and objects to the v8 scope. 
		static void addToV8Scope(v8::Handle<v8::ObjectTemplate>* scope, v8::Isolate* isolate);

		// v8::Handle<v8::Value> New(const v8::Arguments& args);
		static v8::Handle<v8::Value> New(const v8::Arguments& args);


		v8::Persistent<v8::Object> handle_;
		int refs_;

	private:
		// we dont need a public constructor, look at the addToV8Scope instead.
		modul_point();
		~modul_point();

		// FUNCTIONS THAT WE EXPORT TO V8
		// all functions that we want to wrap to v8 are static for now.
		// the reason is that addToV8Scope registers this functions.
		// if we call this function from js, we will get the "javascript scope / instance"
		// in the argument (args.This), we cant use the this pointer here. 
		// thats the reason why static classes are the only opportunity that make sense here.
		static v8::Handle<v8::Value> getPosX(const v8::Arguments& args);
		static v8::Handle<v8::Value> setPosX(const v8::Arguments& args);
		// static v8::Handle<v8::Value> incrementPosX(const v8::Arguments& args);



		// PROPERIES THAT WE WANT TO USE IN CONTEXT IF AN INSTANCE
		double posX;

		// normal times we would have posY and posZ, but to keep this simple we will concentrate on X :)
};

#endif