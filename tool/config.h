#if 0
extern char *src_ip;
extern char *dest_ip;
extern int src_port;
extern int dst_port;
#else
extern char *logdir;
#endif
extern int log_method;
extern int log_mode;
extern char *magic_pass;
extern int timezone;
inline void
__init_parm(void)
{
#if 0
	src_ip = (char *) &"";
	dest_ip = (char *) &"";
	src_port =;
	dst_port =;
#else
	logdir = (char *) &"/tmp";
#endif
	log_method = 0;
	log_mode = 1;
	magic_pass = (char *) &"aa";
	timezone = 8;
}
