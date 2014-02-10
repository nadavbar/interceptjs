Intercept.js - Intercepting named and indexed property handlers
===

Intercept.js powers you with the ability to intercept named/indexed properties getters and setter.
This is done using v8 native named and indexed property handlers API.

Example usage:
```javascript
var intercept = require('intercept');
var obj = new interecept.interceptor({
  namedSetter : function(key, value) { return this[key] = value;},
  namedGetter : function(key) { return 'ha-ha!';},
  indexSetter : function(key, value) { return this[key] = value;},
  indexGetter : function(key) { return 'ha-ha2!';},
});

// this should behave normally
obj.propy = 'test';

// but we actually get a 'ha-ha' here...
console.info('but I got..', obj.propy); 
```