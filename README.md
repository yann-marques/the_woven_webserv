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
+ : separates key from value
+ Expected value types are between [ ] (only in this example)
+ , separates multiple values for the same key
+ ; ends the value definition
+ Here commentaries are preceded by # but are not handled in the configuration file
<pre>
server {
	port: [int] ;				# mandatory
	host: [string] ;			# format: IP "1.222.33.4"
	server_names: [string] , ... ;
	auto_index: [string] ;			# "true", "yes", "1", "false", "no", "0" allowed
	max_body_byte: [int / string] ;		# can be only digits, or contain M, K, G
						# "1K" is 1,024 bytes
						# "1M34" is 1,048,576 + 34 = 1,048,610 bytes
						# "1G" is 1,073,741,824 bytes
	root: [string] ;
	redirect: [string] ;
	upload: [string] ;
	default_pages: [string] , ... ;
	allowed_methods: [string] , ... ;	# "GET", "POST", "DELETE"
	error_pages {
		[int] : [string] ;
		...
	}
	cgi_path {
		[string] : [string] ;		# Key must start with a .
		...
	}
	location: [string] , ... {
		...				# You can redefine arguments in location, except:
						# port, host, server\_names
		location: [string], ... {	# You can also nest locations
			...
		}
	}
}

server {
	...
}
</pre>
## Default values
<pre>
host:			127.0.0.1; 
server_names:		localhost; 
auto_index:		true; 
max_body_bytes:		1024; 
root:			www/; 
redirect:		# no default redirect
upload:			# no default upload
default_pages:		index, index.html; 
allowed_methods:	GET; 
error_pages:		# no default error_page
cgi_path:		# no default cgi_path
location:		# no default location
</pre>