#ifdef DONT_USE_LIBUV
#include "pico.h"

any LISP_uv_loop(any ex)
{
	printf("STUB: LISP_uv_loop\n");
	return Nil;
}

any LISP_uv_run_nowait(any ex)
{
	printf("STUB: LISP_uv_run_nowait\n");
	return Nil;
}

any LISP_uv_queue_work(any ex)
{
	printf("STUB: LISP_uv_queue_work\n");
	return Nil;
}

any LISP_uv_fs_event_start(any ex)
{
	printf("STUB: LISP_fs_event_start\n");
	return Nil;
}

any LISP_uv_fs_event_stop(any ex)
{
	printf("STUB: LISP_fs_event_stop\n");
	return Nil;
}

any LISP_uv_fs_stat(any ex)
{
	printf("STUB: LISP_fs_stat\n");
	return Nil;
}

any LISP_uv_tcp_connect(any ex)
{
	printf("STUB: LISP_uv_tcp_connect\n");
	return Nil;
}

any LISP_uv_tcp_write(any ex)
{
	printf("STUB: LISP_uv_tcp_write\n");
	return Nil;
}

any LISP_uv_read_start(any ex)
{
	printf("STUB: LISP_uv_read_start\n");
	return Nil;
}

any LISP_uv_close(any ex)
{
	printf("STUB: LISP_uv_close\n");
	return Nil;
}

any LISP_uv_stop(any ex)
{
	printf("STUB: LISP_uv_stop\n");
	return Nil;
}

any LISP_uv_tcp_listen(any ex)
{
	printf("STUB: LISP_uv_tcp_listen\n");
	return Nil;
}

any LISP_uv_fs_open(any ex)
{
	printf("STUB: LISP_uv_fs_open\n");
    return Nil;
}

any LISP_uv_fs_read(any ex)
{
	printf("STUB: LISP_uv_fs_read\n");
    return Nil;
}

any LISP_uv_fs_close(any ex)
{
	printf("STUB: LISP_uv_fs_close\n");
    return Nil;
}

#endif //DONT_USE_LIBUV
