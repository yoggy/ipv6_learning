cmake_minimum_required(VERSION 2.8)

PROJECT(echo_client_ipv4)
PROJECT(echo_client_ipv6)
PROJECT(echo_client_ipv6_simple)
PROJECT(echo_client_ipv6_simple2)
PROJECT(echo_server)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS "-g -Wall")

add_executable(echo_client_ipv4 echo_client_ipv4.c)
add_executable(echo_client_ipv6 echo_client_ipv6.c)
add_executable(echo_client_ipv6_simple echo_client_ipv6_simple.c)
add_executable(echo_client_ipv6_simple2 echo_client_ipv6_simple2.c)
add_executable(echo_server echo_server.c)
