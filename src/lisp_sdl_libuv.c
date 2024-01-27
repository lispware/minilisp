#include <uv.h>
#include <SDL.h>
#include "pico.h"

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
    any binding;
    any value;
} WORK;

typedef struct {
    uv_write_t write_req;
    any binding;
    any value;
    any binding2;
    any value2;
    any callback;
} WriteRequest;

typedef struct {
    uv_stream_t _stream;
    any callback;
    any data;
} ReadRequest;

typedef struct {
    uv_stream_t tcp;
    any callback;
    any data;
    any binding;
    any bindingValue;
} TCPHandle;

typedef struct {
    uv_connect_t handle;
    any bindingTCP;
    any bindingDATA;
    any bindingDATAVALUE;
    any callback;
} ConnectionHandle;

typedef struct {
	uv_fs_event_t watcher;
	any callback;
} FileWatcherHandle;

void on_close(uv_handle_t *handle);

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

any LISP_SDL_SetWindowSize(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));

    x = cdr(x);
    any p2 = EVAL(car(x));
    x = cdr(x);
    any p3 = EVAL(car(x));

    UNPACK(p1, w);
    SDL_Window *window = (SDL_Window*)w;

    word W = unBox(p2);
    word H = unBox(p3);

	SDL_SetWindowSize(window, W, H);


    return Nil;
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
            PACK(event.window.event, eventValue);
            y = cdr(y) = cons(eventValue, Nil);
            return Pop(c1);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            any y;
            cell c1;

            PACK(event.type, eventType);
            Push(c1, y = cons(eventType, Nil));
            PACK(event.window.event, eventValue);
            y = cdr(y) = cons(eventValue, Nil);
            return Pop(c1);
        }
        else if (event.type == SDL_USEREVENT)
        {
            any y;
            cell c1;

            PACK(event.type, eventType);
            Push(c1, y = cons(eventType, Nil));
            y = cdr(y) = cons(event.user.data1, Nil);
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

any LISP_SDL_RenderDrawPoint(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any X1 = EVAL(car(x));
    x = cdr(x);
    any Y1 = EVAL(car(x));

    SDL_RenderDrawPoint((SDL_Renderer*)renderer, unBox(X1), unBox(Y1));

    return Nil;
}

// (SDL_SetRenderDrawBlendMode RENDERER MODE)
// 0 SDL_BLENDMODE_NONE
// 1 SDL_BLENDMODE_BLEND
// 2 SDL_BLENDMODE_ADD
// 3 SDL_BLENDMODE_MOD
// 4 SDL_BLENDMODE_MUL
any LISP_SDL_SetRenderDrawBlendMode(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any M = EVAL(car(x));
    int _mode = M == Nil? 0 : unBox(M);
    SDL_BlendMode modes[] = {SDL_BLENDMODE_NONE,SDL_BLENDMODE_BLEND,SDL_BLENDMODE_ADD,SDL_BLENDMODE_MOD,SDL_BLENDMODE_MUL};

	SDL_SetRenderDrawBlendMode((SDL_Renderer*)renderer, modes[_mode]);

    return Nil;
}

any LISP_SDL_RenderFillRect(any ex)
{
    any x = cdr(ex);
    any p1 = EVAL(car(x));
    UNPACK(p1, renderer);
    x = cdr(x);
    any X = EVAL(car(x));
    x = cdr(x);
    any Y = EVAL(car(x));
    x = cdr(x);
    any W = EVAL(car(x));
    x = cdr(x);
    any H = EVAL(car(x));

	SDL_Rect rect;
	rect.x = unBox(X);
	rect.y = unBox(Y);
	rect.w = unBox(W);
	rect.h = unBox(H);
    SDL_RenderFillRect((SDL_Renderer*)renderer, &rect);

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
    x = cdr(x);
    any A = EVAL(car(x));

    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, unBox(R), unBox(G), unBox(B), A == Nil ? SDL_ALPHA_OPAQUE: unBox(A));

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
}

void after_work(uv_work_t *req, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Worker failed: %s\n", uv_strerror(status));
        return;
    }

    WORK *work = (WORK*)req;

    bindFrame f;
    any y = work->binding;
    Bind(y,f),  val(y) = work->value;
    prog(work->callback);
    Unbind(f);
    free(work);
}

// (uv_queue_work LOOP DATA (process DATA))
any LISP_uv_queue_work(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    WORK *work = (WORK*)calloc(sizeof(WORK), 1);

    x = cdr(x);
    work->binding = car(x);
    work->value = EVAL(car(x));

    x = cdr(x);
    work->callback = x;

    uv_queue_work(loop, (uv_work_t*)work, __worker, after_work);
    return Nil;
}

