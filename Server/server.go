package main

import(
	"fmt"
	"net"
	"os"
	"strconv"
	
)

const(
	port string = ":4563"
)
type Server struct{
	conn *net.UDPConn
	clients map
}
type Client struct{
	userID 
}