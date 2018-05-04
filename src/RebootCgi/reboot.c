#include <unistd.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include "my_include.h"
#include "common.h"

HB_S32 main(void)
{

	WRITE_LOG("Go to reboot!\n");

	my_system(RM_SESSION_FILE);

	reboot(LINUX_REBOOT_CMD_RESTART);
	//system("reboot");
	//system("/etc/init.d/S40network restart");

	return 0;
}
