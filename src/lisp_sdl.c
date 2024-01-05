#include "pico.h"
#include <SDL2/SDL.h>
#include <uv.h>

#define PACK(__M, __R) any __R; { \
word __r = (word)__M; \
word allOnes = -1; \
uword __mask = ((uword)allOnes >> (BITS / 2)); \
word __low = __r & __mask; \
word __high = (__r >> (BITS / 2))  & __mask; \
__R = cons(box(__high), box(__low)); }

#define UNPACK(__M, __R) uword __R; {\
	uword __H = unBox(car(__M));\
	uword __L = unBox(cdr(__M));\
	__R = (__H << (BITS / 2)) | __L; }

typedef struct {
        uv_work_t _work;
        any callback;
} WORK;


typedef struct {
	uv_tcp_t _tcp;
	any _read;
	any _readData;
	any _write;
	any _writeData;
} TCPHandle;

typedef struct {
	uv_connect_t _handle;
	TCPHandle *_tcp;
	any callback;
} ConnectionHandle;

typedef struct {
	uv_write_t write_req;
	any callback;
} WriteRequest;

typedef struct {
	uv_stream_t _stream;
	any callback;
	any data;
} ReadRequest;


void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
        buf->base = (char*)calloc(suggested_size, 1);
        buf->len = suggested_size;
}

any LISP_SDL_CreateWindow(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	x = cdr(x);
	any p2 = EVAL(car(x));
	x = cdr(x);
	any p3 = EVAL(car(x));

	char *windowTitle = (char *)calloc(bufSize(p1), 1);
	bufString(p1, windowTitle);

	word W = unBox(p2);
	word H = unBox(p3);

	SDL_Window *window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, 0);
	PACK(window, P);
	return P;
}

any LISP_SDL_CreateRenderer(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    PACK(renderer, P);
    return P;
}

any LISP_SDL_PollEvent(any ex)
{
	SDL_Event event;
	if (SDL_PollEvent( &event ))
	{
		if (event.type == SDL_WINDOWEVENT)
		{
			any y;
			cell c1;

			PACK(event.type, eventType);
			Push(c1, y = cons(eventType, Nil));
			PACK(event.window.event, eventValue)
			y = cdr(y) = cons(eventValue, Nil);
			printf("WINDOW EVENT eventType = %p eventValue = %p\n", eventType, eventValue);
			return Pop(c1);
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			any y;
			cell c1;

			PACK(event.type, eventType);
			Push(c1, y = cons(eventType, Nil));
			PACK(event.window.event, eventValue)
			y = cdr(y) = cons(eventValue, Nil);
			printf("MOUSE EVENT eventType = %p eventValue = %p\n", eventType, eventValue);
			return Pop(c1);
		}
		else if (event.type == SDL_USEREVENT)
		{
			any y;
			cell c1;

			PACK(event.type, eventType);
			Push(c1, y = cons(eventType, Nil));
			y = cdr(y) = cons(event.user.data1, Nil);
			printf("USER EVENT eventType = %p eventValue = %p\n", eventType, event.user.data1);
			return Pop(c1);
		}

		return Nil;
	}
	else
	{
		return Nil;
	}
}


any LISP_SDL_DestroyWindow(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_DestroyWindow((SDL_Window*)w);

    return Nil;
}

any LISP_SDL_Quit(any ex)
{
    SDL_Quit();
    return Nil;
}


any COMP_PACK(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	UNPACK(p1, word1);
	x = cdr(x);
	any p2 = EVAL(car(x));
	UNPACK(p2, word2);

	if (word1 == word2)
	{
		return T;
	}

	return Nil;
}

any doPACK(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	word n = unBox(p1);

	PACK(n, ret);
	return ret;
}

any LISP_SDL_Init(any ex)
{
	SDL_Init(SDL_INIT_VIDEO);
	return Nil;
}


any LISP_SDL_RenderDrawLine(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
	any X1 = EVAL(car(x));
    x = cdr(x);
	any Y1 = EVAL(car(x));
    x = cdr(x);
	any X2 = EVAL(car(x));
    x = cdr(x);
	any Y2 = EVAL(car(x));

	SDL_RenderDrawLine((SDL_Renderer*)renderer, unBox(X1), unBox(Y1), unBox(X2), unBox(Y2));

	return Nil;
}

any LISP_SDL_RenderPresent(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, renderer);
    SDL_RenderPresent((SDL_Renderer*)renderer);

    return Nil;
}

any LISP_SDL_SetRenderDrawColor(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, renderer);

    x = cdr(x);
	any R = EVAL(car(x));
    x = cdr(x);
	any G = EVAL(car(x));
    x = cdr(x);
	any B = EVAL(car(x));
	SDL_SetRenderDrawColor((SDL_Renderer*)renderer, unBox(R), unBox(G), unBox(B), SDL_ALPHA_OPAQUE);

    return Nil;
}

any LISP_SDL_GetMouseState(any ex)
{
	Uint32 mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	return cons(box(mouseX), box(mouseY));
}

any LISP_uv_loop(any ex)
{
	uv_loop_t* uv_loop = uv_default_loop();

    PACK(uv_loop, RET);
    return RET;
}