// (uv_fs_event_start LOOP "/path/file" (callback)
void uv_file_change(uv_fs_event_t* handle, const char* filename, int events, int status)
{
	if (status < 0)
	{
		fprintf(stderr, "Error in file event callback: %s\n", uv_strerror(status));
		return;
	}

    if (events & UV_CHANGE) {
    	FileWatcherHandle *h = (FileWatcherHandle*)handle;
    	prog(h->callback);
    }
}

// TODO: Add the corresponding stop
any LISP_fs_event_start(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    FileWatcherHandle *handle = (FileWatcherHandle*)calloc(sizeof(FileWatcherHandle), 1);

    int ret = uv_fs_event_init(loop, &handle->watcher);
    if (ret)
	{
        fprintf(stderr, "Error initializing file watcher: %s\n", uv_strerror(ret));
		return Nil;
	}

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *fileName = (char *)calloc(bufSize(p2), 1);
    bufString(p2, fileName);

    x = cdr(x);
    handle->callback = x;

    ret = uv_fs_event_start(handle, uv_file_change, fileName, UV_FS_EVENT_RECURSIVE);
    if(ret !=0)
	{
        fprintf(stderr, "Error starting file watcher: %s\n", uv_strerror(ret));
        return Nil;
	}

    PACK(handle, P);
    return P;
}

// this does the closure of the handle too
any LISP_fs_event_stop(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    FileWatcherHandle *handle = (FileWatcherHandle*)l;

    uv_fs_event_stop((uv_fs_event_t*)handle);
	uv_close((uv_handle_t*)handle, on_close);

    return Nil;
}

void on_tcp_connect(uv_connect_t* req, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "TCP connection failed: %s\n", uv_strerror(status));
        uv_close((uv_handle_t*)req->handle, NULL);
        return;
    }

    ConnectionHandle *connection = (ConnectionHandle*)req;

    uv_tcp_t *t = (uv_tcp_t*)req->handle;
    PACK(t, tcp);

    bindFrame f;
    any y = connection->bindingTCP;
    Bind(y,f),  val(y) = tcp;
    y = connection->bindingDATA;
    Bind(y, f);
    val(y) = connection->bindingDATAVALUE;
    prog(connection->callback);
    Unbind(f);
    free(req);
}

// (uv_tcp_connect LOOP "ip" 8080 TCP DATA (handle TCP DATA))
any LISP_uv_tcp_connect(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *host = (char *)calloc(bufSize(p2), 1);
    bufString(p2, host);

    x = cdr(x);
    any p3 = EVAL(car(x));
    word port = unBox(p3);

    struct sockaddr_in* dest = (struct sockaddr_in*)calloc(sizeof(struct sockaddr_in), 1);
    uv_ip4_addr(host, port, dest);

    x = cdr(x);
    any p4 = car(x);
    x = cdr(x);
    any p5 = car(x);
    x = cdr(x);
    any p6 = x;

    ConnectionHandle *connectionHandle = (ConnectionHandle*)calloc(sizeof(ConnectionHandle), 1);
    connectionHandle->bindingTCP = p4;
    connectionHandle->bindingDATA = p5;
    connectionHandle->bindingDATAVALUE = EVAL(p5);
    connectionHandle->callback = p6;
    TCPHandle *tcpHandle = (TCPHandle*)calloc(sizeof(TCPHandle), 1);
    uv_tcp_init(loop, (uv_tcp_t*)tcpHandle);

    uv_tcp_connect((uv_connect_t*)connectionHandle, (uv_tcp_t*)tcpHandle, (const struct sockaddr*)dest, on_tcp_connect);

    free(host);
    free(dest);
    return Nil;
}

void on_tcp_write(uv_write_t* req, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Write failed: %s\n", uv_strerror(status));
        uv_close((uv_handle_t*)req->handle, NULL);
        return;
    }

    WriteRequest* write_req = (WriteRequest*)req;

    bindFrame f;
    any y = write_req->binding;;
    Bind(y,f),  val(y) = write_req->value;
    y = write_req->binding2;
    Bind(y,f),  val(y) = write_req->value2;
    prog(write_req->callback);
    Unbind(f);
    free(req);
}

