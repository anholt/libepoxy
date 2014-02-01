#
# Regular cron jobs for the libepoxy package
#
0 4	* * *	root	[ -x /usr/bin/libepoxy_maintenance ] && /usr/bin/libepoxy_maintenance
