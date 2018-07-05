//
// Created by Daniel Suo on 2/2/17.
// Edit by Fang Fan H on 4/11/18.
#include "docker.h"

void malloc_fail() {
  fprintf(stderr, "ERROR: Failed to allocate memory. Committing seppuku.");
  exit(-1);
}

static size_t write_function(void *data, size_t size, size_t nmemb, void *buffer) {
  size_t realsize = size * nmemb;
  struct buffer *mem = (struct buffer *)buffer;

  mem->data = realloc(mem->data, mem->size + realsize + 1);
  if(mem->data == NULL) {
    /*out of memory*/
    malloc_fail();
  }

  memcpy(&(mem->data[mem->size]), data, realsize);
  mem->size += realsize;
  mem->data[mem->size] = 0;

  return realsize;
}

void init_buffer(DOCKER *client) {
  client->buffer->data = (char *) malloc(1);
  client->buffer->size = 0;
}

void init_curl(DOCKER *client) {
  curl_easy_setopt(client->curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
  curl_easy_setopt(client->curl, CURLOPT_WRITEFUNCTION, write_function);
  curl_easy_setopt(client->curl, CURLOPT_WRITEDATA, client->buffer);
}

CURLcode perform(DOCKER *client, char *url) {
  init_buffer(client);
  curl_easy_setopt(client->curl, CURLOPT_URL, url);
  CURLcode response = curl_easy_perform(client->curl);
  curl_easy_reset(client->curl);

  return response;
}

DOCKER *docker_init(char *version) {
  size_t version_len = strlen(version);

  if (version_len < 5) {
    fprintf(stderr, "WARNING: version malformed.");
    return NULL;
  }

  DOCKER *client = (DOCKER *) malloc(sizeof(struct docker));
  client->buffer = (struct buffer *) malloc(sizeof(struct buffer));

  client->version = (char *) malloc(sizeof(char) * version_len);
  if (client->version == NULL) {
    malloc_fail();
  }

  memcpy(client->version, version, version_len);

  client->curl = curl_easy_init();

  if (client->curl) {
    init_curl(client);
    return client;
  }

  return NULL;
}

int docker_destroy(DOCKER *client) {
  curl_easy_cleanup(client->curl);
  free(client->buffer->data);
  free(client->buffer);
  free(client->version);
  free(client);
  client = NULL;

  return 0;
}

char *docker_buffer(DOCKER *client) {
  return client->buffer->data;
}

static char *get_api_url(char *str1, char *str2, char *str3){
  char *new_str = malloc(strlen(str1)+strlen(str2)+strlen(str3) * sizeof(char));
  new_str[0] = '\0';   
  strcat(new_str, str1);
  strcat(new_str, str2);
  strcat(new_str, str3);

  //free(new_str);
  return new_str;
}

static CURLcode docker_post(DOCKER *client, char *url, char *data) {
  init_curl(client);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  // setopt header parameter
  curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, headers);
  // specfy data post to server
  curl_easy_setopt(client->curl, CURLOPT_POSTFIELDS, (void *)data);
  CURLcode response = perform(client, url);
  curl_slist_free_all(headers);

  return response;
}

