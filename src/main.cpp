// int	main(int argc, char *argv[])
// {
// 	WebServer webserv(argc, argv);
// 	return (0);
// }
# include <netdb.h>

# include <fcntl.h>
# include <unistd.h>
# include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include <sys/socket.h>
# include <sys/types.h>


#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>

#include "Configuration/includes/Configuration.hpp"
#include "WebServer/includes/WebServer.hpp"
#include <fstream>



#define SERVER_PORT  2345

#define TRUE             1
#define FALSE            0


// #include "WebServer.hpp"
int main(int argc, char *argv[])
{
if (argc < 2)
	{
		std::cout << "Server requires a configuration file" << std::endl;
		return(false);
	}
	// int conf_fd = open(argv[1], O_RDONLY);
	// if (conf_fd < 0)
	// {
		// std::cout << "File " << argv[1] << "don't exist or user has no read permission" << std::endl;
		// return(false);
	// }
  Configuration config(argv[1]);
  config.printConfigurationData();
  for (std::vector<ServerConfig>::iterator it =config.servers.begin(); it != config.servers.end(); it++)
  {
	WebServer webserver(*it, config.dictionary);
  }
//   std::string path = "/home/done.html";

	// std::string filePath;
	// std::string requestUri = "/hello/folder/pypy/hoho.";
	// // get the location that matches uri of the request
	// LocationConfig *location = config.servers[0].getLocation(requestUri);
	// // if there no location or method is not allowed set error status code
	// if (location == NULL)
	// 	std::cout << "NO LOCATION" << std::endl;
	// 	// request->setStatusCode(not_found);
	// else if (!location->isMethodAllowed("GET"))
	// 	std::cout << "GET is NOT allowded" << std::endl;

	// 	// request->setStatusCode(method_not_allowed);
	// // get path of file
	// else
	// {

	// 	std::string restUriPath;
	// 	filePath.append(location->root);
	// 	std::cout << "!!! " << requestUri << " " << filePath << std::endl;

	// 	restUriPath.append(requestUri.substr(filePath.size() - 1));

	// 	filePath.append("/");

	// 	filePath.append(restUriPath);

	// 	// std::cout << "!!! " << location->uri << std::endl;
	// 	// if (!serverConfig.index.empty() && getUriExtention(filePath).empty())
	// 		// filePath.append(serverConfig.index);
	// 	// std::string fileName = getFilePath()
	// }
	// std::cout << filePath << std::endl;
	// 	std::cout << "!!! Extention " << getUriExtention(filePath) << std::endl;


//   LocationConfig *curr = config.servers[0].getLocation(path);
//   if (curr)
//   	std::cout << "Location is " <<  curr->uri << std::endl;
// 	else
// 	std::cout <<"Location not found"<< std::endl;
  // nameFileOut.close();
  // close(conf_fd);
}
// int	main_4(int argc, char *argv[])
// {
//   (void) argc;
//   (void) argv;
//   // std::cout << "Extention is :" << get_uri_extention("") << "|" << std::endl;
//   std::cout << "Extention is :" << get_uri_extention("/") << "|" << std::endl;
//   std::cout << "Extention is :" << get_uri_extention("/home") << "|" << std::endl;
//   std::cout << "Extention is :" << get_uri_extention("/home/index") << "|" << std::endl;
//   std::cout << "Extention is :" << get_uri_extention("/home/index.php") << "|" << std::endl;


// 	// WebServer webserv(argc, argv);
// 	return (0);
// }

// int main_1 (int argc, char *argv[])
// {
// (void) argc;
// (void) argv;
//   int    len, rc, on = 1;
//   int    listen_sd = -1, new_sd = -1;
//   int    end_server = FALSE, compress_array = FALSE;
//   int    close_conn;
//   char   buffer[80];
// //   struct sockaddr   addr;
//   int    timeout;
//   struct pollfd fds[200];
//   int    nfds = 1, current_size = 0, i, j;

//   /*************************************************************/
//   /* Create an AF_INET6 stream socket to receive incoming      */
//   /* connections on                                            */
//   /*************************************************************/
// struct addrinfo hints, *res;
// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE;
// 	getaddrinfo("localhost", "2345", &(hints), &res);
//   listen_sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//   if (listen_sd < 0)
//   {
//     perror("socket() failed");
//     exit(-1);
//   }

