#include <yangutil/sys/YangCTimer.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/yangtype.h>

#include <sys/time.h>

#include <stdio.h>
#include <yangutil/yang_unistd.h>
#include <yangutil/sys/YangLog.h>
#include <string.h>

#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <fcntl.h>

void yang_init_timer(YangCTimer *timer, void *user, int32_t taskId,
		int32_t waitTime) {
	if (timer == NULL)
		return;
	timer->isloop = 0;
	timer->isStart = 0;
	timer->waitState = 0;
	timer->waitTime = waitTime;

	timer->timerfd = timerfd_create(CLOCK_REALTIME, 0);
	timer->efd = -1;
	timer->user = user;
	timer->doTask = NULL;
	timer->taskId = taskId;
}
void yang_destroy_timer(YangCTimer *timer) {
	if (timer == NULL)
		return;
}
void* yang_run_timer_thread(void *obj) {
	YangCTimer *timer = (YangCTimer*) obj;
	timer->isStart = 1;
	timer->isloop = 1;

	struct itimerspec itimer;
	itimer.it_value.tv_sec = timer->waitTime / 1000;
	itimer.it_value.tv_nsec = (timer->waitTime % 1000) * 1000 * 1000;
	itimer.it_interval.tv_sec = timer->waitTime / 1000;
	itimer.it_interval.tv_nsec = (timer->waitTime % 1000) * 1000 * 1000;
	int ret = timerfd_settime(timer->timerfd, TFD_TIMER_ABSTIME, &itimer, NULL);
	if (ret == -1) {
		yang_error("timerfd_settime");
	}

	int opts;
	opts = fcntl(timer->timerfd, F_GETFL);
	if (opts < 0) {
		yang_error("fcntl(sock,GETFL)");
		_exit(1);
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(timer->timerfd, F_SETFL, opts) < 0) {
		yang_error("fcntl(sock,SETFL,opts)");
		_exit(1);
	}
	timer->efd = epoll_create(1);
	struct epoll_event tev;
	tev.events = EPOLLIN | EPOLLET;
	tev.data.fd = timer->timerfd;
	epoll_ctl(timer->efd, EPOLL_CTL_ADD, timer->timerfd, &tev);
	struct epoll_event ev[1];
	while (timer->isloop) {
		int nev = epoll_wait(timer->efd, ev, 1, 0);

		if (nev > 0 && (ev[0].events & EPOLLIN)) {
			uint64_t res;
			int bytes = read(timer->timerfd, &res, sizeof(res));

			if (timer->doTask)
				timer->doTask(timer->taskId, timer->user);
		}
	}
	timer->isStart = 0;
	return NULL;
}
void yang_timer_start(YangCTimer *timer) {
	if (timer == NULL)
		return;
	if (pthread_create(&timer->threadId, 0, yang_run_timer_thread, timer)) {
		yang_error("YangThread::start could not start thread");

	}

}
void yang_timer_stop(YangCTimer *timer) {
	if (timer == NULL)
		return;
	if (timer->isStart) {
		timer->isloop = 0;

		if (timer->isStart) {
			struct epoll_event tev;
			tev.events = EPOLLIN | EPOLLET;
			tev.data.fd = timer->timerfd;
			epoll_ctl(timer->efd, EPOLL_CTL_DEL, timer->timerfd, &tev);
			close(timer->efd);
			timer->efd = -1;

		}
		close(timer->timerfd);
		timer->timerfd = -1;
		while (timer->isStart)
			yang_usleep(1000);
	}
}

