# Your Own HTTP Server

## How to start a program？

> ./httpserver --ip 127.0.0.1 --port 8888 --number-thread 8

* --ip                  —— Specify the server IP address.

* --port                —— Selects which port the HTTP server listens on for incoming connections.

* --number-thread       —— Set the number of threads.

## About the HTTP Server

1. You can check that your HTTP server works by opening your web browser and going to the appropriate URL. [Note] IP 127.0.0.1 refers to the IP of local host. So you can use 127.0.0.1 to test your HTTP server on the same local machine.

2. You can also send HTTP requests with the curl program. An example of how to use curl is:

> curl -i -X GET http://127.0.0.1:8888/index.html

3. You can check whether the POST method works by sending a HTTP request with the curl program. Typing the command at terminal:

> curl -i -X POST --data 'Name=HNU&ID=CS06142' http://127.0.0.1:8888/Post_show

4. If the request URL is not /Post_show or the keys are not "Name" and "ID"), you will get a 404 Not Found error message.

5. The HTTP server will not handle HTTP requests except GET requests and POST requests.
If you send a HTTP DELETE (or PUT, HEAD, etc.) request to delete the specified resource, you will get a 501 Not Implemented error message
