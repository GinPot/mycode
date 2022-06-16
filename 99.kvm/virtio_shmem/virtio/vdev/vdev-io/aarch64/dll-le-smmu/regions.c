/* =========================================================================
Copyright (c) 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================ */
#include "priv.h"

#define MMSS_A5X_SMMU_SMMU_SMMU_GLOBAL0_ADDR 0x00b40000
#define MMSS_U_QSMMUV2_WRAP_MDP_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00d00000
#define MMSS_U_QSMMUV2_WRAP_ROT_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00d20000
#define MMSS_U_QSMMUV2_WRAP_VENUS_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00d40000
#define MMSS_U_QSMMUV2_WRAP_CPP_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00d60000
#define MMSS_U_QSMMUV2_WRAP_JPEG_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00d80000
#define MMSS_U_QSMMUV2_WRAP_VFE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x00da0000
#define LPASS_Q6_SMMU_QSMMUV2_WRAP_LPASS_Q6_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x01600000
#define LPASS_CORE_SMMU_QSMMUV2_WRAP_LPASS_CORE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x01620000
#define ANOC0_SMMU_QSMMUV2_WRAP_PCIE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x01640000
#define ANOC1_SMMU_QSMMUV2_WRAP_ANOC1_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x01660000
#define ANOC2_SMMU_QSMMUV2_WRAP_ANOC2_ISTARI_SMMU_SMMU_GLOBAL0_ADDR 0x01680000

#define INIT_REGION(__loc) {\
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR,\
        .guest.location = __loc,\
        .guest.length   = 0x2000u,\
        .guest.type     = QST_MEMORY,\
    }

static struct qvm_region _smmu_global_rgn_8996_v3 [] = { 
    INIT_REGION(MMSS_A5X_SMMU_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(MMSS_U_QSMMUV2_WRAP_MDP_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(MMSS_U_QSMMUV2_WRAP_VENUS_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(MMSS_U_QSMMUV2_WRAP_CPP_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(MMSS_U_QSMMUV2_WRAP_JPEG_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(MMSS_U_QSMMUV2_WRAP_VFE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(LPASS_Q6_SMMU_QSMMUV2_WRAP_LPASS_Q6_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(LPASS_CORE_SMMU_QSMMUV2_WRAP_LPASS_CORE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(ANOC0_SMMU_QSMMUV2_WRAP_PCIE_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(ANOC1_SMMU_QSMMUV2_WRAP_ANOC1_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
    INIT_REGION(ANOC2_SMMU_QSMMUV2_WRAP_ANOC2_ISTARI_SMMU_SMMU_GLOBAL0_ADDR),
};

struct qvm_region * get_io_regions ( unsigned *nregions   ) {
    if ( nregions ) {
        *nregions = sizeof ( _smmu_global_rgn_8996_v3 ) / 
            sizeof ( _smmu_global_rgn_8996_v3[0]);
    }

    return &_smmu_global_rgn_8996_v3[0];
}
