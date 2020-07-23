##Tea project
This is a CPU partition project based on Linux, the partitioned CPU is called teapot, the bare-metal software runs in it called tea, teapot driver runs in Linux kernel mode, tea binary runs on the flat mode.  
An IPC driver(based on IPI) cunstructs the communication road between Linux and Tea.
##Limitation
* Tea runs in 32bit mode.
* Tea runs in flat memory mode, no MMU.
* The resources shared between CPU such as cache or power(core frequency) is not fully controled by Linux or Tea.
* No peripheral devices can be accessed by Tea yet. 
