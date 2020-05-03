#include "http_handle.hpp"

string Folder_path = ".";
string match_Name = "Name=";
string match_ID = "ID=";

//about HTTP Request
void ClientTask::doit(){
    char buffer[BUFFSIZE];
    int size = 0;
read:
    size = read(fd_client, buffer, BUFFSIZE - 1);
	// printf("%s\n", buffer);
	
	//analyse the HTTP Request
	if(size > 0)
	{
		string method = "";
		string path = "";
		int pos = 0;

		//method
		while(buffer[pos] != ' ' && buffer[pos] != '\0')
		{
			method += buffer[pos++];
		}
		cout <<method <<endl;

		//path
		++pos;
		while(buffer[pos] != ' ' && buffer[pos] != '\0')
		{
			path += buffer[pos++];
		}
		cout <<path <<endl;

		//GET method
		if(method == "GET")
		{
			response_get(path);
		}

		//POST method
		else if(method == "POST")
		{
			if(path != "/Post_show")
				response(404);
			else{
				char data[512];
				memset(data, 0, sizeof(data));
				int k = 0;
				char *ch = NULL;
				++pos;
				//get the Content-Length
				while((ch = strstr(data, "Content-Length")) == NULL)
				{
					k = 0;
					memset(data, 0, sizeof(data));
					while(buffer[pos] != '\r' && buffer[pos] != '\0')
					{
						data[k++] = buffer[pos++];
					}
					++pos;
					printf("%s\n", data);
				}

				int length;
				char *str = strchr(data, ':');
				++str;
				sscanf(str, "%d", &length);
				printf("length:%d\n", length);

				//Name=   & ID= 
				while(pos++)
					if(buffer[pos] == '\n' && buffer[pos-1] == '\r' && buffer[pos-2] == '\n')
						break;
				pos++;
				int error = 0;
				int index = 0;
				string Name = "";
				string ID = "";
				//match and get Name
				while(buffer[pos] != '\r' && buffer[pos] != '\0'){
					if(buffer[pos] == '&')
						break;
					Name += buffer[pos++];
					index++;
					if(index == 5){
						if(Name != match_Name)
							break;
						else{
							error++;
							Name = "";
						}
					}

				}
				cout <<Name <<endl;
				index++;
				pos++;
				//get Name! match and get ID
				if(error == 1){
					while(buffer[pos] != '\r' && buffer[pos] != '\0'){
						ID += buffer[pos++];
						index++;
						if(index == int(9 + Name.length())){
							if(ID != match_ID)
								break;
							else{
								error++;
								ID = "";
							}
						}
						if(index == length)
							break;
					}
				}
				cout <<ID <<endl;
				if(error == 2)
					response_post(Name, ID);
				else
					response(404);
			}
		}

		// OTHER
		else
			response(501);
	}

	//nothing
	else if(size < 0)
		goto read;

	//wait for client close, avoid TIME_WAIT
	// sleep(0.1);  

	close(fd_client);
}

//GET
void ClientTask::response_get(string path){
	string File_path = Folder_path;

	//file or folder
	if(path[ path.length()-1 ] == '/')
		File_path += (path + "index.html");
	else
		File_path += path;
	cout <<File_path <<endl;

	//get the status of the file
	struct stat filestat;
	int ret = stat(File_path.c_str(), &filestat);

	//file doesn't exits
	if(ret < 0 || S_ISDIR(filestat.st_mode))
	{
		response(404);
		return;
	}

	int filefd = open(File_path.c_str(), O_RDONLY);
	char buf[128];
	sprintf(buf, "HTTP/1.1 200 OK\r\nConnection: Close\r\n"
	"content-length:%ld\r\n\r\n", filestat.st_size);
	ret = write(fd_client, buf, strlen(buf));
	if(ret < 0)
    {
		printf("write error\n");
		exit(1);
    }
	sendfile(fd_client, filefd, 0, filestat.st_size);
}

//ERROR
void ClientTask::response(int status){
	char buf[512];
	char message[512];
	string description;
	switch (status)
	{
		case 200:
			description = "OK";
			break;

		case 404:
			sprintf(message, "<html><title>HTTP Server Error</title>");
			sprintf(message, "%s<body>\r\n", message);
			sprintf(message, "%s <h1>404</h1>\r\n", message);
			sprintf(message, "%s  <h3>HTTP : Not Found</h3>\r\n", message);
			sprintf(message, "%s<hr><h3>HTTP  Web Server</h3></body>\r\n", message);
			description = "Not Found";
			break;
		
		case 501:
			sprintf(message, "<html><title>HTTP Server Error</title>");
			sprintf(message, "%s<body>\r\n", message);
			sprintf(message, "%s <h1>501</h1>\r\n", message);
			sprintf(message, "%s <h3>HTTP : Not Implemented error message</h3>\r\n", message);
			sprintf(message, "%s<hr><h3>HTTP Web Server</h3></body>\r\n", message);
			description = "Not Implemented error message";
			break;

		default:
			description = "Not Implemented error message";
			break;
	}
	sprintf(buf, "HTTP/1.1 %d %s\r\nConnection: Close\r\n"
	"content-length:%ld\r\n\r\n", status, description.c_str(), strlen(message));

	sprintf(buf, "%s%s", buf, message);
	int ret = write(fd_client, buf, strlen(buf));
	if(ret < 0)
    {
		printf("write error\n");
		exit(1);
    }
}

//POST
void ClientTask::response_post(string Name, string ID){
	char buf[512];
	char message[512];
	sprintf(message, "<html> <title>POST method</title>\r\n");
	sprintf(message, "%s<body>\r\n", message);
	sprintf(message, "%s <h3>Your Name : %s </h3>\r\n", message, Name.c_str());
	sprintf(message, "%s <h3>Your ID : %s </h3>\r\n", message, ID.c_str());
	sprintf(message, "%s<hr><h3>HTTP Web Server</h3></body>\r\n", message);

	sprintf(buf, "HTTP/1.1 200 OK\r\nConnection: Close\r\n"
	"content-length:%ld\r\n\r\n", strlen(message));

	sprintf(buf, "%s%s", buf, message);
	int ret = write(fd_client, buf, strlen(buf));
	if(ret < 0)
    {
		printf("write error\n");
		exit(1);
    }
}