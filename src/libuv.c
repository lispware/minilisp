#include <uv.h>
#include "pico.h"

typedef struct {
    uv_tcp_t handle;
    any cb;
} uv_tcp_t_withCB;


void uv_default_loop_free(External *loop)
{
}


any douv_default_loop(any ex)
{
	uv_loop_t *loop = uv_default_loop();

	External *ext = (External*)calloc(sizeof(External), 1);

	ext->data = (void*)loop;
	ext->free = uv_default_loop_free;

	printf("LOOP = %p ext=%p\n", loop, ext);

	any r = cons(Nil, box2(ext));

	return r;
}

void douv_tcp_init_free(External *ext)
{
	free(ext->data);
}

any douv_tcp_init(any ex)
{
	any x = cdr(ex);
	any y = EVAL(car(x));
	External *loopExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p LOOP=%p\n", y, y->cdr, loopExt, loopExt->data);
	uv_loop_t *loop = (uv_loop_t*)loopExt->data;;

	x = cdr(x);
	y = EVAL(car(x));
	External *serverExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p TCP=%p\n", y, y->cdr, serverExt, serverExt->data);
	uv_tcp_t *server = (uv_tcp_t*)serverExt->data;

	External *ext = (External*)calloc(sizeof(External), 1);

	uv_tcp_init(loop, server);

	return Nil;
}

any douv_ip4_addr(any ex)
{
	any x, y;
	x = cdr(ex);
	y = EVAL(car(x));
   	NeedSymb(ex,y);

      	char *nm = (char*)calloc(pathSize(y), 1);
      	pathString(y,nm);

	x = cdr(x);
	y = EVAL(car(x));
   	word port = unBox(y);

	x = cdr(x);
	y = EVAL(car(x));
	External *addressExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p ADDRESS=%p\n", y, y->cdr, addressExt, addressExt->data);
	struct sockaddr_in *address = (struct sockaddr_in *)addressExt->data;

	printf("IP4 %s %d\n", nm, port);
	uv_ip4_addr(nm, port, address);

	free(nm);
	return Nil;
}

any douv_tcp_bind(any ex)
{
	any x, y;

	x = cdr(ex);
	y = EVAL(car(x));
	External *serverExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p SERVER=%p\n", y, y->cdr, serverExt, serverExt->data);
	uv_tcp_t *server = (uv_tcp_t*)serverExt->data;


	x = cdr(x);
	y = EVAL(car(x));
	External *addressExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p ADDRESS=%p\n", y, y->cdr, addressExt, addressExt->data);
	struct sockaddr_in *address = (struct sockaddr_in *)addressExt->data;

	uv_tcp_bind(server, address, 0);

	return Nil;
}

void on_new_connection(uv_stream_t *server, int status)
{
	printf("on_new_connection called\n");
	uv_tcp_t_withCB *ppp = (uv_tcp_t_withCB*)server;

	cell res, foo;

	Push(res, Nil);
	Push(foo, ppp->cb);


 	External *ext = (External*)calloc(sizeof(External), 1);
 	ext->data = server;

	cell c[2];
	Push(c[0], cons(Nil, box2(ext)));
	Push(c[1], box(status));

	data(res) = apply(ppp->cb, data(foo), NO, 2, &c);

	Pop(res);

	free(ext);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void on_close(uv_handle_t* handle) {
    free(handle);
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    //free_write_req(req);
}

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;
void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {

	printf("ECHO READ\n");
	uv_tcp_t_withCB *ppp = (uv_tcp_t_withCB*)client;

	cell res, foo;

	Push(res, Nil);
	Push(foo, ppp->cb);

	External *ext = (External*)calloc(sizeof(External), 1);
	ext->data=client;

	External *ext2 = (External*)calloc(sizeof(External), 1);
	ext2->data = buf;

	cell c[3];
	Push(c[0], cons(Nil, box2(ext)));
	Push(c[1], cons(Nil, box2(ext2)));
	Push(c[2], box(nread));

printf("BEFORE CLIENT=%p, BUF=%p, NREAD=%d\n", client, buf, nread);
	data(res) = apply(ppp->cb, data(foo), NO, 3, &c);

	Pop(res);

	free(ext);

#if 0
	if (nread > 0) {
		write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
		req->buf = uv_buf_init(buf->base, nread);
		uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);
		for(int i = 0;i < nread; i++)
		{
			char *p = req->buf.base;
			printf("%c\n", p[i]);
		}
		return;
	}
	if (nread < 0) {
		if (nread != UV_EOF)
			fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		uv_close((uv_handle_t*) client, on_close);
	}
#endif

	free(buf->base);
}

