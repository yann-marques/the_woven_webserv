# Welcome to the Woven Webserv !

A web server in cpp98, by Yann Marques and Louis Charvet \
42 school project: webserv

## Configuration file syntax explanation
The arguments must be inside of server scopes, and must be formated as follows:
<pre>key: value;
</pre>except for cgi_path and error_pages:
<pre>cgi_path {
	key: value;
	...
}
error_pages {
	key: value;
	...
}
</pre>and location:
<pre>location: location1, location2, ... {
	key: value;
	cgi_path {
		key: value;
		...
	}
	location: location3, ... {
		...
	}
	...
}
</pre>
Some keys may take multiple values : server_names, default_pages, allowed_methods, location ; \
cgi_path and error_pages can take multiple pairs key/value.

The definition of port is mandatory. \
Other arguments can be ignored. In this case, they would be set by default values ; see below.

## Configuration file template

Here is a detailed configuration file template, observing the following rules: \
+ : separates key from value \
+ Expected value types are between < > (only in this example) \
+ , separates multiple values for the same key \
+ ; ends the value definition
<pre>
server { \
	port:\<int\> ;						_mandatory_ \
	host:\<string\> ;					_format: IP "1.222.33.4"_ \
	server_names:\<string\> , ... ; \
	auto_index:\<string\> ;				_"true", "yes", "1", "false", "no", "0" allowed_ \
	max_body_byte:\<int / string\> ;	_can be only digits, or contain M, K, G_ \
										_"1K" is 1,024 bytes_ \
										_"1M34" is 1,048,576 + 34 = 1,048,610 bytes_ \
										_"1G" is 1,073,741,824 bytes_ \
	root:\<string\> ; \
	redirect:\<string\> ; \
	upload:\<string\> ; \
	default_pages:\<string\> , ... ; \
	allowed_methods:\<string\> , ... ;	_"GET", "POST", "DELETE"_ \
	error_pages { \
		\<int\> : \<string\> ; \
		... \
	} \
	cgi_path { \
		\<string\> : \<string\> ;		_Key must start with a ._ \
		... \
	} \
	location: \<string\> , ... { \
		...								_You can redefine arguments in location, except:_ \
										_port, host, server\_names_ \
		location: \<string\>, ... {		_You can also nest locations_ \
			... \
		} \
	} \
}

server { \
	... \
}
</pre>
## Default values
<pre>
host:				127.0.0.1; \
server_names:		localhost; \
auto_index:			true; \
max_body_bytes:		1024; \
root:				www/; \
redirect:			_no default redirect_ \
upload:				_no default upload_ \
default_pages:		index, index.html; \
allowed_methods:	GET; \
error_pages:		_no default error\_page_ \
cgi_path:			_no default cgi\_path_ \
location:			_no default location_
</pre>