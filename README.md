## Tea project  
This is a CPU partition project based on Linux, the partitioned CPU is called teapot, the bare-metal software runs in it called tea, teapot driver runs in Linux kernel mode, tea binary runs on the flat mode.  
An IPC driver(based on IPI) cunstructs the communication road between Linux and Tea.

## Limitation
* Tea runs in 32bit mode.
* Tea runs in flat memory mode, no MMU.
* The resources shared between CPU such as cache or power(core frequency) is not fully controled by Linux or Tea.
* No peripheral devices can be accessed by Tea yet. 

## Environment
The project has been verified on Ubuntu 18.04, as tea(bare-metal) binary runs on the last core with entry address 0x1000, so kernel must reserved some memory starting from 0x1000, just add "memmap=64k$0x1000" on kernel cmdline, as for Ubuntu, please add "memmap=64k\\\\\\$0x1000" on the "GRUB_CMDLINE_LINUX_DEFAULT" inside file "/etc/defult/grub", then run "sudo update-grub" to take effect.

## Build
```bash
git clone https://github.com/zqiu2000/tea.git
git clone https://github.com/zqiu2000/teapot.git
cd tea
make
sudo cp tea.bin /lib/firmware/
cd ../teapot
make
```

## Run  
```bash
cd teapot
sudo insmod ipc_tea.ko; sudo insmod teapot.ko
```
Check the memory log as following
```bash
sudo dd if=/dev/mem bs=1 skip=40960 count=8192
```