//   /*************************************************************/
//   /* Allow socket descriptor to be reuseable                   */
//   /*************************************************************/
//   rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
//                   (char *)&on, sizeof(on));
//   if (rc < 0)
//   {
//     perror("setsockopt() failed");
//     close(listen_sd);
//     exit(-1);
//   }

//   /*************************************************************/
//   /* Set socket to be nonblocking. All of the sockets for      */
//   /* the incoming connections will also be nonblocking since   */
//   /* they will inherit that state from the listening socket.   */
//   /*************************************************************/
// //   rc = ioctl(listen_sd, FIONBIO, (char *)&on);
// //   if (rc < 0)
// //   {
// //     perror("ioctl() failed");
// //     close(listen_sd);
// //     exit(-1);
// //   }

//   /*************************************************************/
//   /* Bind the socket                                           */
//   /*************************************************************/

// //   memset(&addr, 0, sizeof(addr));
// //   addr.sin6_family      = AF_INET6;
// //   memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
// //   addr.sin6_port        = htons(SERVER_PORT);
// 	// struct sockaddr
// 	// addr = *(res->ai_addr);
//   rc = bind(listen_sd,
//             res->ai_addr, res->ai_addrlen);
//   if (rc < 0)
//   {
//     perror("bind() failed");
//     close(listen_sd);
//     exit(-1);
//   }

//   /*************************************************************/
//   /* Set the listen back log                                   */
//   /*************************************************************/
//   rc = listen(listen_sd, 32);
//   if (rc < 0)
//   {
//     perror("listen() failed");
//     close(listen_sd);
//     exit(-1);
//   }

//   /*************************************************************/
//   /* Initialize the pollfd structure                           */
//   /*************************************************************/
//   memset(fds, 0 , sizeof(fds));

//   /*************************************************************/
//   /* Set up the initial listening socket                        */
//   /*************************************************************/
//   fds[0].fd = listen_sd;
//   fds[0].events = POLLIN;
//   /*************************************************************/
//   /* Initialize the timeout to 3 minutes. If no                */
//   /* activity after 3 minutes this program will end.           */
//   /* timeout value is based on milliseconds.                   */
//   /*************************************************************/
//   timeout = (3 * 60 * 1000);

//   /*************************************************************/
//   /* Loop waiting for incoming connects or for incoming data   */
//   /* on any of the connected sockets.                          */
//   /*************************************************************/
//   do
//   {
//     /***********************************************************/
//     /* Call poll() and wait 3 minutes for it to complete.      */
//     /***********************************************************/
//     printf("Waiting on poll()...\n");
//     rc = poll(fds, nfds, timeout);

//     /***********************************************************/
//     /* Check to see if the poll call failed.                   */
//     /***********************************************************/
//     if (rc < 0)
//     {
//       perror("  poll() failed");
//       break;
//     }

//     /***********************************************************/
//     /* Check to see if the 3 minute time out expired.          */
//     /***********************************************************/
//     if (rc == 0)
//     {
//       printf("  poll() timed out.  End program.\n");
//       break;
//     }


//     /***********************************************************/
//     /* One or more descriptors are readable.  Need to          */
//     /* determine which ones they are.                          */
//     /***********************************************************/
//     current_size = nfds;
//     for (i = 0; i < current_size; i++)
//     {
//       /*********************************************************/
//       /* Loop through to find the descriptors that returned    */
//       /* POLLIN and determine whether it's the listening       */
//       /* or the active connection.                             */
//       /*********************************************************/
//       if(fds[i].revents == 0)
//         continue;

//       /*********************************************************/
//       /* If revents is not POLLIN, it's an unexpected result,  */
//       /* log and end the server.                               */
//       /*********************************************************/
//       if(fds[i].revents != POLLIN)
//       {
//         printf("  Error! revents = %d\n", fds[i].revents);
//         end_server = TRUE;
//         break;

//       }
//       if (fds[i].fd == listen_sd)
//       {
//         /*******************************************************/
//         /* Listening descriptor is readable.                   */
//         /*******************************************************/
//         printf("  Listening socket is readable\n");

