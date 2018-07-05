#include "lib/libdocker/src/docker.h"

int main() {
  char *containerID; 
  char *jsonData;

  fprintf(stderr, "curl_version: %s\n", curl_version());

  DOCKER *docker = docker_init("v1.24");
  CURLcode response;
  if (docker) {  
    /*
    response = docker_pull_image(docker, "alpine");
    if (response == CURLE_OK) {
      fprintf(stderr, "%s\n", docker_buffer(docker));
    }
    */
    response = docker_create(docker, "alpine", "\"ping\", \"8.8.8.8\"");
    if (response == CURLE_OK) {
      fprintf(stderr, "%s\n", docker_buffer(docker));
    }

    response = docker_start(docker, "/practical_jang");
    response = docker_stop(docker, "/serene_sammet");
    response = docker_rm(docker, "/vibrant_goldwasser");
    response = docker_rm(docker, "/jolly_northcutt");
    response = docker_rm(docker, "/adoring_bose");

    //docker_ps(docker);
    //docker_ps_a(docker);
    docker_ps_status(docker);

    docker_destroy(docker);
  } else {
    fprintf(stderr, "ERROR: Failed to get get a docker client!\n");
  }

  return 0;
}