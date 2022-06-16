#include "config.h"

#undef 	KVM_REG_ARM_CORE_REG
#define offsetof__(TYPE, MEMBER)        ((size_t)&((TYPE *)0)->MEMBER)
#define KVM_REG_ARM_CORE_REG(name)      (offsetof__(struct kvm_regs, name) / sizeof(__u32))
#define AARCH64_CORE_REG(x)        		(KVM_REG_ARM64 | KVM_REG_SIZE_U64 | KVM_REG_ARM_CORE | KVM_REG_ARM_CORE_REG(x))


#define OUT_PORT				0x8000
#define IN_PORT					0x8004
#define EXIT_REG				0x10000
#define SP_REG					0x101000



int main(int argc, const char *argv[])
{
	int mmap_size, i;
	__u64 data;

	struct kvm_one_reg reg;
	struct kvm_run *kvm_run;
	struct kvm_vcpu_init init;
    struct kvm_userspace_memory_region mem;
	
	struct kvm_create_device create_dev;
	struct kvm_device_attr kvmattr;


	//print_virt("virt machine init.");

    if((kvm_fd = open("/dev/kvm", O_RDWR)) < 0){																	//open kvm driver
		print_virt("open kvm driver fail: %s", strerror(errno));
		goto main_err;
	}
	
	/***vm***/
    if((vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0)) < 0){																//kvm create vm
		print_virt("kvm create vm fail: %s", strerror(errno));
		goto open_err;
	}

    if((vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0)) < 0){															//kvm create vcpu
		print_virt("kvm create vcpu fail: %s", strerror(errno));
		goto open_err;
	}
	
	/***vcpu***/
    bzero(&init, sizeof(init));
    init.target = KVM_ARM_TARGET_CORTEX_A53;
    if(ioctl(vcpu_fd, KVM_ARM_VCPU_INIT, &init) < 0){																//kvm arm vcpu init
		print_virt("kvm arm vcpu init fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

    if((mmap_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL)) < 0){												//kvm get vcpu mmap size
		print_virt("kvm get vcpu mmap size fail: %s", strerror(errno));
		goto open_err;
	}

    if((kvm_run = (struct kvm_run *)mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0)) < 0){	//mmap vcpu shared memory
		print_virt("mmap vcpu shared memory fail: %s", strerror(errno));
		goto open_err;
	}

	/***memory***/
	//分配一段匿名共享内存，作为客户机的物理内存；guest访问这段内存不会trap，访问这段以外的会trap
    if((userspace_addr = mmap(NULL, GUEST_PHY_ADDR_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0)) < 0){
		print_virt("mmap guest phy addr fail: %s", strerror(errno));
		goto open_guest_err;
	}

    mem.slot = 0;
    mem.flags = 0;
    mem.userspace_addr = (__u64)userspace_addr;																		//host virt addr
    mem.guest_phys_addr = (__u64)GUEST_PHY_ADDR;																	//guest phy addr
    mem.memory_size = (__u64)GUEST_PHY_ADDR_SIZE;																	//addr size
    if(ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &mem) < 0){															//kvm set user memory region
		print_virt("kvm set user memory region fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}

	/***vgic***/
    create_dev.type = KVM_DEV_TYPE_ARM_VGIC_V2;
    create_dev.flags = 0;
    if(ioctl(vm_fd, KVM_CREATE_DEVICE, &create_dev) < 0){															//kvm set user memory region
		print_virt("kvm create vgic fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
	gic_fd = create_dev.fd;

    kvmattr.group = 0;
    kvmattr.attr = 1;
    kvmattr.addr = (__u64)&data;
	data = 0x8010000;				//dtb中定义
    if(ioctl(gic_fd, KVM_SET_DEVICE_ATTR, &kvmattr) < 0){															//kvm set user memory region
		print_virt("KVM_SET_DEVICE_ATTR11: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
    kvmattr.group = 0;
    kvmattr.attr = 0;
    kvmattr.addr = (__u64)&data;
	data = 0x8000000;				//dtb中定义
    if(ioctl(gic_fd, KVM_SET_DEVICE_ATTR, &kvmattr) < 0){															//kvm set user memory region
		print_virt("KVM_SET_DEVICE_ATTR22: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}



	/***Prepare image***/
	//Image
    if((guest_fd = open(GUEST_BIN, O_RDONLY)) < 0){																//open Image
		print_virt("open Image fail: %s", strerror(errno));
		goto mmap_err;
	}
    if(read(guest_fd, userspace_addr+0x80000, 0x2000000) < 0){													//read Image bin to guest phy addr 
		print_virt("read Image to guest phy addr fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
	
	//dtb
    if((guest_dtb_fd = open(GUEST_DTB, O_RDONLY)) < 0){															//open dtb
		print_virt("open dtb fail: %s", strerror(errno));
		goto mmap_err;
	}
    if(read(guest_dtb_fd, userspace_addr+0x8600000, 0x20000) < 0){												//read ramdisk bin to guest phy addr 
		print_virt("read dtb to guest phy addr fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
	
	//ramdisk
    //if((guest_rootfs_fd = open(GUEST_ROOTFS, O_RDONLY)) < 0){													//open ramdisk
	//	print_virt("open ramdisk fail: %s", strerror(errno));
	//	goto mmap_err;
	//}
    //if(read(guest_rootfs_fd, userspace_addr+0x8000000, 0x800000) < 0){											//read dtb bin to guest phy addr 
	//	print_virt("read ramdisk to guest phy addr fail: %s", strerror(errno));
	//	goto mmap_phyaddr_err;
	//}




	/***init cpu reg***/
	reg.addr = (__u64)&data;
	/* x0~x3 */
    reg.id = AARCH64_CORE_REG(regs.regs[0]);
    data = GUEST_DTB_ADD;
    if(ioctl(vcpu_fd, KVM_SET_ONE_REG, &reg) < 0){																	//kvm set vcpu startup phy addr
		print_virt("kvm set x0 fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
    for (i = 1; i < 4; i++) {
		reg.id = AARCH64_CORE_REG(regs.regs[i]);
		data	= 0;
		if(ioctl(vcpu_fd, KVM_SET_ONE_REG, &reg) < 0){																//kvm set vcpu startup phy addr
			print_virt("kvm set x%d fail: %s", i, strerror(errno));
			goto mmap_phyaddr_err;
		}
    }
	/* pstate */
    reg.id = AARCH64_CORE_REG(regs.pstate);
    data = PSR_D_BIT | PSR_A_BIT | PSR_I_BIT | PSR_F_BIT | PSR_MODE_EL1h;
    if(ioctl(vcpu_fd, KVM_SET_ONE_REG, &reg) < 0){																	//kvm set vcpu startup phy addr
		print_virt("kvm set pstate fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}
	/* pc */
    reg.id = AARCH64_CORE_REG(regs.pc);
    data = GUEST_IMAGE_ADD;
    if(ioctl(vcpu_fd, KVM_SET_ONE_REG, &reg) < 0){																	//kvm set vcpu startup phy addr
		print_virt("kvm set pc fail: %s", strerror(errno));
		goto mmap_phyaddr_err;
	}


	/***tty***/
	ter_init();

	/***blk***/
	virtio_blk_init();

	/***net***/
	virtio_net_init();


    while(1){
        if(ioctl(vcpu_fd, KVM_RUN, NULL) < 0){																		//kvm run
			print_virt("kvm run fail: %s", strerror(errno));
			goto mmap_phyaddr_err;
		}

        switch (kvm_run->exit_reason) {																				//get exit reason from vcpu shared memory
			case KVM_EXIT_MMIO:
				/*uart*/
				if((kvm_run->mmio.phys_addr >= 0x1000000) && (kvm_run->mmio.phys_addr <= 0x1000008)){
					serial8250_mmio(kvm_run->mmio.phys_addr, 
									kvm_run->mmio.data, 
									kvm_run->mmio.len, 
									kvm_run->mmio.is_write);
				}
				/*gic*/
				if((kvm_run->mmio.phys_addr >= 0x8020000) && (kvm_run->mmio.phys_addr <= 0x8021000)){
					mgic_mmio(kvm_run->mmio.phys_addr, 
									kvm_run->mmio.data, 
									kvm_run->mmio.len, 
									kvm_run->mmio.is_write);
				}
				/*blk*/
				if((kvm_run->mmio.phys_addr >= 0xa000000) && (kvm_run->mmio.phys_addr <= 0xa000200)){
					blk_mmio(kvm_run->mmio.phys_addr, 
									kvm_run->mmio.data, 
									kvm_run->mmio.len, 
									kvm_run->mmio.is_write);
				}
				/*net*/
				if((kvm_run->mmio.phys_addr >= 0xa001000) && (kvm_run->mmio.phys_addr <= 0xa001200)){
					net_mmio(kvm_run->mmio.phys_addr, 
									kvm_run->mmio.data, 
									kvm_run->mmio.len, 
									kvm_run->mmio.is_write);
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