any douv_write(any ex)
{
	any x, y;

	x = cdr(ex);
	y = EVAL(car(x));
	External *clientExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p client=%p\n", y, y->cdr, clientExt, clientExt->data);
	uv_tcp_t *client = (uv_tcp_t*)clientExt->data;

	x = cdr(x);
	y = EVAL(car(x));
	External *bufExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p buf=%p\n", y, y->cdr, bufExt, bufExt->data);
	uv_buf_t *buf = (uv_buf_t*)bufExt->data;

	x = cdr(x);
	y = EVAL(car(x));
   	word nread = unBox(y);

printf("AFTER CLIENT=%p, BUF=%p, NREAD=%lld\n", client, buf, nread);

	write_req_t *req = (write_req_t*) calloc(sizeof(write_req_t), 1);
	req->buf = uv_buf_init(buf->base, nread);
	uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);

	//External *ext = (External*)calloc(sizeof(External), 1);

	//ext->data = uv_buf_init(bufExt->data, n);
	//// TODO ext->free = uv_default_loop_free;

	//printf("BUF = %p ext=%p\n", ext->data, ext);

	//any r = cons(Nil, box2(ext));

	//return r;

	return Nil;
}


any douv_accept(any ex)
{
	any x, y;

	x = cdr(ex);
	y = EVAL(car(x));
	External *serverExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p SERVER=%p\n", y, y->cdr, serverExt, serverExt->data);
	uv_tcp_t *server = (uv_tcp_t*)serverExt->data;

	x = cdr(x);
	y = EVAL(car(x));
	External *clientExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p CLIENT=%p\n", y, y->cdr, clientExt, clientExt->data);
	uv_tcp_t *client = (uv_tcp_t*)clientExt->data;

	uv_tcp_t_withCB *ppp = (uv_tcp_t_withCB*)client;
	ppp->cb = car(cdr(x));
	

	if (uv_accept(server, (uv_stream_t*) client) == 0) {
		printf("START READING\n");
		uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
	}
	else {
		uv_close((uv_handle_t*) client, on_close);
	}

	if (uv_accept(server, (uv_stream_t*)client) != 0)
	{
		return Nil;
	}


	return T;
}



any douv_listen(any ex)
{
	any x, y;

	x = cdr(ex);
	y = EVAL(car(x));
	External *serverExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p SERVER=%p\n", y, y->cdr, serverExt, serverExt->data);
	uv_tcp_t *server = (uv_tcp_t*)serverExt->data;

	uv_tcp_t_withCB *ppp = (uv_tcp_t_withCB*)server;
	ppp->cb = car(cdr(x));
	

    int r = uv_listen(server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        exit(0);
    }

	return Nil;
}

any douv_run(any ex)
{
	any x = cdr(ex);
	any y = EVAL(car(x));
	External *loopExt = (External*)(((word)y->cdr)-2);
	printf("y = %p y->cdr %p EXT = %p LOOP=%p\n", y, y->cdr, loopExt, loopExt->data);
	uv_loop_t *loop = (uv_loop_t*)loopExt->data;;

    	int ret =  uv_run(loop, UV_RUN_DEFAULT);
}

any dodingo(any ex)
{
   any x = cdr(ex);
   cell res, foo;

   Push(res, Nil);
   Push(foo, EVAL(car(x)));

   x = cdr(x);
   cell c;
   Push(c, EVAL(car(x)));

   data(res) = apply(ex, data(foo), NO, 1, &c);

   return Pop(res);

}
