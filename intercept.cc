#include <node.h>
#include <v8.h>

using namespace v8;

static Persistent<FunctionTemplate> s_interceptTpl;
static Persistent<String> s_namedSetterPropertyName;
static Persistent<String> s_namedGetterPropertyName;
static Persistent<String> s_indexSetterPropertyName;
static Persistent<String> s_indexGetterPropertyName;
static Persistent<String> s_inNamedSetterPropertyName;
static Persistent<String> s_inNamedGetterPropertyName;
static Persistent<String> s_inIndexSetterPropertyName;
static Persistent<String> s_inIndexGetterPropertyName;

Handle<Value> Interceptor(const Arguments& args) {
  HandleScope scope;

  if (args.Length() > 0 && args[0]->IsObject())
  {
    Handle<Object> inputObj = args[0].As<Object>();
    args.This()->ForceSet(s_namedGetterPropertyName, inputObj->Get(s_namedGetterPropertyName));
    args.This()->ForceSet(s_namedSetterPropertyName, inputObj->Get(s_namedSetterPropertyName));
    args.This()->ForceSet(s_indexGetterPropertyName, inputObj->Get(s_indexGetterPropertyName));
    args.This()->ForceSet(s_indexSetterPropertyName, inputObj->Get(s_indexSetterPropertyName));
  }

  return args.This();
}

bool IsIn(Handle<Object> obj, Handle<String> key)
{
  Handle<Value> value = obj->GetHiddenValue(key);
  if (value.IsEmpty())
  {
    return false;
  }

  return value->IsTrue();
}

static Handle<Value> GetIndex(uint32_t index, const AccessorInfo &info)
{
  HandleScope scope;

  Handle<Value> func = info.This()->GetRealNamedProperty(s_indexGetterPropertyName);
  
  if (!func.IsEmpty() && 
    func->IsFunction() &&
    !IsIn(info.This(), s_inIndexGetterPropertyName))
  {
    Handle<Value> args [] = { Integer::NewFromUnsigned(index) };
    info.This()->SetHiddenValue(s_inIndexGetterPropertyName, True());
    Handle<Value> result = func.As<Function>()->Call(info.This(), _countof(args), args);
    info.This()->SetHiddenValue(s_inIndexGetterPropertyName, False());
    return scope.Close(result);
  }

  return Handle<Value>();
}

static Handle<Value> SetIndex(uint32_t index, Local<Value> value, const AccessorInfo& info)
{
  HandleScope scope;

  Handle<Value> func = info.This()->GetRealNamedProperty(s_indexSetterPropertyName);

  if (!func.IsEmpty() && 
    func->IsFunction() &&
    !IsIn(info.This(), s_inIndexSetterPropertyName))
  {
    Handle<Value> args[] = { Integer::NewFromUnsigned(index), value };
    info.This()->SetHiddenValue(s_inIndexSetterPropertyName, True());
    Handle<Value> result = func.As<Function>()->Call(info.This(), _countof(args), args);
    info.This()->SetHiddenValue(s_inIndexSetterPropertyName, False());
    return scope.Close(result);
  }

  return Handle<Value>();
}

Handle<Value> GetNamed(Local<String> property, const AccessorInfo& info)
{
  HandleScope scope;

  Handle<Value> func = info.This()->GetRealNamedProperty(s_namedGetterPropertyName);

  if (!func.IsEmpty() && 
    func->IsFunction() &&
    !IsIn(info.This(), s_inNamedGetterPropertyName))
  {
    if (property->Equals(s_namedSetterPropertyName) ||
      property->Equals(s_namedGetterPropertyName) ||
      property->Equals(s_indexSetterPropertyName) ||
      property->Equals(s_indexGetterPropertyName))
    {
      return Handle<Value>();
    }

    Handle<Value> args[] = { property };
    info.This()->SetHiddenValue(s_inNamedGetterPropertyName, True());
    Handle<Value> result = func.As<Function>()->Call(info.This(), _countof(args), args);
    info.This()->SetHiddenValue(s_inNamedGetterPropertyName, False());
    return scope.Close(result);
  }

  return Handle<Value>();
}


Handle<Value> SetNamed(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
  HandleScope scope;

  Handle<Value> func = info.This()->GetRealNamedProperty(s_namedSetterPropertyName);

  if (!func.IsEmpty() && 
    func->IsFunction() &&
    !IsIn(info.This(), s_inNamedSetterPropertyName))
  {
    if (property->Equals(s_namedSetterPropertyName) ||
      property->Equals(s_namedGetterPropertyName) ||
      property->Equals(s_indexSetterPropertyName) ||
      property->Equals(s_indexGetterPropertyName))
    {
      return Handle<Value>();
    }

    Handle<Value> args[] = { property,value };
    info.This()->SetHiddenValue(s_inNamedSetterPropertyName, True());
    Handle<Value> result = func.As<Function>()->Call(info.This(), _countof(args), args);
    info.This()->SetHiddenValue(s_inNamedSetterPropertyName, False());
    return scope.Close(result);
  }

  return Handle<Value>();
}

void init(Handle<Object> exports) {
  HandleScope scope;
  // Prepare constructor template
  s_interceptTpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Interceptor));
  s_interceptTpl->InstanceTemplate()->SetNamedPropertyHandler(GetNamed, SetNamed);

  s_interceptTpl->InstanceTemplate()->SetIndexedPropertyHandler(GetIndex, SetIndex);
  //s_interceptTpl->InstanceTemplate()->SetInternalFieldCount(1);
  s_interceptTpl->SetClassName(String::New("Interceptor"));
  exports->Set(String::NewSymbol("interceptor"), s_interceptTpl->GetFunction());
  

  // initialize symbols:
  s_namedSetterPropertyName = Persistent<String>::New(String::NewSymbol("namedSetter"));
  s_namedGetterPropertyName = Persistent<String>::New(String::NewSymbol("namedGetter"));
  s_indexSetterPropertyName = Persistent<String>::New(String::NewSymbol("indexSetter"));
  s_indexGetterPropertyName = Persistent<String>::New(String::NewSymbol("indexGetter"));

  s_inNamedSetterPropertyName = Persistent<String>::New(String::NewSymbol("inNamedSetter"));
  s_inNamedGetterPropertyName = Persistent<String>::New(String::NewSymbol("inNamedGetter"));
  s_inIndexSetterPropertyName = Persistent<String>::New(String::NewSymbol("inIndexSetter"));
  s_inIndexGetterPropertyName = Persistent<String>::New(String::NewSymbol("inIndexGetter"));
}

NODE_MODULE(intercept, init)