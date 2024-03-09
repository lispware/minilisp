#ifndef DONT_USE_LIBUV

#include <uv.h>
#include "pico.h"

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
    uv_tcp_t tcp;
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
    any fn;
    any callback;
} FileWatcherHandle;

typedef struct {
	uv_fs_t req;
	any result;
	any callback;
} FileStatRequest;

typedef struct {
	uv_fs_t req;
	any result;
	any callback;
} FileOpenRequest;

typedef struct {
	uv_fs_t req;
	any result;
    any file;
	any callback;
    uv_buf_t buf;
} FileReadRequest;

void on_close(uv_handle_t *handle);

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char*)calloc(suggested_size, 1);
    buf->len = suggested_size;
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
        bindFrame f;
        any y = h->fn;
        Bind(y,f),  val(y) = mkStr(filename);
        prog(h->callback);
    	Unbind(f);
    }
}

// (setq FS_EVENT_HANDLER (uv_fs_event_start LOOP "refresh.l" (file-change)))
any LISP_uv_fs_event_start(any ex)
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
    any p3 = car(x);
    handle->fn = p3;

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

void check_file_existence(uv_fs_t* req) {
	FileStatRequest *handle = (FileStatRequest*)req;
	bindFrame f;
    any y = handle->result;
    Bind(y,f),  val(y) = req->result ? Nil : T;
    prog(handle->callback);
    Unbind(f);

    if (req->result != 0 && req->result != UV_ENOENT)
    {
        printf("Error checking file existence: %s\n", uv_strerror(req->result));
    }

    // Cleanup and free resources
    uv_fs_req_cleanup(req);
    free(req);
}

// (uv_fs_event_start LOOP "file.txt" E (exists E))
any LISP_uv_fs_stat(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *fileName = (char *)calloc(bufSize(p2), 1);
    bufString(p2, fileName);

    FileStatRequest *req = (FileStatRequest*)calloc(sizeof(FileStatRequest), 1);

    x = cdr(x);
    any p3 = car(x);
    req->result = p3;

    x = cdr(x);
    req->callback = x;

	int result = uv_fs_stat(loop, req, fileName, check_file_existence);
	free(fileName);
    if (result)
    {
        fprintf(stderr, "Error scheduling file existence check: %s\n", uv_strerror(result));
        free(req);
        return Nil;
    }

    return Nil;
}

// this does the closure of the handle too
any LISP_uv_fs_event_stop(any ex)
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

void on_uv_fs_open(uv_fs_t* req) {
    if (req->result < 0)
    {
        printf("Error opening file: %s\n", uv_strerror(req->result));
        return;
    }

	FileOpenRequest *handle = (FileOpenRequest*)req;
	any cb = handle->callback;
	any params = cdr(car(cb));
    any y = car(params);

    PACK(req->result, file);
	bindFrame f;
    Bind(y,f),  val(y) = file;
    prog(handle->callback);
    Unbind(f);

    // Cleanup and free resources
    uv_fs_req_cleanup(req);
    free(req);
}

any LISP_uv_fs_open(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    char *fileName = (char *)calloc(bufSize(p2), 1);
    bufString(p2, fileName);

    int mode = UV_FS_O_RDONLY;//READ
    x = cdr(x);
    any p3 = car(x);
    if (isSym(p3))
    {
        char *modeStr = (char *)calloc(bufSize(p3), 1);
        bufString(p3, modeStr);
        if (modeStr[0] == 'W') mode = UV_FS_O_WRONLY | UV_FS_O_CREAT; // WRITE
        if (modeStr[0] == 'A') mode = UV_FS_O_WRONLY | UV_FS_O_CREAT | UV_FS_O_APPEND; // APPEND
        free(modeStr);
    }

    FileOpenRequest *req = (FileOpenRequest*)calloc(sizeof(FileOpenRequest), 1);

    x = cdr(x);
    req->callback = x;

	int result = uv_fs_open(loop, req, fileName, mode, 0666, on_uv_fs_open);

    free(fileName);
    return ex;
}

void on_uv_fs_read(uv_fs_t* req) {
    FileReadRequest *r = (FileReadRequest*)req;

    if (req->result < 0)
    {
        printf("Error opening file: %s\n", uv_strerror(req->result));
        return;
    }

	any cb = r->callback;
	any params = cdr(car(cb));
    any p0 = car(params);
    any p1 = car(cdr(params));
    any p2 = car(cdr(cdr(params)));

	bindFrame e, f, g;

	void *ptr = r->buf.base;
	PACK(ptr, B);
	Bind(p0,e), val(p0) = r->file;
    Bind(p1,f),  val(p1) = B;
    Bind(p2,g),  val(p2) = box(req->result); // TODO this can  be incorrect since num is BITS-2
    prog(r->callback);
    Unbind(e);
    Unbind(f);
    Unbind(g);

    // Cleanup and free resources
    uv_fs_req_cleanup(req);
    free(req);
}

any LISP_uv_fs_read(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    UNPACK(p2, file);

    x = cdr(x);
    any p3 = EVAL(car(x));
    if (isNil(p3)) return Nil;
    if (!isNum(p3)) return Nil;
    word len = unBox(p3);

    FileReadRequest *req = (FileReadRequest*)calloc(sizeof(FileReadRequest), 1);

    x = cdr(x);
    req->callback = x;
    req->buf = uv_buf_init((char*)calloc(len, 1), len);
    req->file=p2;
	int result = uv_fs_read(loop, req, file, &req->buf, 1, -1, on_uv_fs_read);

    return ex;
}

void on_uv_fs_write(uv_fs_t* req) {
    FileReadRequest *r = (FileReadRequest*)req;

    if (req->result < 0)
    {
        printf("Error opening file: %s\n", uv_strerror(req->result));
        return;
    }

	any cb = r->callback;
	any params = cdr(car(cb));
    any p0 = car(params);
    any p1 = car(cdr(params));
    any p2 = car(cdr(cdr(params)));

	bindFrame e, f, g;

	void *ptr = r->buf.base;
	PACK(ptr, B);
	Bind(p0,e), val(p0) = r->file;
    Bind(p1,f),  val(p1) = B;
    Bind(p2,g),  val(p2) = box(req->result); // TODO this can  be incorrect since num is BITS-2
    prog(r->callback);
    Unbind(e);
    Unbind(f);
    Unbind(g);

    // Cleanup and free resources
    uv_fs_req_cleanup(req);
    free(req);
}

// (uv_fs_write LOOP FILE CONTENT 2 (on_write F B L))
any LISP_uv_fs_write(any ex)
{
    any x = ex;

    x = cdr(x);
    any p1 = EVAL(car(x));
    UNPACK(p1, l);
    uv_loop_t *loop = (uv_loop_t*)l;

    x = cdr(x);
    any p2 = EVAL(car(x));
    UNPACK(p2, file);

    x = cdr(x);
    any p3 = EVAL(car(x));
    UNPACK(p3, buffer);

    x = cdr(x);
    any p4 = EVAL(car(x));
    if (isNil(p4)) return Nil;
    if (!isNum(p4)) return Nil;
    word len = unBox(p4);

    FileReadRequest *req = (FileReadRequest*)calloc(sizeof(FileReadRequest), 1);

    x = cdr(x);
    req->callback = x;
    req->buf = uv_buf_init(buffer, len);
    req->file=p2;
	int result = uv_fs_write(loop, req, file, &req->buf, 1, -1, on_uv_fs_write);

    return ex;
}

any LISP_uv_fs_close(any ex)
{
    return ex;
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

#endif // DONT_USE_LIBUV
