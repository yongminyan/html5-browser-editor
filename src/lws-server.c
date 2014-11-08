#include "common.h"
#include "../lib/libwebsockets.h"

uid_t uid = -1;
gid_t gid = -1;

int force_exit = 0;
void sighandler(int sig)
{
	force_exit = 1;
}


static int
callback_my_protocol(
        struct libwebsocket_context *context,
        struct libwebsocket *wsi,
        enum libwebsocket_callback_reasons reason,
        void *user, void *in, size_t len) 
{
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			printf("callback_my_http: New Connection Established!\n");
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			break;

		case LWS_CALLBACK_RECEIVE:
            {
                char *command = (char *)in;

                if ('l' == command[0]) {
                    int sockfd, newsockfd, portno;
                    socklen_t clilen;
                    char buffer[256];
                    struct sockaddr_in serv_addr, cli_addr;
                    int n;
                    sockfd = socket(AF_INET, SOCK_STREAM, 0);
                    if (sockfd < 0) 
                        error("callback_my_protocol, socket()");

                    bzero((char *) &serv_addr, sizeof(serv_addr));
                    portno = PORT_IPC;
                    serv_addr.sin_family = AF_INET;
                    serv_addr.sin_addr.s_addr = INADDR_ANY;
                    serv_addr.sin_port = htons(portno);
                    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
                        error("callback_my_protocol, bind()");

                    listen(sockfd,5);
                    clilen = sizeof(cli_addr);

                    char *argv_proc[] = {"user-proc", "Load", (char *)0};
                    create_proc_as_user(gid, uid, argv_proc, NULL);

                    newsockfd = accept(sockfd, 
                            (struct sockaddr *) &cli_addr, 
                            &clilen);

                    if (newsockfd < 0) 
                        error("callback_my_http, accept()");

                    bzero(buffer,256);
                    n = read(newsockfd,buffer,255);
                    if (n < 0) 
                        error("callback_my_http, read()");
                    close(newsockfd);
                    close(sockfd);
                    libwebsocket_write(wsi, (unsigned char *)buffer, strlen(buffer), LWS_WRITE_TEXT); // sync 
                }
                else if ('s' == command[0]) {
                    char *argv_proc2[] = { "user-proc", "Store", &command[6], (char *)0};
                    create_proc_as_user(gid, uid, argv_proc2, NULL);
                }
                else {
                    printf("callback_my_protocol: invalid command!");
                    return -1;
                }
            }
            break;

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			break;

		default:
			break;
	}

	return 0;
}

static int 
callback_my_http(
        struct libwebsocket_context *context, 
        struct libwebsocket *wsi, 
        enum libwebsocket_callback_reasons reason, 
        void *user, void *in, size_t len)
{
    return 0; // not implemented
}


//**************************************************************************//
/* list of supported protocols and callbacks */
//**************************************************************************//
static struct libwebsocket_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "my_http",             /* protocol name, char */
        callback_my_http,      /* callback function */
        0,                     /* per_session_data_size */
        0,                     /* max buffer */
    },    
	{
		"my_protocol",
		callback_my_protocol,
        0, 
        0,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};



int main(int argc, char **argv)
{
    size_t bufsize = 256;
    char current_login_username[256]; //
    int ret1 = getlogin_r(current_login_username, bufsize);
    int ret2 = name_to_uid(current_login_username, &uid, &gid);

    if (ret1 || ret2)
        return -1;

    int n = 0;
    struct libwebsocket_context *context;
    int opts = 0;
    const char *iface = NULL;
    int syslog_options = LOG_PID | LOG_PERROR;
    struct lws_context_creation_info info;

    int debug_level = 7;

    memset(&info, 0, sizeof info);
    info.port = PORT_LWS; // 

    signal(SIGINT, sighandler);

    /* we will only try to log things according to our debug_level */
    setlogmask(LOG_UPTO (LOG_DEBUG));
    openlog("lwsts", syslog_options, LOG_DAEMON);

    /* tell the library what debug level to emit and to send it to syslog */
    lws_set_log_level(debug_level, lwsl_emit_syslog);

    info.iface = iface;
    info.protocols = protocols;
    info.extensions = libwebsocket_get_internal_extensions();
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;

    info.gid = -1;
    info.uid = -1;
    info.options = opts;

    context = libwebsocket_create_context(&info);
    if (context == NULL) {
	    lwsl_err("libwebsocket init failed\n");
	    return -1;
    }

    n = 0;
    while (n >= 0 && !force_exit) {
	    n = libwebsocket_service(context, 50); 
    };

    libwebsocket_context_destroy(context);
    lwsl_notice("libwebsockets-test-server exited cleanly\n");
    closelog();
    return 0;
}