any LISP_uv_run_nowait(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;
    uv_run(loop, UV_RUN_NOWAIT);
    return Nil;
}

void __worker(uv_work_t *req)
{
        printf("worker callback\n");
}

void after_work(uv_work_t *req)
{
        printf("after callback\n");
	WORK *work = (WORK*)req;

	cell foo, c;
	Push(foo, work->callback);
	Push(c, work->_work.data);
	apply(work->callback, data(foo), NO, 1, &c);
	Pop(foo);

	free(work);
}

any LISP_uv_queue_work(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);

	WORK *work = (WORK*)malloc(sizeof(WORK));
	work->callback = EVAL(car(x));

    x = cdr(x);
	work->_work.data = EVAL(car(x));

	uv_queue_work(loop, work, __worker, after_work);
}

void on_tcp_connect(uv_connect_t* req, int status)
{
	if (status != 0)
	{
		fprintf(stderr, "TCP connection failed: %s\n", uv_strerror(status));
		uv_close(req->handle, NULL);
		return;
	}

	ConnectionHandle *connection = (ConnectionHandle*)req;

	uv_tcp_t *t = connection->_tcp;
	PACK(t, tcp);

	cell foo, c;
	Push(foo, connection->callback);
	Push(c, tcp);
	apply(connection->callback, data(foo), NO, 1, &c);
	Pop(foo);

	free(req);
}

any LISP_uv_tcp_connect(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	x = cdr(x);
	any p2 = EVAL(car(x));
	x = cdr(x);
	any p3 = EVAL(car(x));
	x = cdr(x);
	any p4 = EVAL(car(x));
	x = cdr(x);
	any p5 = EVAL(car(x));
	x = cdr(x);
	any p6 = EVAL(car(x));
	x = cdr(x);
	any p7 = EVAL(car(x));
	x = cdr(x);
	any p8 = EVAL(car(x));

    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

	char *host = (char *)calloc(bufSize(p2), 1);
	bufString(p2, host);

	word port = unBox(p3);

	struct sockaddr_in* dest = (struct sockaddr_in*)calloc(sizeof(struct sockaddr_in), 1);
    uv_ip4_addr(host, port, dest);

	ConnectionHandle *connection = (ConnectionHandle*)calloc(sizeof(ConnectionHandle), 1);
	connection->_tcp = (TCPHandle*)calloc(sizeof(TCPHandle), 1);

	uv_tcp_init(loop, connection->_tcp);
	connection->callback = p4;
	connection->_tcp->_read = p5;
	connection->_tcp->_readData = p6;
	connection->_tcp->_write = p7;
	connection->_tcp->_writeData = p8;
    // Start the asynchronous connect operation
    uv_tcp_connect(&connection->_handle, connection->_tcp, (const struct sockaddr*)dest, on_tcp_connect);

	free(host);
	free(dest);
	return Nil;
}

void on_tcp_write(uv_write_t* req, int status)
{
	if (status != 0)
	{
		fprintf(stderr, "Write failed: %s\n", uv_strerror(status));
		uv_close(req, NULL);
		return;
	}

	WriteRequest* write_req = (WriteRequest*)req;

	cell foo;
	Push(foo, write_req->callback);
	apply(write_req->callback, data(foo), NO, 0, NULL);
	Pop(foo);
}

any LISP_uv_tcp_write(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	x = cdr(x);
	any p2 = EVAL(car(x));

    UNPACK(p1, t);
    TCPHandle *_tcp = (TCPHandle*)t;

	char *text = (char *)calloc(bufSize(p2), 1);
	bufString(p2, text);

	uv_buf_t buf = uv_buf_init((char*)text, strlen(text));
	WriteRequest* write_req = (WriteRequest*)calloc(sizeof(WriteRequest), 1);
	write_req->callback = _tcp->_write;
	uv_write(write_req, (uv_stream_t*)_tcp, &buf, 1, on_tcp_write);

	free(text);
}

void on_tcp_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
	if (nread < 0)
	{
		fprintf(stderr, "TCP read failed: %s\n", uv_strerror(nread));
		return 0;
	}

	if (nread == 0)
	{
		fprintf(stderr, "EMPTY READ\n");
		return;
	}

	ReadRequest* read_req = (ReadRequest*)stream;
	TCPHandle* handle = (TCPHandle*)stream;

	cell foo, c[2];
	Push(foo, handle->_read);
	Push(c[0], mkStr(buf->base));
	Push(c[1], handle->_readData);
	apply(read_req->callback, data(foo), NO, 2, c);
	Pop(foo);
}

any LISP_uv_tcp_read(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));
	x = cdr(x);
	any p2 = EVAL(car(x));

    UNPACK(p1, t);
    TCPHandle *_tcp = (TCPHandle*)t;

    printf("TCP HANDLE = %p\n", _tcp);

    uv_read_start((uv_stream_t*)_tcp, alloc_buffer, on_tcp_read);

    return Nil;
}

any LISP_SDL_PushEvent(any ex)
{
	any x = cdr(ex);
	any p1 = EVAL(car(x));

	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.data1 = p1;
	SDL_PushEvent(&event);
	printf("SDL PUSH %d %p\n", SDL_USEREVENT, p1);
}
