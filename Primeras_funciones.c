#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h> 

int contador;

//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int i;
int sockets[100];

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s = (int *) socket;
	sock_conn = *s;
	
	char peticion[512];
	char respuesta[512];
	int ret;
	
	int terminar = 0;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	// hasta que se desconecte
	while (terminar == 0)
	{
		// Ahora recibimos la petici￳n
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido\n");
		
		// A￱adimos la marca de fin de string 
		peticion[ret] = '\0';
		
		printf("Peticion: %s\n", peticion);
		
		// Vamos a ver qu￩ quieren
		char *p = strtok(peticion, "/");
		int codigo = atoi(p);
		int numForm;
		char nombre[20];
		
		if (codigo != 0)
		{
			p = strtok(NULL, "/");
			numForm = atoi(p);
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		
		if (codigo == 0) // Petici￳n de desconexi￳n
			terminar = 1;
		else if (codigo == 1) // Piden la longitud del nombre
			sprintf(respuesta, "1/%d/%d", numForm, strlen(nombre));
		else if (codigo == 2) // Quieren saber si el nombre es bonito
		{
			if ((nombre[0] == 'M') || (nombre[0] == 'S'))
				sprintf(respuesta, "2/%d/SI", numForm);
			else
				sprintf(respuesta, "2/%d/NO", numForm);
		}
		else if (codigo == 3) // Quieren saber si es alto
		{
			p = strtok(NULL, "/");
			float altura = atof(p);
			if (altura > 1.70)
				sprintf(respuesta, "3/%d/%s: eres alto", numForm, nombre);
			else
				sprintf(respuesta, "3/%d/%s: eres bajo", numForm, nombre);
		}
		else if (codigo == 4) // Verificar si el nombre es palIndromo
		{
			int len = strlen(nombre);
			int palindromo = 1;
			for (int i = 0, j = len - 1; i < j; i++, j--)
			{
				if (tolower(nombre[i]) != tolower(nombre[j]))
				{
					palindromo = 0;
					break;
				}
			}
			
			if (palindromo)
				sprintf(respuesta, "4/%d/%s es un palindromo", numForm, nombre);
			else
				sprintf(respuesta, "4/%d/%s no es un palindromo", numForm, nombre);
		}
		else if (codigo == 5) // Convertir el nombre a may￺sculas
		{
			for (int i = 0; i < strlen(nombre); i++)
			{
				nombre[i] = toupper(nombre[i]);
			}
			sprintf(respuesta, "5/%d/%s", numForm, nombre);
		}
		
		if (codigo != 0)
		{
			printf("Respuesta: %s\n", respuesta);
			// Enviamos la respuesta
			write(sock_conn, respuesta, strlen(respuesta));
		}
		
		// Si la solicitud es codigo 1, 2, 3, 4 o 5, aumentamos el contador
		if ((codigo == 1) || (codigo == 2) || (codigo == 3) || (codigo == 4) || (codigo == 5))
		{
			pthread_mutex_lock(&mutex); // No me interrumpas ahora
			contador++;
			pthread_mutex_unlock(&mutex); // Ya puedes interrumpirme
			
			// Notificar a todos los clientes conectados
			char notificacion[20];
			sprintf(notificacion, "4/%d", contador);
			for (int j = 0; j < i; j++)
				write(sockets[j], notificacion, strlen(notificacion));
		}
	}
	// Se acab￳ el servicio para este cliente
	close(sock_conn);
}

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	// INICIALIZACIONES
	// Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creando socket");
	
	// Bind al puerto
	memset(&serv_adr, 0, sizeof(serv_adr)); // Inicializa a cero serv_addr
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // Asociar socket a cualquier IP de la m￡quina
	serv_adr.sin_port = htons(9050); // Establecer puerto de escucha
	
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf("Error en el bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el listen");
	
	contador = 0;
	
	pthread_t thread;
	i = 0;
	for (;;)
	{
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexi￳n\n");
		
		sockets[i] = sock_conn;
		// sock_conn es el socket que usaremos para este cliente
		
		// Crear thread y decirle lo que tiene que hacer
		pthread_create(&thread, NULL, AtenderCliente, &sockets[i]);
		i++;
	}
}
