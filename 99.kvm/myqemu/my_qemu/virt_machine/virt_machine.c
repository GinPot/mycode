#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>



#undef 	KVM_REG_ARM_CORE_REG
#define offsetof__(TYPE, MEMBER)        ((size_t)&((TYPE *)0)->MEMBER)
#define KVM_REG_ARM_CORE_REG(name)      (offsetof__(struct kvm_regs, name) / sizeof(__u32))
#define AARCH64_CORE_REG(x)        		(KVM_REG_ARM64 | KVM_REG_SIZE_U64 | KVM_REG_ARM_CORE | KVM_REG_ARM_CORE_REG(x))


#define OUT_PORT				0x8000
#define IN_PORT					0x8004
#define EXIT_REG				0x10000
#define SP_REG					0x101000
#define GUEST_PHY_ADDR			0x100000
#define GUEST_PHY_ADDR_SIZE		0x1000


#define GUEST_BIN    			"./guest.bin"
#define print_virt(fmt, ...)	printf("MY_QUEM %s => " fmt "\n", __func__, ##__VA_ARGS__)	



int main(int argc, const char *argv[])
{
	int mmap_size;
	void *userspace_addr;
	__u64 guest_entry = GUEST_PHY_ADDR;
    int kvm_fd, vm_fd, vcpu_fd, guest_fd;

	struct kvm_one_reg reg;
	struct kvm_run *kvm_run;
	struct kvm_vcpu_init init;
    struct kvm_userspace_memory_region mem;


	print_virt("virt machine init.");

    if((kvm_fd = open("/dev/kvm", O_RDWR)) < 0){																	//open kvm driver
		print_virt("open kvm driver fail: %s", strerror(errno));
		goto main_err;
	}

    if((vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0)) < 0){																//kvm create vm
		print_virt("kvm create vm fail: %s", strerror(errno));
		goto open_err;
	}

    if((vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0)) < 0){															//kvm create vcpu
		print_virt("kvm create vcpu fail: %s", strerror(errno));
		goto open_err;
	}

    if((mmap_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL)) < 0){												//kvm get vcpu mmap size
		print_virt("kvm get vcpu mmap size fail: %s", strerror(errno));
		goto open_err;
	}

    if((kvm_run = (struct kvm_run *)mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0)) < 0){	//mmap vcpu shared memory
		print_virt("mmap vcpu shared memory fail: %s", strerror(errno));
		goto open_err;
	}

    if((guest_fd = open(GUEST_BIN, O_RDONLY)) < 0){																	//open guest bin
		print_virt("open guest bin fail: %s", strerror(errno));
		goto mmap_err;
	}

	//分配一段匿名共享内存，作为客户机的物理内存；guest访问这段内存不会trap，访问这段以外的会trap
    if((userspace_addr = mmap(NULL, GUEST_PHY_ADDR_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0)) < 0){
		print_virt("mmap guest phy addr fail: %s", strerror(errno));
		goto open_guest_err;
	}

    if(read(guest_fd, userspace_addr, GUEST_PHY_ADDR_SIZE) < 0){													//read guest bin to guest phy addr 
		print_virt("read guest bin to guest phy addr fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

	//
    mem.slot = 0;
    mem.flags = 0;
    mem.userspace_addr = (__u64)userspace_addr;																		//host virt addr
    mem.guest_phys_addr = (__u64)GUEST_PHY_ADDR;																	//guest phy addr
    mem.memory_size = (__u64)GUEST_PHY_ADDR_SIZE;																	//addr size
    if(ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &mem) < 0){															//kvm set user memory region
		print_virt("kvm set user memory region fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

    bzero(&init, sizeof(init));
    init.target = KVM_ARM_TARGET_CORTEX_A53;
    if(ioctl(vcpu_fd, KVM_ARM_VCPU_INIT, &init) < 0){																//kvm arm vcpu init
		print_virt("kvm arm vcpu init fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

    reg.id = AARCH64_CORE_REG(regs.pc);
    reg.addr = (__u64)&guest_entry;
    if(ioctl(vcpu_fd, KVM_SET_ONE_REG, &reg) < 0){																	//kvm set vcpu startup phy addr
		print_virt("kvm set one reg fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

	print_virt("kvm start run.");
    while(1){
        if(ioctl(vcpu_fd, KVM_RUN, NULL) < 0){																		//kvm run
			print_virt("kvm run fail: %s", strerror(errno));
			//goto mmap_phyaddr_err;
		}

        switch (kvm_run->exit_reason) {																				//get exit reason from vcpu shared memory
        case KVM_EXIT_MMIO:
            if(kvm_run->mmio.is_write && kvm_run->mmio.len == 1){
                if (kvm_run->mmio.phys_addr == OUT_PORT){															//guest printf
                    if(kvm_run->mmio.data[0] < 10)
						printf("%x", kvm_run->mmio.data[0]);
					else
						printf("%c", kvm_run->mmio.data[0]);
				}else{
					if(kvm_run->mmio.phys_addr == EXIT_REG){														//guest exit
						print_virt("******Guest Exit!******");
						goto mmap_phyaddr_err;
					}
				}
            }
			else{
				if(!kvm_run->mmio.is_write && kvm_run->mmio.len == 1){
					if (kvm_run->mmio.phys_addr == IN_PORT){														//guest getchar
						kvm_run->mmio.data[0] = 'G';
					}
				}
			}
            break;
        default:
            print_virt("Unknow exit reason: %d", kvm_run->exit_reason);
            goto mmap_phyaddr_err;
        }
    }



mmap_phyaddr_err:
	munmap(userspace_addr, GUEST_PHY_ADDR_SIZE);
open_guest_err:
	close(guest_fd);
mmap_err:
	munmap(kvm_run, mmap_size);
open_err:
	close(kvm_fd);
main_err:
    return 0;
}