// (uv_write TCP "HELLO" DATA (on_write TCP DATA))
any LISP_uv_tcp_write(any ex)
{
    any x = ex;

    x=cdr(x);
    any p1Sym = car(x);
    any p1 = EVAL(p1Sym);
    UNPACK(p1, t);
    TCPHandle *tcp = (TCPHandle*)t;

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *text = (char *)calloc(bufSize(p2), 1);
    bufString(p2, text);

    x = cdr(x);
    any p3  = car(x);

    x = cdr(x);
    any callback = x;

    uv_buf_t buf = uv_buf_init((char*)text, strlen(text));
    WriteRequest* write_req = (WriteRequest*)calloc(sizeof(WriteRequest), 1);
    write_req->callback = callback;
    write_req->binding = p1Sym;
    write_req->value = p1;
    write_req->binding2 = p3;
    write_req->value2 = EVAL(p3);
    uv_write((uv_write_t*)write_req, (uv_stream_t*)tcp, &buf, 1, on_tcp_write);

    free(text);
    return Nil;
}

void on_close(uv_handle_t *handle)
{
    free(handle);
}

void on_tcp_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    if (nread < 0)
    {
        fprintf(stderr, "TCP read failed: %s\n", uv_strerror(nread));
        return;
    }

    if (nread == 0)
    {
        fprintf(stderr, "EMPTY READ\n");
        return;
    }

    TCPHandle* read_req = (TCPHandle*)stream;

    bindFrame f;
    any y = read_req->data;
    Bind(y,f),  val(y) = mkStr(buf->base);
    y = read_req->binding;
    Bind(y,f),  val(y) = read_req->bindingValue;
    prog(read_req->callback);
    Unbind(f);

    free(buf->base);
}

// (uv_tcp_read TCP DATA DATA2 (process DATA DATA2))
any LISP_uv_read_start(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, t);
    TCPHandle *tcp = (TCPHandle*)t;

    x = cdr(x);
    any p2 = car(x);

    x = cdr(x);
    any p3 = car(x);

    x = cdr(x);
    any p4 = x;

    tcp->data = p2;
    tcp->callback = p4;
    tcp->binding = p3;
    tcp->bindingValue = EVAL(p3);

    uv_read_start((uv_stream_t*)tcp, alloc_buffer, on_tcp_read);

    return Nil;
}

any LISP_uv_close(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_handle_t *handle = (uv_handle_t*)l;

    uv_close(handle, NULL);
    return Nil;
}

any LISP_uv_stop(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    uv_stop(loop);
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
}

void on_connection(uv_stream_t *server, int status)
{
    if (status != 0)
    {
        fprintf(stderr, "Connection failed: %s\n", uv_strerror(status));
        return;
    }

    TCPHandle *tcpHandle = (TCPHandle*)server;

    TCPHandle *client = (TCPHandle*)calloc(sizeof(TCPHandle), 1);
    uv_tcp_init(uv_default_loop(), (uv_tcp_t*)client);

    client->data = tcpHandle->data;
    client->binding = tcpHandle->binding;
    client->bindingValue = tcpHandle->bindingValue;
    client->callback = tcpHandle->callback;

    if (uv_accept(server, (uv_stream_t*)client) != 0)
    {
        free(client);
        uv_close((uv_handle_t*)client, on_close);
    }
    else
    {
        uv_tcp_t *t = (uv_tcp_t*)client;
        PACK(t, tcp);

        bindFrame f;
        any y = client->data;
        Bind(y,f),  val(y) = tcp;
        y = client->binding;
        Bind(y, f);
        val(y) = client->bindingValue;
        prog(client->callback);
        Unbind(f);
    }
}

// (uv_tcp_listen LOOP "127.0.0.1" 8080 TCP  DATA (handle TCP DATA))
any LISP_uv_tcp_listen(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *host = (char *)calloc(bufSize(p2), 1);
    bufString(p2, host);

    x = cdr(x);
    any p3 = EVAL(car(x));
    word port = unBox(p3);

    struct sockaddr_in* addr = (struct sockaddr_in*)calloc(sizeof(struct sockaddr_in), 1);
    uv_ip4_addr(host, port, addr);

    x = cdr(x);
    any p4 = car(x);
    x = cdr(x);
    any p5 = car(x);
    x = cdr(x);
    any p6 = x;

    TCPHandle *tcpHandle = (TCPHandle*)calloc(sizeof(TCPHandle), 1);
    uv_tcp_init(loop, (uv_tcp_t*)tcpHandle);
    tcpHandle->data = p4;
    tcpHandle->binding = p5;
    tcpHandle->bindingValue = EVAL(p5);
    tcpHandle->callback = p6;

    uv_tcp_bind((uv_tcp_t*)tcpHandle, (const struct sockaddr*)addr, 0);
    int r = uv_listen((uv_stream_t*)tcpHandle, 128, on_connection);


    if (r)
    {
        fprintf(stderr, "TCP read failed: %s\n", uv_strerror(r));
        return 0;
    }

    return Nil;
}
