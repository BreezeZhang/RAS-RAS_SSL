#include "openssl.h"
#include "config.h"
#include "unistd.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *ctx;
void ssl_init()
{
	char pwd[MAXBUF];

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	ctx = SSL_CTX_new(SSLv23_server_method());

	if(ctx == NULL)
	{
		printf("ssl_init failed\n");
		ERR_print_errors_fp(stdout);
		exit(1);
	}

	getcwd(pwd, MAXBUF);

	if(strlen(pwd) == 1)
		pwd[0] = '\0';
	  if (SSL_CTX_use_certificate_file(ctx, strcat(pwd,"/cacert.pem"), SSL_FILETYPE_PEM) <= 0)
	  {
	    ERR_print_errors_fp(stdout);
	    exit(1);
	  }

	getcwd(pwd, MAXBUF);

	if(strlen(pwd) == 1)
		pwd[0] = '\0';
	if(SSL_CTX_use_PrivateKey_file(ctx, strcat(pwd, "/privkey.pem"), SSL_FILETYPE_PEM) <= 0)
	{
		printf("SSL_CTX_use_PrivateKey_file failed\n");
		ERR_print_errors_fp(stdout);
		exit(1);
	}

	if(!SSL_CTX_check_private_key(ctx))
	{
		ERR_print_errors_fp(stdout);
		exit(1);
	}
}

void ssl_free()
{
	SSL_CTX_free(ctx);
}

void* get_ssl_fd(void *fd)
{
	SSL *ssl;
	int new_fd = *(int *)fd;
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, new_fd);
	SSL_set_accept_state(ssl);
	if(SSL_accept(ssl) < 0)
	{
		perror("accept");
		close(new_fd);
		return NULL;
	}
	return ssl;
}
void close_ssl_fd(void *fd)
{
	SSL *ssl;
	ssl = (SSL *)fd;
	SSL_shutdown(ssl);
	SSL_free(ssl);
}
void ssl_err_print(void *ssl, int ret)
{
	  switch(SSL_get_error(ssl,ret)){
	        case SSL_ERROR_NONE:
	                printf("no error occur\n");
	                break;
		case SSL_ERROR_ZERO_RETURN:
			printf("The TLS/SSL connection has been closed.\n");
			break;
	        case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
			printf("read or write error\n");
	                break;
	        case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_WANT_ACCEPT:
			printf("connect or accept error\n");
	                break;
	        case SSL_ERROR_SYSCALL:
	                printf("SSL Error:Premature close\n");
	                break;
	        case SSL_ERROR_SSL:
	                printf("A failure in the SSL library occurred, usually a protocol error. \n");
			break;
		default:
			printf("some undefine error occur");
			break;
	 }
}
