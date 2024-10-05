#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int contador;

// Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int i;
int sockets[100];

void *AtenderCliente(void *socket)
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
        // Ahora recibimos la petición
        ret = read(sock_conn, peticion, sizeof(peticion));
        printf("Recibido\n");
        
        // Añadimos la marca de fin de string
        peticion[ret] = '\0';
        
        printf("Peticion: %s\n", peticion);
        
        // Vamos a ver qué quieren
        char *p = strtok(peticion, "/");
        int codigo = atoi(p);
        int numForm;
        float temperatura;
        
        if (codigo != 0)
        {
            p = strtok(NULL, "/");
            numForm = atoi(p);
            p = strtok(NULL, "/");
            temperatura = atof(p);
            printf("Codigo: %d, Temperatura: %.2f\n", codigo, temperatura);
        }
        
        if (codigo == 0) // Petición de desconexión
        {
            terminar = 1;
        }
        else if (codigo == 1) // Conversión de Celsius a Fahrenheit
        {
            float fahrenheit = (temperatura * 9.0 / 5.0) + 32.0;
            sprintf(respuesta, "1/%d/%.2f Grados Celsius son %.2f Grados Fahrenheit", numForm, temperatura, fahrenheit);
        }
        else if (codigo == 2) // Conversión de Fahrenheit a Celsius
        {
            float celsius = (temperatura - 32.0) * 5.0 / 9.0;
            sprintf(respuesta, "2/%d/%.2f Grados Fahrenheit son %.2f Grados Celsius", numForm, temperatura, celsius);
        }
        
        if (codigo != 0)
        {
            printf("Respuesta: %s\n", respuesta);
            // Enviamos la respuesta
            write(sock_conn, respuesta, strlen(respuesta));
        }
        
        // Si la solicitud es código 1 o 2, aumentamos el contador
        if ((codigo == 1) || (codigo == 2))
        {
            pthread_mutex_lock(&mutex); // No me interrumpas ahora
            contador = contador + 1;
            pthread_mutex_unlock(&mutex); // Ya puedes interrumpirme
            
            // Notificar a todos los clientes conectados
            char notificacion[20];
            sprintf(notificacion, "4/%d", contador);
            for (int j = 0; j < i; j++)
                write(sockets[j], notificacion, strlen(notificacion));
        }
    }
    // Se acabó el servicio para este cliente
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
    
    // Asocia el socket a cualquiera de las IP de la máquina.
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Establecemos el puerto de escucha
    serv_adr.sin_port = htons(9050);
     
    if (bind(sock_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
        printf("Error en el bind");
    
    if (listen(sock_listen, 3) < 0)
        printf("Error en el Listen");
    
    contador = 0;
    
    pthread_t thread;
    i = 0;
    for (;;)
    {
        printf("Escuchando\n");
        
        sock_conn = accept(sock_listen, NULL, NULL);
        printf("He recibido conexion\n");
        
        sockets[i] = sock_conn;
        // sock_conn es el socket que usaremos para este cliente
        
        // Crear thread y decirle lo que tiene que hacer
        pthread_create(&thread, NULL, AtenderCliente, &sockets[i]);
        i = i + 1;
    }
}
