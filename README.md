## Warning

The sharing of this project on my GitHub aims to help peoples to open their minds to new logics or help peoples in case of difficulty. In no way, that aims to copy and paste this work on your own repository.

I don't even recommend you to inspire of our webserv because some points don't stick the correction grid like `selecting`. I only recommend you to be inspired by our `parsing` and `cgi` parts if you're interresting.

# webserv			[![mamaurai's 42 webserv Score](https://badge42.vercel.app/api/v2/cl1l4qz93000609l4yixitcl4/project/2807616)](https://github.com/JaeSeoKim/badge42)

**webserv** is a project from the **42** Common Core. The project aims to recreate a tiny webserv customizable by your own configuration file. The server must listen many ports and process them simultaneously, catch HTTP request from a web browser and give response to them.

### Usage

	~/$> git clone https://github.com/mathias-mrsn/webserv.git
	~/$> cd webserv
	~/webserv$> ./webserv [--debug=(1-5)] [configuration file]

### Default configuration file

```
server {
	server_name localhost;
	listen 192.0.0.1:8080;
	client_max_body_size 8m;

	location /
	{
		allow_methods GET POST DELETE;
		root ./www;
		index index.html;

		return 403 www/error_pages/403.html;
		auto_index on;
		cgi .php bin/php-cgi;
		cgi .hello_world bin/hw-cgi_mac;
		upload www/Upload/file_to_upload;
	}
	error_page 404 www/error_pages/404.html;
}
```
