# 前置作業 #

- Libdocker需要環境
	- docker 
		https://docs.docker.com/

- Libdocker需相依2個library
	- curl (Libcurl)
		- https://curl.haxx.se/
		- https://github.com/curl/curl
	- Jansson (JSON parser)
		- http://www.digip.org/jansson/

# 使用說明 #

- 若未安裝docker環境
	1. Ubuntu Install Docker
		- $ apt-get install apt-transport-https
		- $ apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 36A1D7869245C8950F966E92D8576A8BA88D21E9
		- $ bash -c "echo deb https://get.docker.io/ubuntu docker main > /etc/apt/sources.list.d/docker.list"
		- $ apt-get update
		- $ apt-get install -y lxc-docker

- 若尚未安裝各Library
	1. Build curl(../libdocker_test_0502-master/curl)
		- $ ./configure
		- $ make
		- $ make install

		(可能需要修改環境變數)
		- $ vi /etc/ld.so.conf
		- $ add /ur/local/lib in lid.so.conf

	2. Build Jansson  (../libdocker_test_0502-master/jansson)
		- $ cmake ./
		- $ make
		- $ make install
		
	3. Build libdocker (../libdocker_test_0502-master/libdocker_example)
		- $ cmake ./
		- $ make
		
	4. 使用example (../libdocker_test_0502-master/libdocker_example)
		- $ ./example
	
# C Code API介面 #

- CURLcode docker_pull_image(DOCKER *client, char *image);
	- Pulling images form github with the parameter of imput images name.(docker pull IMAGE)
	- @param DOCKER (in) libdocker Handle.
	- @param image (in) String of the images name which you want to pull.
	- @return The response of docker api use cURL.

- CURLcode docker_create(DOCKER *client, char *image, char *command);
	- Create container with the parameter of imput images name and execute the command.(docker run IMAGE COMMAND)
	- @param DOCKER (in) libdocker Handle.
	- @param image (in) String of the containers name.
	- @param command (in) String of the command which you want to execute.
	- @return The response of docker api use cURL.

- CURLcode docker_start(DOCKER *docker_client, char *containerName);
	- Start container with the parameter of imput images name.(docker start CONTAINER)
	- @param DOCKER (in) libdocker Handle.
	- @param containerName (in) String of the containers name.
	- @return The response of docker api use cURL. ERROR CODE -1, -2, -3.

- CURLcode docker_stop(DOCKER *docker_client, char *containerName);
	- Stop container with the parameter of imput images name.(docker stop CONTAINER)
	- @param DOCKER (in) libdocker Handle.
	- @param containerName (in) String of the containers name.
	- @return The response of docker api use cURL. ERROR CODE -1, -2, -3.

- CURLcode docker_rm(DOCKER *docker_client, char *containerName);
	- Remove container with the parameter of imput images name.(docker rm CONTAINER)
	- @param DOCKER (in) libdocker Handle.
	- @param containerName (in) String of the containers name.
	- @return The response of docker api use cURL. ERROR CODE -1, -2, -3.

- void docker_ps(DOCKER *docker_client);
	- Print the information of json data about running containers.(docker ps)
	- @param DOCKER (in) libdocker Handle.

- void docker_ps_a(DOCKER *docker_client);
	- Print the information of json data about all containers.(docker ps -a)
	- @param DOCKER (in) libdocker Handle.

- void docker_ps_status(DOCKER *client);
	- List the Information about the container's status.(docker ps -a)
	- @param DOCKER (in) libdocker Handle.

# ERROR CODE #
=================================================================================================
- JSON_DATA_ERROR	 	 		-1
- JSON_ROOT_ERROR		 		-2
- JSON_CANT_FIND_CONTAINERID  			-3