//         /*******************************************************/
//         /* Accept all incoming connections that are            */
//         /* queued up on the listening socket before we         */
//         /* loop back and call poll again.                      */
//         /*******************************************************/
//         do
//         {
//           /*****************************************************/
//           /* Accept each incoming connection. If               */
//           /* accept fails with EWOULDBLOCK, then we            */
//           /* have accepted all of them. Any other              */
//           /* failure on accept will cause us to end the        */
//           /* server.                                           */
//           /*****************************************************/
//           new_sd = accept(listen_sd, NULL, NULL);
//           if (new_sd < 0)
//           {
//             if (errno != EWOULDBLOCK)
//             {
//               perror("  accept() failed");
//               end_server = TRUE;
//             }
//             break;
//           }

//           /*****************************************************/
//           /* Add the new incoming connection to the            */
//           /* pollfd structure                                  */
//           /*****************************************************/
//           printf("  New incoming connection - %d\n", new_sd);
//           fds[nfds].fd = new_sd;
//           fds[nfds].events = POLLIN;
//           nfds++;

//           /*****************************************************/
//           /* Loop back up and accept another incoming          */
//           /* connection                                        */
//           /*****************************************************/
//         } while (new_sd != -1);
//       }

//       /*********************************************************/
//       /* This is not the listening socket, therefore an        */
//       /* existing connection must be readable                  */
//       /*********************************************************/

//       else
//       {
//         printf("  Descriptor %d is readable\n", fds[i].fd);
//         close_conn = FALSE;
//         /*******************************************************/
//         /* Receive all incoming data on this socket            */
//         /* before we loop back and call poll again.            */
//         /*******************************************************/

//         do
//         {
//           /*****************************************************/
//           /* Receive data on this connection until the         */
//           /* recv fails with EWOULDBLOCK. If any other         */
//           /* failure occurs, we will close the                 */
//           /* connection.                                       */
//           /*****************************************************/
//           rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
//           if (rc < 0)
//           {
//             if (errno != EWOULDBLOCK)
//             {
//               perror("  recv() failed");
//               close_conn = TRUE;
//             }
//             break;
//           }

//           /*****************************************************/
//           /* Check to see if the connection has been           */
//           /* closed by the client                              */
//           /*****************************************************/
//           if (rc == 0)
//           {
//             printf("  Connection closed\n");
//             close_conn = TRUE;
//             break;
//           }

//           /*****************************************************/
//           /* Data was received                                 */
//           /*****************************************************/
//           len = rc;
//           printf("  %d bytes received\n", len);
// 		  std::cout << buffer << std::endl;

//           /*****************************************************/
//           /* Echo the data back to the client                  */
//           /*****************************************************/
//           rc = send(fds[i].fd, buffer, len, 0);
//           if (rc < 0)
//           {
//             perror("  send() failed");
//             close_conn = TRUE;
//             break;
//           }

//         } while(TRUE);

//         /*******************************************************/
//         /* If the close_conn flag was turned on, we need       */
//         /* to clean up this active connection. This            */
//         /* clean up process includes removing the              */
//         /* descriptor.                                         */
//         /*******************************************************/
//         if (close_conn)
//         {
//           close(fds[i].fd);
//           fds[i].fd = -1;
//           compress_array = TRUE;
//         }


//       }  /* End of existing connection is readable             */
//     } /* End of loop through pollable descriptors              */

//     /***********************************************************/
//     /* If the compress_array flag was turned on, we need       */
//     /* to squeeze together the array and decrement the number  */
//     /* of file descriptors. We do not need to move back the    */
//     /* events and revents fields because the events will always*/
//     /* be POLLIN in this case, and revents is output.          */
//     /***********************************************************/
//     if (compress_array)
//     {
//       compress_array = FALSE;
//       for (i = 0; i < nfds; i++)
//       {
//         if (fds[i].fd == -1)
//         {
//           for(j = i; j < nfds; j++)
//           {
//             fds[j].fd = fds[j+1].fd;
//           }
//           i--;
//           nfds--;
//         }
//       }
//     }

//   } while (end_server == FALSE); /* End of serving running.    */

//   /*************************************************************/
//   /* Clean up all of the sockets that are open                 */
//   /*************************************************************/
//   for (i = 0; i < nfds; i++)
//   {
//     if(fds[i].fd >= 0)
//       close(fds[i].fd);
//   }
//   return (0);
// }

