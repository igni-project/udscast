cmake_minimum_required(VERSION 3.15...4.0)

project(
	udscast
	VERSION 0.2
	LANGUAGES C
)

add_executable(
	udscast
	src/main.c
	src/socket.c
	src/cmd/cmdline.c
)

