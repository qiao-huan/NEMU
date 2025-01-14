/***************************************************************************************
* Copyright (c) 2014-2021 Zihao Yu, Nanjing University
* Copyright (c) 2020-2022 Institute of Computing Technology, Chinese Academy of Sciences
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>
#ifdef CONFIG_RVV
#include "../instr/rvv/vreg.h"
#endif // CONFIG_RVV
#include "../local-include/trapinfo.h"

#define FORCE_RAISE_PF

// Execution Guide generated by DUT
struct ExecutionGuide {
  // force raise exception
  bool force_raise_exception;
  uint64_t exception_num;
  uint64_t mtval;
  uint64_t stval;
#ifdef CONFIG_RVH
  uint64_t mtval2;
  uint64_t htval;
  uint64_t vstval;
#endif // CONFIG_RVH
  // force set jump target
  bool force_set_jump_target;
  uint64_t jump_target;
};

struct NonRegInterruptPending {
  bool platform_irp_meip;
  bool platform_irp_mtip;
  bool platform_irp_msip;
  bool platform_irp_seip;
  bool platform_irp_stip;
  bool platform_irp_vseip;
  bool platform_irp_vstip;
  bool lcofi_req;
};

struct Xtopei {
  uint64_t mtopei;
  uint64_t stopei;
  uint64_t vstopei;
};

struct DebugInfo {
  uint64_t current_pc;
};

#ifdef CONFIG_QUERY_REF
typedef enum RefQueryType {
  REF_QUERY_MEM_EVENT
} RefQueryType;

struct MemEventQueryResult {
  uint64_t pc;
  bool mem_access;
  bool mem_access_is_load;
  // uint64_t mem_access_paddr;
  uint64_t mem_access_vaddr;
  // uint64_t mem_access_result;
};
#endif

typedef struct TriggerModule TriggerModule;
typedef struct IpriosModule IpriosModule;
typedef struct IpriosSort IpriosSort;
typedef struct HighestPrioIntr HighestPrioIntr;

typedef struct {
  // Below will be synced by regcpy when run difftest, DO NOT TOUCH
  union {
    uint64_t _64;
  } gpr[32];

#ifndef CONFIG_FPU_NONE
  union {
    uint64_t _64;
  } fpr[32];
#endif // CONFIG_FPU_NONE

  // shadow CSRs for difftest
  uint64_t mode;
  uint64_t mstatus, sstatus;
  uint64_t mepc, sepc;
  uint64_t mtval, stval;
  uint64_t mtvec, stvec;
  uint64_t mcause, scause;
  uint64_t satp;
  // uint64_t mbmc;
  uint64_t mip, mie;
  uint64_t mscratch, sscratch;
  uint64_t mideleg, medeleg;
  uint64_t pc;
  // Above will be synced by regcpy when run difftest, DO NOT TOUCH

#ifdef CONFIG_RVH
  uint64_t v; // virtualization mode
  uint64_t mtval2, mtinst, hstatus, hideleg, hedeleg;
  uint64_t hcounteren, htval, htinst, hgatp, vsstatus;
  uint64_t vstvec, vsepc, vscause, vstval, vsatp, vsscratch;
#endif

//   uint64_t mbmc;

#ifdef CONFIG_RVV
  //vector
  union {
    uint64_t _64[VENUM64];
    uint32_t _32[VENUM32];
    uint16_t _16[VENUM16];
    uint8_t  _8[VENUM8];
  } vr[32];

  uint64_t vstart;
  uint64_t vxsat, vxrm, vcsr;
  uint64_t vl, vtype, vlenb;
#endif // CONFIG_RVV
#ifndef CONFIG_FPU_NONE
  uint64_t fcsr;
#endif // CONFIG_FPU_NONE

#ifdef CONFIG_RV_SDTRIG
  uint64_t tselect;
  uint64_t tdata1;
  uint64_t tinfo;
#endif // CONFIG_RV_SDTRIG


  // exec state
  bool amo;
  int mem_exception;

#ifdef CONFIG_TVAL_EX_II
  uint32_t instr;
#endif
  // for LR/SC
  uint64_t lr_addr;
  uint64_t lr_valid;

  bool INTR;
  // Guided exec
  bool guided_exec;
  struct ExecutionGuide execution_guide;

  struct NonRegInterruptPending non_reg_interrupt_pending;

  // User defined debug info
  struct DebugInfo debug;
#ifdef CONFIG_QUERY_REF
  struct MemEventQueryResult query_mem_event;
#endif // CONFIG_QUERY_REF

#ifdef CONFIG_RV_SDEXT
  bool debug_mode;
#endif // CONFIG_RV_SDEXT

#ifdef CONFIG_RV_SDTRIG
  TriggerModule *TM;
#endif // CONFIG_RV_SDTRIG
#ifdef CONFIG_RV_SMRNMI
  bool hasNMI;
#endif
#ifdef CONFIG_RV_IMSIC
  bool virtualInterruptIsHvictlInject;
#endif
#ifdef CONFIG_RV_SMDBLTRP
  bool critical_error;
#endif

  trap_info_t trapInfo;

#ifdef CONFIG_RV_IMSIC
  struct Xtopei xtopei;
  IpriosModule*  MIprios;
  IpriosModule*  SIprios;
  IpriosModule* VSIprios;
  IpriosSort*    MIpriosSort;
  IpriosSort*    SIpriosSort;
  IpriosSort*   VSIpriosSort;
  HighestPrioIntr* HighestPrioIntr;
#endif

} riscv64_CPU_state;

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11     : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      int32_t  simm12    : 1;
    } b;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      int32_t  simm31_12 :20;
    } u;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm19_12  : 8;
      uint32_t imm11     : 1;
      uint32_t imm10_1   :10;
      int32_t  simm20    : 1;
    } j;
    struct {
      uint32_t pad7      :20;
      uint32_t csr       :12;
    } csr;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t rm        : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t fmt       : 2;
      uint32_t funct5    : 5;
    } fp;
    #ifdef CONFIG_RVV
    //vector-OP-V
    struct {
      uint32_t pad0      : 7;
      uint32_t v_vd      : 5;
      uint32_t pad1      : 3;
      uint32_t v_vs1     : 5;
      uint32_t v_vs2     : 5;
      uint32_t v_vm      : 1;
      uint32_t v_funct6  : 6;
    } v_opv;
    struct {
      uint32_t pad0      :15;
      int32_t  v_simm5   : 5;
      uint32_t v_zimm    :11;
      uint32_t v_bigbit  : 1;
    } v_opsimm;
    struct {
      uint32_t pad0      :15;
      uint32_t v_imm5    : 5;
      uint32_t v_vs2     : 5;
      uint32_t v_vm      : 1;
      uint32_t v_i       : 1;
      uint32_t pad1      : 5;
    } v_opimm;
    struct {
      uint32_t pad0      :15;
      uint32_t v_zimm5   : 5;
      uint32_t v_zimm    :10;
      uint32_t v_bigbit  : 2;
    } v_vseti;
    //vector-LOAD-FP
    struct {
      uint32_t pad0      :12;
      uint32_t v_width   : 3;
      uint32_t pad1      : 5;
      uint32_t v_lsumop  : 5;
      uint32_t pad2      : 1;
      uint32_t v_mop     : 2;
      uint32_t v_mew     : 1;
      uint32_t v_nf      : 3;
    } vldfp;
    //vector-STORE-FP
    struct {
      uint32_t pad0      : 7;
      uint32_t v_vs3     : 5;
      uint32_t pad1      : 8;
      uint32_t v_sumop   : 5;
    } vstfp;
    //vector-AMO
    struct {
      uint32_t pad0      :26;
      uint32_t v_wd      : 1;
      uint32_t v_amoop   : 5;
    } vamo;
    #endif // CONFIG_RVV

    uint32_t val;
  } instr;
} riscv64_ISADecodeInfo;

// MODE_RS:reservation mode for mpp
enum { MODE_U = 0, MODE_S, MODE_RS, MODE_M };

enum { OP_OR = 0, OP_AND, OP_XOR, OP_ADD = 4 };

int get_data_mmu_state();
#ifdef CONFIG_RVH
int get_hyperinst_mmu_state();
#endif //CONFIG_RVH

#define isa_mmu_state() get_data_mmu_state() 
#endif
