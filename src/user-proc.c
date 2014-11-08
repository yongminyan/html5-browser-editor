#include <gtk/gtk.h>
#include "common.h"

char path[256]; // buffer data

void file_ok_sel(GtkWidget *w, GtkFileSelection *fs)
{
    strcpy(path, gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)) );
    gtk_widget_destroy(GTK_WIDGET(fs));
    gtk_main_quit();
}

int file_command(int argc, char *argv[])
{
    setenv("DISPLAY", ":0.0", 0);
    GtkWidget *filew;
    gtk_init (&argc, &argv);

    filew = gtk_file_selection_new ("File selection");

    g_signal_connect (G_OBJECT (filew), "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
            "clicked",
            G_CALLBACK (file_ok_sel), filew);


    g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
            "clicked",
            G_CALLBACK (gtk_widget_destroy), filew);

    gtk_widget_show (filew);
    gtk_main ();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: user-proc Load | user-proc Store Content\n");
        return -1;
    }

    // add more error checking later
    if ('L' == argv[1][0]) {
        file_command(0, NULL);

        char * buffer = 0;
        long length;
        FILE * f = fopen (path, "rb");

        if (f)
        {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (length);
            if (buffer)
            {
                fread (buffer, 1, length, f);
            }
            fclose (f);
        }

        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        portno = PORT_IPC;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
            error("user-proc, socket()");
        server = gethostbyname("localhost"); 

        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
                (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);

        serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
            error("user-proc, connect()");

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) 
            error("user-proc, write()");
        close(sockfd);
    }
    else if ('S' == argv[1][0]) {
        file_command(0, NULL);
        FILE *fp;
        fp = fopen(path, "w+"); 
        fprintf(fp, "%s", argv[2]);
        fclose(fp);
    }
    else {
        assert( 0 ); // impossible
    }
    return 0;
}
