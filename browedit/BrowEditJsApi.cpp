#include "BrowEdit.h"
#include "v8.h"

#include <BroLib/Map.h>
#include <BroLib/Gnd.h>

#include <blib/wm/ActionMenuItem.h>
#include <blib/wm/Menu.h>
#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>
using blib::util::Log;

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
	bool first = true;
	for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope(args.GetIsolate());
		if (first) {
			first = false;
		}
		else {
			printf(" ");
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
	}
	printf("\n");
	fflush(stdout);
}


void getGndHeight(const v8::FunctionCallbackInfo<v8::Value>& args) {
}


class JsMenuItem : public blib::wm::ActionMenuItem
{
public:
	v8::Handle<v8::Object> browObject;
	v8::Local<v8::Object> obj;
	v8::Isolate* isolate;
	BrowEdit* browEdit;

	JsMenuItem(std::string name, v8::Local<v8::Object> object, v8::Isolate* pIsolate, BrowEdit* brow) : ActionMenuItem(name), obj(object), isolate(pIsolate), browEdit(brow)
	{
		callback = [this]()
		{
			//browEdit->runLater<bool>([this](bool bla)
			//{

				v8::Isolate::Scope isolate_scope(isolate);
				v8::HandleScope handle_scope(isolate);
				v8::Local<v8::Context> context = v8::Context::New(isolate, NULL);
				v8::Context::Scope context_scope(context);


				v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(obj->Get(v8::String::NewFromUtf8(this->isolate, "action")));
				v8::Handle<v8::Value> args[1] = {};
				args[0] = browObject;

				func->Call(obj, 1, args);
			//}, true);
		};
	}

};

void BrowEdit::loadJsPlugins()
{
	isolate->Enter();
	isolate->SetData(0, (void*)this);

	v8::Handle<v8::Value> args[1];
	args[0] = brow = v8::Object::New(isolate);
	//build brow object
	brow->Set(v8::String::NewFromUtf8(isolate, "version"), v8::Integer::NewFromUnsigned(isolate, version));
	brow->Set(v8::String::NewFromUtf8(isolate, "log"),   v8::FunctionTemplate::New(isolate, Print)->GetFunction());


	v8::Handle<v8::Object> map = v8::Object::New(isolate);
	brow->Set(v8::String::NewFromUtf8(isolate, "map"), map);

	map->Set(v8::String::NewFromUtf8(isolate, "getFile"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getFileName().c_str()));
	})->GetFunction());


	v8::Handle<v8::Object> gnd = v8::Object::New(isolate);
	map->Set(v8::String::NewFromUtf8(isolate, "gnd"), gnd);
	gnd->Set(v8::String::NewFromUtf8(isolate, "getHeight"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getGnd()->height));
	})->GetFunction());
	gnd->Set(v8::String::NewFromUtf8(isolate, "getWidth"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getGnd()->width));
	})->GetFunction());

	v8::Handle<v8::Object> rsw = v8::Object::New(isolate);
	map->Set(v8::String::NewFromUtf8(isolate, "rsw"), rsw);
	rsw->Set(v8::String::NewFromUtf8(isolate, "getVersion"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) 
	{
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->version));
	})->GetFunction());



	v8::Handle<v8::Object> water = v8::Object::New(isolate);
	rsw->Set(v8::String::NewFromUtf8(isolate, "water"), water);
	water->Set(v8::String::NewFromUtf8(isolate, "getHeight"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.height));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getType"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.type));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getAmplitude"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.amplitude));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getAmplitude"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.amplitude));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getPhase"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.phase));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getSurfaceCurve"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.surfaceCurve));
	})->GetFunction());
	water->Set(v8::String::NewFromUtf8(isolate, "getAnimSpeed"), v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), static_cast<BrowEdit*>(args.GetIsolate()->GetData(0))->map->getRsw()->water.animSpeed));
	})->GetFunction());



	
	std::vector<std::string> files = blib::util::FileSystem::getFileList("assets/plugins");
	for (size_t i = 0; i < files.size(); i++)
	{
		if (files[i].length() < 3)
			continue;
		if (files[i].substr(files[i].length() - 3) != ".js")
			continue;

		v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, blib::util::FileSystem::getData("assets/plugins/" + files[i]).c_str());

		// Compile the source code.
		v8::Local<v8::Script> script = v8::Script::Compile(source);

		// Run the script to get the result.
		v8::Local<v8::Value> result = script->Run();
		v8::Local<v8::Object> bla(result->ToObject());

		v8::String::Utf8Value name(bla->Get(v8::String::NewFromUtf8(isolate, "name")));
		v8::String::Utf8Value author(bla->Get(v8::String::NewFromUtf8(isolate, "author")));

		v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(bla->Get(v8::String::NewFromUtf8(isolate, "init")));
		func->Call(bla, 1, args);
		Log::out << "Loaded plugin '" << *name << "' by " << *author << Log::newline;


		if (bla->Has(v8::String::NewFromUtf8(isolate, "menu")))
		{
			std::string menuLoc = *v8::String::Utf8Value(bla->Get(v8::String::NewFromUtf8(isolate, "menu")));
			std::string menuName = menuLoc;
			if (menuName.find("/") != std::string::npos)
			{
				menuName = menuName.substr(menuName.rfind("/") + 1);
				menuLoc = menuLoc.substr(0, menuLoc.rfind("/"));
			}
			else
				menuLoc = "";


			JsMenuItem* menuItem = new JsMenuItem(menuName, bla, isolate, this);
			menuItem->browObject = brow;
			rootMenu->setMenu(menuLoc, menuItem);
		}
	}


}