static CURLcode docker_delete(DOCKER *client, char *url, char *data){
  init_curl(client);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(client->curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(client->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  CURLcode response = perform(client, url);
  curl_slist_free_all(headers);

  return response;
}

static CURLcode docker_get(DOCKER *client, char *url) {
  init_curl(client);

  return perform(client, url);
}

CURLcode docker_pull_image(DOCKER *client, char *image){
  const char *url;

  url = get_api_url(DOCKER_API_HTTP_IMAGE, "?fromImage=", image);
  return docker_post(client, url, "");
}

CURLcode docker_create(DOCKER *client, char *image, char *command) {
  const char *url = "http:/v1.24/containers/create";
  const char *str1 = "{\"Image\": \"";
  const char *str2 = "\", \"Cmd\": [";
  const char *str3 = "]}";
  char *data = malloc(strlen(str1)+strlen(str2)+strlen(str3)+strlen(image)+strlen(command) * sizeof(char));

  strcat(data, str1);
  strcat(data, image); 
  strcat(data, str2);
  strcat(data, command);
  strcat(data, str3);
  //free(data);
  return docker_post(client, url, data);
}

CURLcode docker_start(DOCKER *client, char* containerName){
  char *containerID; 
  char *url; 
  
  containerID = containerInfo_parser(docker_listAllContainer(client), containerName);
  if (containerID == -1)      return JSON_DATA_ERROR;
  else if (containerID == -2) return JSON_ROOT_ERROR;
  else if (containerID == -3) return JSON_CANT_FIND_CONTAINERID;

  url = get_api_url(DOCKER_API_HTTP, containerID, "/start");
  
  return docker_post(client, url, "");
}

CURLcode docker_wait(DOCKER *client, char* containerName){
  char *containerID; 
  char *url; 
  
  containerID = containerInfo_parser(docker_listAllContainer(client), containerName);
  if (containerID == -1)      return JSON_DATA_ERROR;
  else if (containerID == -2) return JSON_ROOT_ERROR;
  else if (containerID == -3) return JSON_CANT_FIND_CONTAINERID;

  url = get_api_url(DOCKER_API_HTTP, containerID, "/wait");

  return docker_post(client, url, containerID);
}

CURLcode docker_stop(DOCKER *client, char *containerName){
  char *containerID; 
  char *url; 

  containerID = containerInfo_parser(docker_listAllContainer(client), containerName);
  if (containerID == -1)      return JSON_DATA_ERROR;
  else if (containerID == -2) return JSON_ROOT_ERROR;
  else if (containerID == -3) return JSON_CANT_FIND_CONTAINERID;
  
  url = get_api_url(DOCKER_API_HTTP, containerID, "/stop");

  return docker_post(client, url, "");
}

CURLcode docker_rm(DOCKER *client, char* containerName){
  char *containerID; 
  char *url; 
  
  containerID = containerInfo_parser(docker_listAllContainer(client), containerName);
  if (containerID == -1)      return JSON_DATA_ERROR;
  else if (containerID == -2) return JSON_ROOT_ERROR;
  else if (containerID == -3) return JSON_CANT_FIND_CONTAINERID;

  url = get_api_url(DOCKER_API_HTTP, containerID, "");

  return docker_delete(client, url, ""); 
}

void docker_ps(DOCKER *client){
  printf("%s\n", docker_listContainer(client));
}

void docker_ps_a(DOCKER *client){
  printf("%s\n", docker_listAllContainer(client));
}

static char *docker_listContainer(DOCKER *client){
  CURLcode response = docker_get(client, get_api_url("http:/", "v1.24", "/containers/json"));

  return docker_buffer(client);
}

static char *docker_listAllContainer(DOCKER *client){
  CURLcode response = docker_get(client, get_api_url("http:/", "v1.24", "/containers/json?all=1"));

  return docker_buffer(client);
}

//json parser use jansson
static char *containerInfo_parser(char *jsonString, char *containerName){
  int arraylen, i;
  const char *message_text = ""; 
  json_t *root;
  json_error_t error;
  root = json_loads(jsonString, 0, &error);
  if(!root){
    printf("ERROR: root error\n");
    return JSON_ROOT_ERROR;
  }

  for(i=0;i<json_array_size(root);i++){
    json_t *jsonData, *Id, *Names, *NamesData;
    jsonData = json_array_get(root, i);

    Names = json_object_get(jsonData, "Names");
    arraylen = json_array_size(Names);
    NamesData = json_array_get(Names, 0);
    message_text = json_string_value(NamesData);
    if(!json_is_string(NamesData)){
      fprintf(stderr, "ERROR: commit %d: sha is not a string\n", i + 1);
      return JSON_DATA_ERROR;
    }

    Id = json_object_get(jsonData, "Id");
    if(!json_is_string(Id)){
      fprintf(stderr, "ERROR: commit %d: sha is not a string\n", i + 1);
      return JSON_DATA_ERROR;
    }

    if (strcmp(containerName, message_text) == 0){
      message_text = json_string_value(Id);
      return message_text;
    }
  }
  json_decref(root);
  printf("ERROR: can't find containers ID(%s)\n", containerName);
  return JSON_CANT_FIND_CONTAINERID;
}

void docker_ps_status(DOCKER *client){
  int arraylen, i, j;
  int cnt = 40;
  const char *message_text; 
  char *jsonString = docker_listAllContainer(client);
  json_t *root;
  json_error_t error;
  root = json_loads(jsonString, 0, &error);
  if(!root){
    printf("ERROR: json root error\n");
    return JSON_ROOT_ERROR;    
  }

  printf("CONTAINER ID");
  for(j=0;j<cnt-12;j++){ printf(" ");}

  printf("STATUS");
  for(j=0;j<cnt-6;j++){ printf(" ");}

  printf("NAMES");
  for(j=0;j<cnt-5;j++){ printf(" ");}
  printf("\n");

  for(i=0;i<json_array_size(root);i++){
    json_t *jsonData, *Id, *Names, *NamesData, *Status;
    jsonData = json_array_get(root, i);

    // get id data
    Id = json_object_get(jsonData, "Id");
    if(!json_is_string(Id)){
      fprintf(stderr, "ERROR: commit %d: sha is not a string\n", i + 1);
      return JSON_DATA_ERROR;
    }
    message_text = json_string_value(Id);
    printf("%.12s", message_text);
    for(j=0;j<cnt-12;j++){ printf(" ");}
    
    Status = json_object_get(jsonData, "Status");
    if(!json_is_string(Status)){
      fprintf(stderr, "ERROR: commit %d: sha is not a string\n", i + 1);
      return JSON_DATA_ERROR;
    }
    message_text = json_string_value(Status);
    printf("%s", message_text);
    if(strlen(message_text)>=40)
      cnt+= strlen(message_text)-40;
    for(j=0;j<cnt-strlen(message_text);j++){ printf(" ");}
    cnt = 40;

    Names = json_object_get(jsonData, "Names");
    arraylen = json_array_size(Names);
    NamesData = json_array_get(Names, 0);
    message_text = json_string_value(NamesData);
    if(!json_is_string(NamesData)){
      fprintf(stderr, "ERROR: commit %d: sha is not a string\n", i + 1);
      return JSON_DATA_ERROR;
    }
    printf("%s", message_text);
    if(strlen(message_text)>=40)
      cnt+= strlen(message_text)-40;
    for(j=0;j<cnt-strlen(message_text);j++){ printf(" ");}
    cnt = 40;
    printf("\n");
  }
  json_decref(root);
  message_text = "NULL";
}
/*
ContainerInfo *containerInfo_init(DOCKER *client){
  ContainerInfo *container_info = (ContainerInfo *) malloc(sizeof(ContainerInfo));
  ContainerInfo *header = NULL;
  ContainerInfo *tail = (ContainerInfo *) malloc(sizeof(ContainerInfo));


}

void push(ContainerInfo *header, char *data){
  ContainerInfo current = header;
  while (current->next != NULL){
    current = current->next;
  } 

  current->next = malloc(sizeof(ContainerInfo));
  current->next->id = data;

}
*/