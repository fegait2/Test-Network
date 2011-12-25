#!/bin/sh

gcc mod_core.c mod_http.c network.c server.c util.c mod_rtsp.c player.c tcp_player.c datasource.c file_datasource.c -o test
