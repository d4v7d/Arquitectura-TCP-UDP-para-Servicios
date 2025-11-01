/**
 *
 *   UCR-ECCI
 *
 *   IPv6 TCP client normal or SSL according to parameters
 *
 **/

#include <stdio.h>
#include <string.h>

#include "../network/VSocket.h"
#include "../network/Socket.h"
#include "../network/SSLSocket.h"

#define	MAXBUF	1024

/**
**/
int main( int argc, char * argv[] ) {
   VSocket * client;
   int st, port = 80;
   char a[ MAXBUF ];
   const char * os = "fe80::194d:4e78:60c:fedc\%eno1";
   char * lego = (char *) "GET /lego/ HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";
   char * request = (char *) "GET /ci0123 HTTP/1.1\r\nhost:redes.ecci\r\n\r\n";

   if (argc > 1 ) {
      port = 443;
      client = new SSLSocket(true);	// Create a new stream socket for IPv6
   } else {
      client = new Socket( 's', true );
      port = 80;
   }

   memset( a, 0 , MAXBUF );
   client->Connect( os, port );
   client->Write(  (char * ) request, strlen( request ) );
   st = client->Read( a, MAXBUF );
   printf( "Bytes read %d\n%s\n", st, a);

}

