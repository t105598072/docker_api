//
// Created by Daniel Suo on 2/2/17.
//

#ifndef DOCKER_DOCKER_H
#define DOCKER_DOCKER_H

#define DOCKER_API_VERSION v1.24
#define DOCKER_API_HTTP "http:/v1.24/containers/"
#define DOCKER_API_HTTP_IMAGE "http:/v1.24/images/create"

#define JSON_DATA_ERROR	 	 		-1
#define JSON_ROOT_ERROR		 		-2
#define JSON_CANT_FIND_CONTAINERID  -3

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#ifdef __cplusplus
extern "C"{
#endif

struct buffer {
  char *data;
  size_t size;
};

struct docker {
  CURL *curl;
  char *version;
  struct buffer *buffer;
};
typedef struct docker DOCKER;

typedef struct data {
  char *id;
  char *name;
  char *status;
  struct data *next;
  size_t size;
}ContainerInfo;

DOCKER *docker_init(char *version);
int docker_destroy(DOCKER *docker_client);
char *docker_buffer(DOCKER *docker_client);

static char *docker_listContainer(DOCKER *docker_client);
static char *docker_listAllContainer(DOCKER *docker_client);
static char *get_api_url(char *str1, char *str2, char *str3);	
static char *containerInfo_parser(char *data, char *containerName);

static CURLcode docker_post(DOCKER *docker_client, char *url, char *data);
static CURLcode docker_delete(DOCKER *client, char *url, char *data);
static CURLcode docker_get(DOCKER *docker_client, char *url);

CURLcode docker_pull_image(DOCKER *client, char *image);
CURLcode docker_create(DOCKER *client, char *image, char *command);
CURLcode docker_start(DOCKER *docker_client, char *data);
CURLcode docker_wait(DOCKER *docker_client, char *data);
CURLcode docker_stop(DOCKER *docker_client, char *data);
CURLcode docker_rm(DOCKER *docker_client, char *data);
void docker_ps(DOCKER *docker_client);
void docker_ps_a(DOCKER *docker_client);
void docker_ps_status(DOCKER *client);


#ifdef __cplusplus
}
#endif

#endif //DOCKER_DOCKER_H
