#ifndef	__OPENSSL_H__
#define __OPENSSL_H__

void ssl_init();
void ssl_free();
void* get_ssl_fd(void *fd);
void close_ssl_fd(void *fd);
void ssl_err_print(void *ssl, int ret);

#endif
